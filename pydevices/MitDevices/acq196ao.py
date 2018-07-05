# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from acq import Acq

class ACQ196AO(Acq):
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
        {'path':':FAWG_DIV','type':'numeric','value':1,'options':('no_write_shot',)},
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
        """Tell the board to arm"""
        import ftplib
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

    def doFit(self, x, y, newx, type):
        import scipy.interpolate
        fitfun = scipy.interpolate.interp1d(x,y, bounds_error=False, fill_value=0.0, kind=type.lower()) 
        return fitfun(newx)
    
    def getDelta(self, thing):
        import MDSplus
        if isinstance(thing, MDSplus.TreeNode) :
	    return self.getDelta(thing.record)
        else :
	    return thing.delta
 
    def init(self, auto_store=None):
        import tempfile
        import time
        import uu
        import MDSplus
        import StringIO
        import subprocess
        import numpy
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
            max_samples = int(self.max_samples)
            if max_samples < 0 :
                raise Exception(message)
            msg="Could not read clock source"
            clock_src=self.ao_clock.record.getOriginalPartName().getString()[1:]
            msg="Clock must be filled in with valid Range value"
            clock = self.__getattr__(clock_src.lower())
            if self.debugging():
                print "clock source is %s clock is %s\n"%(clock_src, clock,)
            msg="Could not read trigger source"
            trig_src=self.ao_trig.record.getOriginalPartName().getString()[1:]
            msg="Trigger time must be defined  at init time"
            trigger = self.__getattr__(trig_src.lower())
            msg="FAWG_DIV must be a postive integer"
            fawg_div = int(self.fawg_div)
            if fawg_div <= 0:
                raise Exception(message)
            message = "Could not retrieve the slope from the specified clock"
            delta = self.getDelta(clock.record)
            slope = delta*fawg_div

            msg="Could not read cycle type"
            cycle_type = str(self.cycle_type.record)
            if cycle_type not in self.cycle_types :
               raise Exception("cycle type must be one of %s\n" % (self.cycle_types,))
            msg="Could not read trigger type"
            trig_type = str(self.trig_type.record)
            if trig_type not in self.trig_types :
               raise Exception("trigger type must be one of %s\n" % (self.trig_types,))
            msg = "Error constructing output timebase"
            dim = MDSplus.Dimension(MDSplus.Window(0, max_samples-1, trigger), MDSplus.Range(None, None, slope)).data()

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
                chan_node = self.__getattr__('output_%2.2d' % (chan+1,))
                if chan_node.on :
                    if self.debugging():
                        print "it is on so ..."
                    try:
                        sig = chan_node.record
                        knots_x = sig.dim_of().data()
                        knots_y = sig.data()
                        fit_type = str(chan_node.getNode('fit').record)
                        if  fit_type not in self.fits:
                            raise Exception( "Error reading Fit for channel %d\n" %(chan+1,))
                    except Exception,e:
                        print "Error on channel %d - ZEROING\n%s\n" %(chan+1, e,)
                        knots_x = [0.0, 1.0]
                        knots_y = [0.0, 0.0]
                        fit_type='LINEAR'
                else:
                    if self.debugging():
                        print "   it is off so Zeroing\n"
                    knots_x = [0.0, 1.0]
                    knots_y = [0.0, 0.0]
                    fit_type='LINEAR'
                if self.debugging():
                    print "  call the fit\n"
                wave=self.doFit(knots_x, knots_y, dim, fit_type)
                if self.debugging():
                    print "  have the fit now subscript and scale"
                if len(wave) > max_samples:
                    wave = wave[0:max_samples-1]
                wave = wave/10.*2**15
                wave = wave.astype(numpy.int16)
#                uuinfd = StringIO.StringIO(wave)
                outname = "/dev/acq196/AO/f.%2.2d\n" % (chan + 1,)
                fd.write("cat - <<EOF | uudecode  -o %s" % (outname,))
                fd.flush()
                if self.debugging():
                    print "   ready to uuencode"
                p = subprocess.Popen(["uuencode", "-m", "%s"%(outname,)], stdout=fd, stdin=subprocess.PIPE)
                p.communicate(wave.tostring())
                fd.flush()
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
        
        if self.debugging():
            print "starting zero\n"
        s=socket.socket()
        try:
            s.setimeout=(15)
            s.connect((self.getBoardIp(),54548))
            s.send("zero")
        except Exception,e:
            status=0
            print "Error sending doZero: %s" % (str(e),)
        s.close()
        if self.debugging():
            print "finishing zero\n"
        return 0
    ZERO=zero
        
    def setvoltage(self, arg) :
        import socket
        if self.debugging():
            print "starting setvoltage /%s/\n" %(arg,)
        if arg == None:
            print "setvoltage method requires an argument string 'N V'\n"
            return 0

        args = arg.split()
        if len(args) != 2 :
            print "setvoltage method takes exactly 2 arguments\n"
            return 0

        if args[0] != "XX" :
            try :
                chan = int(args[0])
                if  chan < 0 or chan > 16 :
                    raise Exception("out of range")
            except:
	       print "setvoltage method 1st arg must be XX or [1-16]\n"
               return 0
	try:
            v = float(args[1])
	except:
            print "setvoltage method 2nd argument must be a floating point voltage\n"
            return 0

        s = socket.socket()
        try :
            s.connect((self.getBoardIp(),54549))
            s.send(str(arg))
	except Exception,e:
 	    print "setvoltage failed to set voltage %s\ntype of arg is %s\n%s\n" % (arg,type(arg),e,)
            return 0
        return 1
    SETVOLTAGE=setvoltage
	    
