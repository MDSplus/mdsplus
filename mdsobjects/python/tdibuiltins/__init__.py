try:
    from tdibuiltins.builtin import Builtin
    from tdibuiltins.constants import *
    from tdibuiltins.mathops import *
    from tdibuiltins.builtins_other import *
except:
    from builtin import Builtin
    from constants import *
    from mathops import *
    from builtins_other import *
Builtin.loadBuiltins(globals())
