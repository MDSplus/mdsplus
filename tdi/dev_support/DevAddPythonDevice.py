from MDSplus import Data, Tree, Device
from MDSplus.mdsExceptions import DevPYDEVICE_NOT_FOUND,MDSplusException
import sys
def DevAddPythonDevice(path, model):
    """Add a python device to the tree by:
    1) finding the model in the list defined by
       the tdi function, MdsDevices.
    2) try importing the package for the model and calling its Add method.

    Both the path and model are passed in as Ident instances.
    The StringArray returned by MdsDevices() contains String instances
    containing blank filled values containing an \0 character embedded.
    These Strings have to be manipulated to produce simple str() values.
    """
    model = model.data().upper().rstrip()
    mod = Device.importPyDeviceModule(model)
    path = str(path.data())
    if mod is not None and model in mod.__dict__:
        try:
            mod.__dict__[model].Add(Tree(), path)
            return 1
        except:
            e=sys.exc_info()[1]
            if isinstance(e,MDSplusException):
                return e.status
            else:
                print ("Error adding device instance of %s: %s" % (model, sys.exc_info()[1]))
                return 0
        
    models = Data.execute('MdsDevices()')
    cls = None

    for idx in range(0, len(models), 2):
        try:
            modname = models[idx].data().upper().rstrip()
            package = models[idx+1].data().rstrip()
            if model == modname:
                cls = __import__(package).__dict__[model]
                break
        except Exception,e:
            pass
    if cls is not None:
        try:
            cls.Add(Tree(), path)
            return 1
        except:
            e=sys.exc_info()[1]
            if isinstance(e,MDSplusException):
                return e.status
            else:
                print ("Error adding device instance of %s: %s" % (model, sys.exc_info()[1]))
                return 0
    return DevPYDEVICE_NOT_FOUND.status
    
