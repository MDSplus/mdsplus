"""
This function can be used to generate a list of descriptors
"""
def List(*args):
    from MDSplus import List
    return List(map(List.evaluate,args))
