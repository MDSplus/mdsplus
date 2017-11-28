import os,numpy
rfxdir = os.path.dirname(__file__)
def RFXDEVICES():
    from MDSplus import version
    return numpy.array([
        [version.tobytes(file[0:len(file)-9]),b'RfxDevices']
          for file in os.listdir(rfxdir)
            if file.lower().endswith('__add.fun')
        ])
