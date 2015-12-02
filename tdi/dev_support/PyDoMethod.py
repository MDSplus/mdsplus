from __future__ import print_function
from MDSplus import StringArray, Int32
from MDSplus.mdsExceptions import TreeNOMETHOD
from sys import stderr

def PyDoMethod(n,method,*args):
    c=n.conglomerate_nids[0].record
    q=c.qualifiers
    model=str(c.model)
    method=str(method)
    status = TreeNOMETHOD.status
    if isinstance(q,StringArray):
        for i in range(len(q)):
            exec(str(q[i])) in globals()
    else:
        exec(str(q)) in globals()
    if ((model in globals()) and hasattr(globals()[model],method)):
        try:
            device=globals()[model]
            d=device(n)
            status = d.__getattribute__(method)(d,*args)
        except:
            e = _sys.exc_info()[1]
            print("Error: "+str(_sys.exc_info()),file=_sys.stderr)
            if hasattr(e,'status'):
                status = e.status
            else:
                status = 0
        Int32(status).setTdiVar("_method_status")
        return status
    else:
        return TreeNOMETHOD.status
