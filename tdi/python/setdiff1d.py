def setdiff1d(a, b):
    """
    setdiff1d(a, b) returns the elements of a that are not in b.
    """
    from MDSplus import Data
    from numpy.lib.arraysetops import setdiff1d
    return Data(setdiff1d(a.data(),b.data()))

