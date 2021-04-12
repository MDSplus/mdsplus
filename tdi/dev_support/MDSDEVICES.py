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


import threading


# locks the cache
def MDSDEVICES(lock=threading.Lock(), cache=[None]):
    with lock:
        if cache[0] is None:
            import numpy
            import MDSplus

            def importDevices(name):
                bname = MDSplus.version.tobytes(name)
                try:
                    module = __import__(name)
                    ans = [
                        [MDSplus.version.tobytes(k), bname]
                        for k, v in module.__dict__.items()
                        if (isinstance(v, int.__class__)
                            and issubclass(v, MDSplus.Device))
                    ]
                except (ImportError, ImportWarning):
                    ans = []
                tdidev = MDSplus.tdi("if_error(%s(),*)" % name)
                if tdidev is None:
                    return ans
                tdidev = [
                    [k.rstrip(), v.rstrip()]
                    for k, v in tdidev.value.reshape(
                        (int(tdidev.value.size/2), 2)).tolist()
                ]
                return tdidev+ans
            ans = [
                [MDSplus.version.tobytes(d), b'pydevice']
                for d in MDSplus.Device.findPyDevices().keys()
            ]
            mdsdevs = MDSplus.getenv('MDS_DEVICES')
            if mdsdevs is not None:
                modules = mdsdevs.split(':')
            else:
                modules = [
                    "KbsiDevices", "MitDevices", "RfxDevices", "W7xDevices"]
            for module in modules:
                ans += importDevices(module)
            ans = numpy.array(list(dict(ans).items()))
            ans.view('%s,%s' % (
                ans.dtype, ans.dtype)).sort(order=['f0'], axis=0)
            cache[0] = ans
    return cache[0]
