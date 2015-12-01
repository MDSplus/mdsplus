from __future__ import print_function
import sys as _sys
from MDSplus import TreeRef,MDSplusException,StringArray,Int32,Data
from MDSplus.mdsExceptions import TreeNOMETHOD


def PyDoMethod(n,method,*args):
    top=n.conglomerate_nids[0]
    c=top.record
    q=c.qualifiers
    model=str(c.model)
    method=str(method)
    status = TreeNOMETHOD.status
    if isinstance(q,StringArray):
        for i in range(len(q)):
            exec( str(q[i])) in globals()
    else:
        exec(str(q)) in globals()
    if (model in globals()) and (method in (globals()[model].__dict__)):
        try:
            device=globals()[model]
            status = device.__dict__[str(method)](device(n),args)
        except:
            e = _sys.exc_info()[1]
            print("Error: "+str(e),file=_sys.stderr)
            if hassatr(e,'status'):
                status = e.status
            else:
                status = 0
        Int32(status).setTdiVar("_method_status")
    return status
