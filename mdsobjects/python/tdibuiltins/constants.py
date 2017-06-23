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

_scalar=_mimport('mdsscalar',2)


"""TDI Constant evaluation such as $A0,$PI etc..."""

class _constant(Builtin):
    def __str__(self):
        return self.name

class dA0(_constant):
    max_args=0
    name='$A0'
    opcode=1
    def evaluate(self):
        return _scalar.Float32(52.9177E-12).setUnits('m').setError(_scalar.Float32(2400E-21))

class dALPHA(_constant):
    max_args=0
    name='$ALPHA'
    opcode=2
    def evaluate(self):
        return _scalar.Float32(7.29735308e-3).setError(_scalar.Float32(0.00000033e-3))

class dAMU(_constant):
    max_args=0
    name='$ALPHA'
    opcode=3
    def evaluate(self):
        return _scalar.Float32(1.6605402e-27).setError(_scalar.Float32(0.0000010e-27))

class dC(_constant):
    max_args=0
    name='$C'
    opcode=4
    def evaluate(self):
        return _scalar.Float32(299792458.).setUnits('m/s')

class dCAL(_constant):
    max_args=0
    name='$CAL'
    opcode=5
    def evaluate(self):
        return _scalar.Float32(4.1868).setUnits('J')

class dDEGREE(_constant):
    max_args=0
    name='$DEGREE'
    opcode=6
    def evaluate(self):
        return _scalar.Float32(.01745329252)

class dEV(_constant):
    max_args=0
    name='$EV'
    opcode=7
    def evaluate(self):
        return _scalar.Float32(1.60217733e-19).setUnits('J/eV').setError(_scalar.Float32(0.00000049e-19))

class dFALSE(_constant):
    max_args=0
    name='$FALSE'
    opcode=8
    def evaluate(self):
        return False

class dFARADAY(_constant):
    max_args=0
    name='$FARADAY'
    opcode=9
    def evaluate(self):
        return _scalar.Float32(9.6485309e4).setUnits('C/mol').setError(_scalar.Float32(0.0000029e4))

class dG(_constant):
    max_args=0
    name='$G'
    opcode=10
    def evaluate(self):
        return _scalar.Float32(6.67259e-11).setUnits('m^3/s^2/kg').setError(_scalar.Float32(0.00085))

class dGAS(_constant):
    max_args=0
    name='$GAS'
    opcode=11
    def evaluate(self):
        return _scalar.Float32(8.314510).setUnits('J/K/mol').setError(_scalar.Float32(0.000070))

class dH(_constant):
    max_args=0
    name='$H'
    opcode=12
    def evaluate(self):
        return _scalar.Float32(6.6260755e-34).setUnits('J*s').setError(_scalar.Float32(0.0000040))

class dHBAR(_constant):
    max_args=0
    name='$HBAR'
    opcode=12
    def evaluate(self):
        return _scalar.Float32(1.05457266e-34).setUnits('J*s').setError(_scalar.Float32(0.00000063))

class dI(_constant):
    max_args=0
    name='$I'
    opcode=14
    def evaluate(self):
        return _scalar.Complex64(1j)

class dK(_constant):
    max_args=0
    name='$K'
    opcode=15
    def evaluate(self):
        return _scalar.Float32(1.380658e-23).setUnits('J/K').setError(_scalar.Float32(1.380658e-23))

class dME(_constant):
    max_args=0
    name='$ME'
    opcode=16
    def evaluate(self):
        return _scalar.Float32(9.1093897e-31).setUnits('kg').setError(_scalar.Float32(0.0000054e-31))

class dMISSING(_constant):
    max_args=0
    name='$MISSING'
    opcode=17
    def evaluate(self):
        return self

class dMP(_constant):
    max_args=0
    name='$MP'
    opcode=18
    def evaluate(self):
        return _scalar.Float32(1.6726231e-27).setUnits('kg').setError(_scalar.Float32(0.0000010e-27))

class dN0(_constant):
    max_args=0
    name='$N0'
    opcode=19
    def evaluate(self):
        return _scalar.Float32(2.686763e25).setUnits('/m^3').setError(_scalar.Float32(0.000023e25))

class dNA(_constant):
    max_args=0
    name='$NA'
    opcode=20
    def evaluate(self):
        return _scalar.Float32(6.0221367e23).setUnits('/mol').setError(_scalar.Float32(0.0000036e23))

class dP0(_constant):
    max_args=0
    name='$P0'
    opcode=21
    def evaluate(self):
        return _scalar.Float32(1.01325e5).setUnits('Pa')

class dPI(_constant):
    max_args=0
    name='$PI'
    opcode=22
    def evaluate(self):
        return _scalar.Float32(3.1415926536)

class dQE(_constant):
    max_args=0
    name='$QE'
    opcode=23
    def evaluate(self):
        return _scalar.Float32(1.60217733e-19).setUnits('C').setError(_scalar.Float32(0.000000493-19))

class dRE(_constant):
    max_args=0
    name='$RE'
    opcode=24
    def evaluate(self):
        return _scalar.Float32(2.81794092e-15).setUnits('m').setError(_scalar.Float32(0.00000038e-15))

class dROPRAND(_constant):
    max_args=0
    name='$ROPRAND'
    opcode=25
    def evaluate(self):
        from numpy import nan
        return _scalar.Float32(nan)

class dRYDBERG(_constant):
    max_args=0
    name='$RYDBER'
    opcode=26
    def evaluate(self):
        return _scalar.Float32(1.0973731534e7).setUnits('/m').setErrors(_scalar.Float32(0.0000000013e7))

class dT0(_constant):
    max_args=0
    name='$T0'
    opcode=27
    def evaluate(self):
        return _scalar.Float32(273.16).setUnits('K')

class dTORR(_constant):
    max_args=0
    name='$TORR'
    opcode=28
    def evaluate(self):
        return _scalar.Float32(1.3332e2).setUnits('Pa')

class dTRUE(_constant):
    max_args=0
    name='$TRUE'
    opcode=29
    def evaluate(self):
        return True

globs=list(globals().values())
for c in globs:
    if hasattr(c,'__name__') and hasattr(c,'name'):
        del globals()[c.__name__]
        globals()[c.name]=c
        c.__name__=c.name
