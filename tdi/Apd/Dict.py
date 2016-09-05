"""
This function can be used to generate a dictionary of descriptors
"""
def Dict(*args):
    from MDSplus.apd import Dictionary,Apd
    from MDSplus.mdsdata import Data
    args = map(Data.evaluate,args)
    if (len(args)==1 and isinstance(args[0],Apd)):
        return Dictionary(args[0])
    else:
        return Dictionary(args)
