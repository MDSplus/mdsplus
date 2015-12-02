from MDSplus import StringArray, Int32
from MDSplus.mdsExceptions import TreeNOMETHOD
from sys import stderr

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
    if (model in globals()) and (method in (globals()[model].__dict__)):
        try:
            device=globals()[model]
            status = device.__dict__[str(method)](device(n),*args)
        except Exception as exc:
            stderr.write("Error: "+repr(exc)+"\n")
            if hasattr(exc,'status'):
                status = exc.status
            else:
                status = 0
        Int32(status).setTdiVar("_method_status")
        return status
    else:
        return TreeNOMETHOD.status
