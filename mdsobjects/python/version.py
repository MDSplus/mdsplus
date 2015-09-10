"""
This is a helper module.
Its purpose is to supply tools that are used to generate version specific code.
Goal is to generate code that work on both python2x and python3x.
"""
from sys import version_info as pyver
ispy3 = pyver>(3,)
ispy2 = pyver<(3,)
#__builtins__ is dict
has_long      = 'long'       in __builtins__
has_unicode   = 'unicode'    in __builtins__
has_basestring= 'basestring' in __builtins__
has_bytes     = 'bytes'      in __builtins__
has_buffer    = 'buffer'     in __builtins__

# substitute missing builtins
if has_long:
    long = long
else: 
    long = int
if has_basestring:
    basestring = basestring
else: 
    basestring = str
if has_unicode:
    unicode = unicode
else:
    unicode = type(None)
if has_bytes:
    bytes = bytes
else:
    bytes      = type(None)
if has_buffer:
    buffer = buffer
else:
    buffer = memoryview

#helper variant string
if has_unicode:
    varstr = unicode
else:
    varstr = bytes

# Extract the code attribute of a function. Different implementations
# are for Python 2/3 compatibility.

if ispy2:
    def func_code(f):
        return f.func_code
else:
    def func_code(f):
        return f.__code__