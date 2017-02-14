from MDSplus import TreeNode, Device, List
from MDSplus import TreeNOMETHOD,MDSplusException,PyUNHANDLED_EXCEPTION,MDSplusSUCCESS
from sys import stderr,exc_info

def PyDoMethod(n,method,*args):
    def domethod(methodobj,args):
        try:
            return methodobj(*args)
        except TypeError:
            exc = exc_info()[1]
            print(exc)
            if exc.message.startswith(method+'()'):
                print('Your device method %s.%s requires at least one argument.' % (model,method))
                print('No argument has been provided as it is probably not required by the method.')
                print('MDSplus does not require device methods to accept an argument anymore.\n')
                return methodobj(None)
            else:
                raise exc
    method = str(method)
    model = n.__class__.__name__
    result = None
    try:
        if method in TreeNode.__dict__:
            methodobj = n.__getattribute__(method)
        else:
            device = n.conglomerate_nids[0]
            c = device.record
            model = str(c.model)
            result = None
            if not isinstance(device, (Device,)):
                device = c.getDevice(device)
            try:
                methodobj = device.__getattribute__(method)
            except AttributeError:
                raise TreeNOMETHOD()
            if not method in Device.__dict__:
                print("doing %s(%s).%s(%s)"%(device,model,method,','.join(map(str,args))))
        result = domethod(methodobj,args)
        status = MDSplusSUCCESS.status
    except MDSplusException as exc:
        status = exc.status
    except Exception as exc:
        stderr.write("Python error in %s.%s:\n%s\n\n" % (model,method,str(exc)))
        status = PyUNHANDLED_EXCEPTION.status
    return List([status,result])
