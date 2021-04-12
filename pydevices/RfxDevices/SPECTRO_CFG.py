from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Float32Array, Int16Array, StringArray
from ctypes import CDLL, cdll, memmove, byref, c_int, c_char, c_void_p, c_byte, c_float, c_char_p, c_long, c_longlong, c_ushort, Structure, POINTER, cast, sizeof
import numpy as np
import sys


class SPECTRO_CFG(Device):
    """SPIDER Spectroscopy Configuration Device"""
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':CONFIG_FILE', 'type': 'text'},
             {'path': ':CONFIG_DATE', 'type': 'text'},
             {'path': '.CONFIG_LOS', 'type': 'structure'},
             {'path': '.CONFIG_LOS:LABEL', 'type': 'text'},
             {'path': '.CONFIG_LOS:TELESCOPE', 'type': 'text'},
             {'path': '.CONFIG_LOS:C_PATCH', 'type': 'text'},
             {'path': '.CONFIG_LOS:B_PATCH', 'type': 'text'},
             {'path': '.CONFIG_LOS:OR_PANNEL', 'type': 'text'},
             {'path': '.CONFIG_LOS:A_PATCH', 'type': 'text'},
             {'path': '.CONFIG_LOS:DIAMETER', 'type': 'text'},
             {'path': '.CONFIG_LOS:DIAPHRAGM', 'type': 'text'},
             {'path': '.CONFIG_LOS:P0', 'type': 'text'},
             {'path': '.CONFIG_LOS:P1', 'type': 'text'},
             {'path': '.CONFIG_LOS:ROI', 'type': 'text'},
             {'path': '.CONFIG_LOS:SPECTROMETER', 'type': 'text'},
             {'path': '.CONFIG_LOS:NOTE', 'type': 'text'},
             {'path': '.CONFIG_LOS:CALIB_FLAG', 'type': 'text'}]

    for i in range(0, 180):
        parts.append({'path': '.LOS_%03d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.LOS_%03d:LABEL' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:TELESCOPE' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:C_PATCH' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:B_PATCH' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:OR_PANNEL' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:A_PATCH' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:DIAMETER' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.LOS_%03d:DIAPHRAGM' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.LOS_%03d:P0' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.LOS_%03d:P1' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.LOS_%03d:ROI' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.LOS_%03d:SPECTROMETER' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:NOTE' % (i+1), 'type': 'text'})
        parts.append({'path': '.LOS_%03d:CALIB_FLAG' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.LOS_%03d:SPECTRA_REF' % (i+1), 'type': 'signal'})

    for i in range(0, 180):
        parts.append(
            {'path': '.LOS_%03d:PHOTODIODE' % (i+1), 'type': 'signal'})

    del(i)

    class bcolors:
        HEADER = '\033[95m'
        OKBLUE = '\033[94m'
        OKCYAN = '\033[96m'
        OKGREEN = '\033[92m'
        WARNING = '\033[93m'
        FAIL = '\033[91m'
        ENDC = '\033[0m'
        BOLD = '\033[1m'
        UNDERLINE = '\033[4m'

    LOS_NUM = 180

    def getDevice(self, n):
        out = None
        if n.getUsage() == 'DEVICE':
            return n
        if n.getUsage() == 'SUBTREE':
            return None
        return self.getDevice(n.getParent())

    def getDeviceName(self, n):
        return self.getDevice(n).getData().getDescs()[1]

    def write_los_tag(self):
        """
        try :
            for i in range(0,self.LOS_NUM):
               try :
                    curr_tags = getattr(self, 'los_%03d'%(i+1)).getTags()
               except:
                    continue
               if curr_tags == None :
                    continue
               print 'curr LOS_%03d = %s'%(i+1, curr_tags)
               for curr_tag in curr_tags :  
                   print 'curr_tag', curr_tag
                   getattr(self, 'los_%03d'%(i+1)).removeTag(curr_tag)                            
        except Exception as e :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot remove LoS tag experiment must be open in Edit Mode ' + str(e) )
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """

        try:
            los_tags = self.config_los_label.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error reading LoS labels ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            for i in range(0, self.LOS_NUM):
                print ('LOS_%03d = @%s@ len %d' %
                       (i+1, los_tags[i].strip(), len(los_tags[i])))
                if "None" in los_tags[i] or (len(los_tags[i].strip()) == 0):
                    continue
                try:
                    """
                    curr_tags = getattr(self, 'los_%03d'%(i+1)).getTags()
                    print 'curr LOS_%03d = %s'%(i+1, curr_tags)                    
                    if los_tags[i] in curr_tags :
                        continue
                    """
                    print ('----- LOS_%03d = %s len %d' %
                           (i+1, los_tags[i], len(los_tags[i])))
                    getattr(self, 'los_%03d' %
                            (i+1)).addTags(los_tags[i].strip())
                except Exception as e:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Error writing label field on LoS %s ' % (i+1) + str(e))
                    raise mdsExceptions.TclFAILED_ESSENTIAL

        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot write LoS tag experiment must be open in Edit Mode ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1

    def load_config(self):

        print (sys.version_info)

        from openpyxl import load_workbook

        try:
            configurationFile = self.config_file.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot read configuration file' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            param = 'configurationFile'
            wb = load_workbook(configurationFile, data_only=True)

            sheet = wb['INFO']
            config_date = str(sheet['C1'].value)
            config_comment = str(sheet['C2'].value)

            sheet = wb['CONFIG']

            param = 'label_arr'
            # Set column range for all read column. Diect access, with value fild, on the first col value dosen't work
            col = sheet['B2:B%d' % (self.LOS_NUM+1)]
            col = sheet['B']
            label_arr = [str(c.value if c != None else "")
                         for c in col[1:len(col):1]]

            param = 'telescope_arr'
            col = sheet['C']
            telescope_arr = [str(c.value if c.value != None else "")
                             for c in col[1:len(col):1]]

            param = 'c_patch_arr'
            col = sheet['D']
            c_patch_arr = [str(c.value if c.value != None else "")
                           for c in col[1:len(col):1]]

            param = 'b_patch_arr'
            col = sheet['E']
            b_patch_arr = [str(c.value if c.value != None else "")
                           for c in col[1:len(col):1]]

            param = 'or_pannel_arr'
            col = sheet['F']
            or_pannel_arr = [str(c.value if c.value != None else "")
                             for c in col[1:len(col):1]]

            param = 'a_patch_arr'
            col = sheet['G']
            a_patch_arr = [str(c.value if c.value != None else "")
                           for c in col[1:len(col):1]]

            param = 'diameter_str_arr'
            col = sheet['H']
            diameter_arr = [float(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else -1) for c in col[1:len(col):1]]
            diameter_str_arr = [str(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else "") for c in col[1:len(col):1]]

            param = 'diaphragm_str_arr'
            col = sheet['I']
            diaphragm_arr = [float(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else -1) for c in col[1:len(col):1]]
            diaphragm_str_arr = [str(c.value if c.value != None and (type(
                c.value) == long or type(c.value) == float) else "") for c in col[1:len(col):1]]

            param = 'P0_str_arr'
            matrix = sheet['J2':'L%d' % (self.LOS_NUM+1)]
            data_arr = [c.value for c in np.asarray(
                matrix).reshape(3 * self.LOS_NUM)]
            P0_arr = np.array(data_arr, 'float32').reshape([self.LOS_NUM, 3])
            P0_str_arr = [str(c) for c in P0_arr]

            param = 'P1_str_arr'
            matrix = sheet['M2':'O%d' % (self.LOS_NUM+1)]
            data_arr = [c.value for c in np.asarray(
                matrix).reshape(3 * self.LOS_NUM)]
            P1_arr = np.array(data_arr, 'float32').reshape([self.LOS_NUM, 3])
            P1_str_arr = [str(c) for c in P1_arr]

            param = 'roi_str_arr'
            col = sheet['P']
            roi_arr = [int(str(c.value if c.value != None and (type(c.value) == long or type(
                c.value) == float) else -1)) for c in col[1:len(col):1]]
            roi_str_arr = [str(c.value if c.value != None and (type(c.value) == long or type(
                c.value) == float) else "") for c in col[1:len(col):1]]

            param = 'spec_arr'
            col = sheet['Q']
            spec_arr = [str(c.value if c.value != None else "")
                        for c in col[1:len(col):1]]

            param = 'note_arr'
            col = sheet['R']
            note_arr = [str(c.value if c.value != None else "")
                        for c in col[1:len(col):1]]

            param = 'calib_flag_arr'
            col = sheet['S']
            calib_flag_arr = [
                str(c.value if c.value != None else "false").lower() for c in col[1:len(col):1]]

            wb.close()

        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot read or invalid spectro param %s configuration file : %s ' % (param, str(e)))
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
            self.config_los_label.putData(StringArray(label_arr))
            field = 'telescope'
            self.config_los_telescope.putData(StringArray(telescope_arr))
            field = 'a_patch'
            self.config_los_a_patch.putData(StringArray(a_patch_arr))
            field = 'b_patch'
            self.config_los_b_patch.putData(StringArray(b_patch_arr))
            field = 'c_patch'
            self.config_los_c_patch.putData(StringArray(c_patch_arr))
            field = 'or_pannel'
            self.config_los_or_pannel.putData(StringArray(or_pannel_arr))
            field = 'diameter'
            self.config_los_diameter.putData(StringArray(diameter_str_arr))
            field = 'diaphragm'
            self.config_los_diaphragm.putData(StringArray(diaphragm_str_arr))
            field = 'P0'
            self.config_los_p0.putData(StringArray(P0_str_arr))
            field = 'P1'
            self.config_los_p1.putData(StringArray(P1_str_arr))
            field = 'roi'
            self.config_los_roi.putData(StringArray(roi_str_arr))
            field = 'spectrometer'
            self.config_los_spectrometer.putData(StringArray(spec_arr))
            field = 'note'
            self.config_los_note.putData(StringArray(note_arr))
            field = 'calib_flag'
            self.config_los_calib_flag.putData(StringArray(calib_flag_arr))
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error writing %s on LoS  : %s ' % (field, str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        spectrometer_list = []
        for i in range(0, self.LOS_NUM):

            try:
                getattr(self, 'los_%03d_label' % (i+1)).putData(label_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing label field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_telescope' %
                        (i+1)).putData(telescope_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing telescope field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_c_patch' %
                        (i+1)).putData(c_patch_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing C patch field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_b_patch' %
                        (i+1)).putData(b_patch_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing B patch field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_or_pannel' %
                        (i+1)).putData(or_pannel_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing OR pannel field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_a_patch' %
                        (i+1)).putData(a_patch_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing A patch field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_diameter' %
                        (i+1)).putData(diameter_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing diameter field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_diaphragm' %
                        (i+1)).putData(diaphragm_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing diaphragm field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_p0' %
                        (i+1)).putData(Float32Array(P0_arr[i]))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing start point P0 field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_p1' %
                        (i+1)).putData(Float32Array(P1_arr[i]))
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing end point P1 field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_roi' % (i+1)).putData(roi_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing roi field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_spectrometer' %
                        (i+1)).putData(spec_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing spectrometer field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_note' % (i+1)).putData(note_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing note field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                getattr(self, 'los_%03d_calib_flag' %
                        (i+1)).putData(calib_flag_arr[i])
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error writing calib flag field on LoS %s ' % (i+1) + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                if label_arr[i] == 'None':
                    continue

                if len(spec_arr[i].strip()) == 0:
                    print(self.bcolors.OKBLUE + 'Info LoS %d (%s) acquisition not configured ' %
                          (i+1, label_arr[i]) + self.bcolors.ENDC)
                    continue

                if self.getDeviceName(self.__getattr__('\\%s' % (spec_arr[i]))) == 'PI_SCT320':
                    # Set link LOS_xxx spectra to ROI_xx spectrometer
                    try:
                        # Reset current link
                        getattr(self, 'los_%03d_spectra_ref' %
                                (i+1)).deleteData()
                        if len(spec_arr[i]) > 0 and not (spec_arr[i] in spectrometer_list):
                            for r in range(1, 25):
                                self.__getattr__('\\%s.ROIS.ROI_%02d:LOS_PATH' % (
                                    spec_arr[i], r)).deleteData()
                            # Register spectrometer already reset
                            spectrometer_list.append(spec_arr[i])

                        if roi_arr[i] == -1:
                            continue

                        spectraRoiNode = self.__getattr__(
                            '\\%s.ROIS.ROI_%02d:SPECTRA' % (spec_arr[i], roi_arr[i]))
                        getattr(self, 'los_%03d_spectra_ref' %
                                (i+1)).putData(spectraRoiNode)
                        self.__getattr__('\\%s.ROIS.ROI_%02d:LOS_PATH' % (
                            spec_arr[i], roi_arr[i])).putData(label_arr[i])

                        print(self.bcolors.OKGREEN + 'Info LoS %d (%s) SPECTRO acquisition configured on Spectrometer %s Roi %d' %
                              (i+1, label_arr[i], spec_arr[i], roi_arr[i]) + self.bcolors.ENDC)

                    except Exception as e:
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting link from LoS %d (%s) to ROI %d on spectrometer %s : %s' % (
                            i+1, label_arr[i], roi_arr[i], spec_arr[i], str(e)))

                # spec_arr for photodiode is the Front End device BOARD
                elif self.getDeviceName(self.__getattr__('\\%s' % (spec_arr[i]))) == 'PLFE':
                    # Set link LOS_xxx photodiode to Front End
                    try:
                        # Reset photodiode acquisitio channel link
                        getattr(self, 'los_%03d_photodiode' %
                                (i+1)).deleteData()

                        # ROI for phothodiode is the Front End Board channel and adc channel coded as NXX
                        # where N is the BOARD channel 1 or 2 and XX is adc channel from 1 to 32
                        if roi_arr[i] == -1:
                            continue

                        # From FE board path retrives FE BOARD index form 1 to 6
                        feBoardIdx = int(spec_arr[i][-1:])
                        if feBoardIdx < 1 or feBoardIdx > 6:
                            raise Exception(
                                'Invalid board index, valid values from 1 to 6')
                        code = int(roi_arr[i])

                        feBoardCh = code/100  # ROI for front end identify the FE board channl 1 or 2
                        if feBoardCh != 1 and feBoardCh != 2:
                            raise Exception(
                                'Invalid board channel, valid values 1 or 2')

                        adcChan = code - feBoardCh*100
                        if adcChan < 1 or adcChan > 32:
                            raise Exception(
                                'Invalid adc channel number %d for adc device define in front end device, valid values are from 1 to 32' % (adcChan))

                        fePath = self.getDevice(self.__getattr__(
                            '\\%s' % (spec_arr[i]))).getMinPath()
                        adcPath = (self.__getattr__('%s:ADC' %
                                                    (fePath))).getData().getMinPath()

                        expr = Data.compile('%s.CHANNEL_%d:DATA / (%s.BOARD_%02d.SETUP.CHANNEL_%02d.WR:GAIN * (%s.BOARD_%02d.SETUP.CHANNEL_%02d.WR:TRANS == 0 ? 1 : 100))' % (
                            adcPath, adcChan, fePath, feBoardIdx, feBoardCh, fePath, feBoardIdx, feBoardCh))

                        getattr(self, 'los_%03d_photodiode' %
                                (i+1)).putData(expr)

                        print(self.bcolors.OKGREEN + 'Info LoS %d (%s) PHOTODIODE acquisition configured on Adc %s ch %d Front End %s Board %d Ch %d' %
                              (i+1, label_arr[i], adcPath, adcChan, fePath, feBoardIdx, feBoardCh) + self.bcolors.ENDC)

                    except Exception as e:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Error setting link from LoS %d (%s) to front end board %s : %s' % (i+1, label_arr[i], spec_arr[i], str(e)))
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Invalid definition of spectrometer/front-end device (%s) for LoS %d (%s)' % (spec_arr[i], i+1, label_arr[i]))
            except Exception as e:
                print (str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Error definition of spectrometer/front-end device (%s) for LoS %d (%s) : %s' % (spec_arr[i], i+1, label_arr[i], str(e)))
        return 1
