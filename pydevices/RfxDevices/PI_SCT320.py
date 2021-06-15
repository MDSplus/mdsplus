from MDSplus import mdsExceptions, Device, Data, Float32, Signal
import numpy as np
import time


class PI_SCT320(Device):
    """Princeton Instrument ISO PLANE SCT-320 spectrograph"""
    parts = [{'path': ':SERIAL_PORT', 'type': 'text'},
             {'path': ':MODEL_NAME', 'type': 'text'},
             {'path': ':SERIAL_NUM', 'type': 'numeric', 'value': 0},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':DETECTOR', 'type': 'numeric'},
             {'path': ':LAMBDA_0', 'type': 'numeric', 'value': 50},
             {'path': ':TURRET', 'type': 'numeric', 'value': 1},
             {'path': ':GRATING', 'type': 'numeric', 'value': 1},
             {'path': ':FOCAL', 'type': 'numeric', 'value': 1},
             {'path': ':SLIT', 'type': 'text', 'value': "SLIT1"},
             {'path': ':SLIT1_WIDTH', 'type': 'numeric', 'value': 50},
             {'path': ':SLIT2_WIDTH', 'type': 'numeric', 'value': 50},
             {'path': ':SHUTTER', 'type': 'text', 'value': 'SPECTROMETER'},
             {'path': ':SHUT_TIME', 'type': 'numeric', 'value': 1},
             {'path': ':INTENS_CAL', 'type': 'text', 'value': 'NONE'},
             {'path': '.FIBERS', 'type': 'structure'},
             {'path': '.FIBERS:NUM', 'type': 'numeric', 'value': 22},
             {'path': '.FIBERS:LENGHT', 'type': 'numeric', 'value': 40},
             {'path': '.FIBERS:CORE', 'type': 'numeric', 'value': 400},
             {'path': '.FIBERS:MATERIAL', 'type': 'text', 'value': 'QUARZ'},
             {'path': '.SENSOR', 'type': 'structure'},
             {'path': '.SENSOR:PIXEL_HEIGHT', 'type': 'numeric'},
             {'path': '.SENSOR:PIXEL_WIDTH', 'type': 'numeric'},
             {'path': '.SENSOR:HEIGHT', 'type': 'numeric'},
             {'path': '.SENSOR:WIDTH', 'type': 'numeric'}]

    parts.append({'path': '.GRATINGS', 'type': 'structure'})
    for i in range(0, 9):
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d' % (i+1), 'type': 'structure'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:GROOVE' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:BLAZE' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:MODEL' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:OFFSET' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:ADJUST' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:FOCAL' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:INCLUSION' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:DETECTOR' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:DISPERSION' % (i+1), 'type': 'signal'})
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:LAMBDA_NM' % (i+1), 'type': 'numeric'})

    parts.append({'path': '.CALIBRATION', 'type': 'structure'})
    parts.append({'path': '.CALIBRATION:FILE_PATH', 'type': 'text'})
    parts.append({'path': '.CALIBRATION.GEOMETRY', 'type': 'structure'})
    parts.append({'path': '.CALIBRATION.GEOMETRY:DATE', 'type': 'text'})
    parts.append({'path': '.CALIBRATION.GEOMETRY:COMMENT', 'type': 'text'})
    parts.append({'path': '.CALIBRATION.GEOMETRY:FWHM', 'type': 'numeric'})
    parts.append({'path': '.CALIBRATION.GEOMETRY:OFFSET', 'type': 'numeric'})
    parts.append(
        {'path': '.CALIBRATION.GEOMETRY:GRATING_DISP', 'type': 'numeric'})
    parts.append({'path': '.CALIBRATION.INTENSITY', 'type': 'structure'})
    parts.append({'path': '.CALIBRATION.INTENSITY:DATE', 'type': 'text'})
    parts.append({'path': '.CALIBRATION.INTENSITY:COMMENT', 'type': 'text'})
    parts.append({'path': '.CALIBRATION.INTENSITY:COEFF', 'type': 'numeric'})

    parts.append({'path': '.ROIS', 'type': 'structure'})
    for i in range(0, 24):
        parts.append({'path': '.ROIS.ROI_%02d' % (i+1), 'type': 'structure'})
        parts.append(
            {'path': '.ROIS.ROI_%02d:LOS_PATH' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.ROIS.ROI_%02d:SPECTRA' % (i+1), 'type': 'signal'})

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('SPECTRO_SERVER','INIT',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': '.GRATINGS:DATE', 'type': 'text'})
    parts.append({'path': '.GRATINGS:COMMENT', 'type': 'text'})

    parts.append({'path': ':CTRL_MODE', 'type': 'text', 'value': 'LOCAL'})
    parts.append({'path': ':CTRL_SERVER', 'type': 'text'})

    for i in range(0, 9):
        parts.append(
            {'path': '.GRATINGS.GRATING_%02d:FOCAL_POS' % (i+1), 'type': 'numeric'})

    del(i)

    SCT_320_OK = 1
    SCT_320_ERROR = -1
    valueList = [['model_name', 0, 0, 'text'],
                 ['serial_num', 1, 2, 'int'],
                 ['gratings_grating_01_groove', 5, 1, 'int'],
                 ['gratings_grating_01_blaze',  5, 4, 'text'],
                 ['gratings_grating_01_offset', 15, 1, 'int'],
                 ['gratings_grating_01_adjust', 16, 1, 'int'],
                 ['gratings_grating_02_groove', 6, 1, 'int'],
                 ['gratings_grating_02_blaze',  6, 4, 'text'],
                 ['gratings_grating_02_offset', 15, 2, 'int'],
                 ['gratings_grating_02_adjust', 16, 2, 'int'],
                 ['gratings_grating_03_groove', 7, 1, 'int'],
                 ['gratings_grating_03_blaze',  7, 4, 'text'],
                 ['gratings_grating_03_offset', 15, 3, 'int'],
                 ['gratings_grating_03_adjust', 16, 3, 'int']]

    ROI_NUM = 24
    GRATING_NUM = 3
    SLIT_NUM = 2
    WIDTH_NUM = 3

    def sendCommand(self, dev, cmd, out):
        dev.write(cmd+'\r')
        time.sleep(1)
        count = 0
        while True:
            answer = dev.read(5000)
            for line in answer.splitlines():
                line = line.strip()
                #for ch in bytearray(line): print hex(ch),
                ls = line.split()
                if len(ls) == 0:
                    continue
                if len(ls) > 0 and ls[len(ls) - 1] == 'ok':
                    ls = ls[:len(ls) - 1]
                    if len(ls) > 0:
                        out.append(ls)
                    return self.SCT_320_OK
                if line.find('?') != -1:
                    print('Unknow command')
                    out.append('Unknow command')
                    return self.SCT_320_ERROR
                #print "-%02d-%s"%(count,ls)
                out.append(ls)
                count = count+1

    def setParameter(self, dev, value, param):
        out = []
        print ('setParameter ', param,  value)
        status = self.sendCommand(dev, '%d %s' % (value, param), out)
        print ('Parameter configured ',  status)
        return status

    def getParameter(self, dev, param, out=[]):
        print ('getParameter ', param)
        status = self.sendCommand(dev, '?'+param, out)
        print ('getParameter ', param, out)
        return status

    def computeLambda(self, grating):
        try:
            print ('OK', grating)
            G = float(getattr(self, 'gratings_grating_%02d_groove' %
                              (grating)).data())  # ;grooving reticolo gr/mm
            print ('G', G)
            # ; inclusion angle gamma=18.5*!dtor
            gamma = np.pi / 180. * \
                getattr(self, 'gratings_grating_%02d_inclusion' %
                        (grating)).data()
            print ('gamma', gamma)
            # ; detector angle 8.77*!dtor
            delta = np.pi / 180. * \
                float(getattr(self, 'gratings_grating_%02d_detector' %
                              (grating)).data())
            print ('delta', delta)
            f = getattr(self, 'gratings_grating_%02d_focal' %
                        (grating)).data()  # ; focal length [mm]
            print ('f', f)
            # ; pixel size (width) [micron]
            p_size = self.sensor_pixel_width.data()
            print ('p_size', p_size)
            n_pix = float(self.sensor_width.data())  # ; xdimDet
            print ('n_pix', n_pix)
            lambda_0 = self.lambda_0.data() * 10  # ; (A)
            print ('lambda_0', lambda_0)
            d = 1./G  # ;mm
            print ('d', d)

            x_max = n_pix*p_size/1000.  # ;mm chip size
            print ('x_max', x_max)
            m = 1  # ;diffraction order
            psi = np.arcsin(m*lambda_0*1E-7/2/d/np.cos(gamma/2.))
            print ('psi xx', psi)
            # ;x=(findgen(n_pix)+1)/n_pix*x_max-x_max/2.
            x = (np.arange(n_pix) + 1)/n_pix*x_max-x_max/2.
            print ('x', x, np.cos(delta))
            # csi=atan(x/f)                                                                    #;vale se delta e' piccolo
            csi = np.arctan(x*np.cos(delta)/(f+x*np.sin(delta)))
            print ('csi', csi)

            lambda_arr = d/m*2*(np.sin(psi+csi/2)*np.cos(gamma/2+csi/2))*1E7
            print ('lambda_arr', len(lambda_arr), lambda_arr)
            getattr(self, 'gratings_grating_%02d_lambda_nm' %
                    (grating)).putData(lambda_arr)

        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot compute lambda axis for grating %d : %s' % (grating, str(e)))

    def load_calib(self):

        from openpyxl import load_workbook

        gratingsNodeList = [['gratings_grating_%02d_groove', 'float'],  # <node>, <type>, <values>
                            ['gratings_grating_%02d_blaze',  'text'],
                            ['gratings_grating_%02d_model', 'text'],
                            ['gratings_grating_%02d_offset', 'int'],
                            ['gratings_grating_%02d_adjust',  'int'],
                            ['gratings_grating_%02d_focal', 'float'],
                            ['gratings_grating_%02d_inclusion', 'float'],
                            ['gratings_grating_%02d_detector', 'float'],
                            ['gratings_grating_%02d_focal_pos', 'float'],
                            ['gratings_grating_%02d_dispersion', 'signal']]  # <node>, <signal>, <x values, y values>

        try:
            print ("Load Calibration File ", self.calibration_file_path.data())
            spectroCalibFile = self.calibration_file_path.data()
            wb = load_workbook(spectroCalibFile, data_only=True)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot load geometry calibration file  %s %s' % (spectroCalibFile, str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:

            param = 'INFO'
            sheet = wb['INFO']
            cal_geometry_date = str(sheet['C1'].value)
            cal_geometry_comment = str(sheet['C2'].value)
            cal_dispersion_fit_points = int(sheet['C3'].value)

            cal_intensity_date = str(sheet['C4'].value)
            cal_intensity_comment = str(sheet['C5'].value)
            cal_intensity_degree = int(sheet['C6'].value)
            cal_intensity_type = str(sheet['C7'].value).upper()

            cal_gratings_date = str(sheet['C8'].value)
            cal_gratings_comment = str(sheet['C9'].value)

            param = 'GRATINGS'
            sheet = wb['GRATINGS']
            col = sheet['B']
            geoove_arr = [float(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[0].append(geoove_arr)

            col = sheet['C']
            blaze_arr = [str(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[1].append(blaze_arr)

            col = sheet['D']
            model_arr = [str(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[2].append(model_arr)

            col = sheet['E']
            offset_arr = [int(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[3].append(offset_arr)

            col = sheet['F']
            adjust_arr = [int(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[4].append(adjust_arr)

            col = sheet['G']
            focal_arr = [float(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[5].append(focal_arr)

            col = sheet['H']
            inclusion_arr = [float(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[6].append(inclusion_arr)

            col = sheet['I']
            detector_arr = [float(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[7].append(detector_arr)

            col = sheet['J']
            flocal_pos_arr = [float(c.value) for c in col[1:len(col):1]]
            gratingsNodeList[8].append(flocal_pos_arr)

            param = 'FWHM'
            sheet = wb['FWHM']
            col = sheet['E']
            fwhm_arr = [c.value for c in col[1:len(col):1]]

            param = 'OFFSET'
            sheet = wb['OFFSET']
            col = sheet['E']
            offset_arr = [c.value for c in col[1:len(col):1]]

            param = 'DISPERSION'
            sheet = wb['DISPERSION']
            matrix = sheet['C2':chr(
                ord('C') + cal_dispersion_fit_points - 1)+'7']
            grating_disp_arr = [c.value for c in np.asarray(
                matrix).reshape(cal_dispersion_fit_points * 2*3)]
            gratingsNodeList[9].append(Float32(np.array(
                grating_disp_arr, 'float32').reshape([2*3, cal_dispersion_fit_points])))

            param = 'INTENSITY'
            sheet = wb['INTENSITY']
            matrix = sheet['E2':chr(ord('E') + cal_intensity_degree)+'433']
            intensity_coeff_arr = [c.value for c in np.asarray(matrix).reshape(
                self.ROI_NUM*self.GRATING_NUM*self.SLIT_NUM*self.WIDTH_NUM*(cal_intensity_degree+1))]

            wb.close()

        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot read or invalid format on sheet %s geometry calibration file %s' % (param, str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.calibration_geometry_comment.putData(cal_geometry_comment)
        except Exception as e:
            del cal_geometry_comment
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing geometry calibration comment field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.calibration_geometry_date.putData(cal_geometry_date)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing geometry calibration date field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.calibration_intensity_date.putData(cal_intensity_date)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing intensity calibration date field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.calibration_intensity_comment.putData(cal_intensity_comment)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing intensity calibration comment field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            self.intens_cal.putData(cal_intensity_type)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing intensity calibration type field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            data = Float32(np.array(fwhm_arr, 'float32').reshape(
                [self.WIDTH_NUM, self.SLIT_NUM, self.GRATING_NUM, self.ROI_NUM]))
            self.calibration_geometry_fwhm.putData(data)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing geometry calibration fwhm field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            data = Float32(np.array(offset_arr, 'float32').reshape(
                [self.WIDTH_NUM, self.SLIT_NUM, self.GRATING_NUM, self.ROI_NUM]))
            self.calibration_geometry_offset.putData(data)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing geometry calibration offset field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            data = Float32(np.array(grating_disp_arr, 'float32').reshape(
                [6, cal_dispersion_fit_points]))
            self.calibration_geometry_grating_disp.putData(data)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing geometry calibration dispersion field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            data = Float32(np.array(intensity_coeff_arr, 'float32').reshape(
                [self.WIDTH_NUM, self.SLIT_NUM, self.GRATING_NUM, self.ROI_NUM, (cal_intensity_degree+1)]))
            self.calibration_intensity_coeff.putData(data)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing geometry calibration offset field ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            for gr_val in gratingsNodeList:
                for gr in range(0, 3):
                    #print gr_val[0]%(gr+1), gr_val[1], gr_val[2]
                    if gr_val[1] == 'text':
                        getattr(self, gr_val[0] %
                                (gr+1)).putData(gr_val[2][gr])
                    elif gr_val[1] == 'int':
                        getattr(self, gr_val[0] %
                                (gr+1)).putData(int(gr_val[2][gr]))
                    elif gr_val[1] == 'float':
                        getattr(self, gr_val[0] % (gr+1)
                                ).putData(float(gr_val[2][gr]))
                    elif gr_val[1] == 'signal':
                        getattr(self, gr_val[0] % (
                            gr+1)).putData(Signal(gr_val[2][1 + (gr * 2)], None, gr_val[2][gr * 2]))
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Error writing grating calibration %s field %s' % (gr_val[0] % (gr+1), str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

    def init(self):

        print ('---------------- INNIT --------------------')
        import serial

        try:
            detector = self.detector.getData().getPath()
            print ("DETECTOR ", detector)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve DETECTOR ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

# Read sensor values

        try:
            height = Data.execute(detector+'.SENSOR.INFORMATION.ACTIVE:HEIGHT')
            print (height)
            self.sensor_height.putData(height)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve HEIGHT sensor ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            width = Data.execute(detector+'.SENSOR.INFORMATION.ACTIVE:WIDTH')
            print (width)
            self.sensor_width.putData(width)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve WIDTH sensor ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            pixelHeigh = Data.execute(
                detector+'.SENSOR.INFORMATION.PIXEL:HEIGHT')
            print (pixelHeigh)
            self.sensor_pixel_height.putData(pixelHeigh)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve PIXEL_HEIGHT sensor ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            pixelWidth = Data.execute(
                detector+'.SENSOR.INFORMATION.PIXEL:HEIGHT')
            print (pixelWidth)
            self.sensor_pixel_width.putData(pixelWidth)
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve PIXEL_WIDTH sensor ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            serialPort = self.serial_port.data()
            dev = serial.Serial(serialPort, 9600, serial.EIGHTBITS,
                                stopbits=serial.STOPBITS_ONE, timeout=0, parity=serial.PARITY_NONE)
        except serial.SerialException as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot connect to serial port  ' + serialPort + ' ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve serial port value ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        out = []
        if self.sendCommand(dev, 'MONO-EESTATUS', out) == self.SCT_320_OK:
            # self.model_name.putData(out[0][0])
            for val in self.valueList:
                if val[3] == 'text':
                    d = getattr(self, val[0]).getData()
                    #print val, out[val[1]][val[2]]
                    if d != out[val[1]][val[2]]:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Inconsisent grating calibration value %s spectrometer %s pulse file %s  ' % (val[0], out[val[1]][val[2]], d))
                elif val[3] == 'int':
                    d = getattr(self, val[0]).getData()
                    #print val, int(out[val[1]][val[2]])
                    if d != int(out[val[1]][val[2]]):
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Inconsisent grating calibration value %s spectrometer %d pulse file %d  ' % (val[0], int(out[val[1]][val[2]]), d))
                elif val[3] == 'float':
                    d = getattr(self, val[0]).getData()
                    #print val, float(out[val[1]][val[2]])
                    if d != float(out[val[1]][val[2]]):
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Inconsisent grating calibration value %s spectrometer %f pulse file %f  ' % (val[0], float(out[val[1]][val[2]]), d))
        else:
            if dev != 0:
                dev.close()
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot cannot read spectrograph configuration ')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            grating = self.grating.data()
            if self.setParameter(dev, grating, 'GRATING') == self.SCT_320_OK:
                out = []
                if self.getParameter(dev, 'GRATING', out) == self.SCT_320_OK:
                    if grating != int(out[0][0]):
                        Data.execute('DevLogErr($1,$2)', self.getNid(),
                                     'Error GRATING cofigured value')
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Error reading GRATING value')
            else:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Error witing GRATING value')
        except Exception as e:
            if dev != 0:
                dev.close()
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve GRATING value ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            lambda_0 = self.lambda_0.data()
            if self.setParameter(dev, lambda_0, 'GOTO') == self.SCT_320_OK:
                out = []
                if self.getParameter(dev, 'NM', out) == self.SCT_320_OK:
                    readLambda = float(out[0][0])
                    self.lambda_0.putData(readLambda)
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Error reading LAMBDA_0 value')
            else:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Error witing LAMBDA_0 value')
        except Exception as e:
            if dev != 0:
                dev.close()
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve LAMBDA_0 value ' + str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if dev != 0:
            dev.close()

        for i in range(1, self.GRATING_NUM+1):
            self.computeLambda(i)

        for i in range(self.ROI_NUM):
            try:
                ccdRoiNode = self.__getattr__(
                    '%s.SPECTRUM_%02d:DATA' % (detector, i+1))
                getattr(self, 'rois_roi_%02d_spectra' %
                        (i+1)).putData(ccdRoiNode)
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot save reference spectrometer roi to ccd roi data ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        print ('-------------------------------------------')
        return (1)
