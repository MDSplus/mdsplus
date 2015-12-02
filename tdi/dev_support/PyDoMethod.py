from MDSplus import StringArray
from MDSplus.mdsExceptions import TreeNOMETHOD,DevPYDEVICE_NOT_FOUND
from sys import stderr,exc_info

def PyDoMethod(n,method,*args):
    c = n.conglomerate_nids[0].record
    q = c.qualifiers
    model = str(c.model)
    method = str(method)
    if isinstance(q,StringArray):
        for i in range(len(q)):
            exec(str(q[i])) in globals()
    else:
        exec(str(q)) in globals()
    if not model in globals():
        stderr.write("Python device implementation not found for %s after doing %s" % (model,str(q)))
        return [DevPYDEVICE_NOT_FOUND.status,None]
    try:
        device = globals()[model](n)
        try:
            methodobj = device.__getattribute__(method)
        except AttributeError:
            return [TreeNOMETHOD.status,None]
        try:
            return [1,methodobj(*args)]
        except TypeError:
            print('Your device method %s.%s requires at least one argument.' % (model,method))
            print('No argument has been provided as it is probably not required by the method.')
            print('MDSplus does not require device methods to accept an argument anymore.')
            return [1,methodobj(None)]
    except:
        exc = exc_info()[1]
        stderr.write("Python error in %s.%s:\n%s\n" % (model,method,repr(exc)))
        if hasattr(exc,'status'):
            return [exc.status,None]
        else:
            return [0,None]
