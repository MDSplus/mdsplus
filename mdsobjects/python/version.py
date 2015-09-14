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
elif has_bytes: 
    basestring = (str,bytes)
else:
    basestring = str
if has_unicode:
    unicode = unicode
else:#py3 str is unicode
    unicode = str
if has_bytes:
    bytes = bytes
else:#py2 str is bytes
    bytes = str
if has_buffer:
    buffer = buffer
else:
    buffer = memoryview

#helper variant string
if has_unicode:
    varstr = unicode
else:
    varstr = bytes

def tostr(string):
    if isinstance(string,basestring):
        if isinstance(string,str):
            return string
        elif isinstance(string, unicode):
            return string.encode('utf-8','backslashreplace')
        else:
            try:
                return string.decode('utf-8','backslashreplace')
            except:
                return string.decode('CP1252','backslashreplace')
    else:
        return str(string)

def tobytes(string):
    if isinstance(string,basestring):
        if isinstance(string,bytes):
            return string
        else:
            return string.encode('utf-8','backslashreplace')
    else:
        return bytes(string)
def tounicode(string):
    if isinstance(string,basestring):
        if isinstance(string,unicode):
            return string
        else:
            return string.decode('utf-8','backslashreplace')
    else:
        return unicode(string)

# Extract the code attribute of a function. Different implementations
# are for Python 2/3 compatibility.

if ispy2:
    def func_code(f):
        return f.func_code
else:
    def func_code(f):
        return f.__code__