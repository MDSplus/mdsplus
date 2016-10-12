from MDSplus import Int32, Device
from MDSplus import TreeNOMETHOD,DevPYDEVICE_NOT_FOUND,MDSplusException,PyUNHANDLED_EXCEPTION,MDSplusSuccess
from sys import stderr,exc_info

def PyDoMethod(n,method,*args):
    c = n.conglomerate_nids[0].record
    model = str(c.model)
    method = str(method)
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
        return [Int32(DevPYDEVICE_NOT_FOUND.status),None]
    try:
        device = safe_env[model](n)
        try:
            methodobj = device.__getattribute__(method)
        except AttributeError:
            return [Int32(TreeNOMETHOD.status),None]
        try:
            result = methodobj(*args)
        except TypeError:
            exc = exc_info()[1]
            print exc
            if exc.message.startswith(method+'()'):
                print('Your device method %s.%s requires at least one argument.' % (model,method))
                print('No argument has been provided as it is probably not required by the method.')
                print('MDSplus does not require device methods to accept an argument anymore.\n')
                result = methodobj(None)
            else:
                raise exc
    except:
        exc = exc_info()[1]
        stderr.write("Python error in %s.%s:\n%s\n\n" % (model,method,str(exc)))
        if isinstance(exc,(MDSplusException,)):
            return [exc.status,None]
        else:
            return [Int32(PyUNHANDLED_EXCEPTION.status),None]
    return [Int32(MDSplusSuccess.status),result]
