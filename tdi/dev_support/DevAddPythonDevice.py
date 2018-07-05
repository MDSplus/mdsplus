# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

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
