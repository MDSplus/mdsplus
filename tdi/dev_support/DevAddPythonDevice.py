from MDSplus import Data, Tree, Device
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
    if mod is not None and model in mod.__dict__:
        try:
            mod.__dict__[model].Add(Tree(), path)
            return 1
        except:
            raise
            print ("Error adding device instance of %s: %s" % (model, sys.exc_info()[1]))
            return 0
        
    path = path.data()
    models = Data.execute('MdsDevices()')

    for idx in range(0, len(models), 2):
        try:
            modname = models[idx].data().upper()
            modname = modname[0:modname.find('\0')]
            package = models[idx+1].data()
            package = package[0:package.find('\0')]
            if model == modname:
                try:
                    __import__(package).__dict__[model].Add(Tree(), path)
                    return 1
                except:
                    print ("Error adding device instance of %s from %s: %s" % (modname, package, sys.exc_info()[1]))
                    return 0
        except:
            pass
    return 0
    
