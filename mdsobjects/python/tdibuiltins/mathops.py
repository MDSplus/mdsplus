def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        try:
            return __import__(name, globals())
        except:
            return __import__('MDSplus.'+name,globals())

_builtin=_mimport('builtin')
Builtin=_builtin.Builtin

class ABS(Builtin):
    min_args=1
    max_args=1
    name="ABS"
    opcode=32

class ABS1(Builtin):
    min_args=1
    max_args=1
    name="ABS1"
    opcode=33

class ABSSQ(Builtin):
    min_args=1
    max_args=1
    name="ABSSQ"
    opcode=34

class ACOS(Builtin):
    min_args=1
    max_args=1
    name="ACOS"
    opcode=36

class ACOSD(Builtin):
    min_args=1
    max_args=1
    name="ACOSD"
    opcode=37
