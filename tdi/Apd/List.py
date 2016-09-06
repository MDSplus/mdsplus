"""
This function can be used to generate a list of descriptors
"""
def List(*args):
    from MDSplus.apd import List,Apd
    from MDSplus.mdsdata import Data
    args = map(Data.evaluate,args)
    if (len(args)==1 and isinstance(args[0],Apd)):
        return List(args[0])
    else:
        return List(args)
