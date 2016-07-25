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


"""TDI Constant evaluation such as $A0,$PI etc..."""

class _constant(Builtin):
    def __str__(self):
        return self.name

class dA0(_constant):
    max_args=0
    name='$A0'
    opcode=1

class dALPHA(_constant):
    max_args=0
    name='$ALPHA'
    opcode=2

class dAMU(_constant):
    max_args=0
    name='$ALPHA'
    opcode=3

class dC(_constant):
    max_args=0
    name='$C'
    opcode=4

class dCAL(_constant):
    max_args=0
    name='$CAL'
    opcode=5

class dDEGREE(_constant):
    max_args=0
    name='$DEGREE'
    opcode=6

class dEV(_constant):
    max_args=0
    name='$EV'
    opcode=7

class dFALSE(_constant):
    max_args=0
    name='$FALSE'
    opcode=8

class dFARADAY(_constant):
    max_args=0
    name='$FARADAY'
    opcode=9

class dG(_constant):
    max_args=0
    name='$G'
    opcode=10

class dGAS(_constant):
    max_args=0
    name='$GAS'
    opcode=11

class dH(_constant):
    max_args=0
    name='$H'
    opcode=12

class dHBAR(_constant):
    max_args=0
    name='$HBAR'
    opcode=12

class dI(_constant):
    max_args=0
    name='$I'
    opcode=14

class dK(_constant):
    max_args=0
    name='$K'
    opcode=15

class dME(_constant):
    max_args=0
    name='$ME'
    opcode=16

class dMISSING(_constant):
    max_args=0
    name='$MISSING'
    opcode=17

class dMP(_constant):
    max_args=0
    name='$MP'
    opcode=18

class dN0(_constant):
    max_args=0
    name='$N0'
    opcode=19

class dNA(_constant):
    max_args=0
    name='$NA'
    opcode=20

class dP0(_constant):
    max_args=0
    name='$P0'
    opcode=21

class dPI(_constant):
    max_args=0
    name='$PI'
    opcode=22

class dQE(_constant):
    max_args=0
    name='$QE'
    opcode=23

class dRE(_constant):
    max_args=0
    name='$RE'
    opcode=24

class dROPRAND(_constant):
    max_args=0
    name='$ROPRAND'
    opcode=25

class dRYDBERG(_constant):
    max_args=0
    name='$RYDBER'
    opcode=26

class dT0(_constant):
    max_args=0
    name='$T0'
    opcode=27

class dTORR(_constant):
    max_args=0
    name='$TORR'
    opcode=28

class dTRUE(_constant):
    max_args=0
    name='$TRUE'
    opcode=29

globs=list(globals().values())
for c in globs:
    if hasattr(c,'__name__') and hasattr(c,'name'):
        del globals()[c.__name__]
        globals()[c.name]=c
        c.__name__=c.name
