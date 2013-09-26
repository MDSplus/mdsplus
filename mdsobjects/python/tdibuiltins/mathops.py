try:
    exec("from ..mdsscalar import Scalar as _Scalar, Complex64 as _Complex64, Complex128 as _Complex128, Float32 as _Float32")
    exec("from ..mdsarray import Array as _Array, Complex64Array as _Complex64Array, Complex128Array as _Complex128Array, Float32Array as _Float32Array")
    exec("from ..mdsdata import makeData as _makeData")
    exec("from ..compound import Signal as _Signal")
except:
    try:
        from mdsscalar import Scalar as _Scalar, Complex64 as _Complex64, Complex128 as _Complex128, Float32 as _Float32
        from mdsarray import Array as _Array, Complex64Array as _Complex64Array, Complex128Array as _Complex128Array, Float32Array as _Float32Array
        from mdsdata import makeData as _makeData
        from compound import Signal as _Signal
    except:
        from MDSplus.mdsscalar import Scalar as _Scalar, Complex64 as _Complex64, Complex128 as _Complex128, Float32 as _Float32
        from MDSplus.mdsarray import Array as _Array, Complex64Array as _Complex64Array, Complex128Array as _Complex128Array, Float32Array as _Float32Array
        from MDSplus.mdsdata import makeData as _makeData
        from MDSplus.compound import Signal as _Signal

try:
    from builtin import Builtin
except:
    from tdibuiltin.builtin import Builtin

def _evaluateArg(arg):
    try:
        arg=arg.evaluate()
    except:
        arg=_makeData(arg)
    return arg

def _evaluateArgs(args):
    ans=list()
    for arg in args:
        ans.append(_evaluateArg(arg))
    return ans

class ABS(Builtin):
    min_args=1
    max_args=1
    name="ABS"
    opcode=32
    def evaluate(self):
        args=_evaluateArgs(self.args)
        if isinstance(args[0],_Scalar) or isinstance(args[0],_Array):
            if hasattr(args[0],'_units'):
                ans = _makeData(abs(args[0])).setUnits(args[0]._units)
            else:
                ans = _makeData(abs(args[0]))
        elif isinstance(args[0],_Signal):
            args[0].value=ABS(args[0].value).evaluate()
            ans = args[0]
        else:
            raise Exception("Invalid argument type for ABS function: %s" % (str(type(args[0])),))
        return ans

class ABS1(Builtin):
    min_args=1
    max_args=1
    name="ABS1"
    opcode=33
    def evaluate(self):
        args=_evaluateArgs(self.args)
        if isinstance(args[0],_Scalar) or isinstance(args[0],_Array):
            ans=_makeData(abs(args[0].real)+abs(args[0].imag))
            if hasattr(args[0],'_units'):
                ans=ans.setUnits(args[0].units)
        elif isinstance(args[0],_Signal):
            args[0].value=ABS1(args[0].value).evaluate()
            ans=args[0]
        else:
            raise Exception("Invalid argument type for ABS function: %s" % (str(type(args[0])),))
        return ans

class ABSSQ(Builtin):
    min_args=1
    max_args=1
    name="ABSSQ"
    opcode=34
    def evaluate(self):
        args=_evaluateArgs(self.args)
        if isinstance(args[0],_Scalar) or isinstance(args[0],_Array):
            ans=_makeData(abs(args[0].real)**2+abs(args[0].imag)**2)
            if isinstance(args[0],_Complex64):
                ans=_Float32(ans)
            elif isinstance(args[0],_Complex128):
                ans=_Float64(ans)
            elif isinstance(args[0],_Complex64Array):
                ans=_Float32Array(ans)
            elif isinstance(args[0],_Complex128Array):
                ans=_Float64Array(ans)
            else:
                ans=type(args[0])(ans)
            if hasattr(args[0],'_units'):
                ans=ans.setUnits(args[0].units+"*"+args[0].units)
        elif isinstance(args[0],_Signal):
            args[0].value=ABSSQ(args[0].value).evaluate()
            ans=args[0]
        else:
            raise Exception("Invalid argument type for ABS function: %s" % (str(type(args[0])),))
        return ans

class ACOS(Builtin):
    min_args=1
    max_args=1
    name="ACOS"
    opcode=36
    def evaluate(self):
        from numpy import arccos
        args=_evaluateArgs(self.args)
        if isinstance(args[0],_Scalar) or isinstance(args[0],_Array):
            ans = _makeData(arccos(args[0].value))
        elif isinstance(args[0],_Signal):
            args[0].value=ACOS(args[0].value).evaluate()
            ans = args[0]
        else:
            raise Exception("Invalid argument type for ABS function: %s" % (str(type(args[0])),))
        return ans

class ACOSD(Builtin):
    min_args=1
    max_args=1
    name="ACOSD"
    opcode=37
    def evaluate(self):
        from numpy import arccos,pi
        args=_evaluateArgs(self.args)
        if isinstance(args[0],_Scalar) or isinstance(args[0],_Array):
            ans = type(args[0])(arccos(args[0].value)/pi*180.)
        elif isinstance(args[0],_Signal):
            args[0].value=ACOSD(args[0].value).evaluate()
            ans = args[0]
        else:
            raise Exception("Invalid argument type for ABS function: %s" % (str(type(args[0])),))
        return ans
