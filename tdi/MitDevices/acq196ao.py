from acq import ACQ

class ACQ196AO(ACQ):
    """
    D-Tacq ACQ196  16 channel arbitrary waveform generator piggyback function
    
    """
    parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254'},
        {'path':':COMMENT','type':'text'},
        ]

    for i in range(6):
        parts.append({'path':':DI%1.1d'%(i,),'type':'axis','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:WIRE'%(i,),'type':'text','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:BUS'%(i,),'type':'text','options':('no_write_shot',)})


    parts2=[
        {'path':':AO_CLOCK','type':'numeric','valueExpr':'head.di0','options':('no_write_shot',)},
        {'path':':AO_TRIG','type':'numeric','valueExpr':'head.di3','options':('no_write_shot',)},
        {'path':':FWAG_DIV','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':CYCLE_TYPE','type':'text','value':'ONCE','options':('no_write_shot',)},
        {'path':':TRIG_TYPE','type':'text','value':'HARD_TRIG','options':('no_write_shot',)},
        {'path':':MAX_SAMPLES','type':'numeric','value':16384,'options':('no_write_shot',)},
       ]
    parts.extend(parts2)
    del parts2

    for i in range(16):
        parts.append({'path':':OUTPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_shot',)})
	parts.append({'path':':OUTPUT_%2.2d:FIT'%(i+1,),'type':'TEXT', 'value':'LINEAR','options':('no_write_shot')})
    del i

    parts.append({'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
         'options':('no_write_shot',)})
    parts.append({'path':':ZERO_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'ZERO',head))",
         'options':('no_write_shot',)})

    fits = ('LINEAR', 'CUBIC')
    trig_types = ('HARD_TRIG', 'SOFT_TRIG')
    cycle_types = ('ONCE', 'CYCLIC')

#
# doAOInit - just sends the file /tmp/AOInitialize on the board
#            it does not invoke it !
#
#            Instead it is invoked by the doInit.sh script if it is present
#            and then removed
#
    def doAOInit(self,fd):
        import tempfile
        import time
        """Tell the board to arm"""
        if self.debugging():
	    print "starting doInit"
        status=1
        try:
            ftp = ftplib.FTP(self.getBoardIp())
            ftp.login('dt100','dt100')
            ftp.storlines("STOR /tmp/AOinitialize",fd)
        except Exception,e:
            print "Error sending arm commands via ftp to %s\n%s\n" % (self.getBoardIp(), e,)
            return

    def fit(self, x, y, newx, type):
        import scipy.interpolate
        fitfun = scipy.interpolate.interp1d(x,y, bounds_error=False, fill_value=0.0, kind=type.lower()) 
        return fitfun(newx)
    
    def init(self, auto_store=None):
        import ftplib
        import uu
        """
        Initialize the device
        Send parameters
        Arm hardware
        """
        start=time.time()
        msg=None

	try:
            if self.debugging():
                print "starting init\n";
	    path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot

            message = "max_samples must be a postiive integer"
            max_samples(int(self.max_samples))
            if max_samples < 0 :
                print message
                return 0
            msg="Could not read clock source"
            clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
            msg="Clock must be filled in with valid Range value"
            clock = self.__getattr__(str(self.clock_src.record).lower())

            msg="Could not read trigger source"
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
            msg="FAWG_DIV must be a postive integer"
            fawg_div = int(self.fawg_div)
            if fawg_div <= 0:
                print msg
                return 0
            message = "Could not retrieve the slope from the specified clock"
            slope = clock.getDelta()*fawg_div

            msg="Could not read cycle type"
            cycle_type = str(self.cycle_type)
            if cycle_type not in self.cycle_types :
                print "cycle type must be one of %s\n" % (self.cycle_types,)
                return 0
            msg="Could not read trigger type"
            trig_type = str(self.trig_type)
            if trig_type not in self.trig_types :
                print "Trigger type must be one of %s\n" % (self.trig_types,)
                return 0

            dim = Dimension(Window(0, max_samples-1, trigger), Range(None, None, slope))

            msg = "Error writing initialization file"


#
# now create the post_shot ftp command file
#
            fd = tempfile.TemporaryFile()
            host = self.getMyIp()
            fd.write("acqcmd setAbort\n")
	    fd.write("host=%s\n"%(host,))
            fd.write("tree=%s\n"%(tree,))
            fd.write("shot=%s\n"%(shot,))
            fd.write("path='%s'\n"%(path,))

            for chan in range(16):
                if self.debugging():
                    print "working on channel %d" % chan
                chan_node = self.__getattr__('input_%2.2d' % (chan+1,))
                if chan_node.on :
                    if self.debugging():
                        print "it is on so ..."
                try:
                    sig = chan_node.record
                    knots_x = sig.dim_of().data()
                    knots_y = sig.data()
                    fit = str(chan_node.getNode('fit').record)
                    s = 1
                    if  fit not in self.fits:
                        raise Exception( "Error reading Fit for channel %d\n" %(chan+1,))
                except Exception,e:
                    print "Error on channel %d - ZEROING\n%s\n" %(e,)
                    knots_x = [0.0, 1.0]
                    knots_y = [0.0, 0.0]
                    fit='LINEAR'
                
                wave=self.fit(knots_x, knots_y, dim, fit_type)
                if len(wave) > max_samples:
                    wave = wave[0:max_samples-1]
                wave = int(wave/10.*2^15)
                uuinfd = StringIO.StringIO(wave)
                fd.write("cat - <<EOF | uudecode /dev/stdout > /dev/acq196/AO/%2.2d\n" % (chan + 1,))
                uu.encode( uuinfd, fd)
                fd.write("EOF\n")

            fd.write("set.ao_clk %s rising\n" %(clock_src,))
            fd.write("set.ao_trig %s rising\n" % (trig_src,))
            fd.write("set.dtacq FAWG_div %d\n" % (fawg_div))
            fd.write("set.dtacq AO_sawg_rate 10000\n")
            fd.write("set.arm.AO.FAWG %s %s\n" % (cycle_type, trig_type,))


            fd.flush()
            fd.seek(0,0)
	    print "Time to make init file = %g\n" % (time.time()-start)
	    start=time.time()
            self.doAOInit(fd)
	    fd.close()

	    print "Time for board to init = %g\n" % (time.time()-start)
            return  1

        except Exception,e:
            if msg != None:
                print 'error = %s\nmsg = %s\n' %(msg, str(e),)
            else:
                print "%s\n" % (str(e),)
            return 0
    INIT=init

    def zero(self) :
        import socket
        import signal
        
        if self.debugging():
            print "starting zero\n"
        s=socket.socket()
        try:
            signal.signal(signal.SIGALRM,self.timeoutHandler)
            signal.alarm(15)
            s.connect((self.getBoardIp(),54548))
            s.send("zero")
        except Exception,e:
            status=0
            print "Error sending doZero: %s" % (str(e),)
        signal.alarm(0)
        s.close()
        if self.debugging():
            print "finishing zero\n"
        return 0
    ZERO=zero
        
        
