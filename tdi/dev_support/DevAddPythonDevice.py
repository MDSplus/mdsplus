from MDSplus import Data, Tree, Device, Ident, Int32
from MDSplus import DevPYDEVICE_NOT_FOUND,MDSplusException,MDSplusERROR,TreeSUCCESS
import sys
def DevAddPythonDevice(path, model, nidout=None):
    """Add a python device to the tree by:
    1) finding the model in the list defined by
       the tdi function, MdsDevices.
    2) try importing the package for the model and calling its Add method.

    Both the path and model are passed in as Ident instances.
    The StringArray returned by MdsDevices() contains String instances
    containing blank filled values containing an \0 character embedded.
    These Strings have to be manipulated to produce simple str() values.
    """
    model = str(model.data()).strip()
    path  = str(path.data()).strip()
    try:
        node = Device.PyDevice(model).Add(Tree(),path)
        if isinstance(nidout,(Ident,)):
            Data.execute("$=$",nidout,Int32(node.nid))
        return TreeSUCCESS.status
    except MDSplusException:
        return sys.exc_info()[1].status
    except:
        print ("Error adding device instance of %s: %s" % (model, sys.exc_info()[1]))
        return MDSplusERROR.status
