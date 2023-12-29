from MDSplus import Device, Data, Int32, Float32
from ctypes import CDLL,c_char_p,c_short,byref, c_int
from MDSplus.mdsExceptions import DevBAD_PARAMETER

class BCM_CONFIG(Device):
    """Beam Current Monitor Timing Configuration"""
    parts = [
        {'path':':COMMENT','type':'text'},
    ]
    for i in range(1,7):
        parts.extend([
            {'path':'.CONF_%d'%(i),'type':'structure'},
            {'path':'.CONF_%d:DEV_PATH'%(i),'type':'text'},
            {'path':'.CONF_%d:MODE'%(i),'type':'text', 'value':'CONTINUOUS'},
            {'path':'.CONF_%d:START_TIME'%(i),'type':'numeric', 'value':0},
            {'path':'.CONF_%d:STOP_TIME'%(i),'type':'numeric', 'value':1},
            {'path':'.CONF_%d:PRE_SMP'%(i),'type':'numeric', 'value':0},
            {'path':'.CONF_%d:POST_SMP'%(i),'type':'numeric', 'value':10000},
            {'path':'.CONF_%d:CONT_FREQ'%(i),'type':'numeric', 'value':10000},
            {'path':'.CONF_%d:FAST_FREQ'%(i),'type':'numeric', 'value':10000},
            {'path':'.CONF_%d:SLOW_FREQ'%(i),'type':'numeric', 'value':10000},
            {'path':'.CONF_%d:SLOW_EV'%(i),'type':'text'},
            {'path':'.CONF_%d:FAST_EV'%(i),'type':'text'},
            {'path':'.CONF_%d:CONT_EV'%(i),'type':'text'},
            {'path':'.CONF_%d:SLOW_TRIG'%(i),'type':'numeric'},
            {'path':'.CONF_%d:FAST_TRIG'%(i),'type':'numeric'},
            {'path':'.CONF_%d:CONT_TRIG'%(i),'type':'numeric'},
            {'path':'.CONF_%d:FREQ'%(i),'type':'numeric'},
            {'path':'.CONF_%d:EV'%(i),'type':'text'},
            {'path':'.CONF_%d:TRIG'%(i),'type':'numeric'}
        ])
    parts.extend([
        {'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)}
    ])
    del(i)


    modeDict = {'CONTINUOUS':'STREAMING', 'SLOW':'TRIGGER_STREAMING', 'FAST':'TRIGGER_STREAMING'}

    def init(self):

        error = False
        for board in range(1,7):
       
            try:
                devPath = self.__getattr__('conf_%d_dev_path'%(board)).getData()
            except Exception as ex:
                emsg = 'Missing device Path for board %d : %s'%(board, str(ex))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                error = True
                continue

            try:
                mode = self.__getattr__('conf_%d_mode'%(board)).data()
            except:
                emsg = 'Missing device acquisition mode for conf %d'%(board)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                error = True
                continue

            try:
                self.getTree().getNode(devPath.getPath()+':MODE').putData(self.modeDict[mode]);
            except Exception as ex:
                emsg = 'Error on setting acquisition mode for conf %d : %s'%(board, str(ex))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                error = True
                continue


            if mode == 'CONTINUOUS' :

                try:
                    freq = self.__getattr__('conf_%d_cont_freq'%(board)).data()
                    self.__getattr__('conf_%d_freq'%(board)).putData(Float32(freq))
                except Exception as ex:
                    emsg = 'Error on updating continuous frequency for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    mdsEvent = self.__getattr__('conf_%d_cont_ev'%(board)).getData()
                    self.__getattr__('conf_%d_ev'%(board)).putData(mdsEvent)
                    self.getTree().getNode(devPath.getPath()+':MDS_TRIG_EV').putData(mdsEvent);
                except Exception as ex:
                    emsg = 'Error on updating continuous mds event for conf %d : %s'%(board,str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    trig = self.__getattr__('conf_%d_cont_trig'%(board)).data()
                    self.__getattr__('conf_%d_trig'%(board)).putData(Float32(trig))
                except Exception as ex:
                    emsg = 'Error on updating continuous trigger for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue


            elif mode == 'SLOW' :

                try:
                    freq = self.__getattr__('conf_%d_slow_freq'%(board)).data()
                    self.__getattr__('conf_%d_freq'%(board)).putData(Float32(freq))
                except Exception as ex:
                    emsg = 'Error on updating continuous frequency for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    mdsEvent = self.__getattr__('conf_%d_slow_ev'%(board)).getData()
                    self.__getattr__('conf_%d_ev'%(board)).putData(mdsEvent)
                    self.getTree().getNode(devPath.getPath()+':MDS_TRIG_EV').putData(mdsEvent);
                except Exception as ex:
                    emsg = 'Error on updating continuous mds event for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    trig = self.__getattr__('conf_%d_slow_trig'%(board)).data()
                    self.__getattr__('conf_%d_trig'%(board)).putData(Float32(trig))
                except Exception as ex:
                    emsg = 'Error on updating continuous trigger for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

            elif mode == 'FAST' :

                try:
                    freq = self.__getattr__('conf_%d_fast_freq'%(board)).data()
                    self.__getattr__('conf_%d_freq'%(board)).putData(Float32(freq))
                except Exception as ex:
                    emsg = 'Error on updating continuous frequency for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    mdsEvent = self.__getattr__('conf_%d_fast_ev'%(board)).getData()
                    self.__getattr__('conf_%d_ev'%(board)).putData(mdsEvent)
                    self.getTree().getNode(devPath.getPath()+':MDS_TRIG_EV').putData(mdsEvent);
                except Exception as ex:
                    emsg = 'Error on updating continuous mds event for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    trig = self.__getattr__('conf_%d_fast_trig'%(board)).data()
                    self.__getattr__('conf_%d_trig'%(board)).putData(Float32(trig))
                except Exception as ex:
                    emsg = 'Error on updating continuous trigger for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue
                
            else:
                emsg = 'Invalid acquisition mode %s for conf %d'%(mode, board)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                error = True
                continue


            if mode == 'SLOW' or mode == 'FAST' or  mode == 'CONTINUOUS':

                try:
                    startTime = self.__getattr__('conf_%d_start_time'%(board)).data()
                except Exception as ex:
                    emsg = 'Missing start time value for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    stopTime = self.__getattr__('conf_%d_stop_time'%(board)).data()
                except Exception as ex:
                    emsg = 'Missing stop time value for conf %d : %s'%(board, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue
                
                try:
                    preSmp = int( abs(startTime) * freq );
                    self.__getattr__('conf_%d_pre_smp'%(board)).putData(Int32(preSmp))
                except Exception as ex:
                    emsg = 'Error on updating pre samples value for conf %d in %s acquisition mode : %s'%(board, mode, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

                try:
                    postSmp = int( abs(stopTime) * freq );
                    self.__getattr__('conf_%d_post_smp'%(board)).putData(Int32(postSmp))
                except Exception as ex:
                    emsg = 'Error on updating post samples value for conf %d in %s acquisition mode : %s'%(board, mode, str(ex))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                    error = True
                    continue

        if error :
            raise DevBAD_PARAMETER

        return 1
