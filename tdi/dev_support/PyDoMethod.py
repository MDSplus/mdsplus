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

import sys
import traceback
from MDSplus import TreeNode, Device
from MDSplus import TreeNOMETHOD, MDSplusException, PyUNHANDLED_EXCEPTION


def PyDoMethod(n,method,*args):
    def is_property(obj): return len(args) == 0 and not hasattr(obj,'__call__')
    method = str(method)
    model = n.__class__.__name__
    try:
        if method in TreeNode.__dict__:
            methodobj = n.__getattribute__(method)
            if is_property(methodobj):
                return methodobj
        else:
            device = n.conglomerate_nids[0]
            c = device.record
            model = str(c.model)
            if not isinstance(device, (Device,)):
                device = c.getDevice(device)
            try:
                methodobj = device.__getattribute__(method)
            except AttributeError:
                raise TreeNOMETHOD
            if is_property(methodobj):
                return methodobj
            if not method in Device.__dict__:
                print("doing %s(%s).%s(%s)" % (
                    device, model, method,','.join(map(str,args))))
        try:
            return methodobj(*args)
        except TypeError as exc:
            if args or not (method+'()') in str(exc):
                raise
            print("""
Your device method %s.%s requires at least one argument.
No argument has been provided as it is probably not required by the method.
MDSplus does not require device methods to accept an argument anymore.
""" % (model,method))
            return methodobj(None)
    except MDSplusException:
        raise
    except Exception:
        print("error doing %s(%s).%s:" % (n,model,method))
        traceback.print_exc(file=sys.stdout)
        raise PyUNHANDLED_EXCEPTION
