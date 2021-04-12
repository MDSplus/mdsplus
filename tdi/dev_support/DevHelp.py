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


def DevHelp(arg, full=0, device_list=[None]):
    import pydoc
    import MDSplus

    if isinstance(arg, MDSplus.TreeNode):
        try:
            elt = int(arg.conglomerate_elt)
            if elt == 0:
                return ""
            if elt == 1:
                return DevHelp(arg.record.model, full)
            else:
                cls = arg.head.record.getDevice()
                return cls.parts[elt-2].get('help', "")
        except Exception as e:
            return "ERROR: %s" % (e,)
    else:
        full = int(full)
        devtype = arg.upper()
        if device_list[0] is None:
            alldevices = MDSplus.Data.execute('MDSDEVICES()')
            device_list[0] = [item[0].strip() for item in alldevices]
        if ('*' in devtype) or ('?' in devtype):
            devnames = []
            for device in device_list[0]:
                if MDSplus.Data.execute(
                        'MdsShr->StrMatchWild(descr($), descr($))',
                        (device.upper(), devtype)) & 1:
                    devnames.append(DevHelp(device, -1))
            return '\n'.join(devnames)
        else:
            try:
                cls = MDSplus.Device.PyDevice(devtype)
                if full == 1:
                    return pydoc.TextDoc().docclass(cls)
                elif full == -1:
                    return "%s: python device" % devtype
                else:
                    return cls.__doc__
            except Exception as e:
                for device in device_list[0]:
                    if device.upper() == devtype:
                        return "%s: tdi, java or shared library device" % (
                            device,)
                return "Error obtaining help on device %s: %s" % (devtype, e)
