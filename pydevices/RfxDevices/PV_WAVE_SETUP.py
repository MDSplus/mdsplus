from MDSplus import mdsExceptions, Device, Data


class PV_WAVE_SETUP(Device):
    """Epics Waveform Configurator"""

    parts = [{'path': ':BOARD_ID', 'type': 'numeric', 'value': 0},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':TRIG_SOURCE', 'type': 'numeric', 'value': 0},
             {'path': ':PV_TRIG_NAME', 'type': 'text'}]

    for i in range(0, 8):
        parts.append({'path': '.WAVE_%d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.WAVE_%d:NAME' % (i+1), 'type': 'text'})
        parts.append({'path': '.WAVE_%d:X' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.WAVE_%d:Y' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.WAVE_%d:PV_X' % (i+1), 'type': 'text'})
        parts.append({'path': '.WAVE_%d:PV_Y' % (i+1), 'type': 'text'})
    del (i)

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('EPICS_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': ':RESET_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('EPICS_SERVER','FINISH_SHOT',50,None),Method(None,'reset',head))",
                  'options': ('no_write_shot',)})

    def init(self):
        print ('================= INIT EPICS WAVE SETUP ===============')
        from CaChannel import CaChannel

        wavex = CaChannel()
        wavey = CaChannel()
        trig = CaChannel()

        try:
            pv_trig_name = self.pv_trig_name.data()
            trig.searchw(pv_trig_name)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error on PV trigger variable')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # trig.searchw('NB-SIGV-GISA:REF-TR_TIME')

        try:
            trig_data = self.trig_source.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Trigger time')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        #print('Trigger ', trig_data)

        trig.putw(float(trig_data))

        for chan in range(1, 8):

            # wavex.searchw('NB-SIGV-GISA:PZ%d-WAVEX'%(chan))
            # wavey.searchw('NB-SIGV-GISA:PZ%d-WAVEY'%(chan))

            if getattr(self, 'wave_%d' % (chan)).isOn():

                try:
                    pv_wave_x = getattr(self, 'wave_%d_pv_x' % (chan)).data()
                    wavex.searchw(pv_wave_x)
                except:
                    msg = 'Error on PV waveform %d x variable' % (chan)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), msg)
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                try:
                    wavex_data = getattr(self, 'wave_%d_x' % (chan)).data()
                except:
                    msg = 'Error reading x waveform %d values' % (chan)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), msg)
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                try:
                    pv_wave_y = getattr(self, 'wave_%d_pv_y' % (chan)).data()
                    wavey.searchw(pv_wave_y)
                except:
                    msg = 'Error reading y waveform %d values' % (chan)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), msg)
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                try:
                    wavey_data = getattr(self, 'wave_%d_y' % (chan)).data()
                except:
                    msg = 'Error reading y waveform %d values' % (chan)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), msg)
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                print('CH %d' % (chan))
                print('X = ', wavex_data)
                print('Y = ', wavey_data)

                wavex.putw(wavex_data)
                wavey.putw(wavey_data)

        del wavex
        del wavey
        del trig
        print ('=======================================================')

        return 1

    def reset(self):
        print ('================= RESET EPICS WAVE SETUP ===============')
        from CaChannel import CaChannel

        wavex = CaChannel()
        wavey = CaChannel()

        for chan in range(1, 3):

            if getattr(self, 'wave_%d' % (chan)).isOn():

                try:
                    pv_wave_x = getattr(self, 'wave_%d_pv_x' % (chan)).data()
                    wavex.searchw(pv_wave_x)
                except:
                    msg = 'Error on PV waveform %d x variable' % (chan)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), msg)
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                try:
                    pv_wave_y = getattr(self, 'wave_%d_pv_y' % (chan)).data()
                    wavey.searchw(pv_wave_y)
                except:
                    msg = 'Error reading y waveform %d values' % (chan)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), msg)
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                wavex.putw([0, 0])
                wavey.putw([0, 0])

        del wavex
        del wavey
        print ('========================================================')

        return 1
