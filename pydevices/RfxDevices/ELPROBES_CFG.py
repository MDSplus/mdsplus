from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Float32Array, Int16Array, StringArray
from ctypes import CDLL, cdll, memmove, byref, c_int, c_char, c_void_p, c_byte, c_float, c_char_p, c_long, c_longlong, c_ushort, Structure, POINTER, cast, sizeof
import numpy as np
import sys
import traceback


class ELPROBES_CFG(Device):
    """SPIDER Electrostatic Probes Configuration Device"""
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':CONFIG_FILE', 'type': 'text'},
             {'path': ':CONFIG_DATE', 'type': 'text'},
             {'path': '.CFG_PROBES', 'type': 'structure'},
             {'path': '.CFG_PROBES:LABEL', 'type': 'text'},
             {'path': '.CFG_PROBES:FRONT_END', 'type': 'text'},
             {'path': '.CFG_PROBES:FRONT_END_CH', 'type': 'text'},
             {'path': '.CFG_PROBES:ADC_SLOW', 'type': 'text'},
             {'path': '.CFG_PROBES:ADC_SLOW_CON', 'type': 'text'},
             {'path': '.CFG_PROBES:ADC_FAST', 'type': 'text'},
             {'path': '.CFG_PROBES:CALIB_RV', 'type': 'text'},
             {'path': '.CFG_PROBES:CALIB_RI_H', 'type': 'text'},
             {'path': '.CFG_PROBES:CALIB_RI_L', 'type': 'text'},
             {'path': '.CFG_PROBES:NOTE', 'type': 'text'}]

    for i in range(0, 99):
        parts.append({'path': '.PROBE_%02d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.PROBE_%02d:LABEL' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:FRONT_END' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.PROBE_%02d:FRONT_END_CH' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.PROBE_%02d:ADC_SLOW' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.PROBE_%02d:ADC_SLOW_CON' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:ADC_FAST' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.PROBE_%02d:CALIB_RV' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.PROBE_%02d:CALIB_RI_H' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.PROBE_%02d:CALIB_RI_L' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.PROBE_%02d:NOTE' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.PROBE_%02d:BIAS_SOURCE' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:BIAS' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:IRANGE' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:DATA_FV' % (i+1), 'type': 'signal'})
        parts.append({'path': '.PROBE_%02d:HELP_FV' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:DATA_FI' % (i+1), 'type': 'signal'})
        parts.append({'path': '.PROBE_%02d:HELP_FI' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:DATA_SV' % (i+1), 'type': 'signal'})
        parts.append({'path': '.PROBE_%02d:HELP_SV' % (i+1), 'type': 'text'})
        parts.append({'path': '.PROBE_%02d:DATA_SI' % (i+1), 'type': 'signal'})
        parts.append({'path': '.PROBE_%02d:HELP_SI' % (i+1), 'type': 'text'})

    del(i)

    PROBES_NUM = 99

    def write_probe_tag(self):

        try:
            for i in range(0, self.PROBES_NUM):
                try:
                    curr_tags = getattr(self, 'probe_%02d' % (i+1)).getTags()
                except:
                    continue
                for curr_tag in curr_tags:
                    getattr(self, 'probe_%02d' % (i+1)).removeTag(curr_tag)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot remove probes tag, experiment must be open in Edit Mode ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            probes_tag = self.cfg_probes_label.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error reading probes labels ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        for i in range(0, self.PROBES_NUM):
            if "None" in probes_tag[i] or (len(probes_tag[i].strip()) == 0):
                continue
            try:
                tag = probes_tag[i].strip()+'_CFG'
                getattr(self, 'probe_%02d' % (i+1)).addTags(tag)
                print ('PROBE_%02d = %s' % (i+1, tag))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot write tag to probe %d. Experiment must be open in Edit Mode : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1

    def __setNodeState(self, path, state):
        self.__getattr__(path).setOn(state)
        if state:
            self.__getattr__(path+':STATE').putData("ENABLED")
        else:
            self.__getattr__(path+':STATE').putData("DISABLED")
        #print path+' OFF'
        #print path+':DISABLED -- DISABLED'

    def load_config(self):

        #print sys.version_info

        from openpyxl import load_workbook

        try:
            configurationFile = self.config_file.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot read configuration file' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            print ('Loading data from excel configuration file [%s] ...' % (
                configurationFile),)

            param = 'configurationFile'
            wb = load_workbook(configurationFile, data_only=True)

            sheet = wb['INFO']
            config_date = str(sheet['C1'].value)
            config_comment = str(sheet['C2'].value)

            sheet = wb['CONFIG']

            param = 'label_arr'
            # Set column range for all read column. Diect access, with value fild, on the first col value dosen't work
            col = sheet['B2:B%d' % (self.PROBES_NUM+1)]
            col = sheet['B']
            label_arr = [str(c.value if c != None else "")
                         for c in col[1:len(col):1]]

            param = 'front_end_arr'
            col = sheet['C']
            front_end_arr = [str(c.value if c.value != None else "")
                             for c in col[1:len(col):1]]

            param = 'front_end_ch_arr'
            col = sheet['D']
            front_end_ch_arr = [str(c.value if c.value != None else "")
                                for c in col[1:len(col):1]]

            param = 'adc_slow_arr'
            col = sheet['E']
            adc_slow_arr = [str(c.value if c.value != None else "")
                            for c in col[1:len(col):1]]

            param = 'adc_slow_con_arr'
            col = sheet['F']
            adc_slow_con_arr = [str(c.value if c.value != None else "")
                                for c in col[1:len(col):1]]

            param = 'adc_fast_arr'
            col = sheet['G']
            adc_fast_arr = [str(c.value if c.value != None else "")
                            for c in col[1:len(col):1]]

            param = 'calib_RV_arr'
            col = sheet['H']
            calib_RV_arr = [float(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else -1) for c in col[1:len(col):1]]
            calib_RV_str_arr = [str(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else "") for c in col[1:len(col):1]]

            param = 'calib_RI_high_arr'
            col = sheet['I']
            calib_RI_high_arr = [float(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else -1) for c in col[1:len(col):1]]
            calib_RI_high_str_arr = [str(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else "") for c in col[1:len(col):1]]

            param = 'calib_RI_low_arr'
            col = sheet['J']
            calib_RI_low_arr = [float(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else -1) for c in col[1:len(col):1]]
            calib_RI_low_str_arr = [str(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else "") for c in col[1:len(col):1]]

            param = 'note_arr'
            col = sheet['K']
            note_arr = [str(c.value if c.value != None else "")
                        for c in col[1:len(col):1]]

            wb.close()

            print (' data loaded')

        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot read or invalid probes param : [%s] configuration file : %s ' % (param, str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.comment.putData(config_comment)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error writing comment field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.config_date.putData(config_date)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error writing configuration date field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            field = 'label'
            self.cfg_probes_label.putData(StringArray(label_arr))
            field = 'front_end'
            self.cfg_probes_front_end.putData(StringArray(front_end_arr))
            field = 'front_end_ch'
            self.cfg_probes_front_end_ch.putData(StringArray(front_end_ch_arr))
            field = 'adc_slow'
            self.cfg_probes_adc_slow.putData(StringArray(adc_slow_arr))
            field = 'adc_slow_con'
            self.cfg_probes_adc_slow_con.putData(StringArray(adc_slow_con_arr))
            field = 'adc_fast'
            self.cfg_probes_adc_fast.putData(StringArray(adc_fast_arr))
            field = 'calib_RV'
            self.cfg_probes_calib_rv.putData(StringArray(calib_RV_str_arr))
            field = 'calib_RI_high'
            self.cfg_probes_calib_ri_h.putData(
                StringArray(calib_RI_high_str_arr))
            field = 'calib_RI_low'
            self.cfg_probes_calib_ri_l.putData(
                StringArray(calib_RI_low_str_arr))
            field = 'note'
            self.cfg_probes_note.putData(StringArray(note_arr))
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error writing param %s probes : %s ' % (field, str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Set off oll channel of the ADC module fast and slow defined in the configuration.
        # ADC Channel is set ON if it is used. Thi is done to not acquire not usefull signals

        adcSet = []
        for adc in adc_slow_arr:
            adc = adc.strip()
            if len(adc) == 0:
                continue
            if not (adc in adcSet):
                adcSet.append(adc)
                for i in range(1, 33):
                    self.__setNodeState('\\%s.CHANNEL_%d' % (adc, i), False)
        del adcSet

        adcSet = []
        for adc in adc_fast_arr:
            adc = adc.strip()
            if len(adc) == 0:
                continue
            if not (adc in adcSet):
                adcSet.append(adc)
                for i in range(1, 17):
                    self.__setNodeState('\\%s.CHANNEL_%d' % (adc, i), False)

        for i in range(0, self.PROBES_NUM):

            if len(label_arr[i].strip()) == 0 or label_arr[i].strip() == 'None':
                continue

            print ('[%0.2d] Configuration for probe %s : ' %
                   (i+1, label_arr[i]),)

            #print "Save probes %d %s"%(i+1, label_arr[i])

            try:
                getattr(self, 'probe_%02d_label' % (i+1)).putData(label_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing label field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_note' % (i+1)).putData(note_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing note field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            if len(front_end_arr[i].strip()) == 0 or front_end_arr[i].strip() == 'None':
                #print label_arr[i] + " probe reset data and set off"
                getattr(self, 'probe_%02d' % (i+1)).setOn(False)
                getattr(self, 'probe_%02d_front_end' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_front_end_ch' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_adc_slow' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_adc_slow_con' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_adc_fast' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_calib_rv' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_calib_ri_h' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_calib_ri_l' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_bias_source' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_bias' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_irange' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_data_sv' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_help_sv' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_data_si' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_help_si' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_data_fv' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_help_fv' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_data_fi' % (i+1)).deleteData()
                getattr(self, 'probe_%02d_help_fi' % (i+1)).deleteData()

                try:
                    self.__getattr__('\\%sSV' % (label_arr[i])).setOn(False)
                    self.__getattr__('\\%sSI' % (label_arr[i])).setOn(False)
                    self.__getattr__('\\%sFV' % (label_arr[i])).setOn(False)
                    self.__getattr__('\\%sFI' % (label_arr[i])).setOn(False)
                except Exception:
                    print ('WARNING : The configuration excel file added the probe %s.\nThe updateSignalsProbes command MUST be execute' % (
                        label_arr[i]))

                print ('[set OFF]')

                continue
            else:
                print ('[set ON]')
                getattr(self, 'probe_%02d' % (i+1)).setOn(True)

            try:
                getattr(self, 'probe_%02d_front_end' %
                        (i+1)).putData(front_end_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing front end field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_front_end_ch' %
                        (i+1)).putData(Int32(int(front_end_ch_arr[i])))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), '---Error writing front end channel field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_adc_slow' %
                        (i+1)).putData(adc_slow_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing adc slow field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                if adc_slow_con_arr[i] == "CON_0" or adc_slow_con_arr[i] == "CON_1":
                    getattr(self, 'probe_%02d_adc_slow_con' %
                            (i+1)).putData(adc_slow_con_arr[i])
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Invalid value on adc slow connection field on probe %d, valid values CON_0, CON 1 ' % (i+1))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing adc slow connection field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_adc_fast' %
                        (i+1)).putData(adc_fast_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing adc fast field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_calib_rv' %
                        (i+1)).putData(Float32(calib_RV_arr[i]))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing resistence voltage calibration field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_calib_ri_h' %
                        (i+1)).putData(Float32(calib_RI_high_arr[i]))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing high current resistence calibration field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'probe_%02d_calib_ri_l' %
                        (i+1)).putData(Float32(calib_RI_low_arr[i]))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing low current resistence calibration field on probe %d : %s ' % (i+1, str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                # Reset current link

                node = self.__getattr__('\\%s.CH_%02d:SOURCE' % (
                    front_end_arr[i], int(front_end_ch_arr[i])))
                expr_val = Data.compile('build_path($)', node.getFullPath())
                getattr(self, 'probe_%02d_bias_source' %
                        (i+1)).putData(expr_val)

                """
               node = self.__getattr__('\\%s.CH_%02d:BIAS'%(front_end_arr[i], int( front_end_ch_arr[i])));
	       expr_val = Data.compile('build_path($)', node.getFullPath())	
               getattr(self, 'probe_%02d_bias'%(i+1)).putData( expr_val ) 
               """

                node = self.__getattr__('\\%s.CH_%02d:IRANGE' % (
                    front_end_arr[i], int(front_end_ch_arr[i])))
                expr_val = Data.compile('build_path($)', node.getFullPath())
                irange_node = getattr(self, 'probe_%02d_irange' % (i+1))
                irange_node.putData(expr_val)

                calib_rv_node = getattr(self, 'probe_%02d_calib_rv' % (i+1))
                calib_ri_high_node = getattr(
                    self, 'probe_%02d_calib_ri_h' % (i+1))
                calib_ri_low_node = getattr(
                    self, 'probe_%02d_calib_ri_l' % (i+1))

                if len(adc_slow_arr[i].strip()) != 0 and adc_slow_arr[i].strip() != 'None':

                    if adc_slow_con_arr[i] == "CON_0":
                        adc_slow_i_node = self.__getattr__('\\%s.CHANNEL_%d:DATA' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])))
                        self.__setNodeState('\\%s.CHANNEL_%d' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])), True)
                        adc_slow_v_node = self.__getattr__('\\%s.CHANNEL_%d:DATA' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])+8))
                        self.__setNodeState('\\%s.CHANNEL_%d' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])+8), True)
                    elif adc_slow_con_arr[i] == "CON_1":
                        adc_slow_i_node = self.__getattr__('\\%s.CHANNEL_%d:DATA' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])+16))
                        self.__setNodeState('\\%s.CHANNEL_%d' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])+16), True)
                        adc_slow_v_node = self.__getattr__('\\%s.CHANNEL_%d:DATA' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])+24))
                        self.__setNodeState('\\%s.CHANNEL_%d' % (
                            adc_slow_arr[i], int(front_end_ch_arr[i])+24), True)
                    else:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Error invalid slow adc commector for probe %d (%s)' % (i+1, label_arr[i]))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    expr_slow_v = Data.compile(
                        'build_path($) *  build_path($)', calib_rv_node.getFullPath(), adc_slow_v_node.getFullPath())
                    expr_slow_i = Data.compile("(build_path($) == 'LOW'  ?  build_path($) : build_path($) ) *  Build_path($)", irange_node.getFullPath(
                    ), calib_ri_low_node.getFullPath(), calib_ri_high_node.getFullPath(), adc_slow_i_node.getFullPath())

                    getattr(self, 'probe_%02d_data_sv' %
                            (i+1)).putData(expr_slow_v)
                    getattr(self, 'probe_%02d_help_sv' % (
                        i+1)).putData('%s slow acquisition voltage' % (label_arr[i]))

                    getattr(self, 'probe_%02d_data_si' %
                            (i+1)).putData(expr_slow_i)
                    getattr(self, 'probe_%02d_help_si' % (
                        i+1)).putData('%s slow acquisition current' % (label_arr[i]))

                    getattr(self, 'probe_%02d_data_sv' % (i+1)).setOn(True)
                    getattr(self, 'probe_%02d_data_si' % (i+1)).setOn(True)

                    print ('[slow adc %s %s]' %
                           (adc_slow_arr[i], adc_slow_con_arr[i]),)

                    try:
                        self.__getattr__('\\%sSV' % (label_arr[i])).setOn(True)
                        self.__getattr__('\\%sSI' % (label_arr[i])).setOn(True)
                    except Exception:
                        print ('\nWARNING : The configuration excel file added the probe %s.\nThe updateSignalsProbes command MUST be execute' % (
                            label_arr[i]))

                else:
                    print (' ')
                    getattr(self, 'probe_%02d_data_sv' % (i+1)).deleteData()
                    getattr(self, 'probe_%02d_data_sv' % (i+1)).setOn(False)
                    getattr(self, 'probe_%02d_data_si' % (i+1)).deleteData()
                    getattr(self, 'probe_%02d_data_si' % (i+1)).setOn(False)
                    try:
                        self.__getattr__('\\%sSV' %
                                         (label_arr[i])).setOn(False)
                        self.__getattr__('\\%sSI' %
                                         (label_arr[i])).setOn(False)
                    except Exception:
                        print ('\nWARNING : The configuration excel file added the probe %s.\nThe updateSignalsProbes command MUST be execute' % (
                            label_arr[i]))

                if len(adc_fast_arr[i].strip()) != 0 and adc_fast_arr[i].strip() != 'None':

                    getattr(self, 'probe_%02d_data_fv' % (i+1)).setOn(True)
                    getattr(self, 'probe_%02d_data_fi' % (i+1)).setOn(True)

                    adc_fast_i_node = self.__getattr__('\\%s.CHANNEL_%d:DATA' % (
                        adc_fast_arr[i], int(front_end_ch_arr[i])))
                    self.__setNodeState('\\%s.CHANNEL_%d' % (
                        adc_fast_arr[i], int(front_end_ch_arr[i])), True)
                    adc_fast_v_node = self.__getattr__('\\%s.CHANNEL_%d:DATA' % (
                        adc_fast_arr[i], int(front_end_ch_arr[i])+8))
                    self.__setNodeState('\\%s.CHANNEL_%d' % (
                        adc_fast_arr[i], int(front_end_ch_arr[i])+8), True)

                    expr_fast_v = Data.compile(
                        'build_path($) *  build_path($)', calib_rv_node.getFullPath(), adc_fast_v_node.getFullPath())
                    expr_fast_i = Data.compile("(build_path($) == 'LOW'  ?  build_path($) : build_path($) ) *  Build_path($)", irange_node.getFullPath(
                    ), calib_ri_low_node.getFullPath(), calib_ri_high_node.getFullPath(), adc_fast_i_node.getFullPath())

                    getattr(self, 'probe_%02d_data_fv' %
                            (i+1)).putData(expr_fast_v)
                    getattr(self, 'probe_%02d_help_fv' % (
                        i+1)).putData('%s fast acquisition voltage' % (label_arr[i]))

                    getattr(self, 'probe_%02d_data_fi' %
                            (i+1)).putData(expr_fast_i)
                    getattr(self, 'probe_%02d_help_fi' % (
                        i+1)).putData('%s fast acquisition current' % (label_arr[i]))

                    print ('[fast adc %s]' % (adc_fast_arr[i]))

                    try:
                        self.__getattr__('\\%sFV' % (label_arr[i])).setOn(True)
                        self.__getattr__('\\%sFI' % (label_arr[i])).setOn(True)
                    except Exception:
                        print ('\nWARNING : The configuration excel file added the probe %s.\nThe updateSignalsProbes command MUST be execute' % (
                            label_arr[i]))

                else:
                    print (' ')
                    getattr(self, 'probe_%02d_data_fv' % (i+1)).deleteData()
                    getattr(self, 'probe_%02d_data_fv' % (i+1)).setOn(False)
                    getattr(self, 'probe_%02d_data_fi' % (i+1)).deleteData()
                    getattr(self, 'probe_%02d_data_fi' % (i+1)).setOn(False)
                    try:
                        self.__getattr__('\\%sFV' %
                                         (label_arr[i])).setOn(False)
                        self.__getattr__('\\%sFI' %
                                         (label_arr[i])).setOn(False)
                    except Exception:
                        print ('WARNING : The configuration excel file added the probe %s.\nThe updateSignalsProbes command MUST be execute' % (
                            label_arr[i]))

            except Exception as e:
                traceback.print_exc()
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting link for probe %d (%s) to Front End %s ch %s : %s' % (
                    i+1, label_arr[i], front_end_arr[i], front_end_ch_arr[i], str(e)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1
