from MDSplus import Device, List
from MDSplus import TreeNOMETHOD,DevPYDEVICE_NOT_FOUND,MDSplusException,PyUNHANDLED_EXCEPTION,MDSplusSuccess
from sys import stderr,exc_info

def PyDoMethod(n,method,*args):
    def domethod(methodobj,args):
        try:
            return methodobj(*args)
        except TypeError:
            exc = exc_info()[1]
            print exc
            if exc.message.startswith(method+'()'):
                print('Your device method %s.%s requires at least one argument.' % (model,method))
                print('No argument has been provided as it is probably not required by the method.')
                print('MDSplus does not require device methods to accept an argument anymore.\n')
                return methodobj(None)
            else:
                raise exc
    try:
        device = n.conglomerate_nids[0]
        c = device.record
        model = str(c.model)
        method = str(method)
        result = None
        if not hasattr(Device,method):
            print("doing %s(%s).%s(%s)"%(device,model,method,','.join(map(str,args))))
        if not isinstance(device, (Device,)):
            safe_env = {}
            try:
                mod = Device.importPyDeviceModule(model)
                safe_env[model]=mod.__dict__[model]
            except:
                qualifiers = c.qualifiers.value.tolist()
                if isinstance(qualifiers,list): qualifiers = ';'.join(qualifiers)  # make it a list of statements
                exec(compile(qualifiers,'<string>','exec')) in safe_env
            if not model in safe_env:
                stderr.write("Python device implementation not found for %s after doing %s\n\n" % (model,qualifiers))
                raise DevPYDEVICE_NOT_FOUND()
            device = safe_env[model](n)
        try:
            methodobj = device.__getattribute__(method)
        except AttributeError:
            raise TreeNOMETHOD()
        result = domethod(methodobj,args)
        status = MDSplusSuccess.status
    except MDSplusException as exc:
        status = exc.status
    except Exception as exc:
        stderr.write("Python error in %s.%s:\n%s\n\n" % (model,method,str(exc)))
        status = PyUNHANDLED_EXCEPTION.status
    return List([status,result])
