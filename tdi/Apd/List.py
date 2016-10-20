"""
This function can be used to generate a list of descriptors
"""
def List(*args):
    from MDSplus.apd import List    
    return List((None if a is None else a.evaluate() for a in args))
