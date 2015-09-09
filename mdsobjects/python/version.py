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

#substitute missing builtins
long       = long       if has_long       else int
basestring = basestring if has_basestring else str
unicode    = unicode    if has_unicode    else type(None)
bytes      = bytes      if has_bytes      else type(None)
buffer     = buffer     if has_buffer     else memoryview

#helper variant string
varstr     = unicode    if has_unicode    else bytes
