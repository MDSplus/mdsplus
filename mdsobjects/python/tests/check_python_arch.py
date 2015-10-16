#
# Use to check if architecture of the MDSplus libraries matches the python used
#
# Usage: python check_python_arch.py dirspec-of-mdsplus-libs
#
# If ok the above command will return a status of 0.
# If there is a architecture mismatch it will return 1.
# Otherwise it will display an exception and return 1.
#
import ctypes,sys,os
if sys.platform.startswith('win'):
    lib='MdsShr.dll'
elif sys.platform.startswith('darwin'):
    lib='libMdsShr.dylib'
else:
    lib='libMdsShr.so'
    
try:
    ctypes.CDLL(os.path.join(sys.argv[1],lib))
    sys.exit(0)
except OSError:
    sys.exit(1)
