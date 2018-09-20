from threading import Lock
from os import getenv

cache = [None]
lock = Lock()  # locks the cache


def MDSDEVICES():
    # let "with" statement do the aquire() and release() methods on lock
    with lock:
        if cache[0] is None:
            from MDSplus import Device, tdi, version
            from numpy import array

            def importDevices(name):
                bname = version.tobytes(name)
                # import Python module and search for Device classes
                try:
                    module = __import__(name)
                    # go through components of module to find Devices
                    ans = [[version.tobytes(k), bname]
                           for k, v in module.__dict__.items()
                           if isinstance(v, int.__class__)
                           and issubclass(v, Device)]
                except (ImportError, ImportWarning):
                    ans = []
                # call TDI device function
                tdidev = tdi("if_error(%s(), *)" % name)
                # if TDI device function fails, return ans
                if tdidev is None:
                    return ans
                # parse TDI devices from function call (stripping right spaces)
                tdidev = [[k.rstrip(), v.rstrip()]
                          for k, v in
                          tdidev.value.reshape((int(tdidev.value.size/2),
                                               2)).tolist()]
                # concatenate TDI and Python Devices
                return tdidev + ans

            # start with Python Devices from Device class
            ans = [[version.tobytes(d), b'pydevice']
                   for d in Device.findPyDevices()]

            # go through the TDI functions for all devices
            # if MDS_DEVICES environment name is defined containing
            # colon delimited device directories look in those directories
            # for devices.
            try:
                modules=getenv('MDS_DEVICES').split(':')
            except AttributeError:
                modules=["KbsiDevices","MitDevices","RfxDevices","W7xDevices"]
            for module in modules:
                ans += importDevices(module)

            # clean up ans, sort, and return
            ans = array(list(dict(ans).items()))
            ans.view('%s, %s' % (ans.dtype, ans.dtype)).sort(order=['f0'],
                                                             axis=0)
            cache[0] = ans
        return cache[0]


if __name__ == "__main__":
    print(MDSDEVICES())
