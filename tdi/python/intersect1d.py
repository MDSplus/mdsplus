def intersect1d(a,b):
    from MDSplus import Data
    from numpy.lib.arraysetops import intersect1d
    return Data(intersect1d(a.data(),b.data()))

