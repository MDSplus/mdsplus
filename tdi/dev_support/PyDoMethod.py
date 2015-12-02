from MDSplus import StringArray, Int32
from MDSplus.mdsExceptions import TreeNOMETHOD
from sys import stderr, exc_info

def PyDoMethod(n,method,*args):
    top = n.conglomerate_nids[0]
    c = top.record
    q = c.qualifiers
    model = str(c.model)
    method = str(method)
    if isinstance(q,StringArray):
        for i in range(len(q)):
            exec(str(q[i])) in globals()
    else:
        exec(str(q)) in globals()
    if not model in globals():
        return -1  # Device not found
    device = globals()[model](n)
    try:
        try:
            methodobj = device.__getattribute__(method)
        except AttributeError:
            return TreeNOMETHOD.status
        try:
            status = methodobj(*args)
        except TypeError:
            print('Your device method %s.%s requires at least one argument.' % (model,method))
            print('No argumnet has been provided as it is probably not required by the method.')
            print('MDSplus does not require device methods to accept an argument anymore.')
            status = methodobj(None)
    except:
        exc = exc_info()[1]
        stderr.write("Python error in %s.%s:\n%s\n" % (model,method,repr(exc)))
        if hasattr(exc,'status'):
            status = exc.status
        else:
            status = 0
    Int32(status).setTdiVar("_method_status")
    return status
