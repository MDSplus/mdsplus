import os,numpy
rfxdir = os.path.dirname(__file__)
def KBISDEVICES():
    from MDSplus import version
    return numpy.array([
        [version.tobytes(file[0:len(file)-9]),b'KbisDevices']
          for file in os.listdir(rfxdir)
            if file.lower().endswith('__add.fun')
        ])
