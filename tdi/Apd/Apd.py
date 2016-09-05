"""
This function can be used to generate an array of descriptors
"""
def Apd(*args):
    from MDSplus.apd import Apd
    from MDSplus.mdsdata import Data
    args = map(Data.evaluate,args)
    if (len(args)==1 and isinstance(args[0],Apd)):
        return args[0]
    else:
        return Apd(args)
