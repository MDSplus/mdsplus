import sys
if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    try:
        return __import__(name,globals())
    except:
        return __import__('MDSplus.'+name,globals())
else:
  def _mimport(name,level):
    try:
      return __import__(name,globals(),{},[],level)
    except ValueError:
      return __import__(name,globals())

_builtin=_mimport('builtin',1)
Builtin=_builtin.Builtin

_scalar=_mimport('mdsscalar',2)
_array=_mimport('mdsarray',2)
_compound=_mimport('compound',2)




def _evaluateArg(arg):
    try:
        arg=arg.evaluate()
    except:
        arg=_data.makeData(arg)
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
        if isinstance(args[0],_scalar.Scalar) or isinstance(args[0],_array.Array):
            if hasattr(args[0],'_units'):
                ans = _data.makeData(abs(args[0])).setUnits(args[0]._units)
            else:
                ans = _data.makeData(abs(args[0]))
        elif isinstance(args[0],_compound._Signal):
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
        if isinstance(args[0],_scalar.Scalar) or isinstance(args[0],_array.Array):
            ans=_data.makeData(abs(args[0].real)+abs(args[0].imag))
            if hasattr(args[0],'_units'):
                ans=ans.setUnits(args[0].units)
        elif isinstance(args[0],_compound._Signal):
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
        if isinstance(args[0],_scalar.Scalar) or isinstance(args[0],_array.Array):
            ans=_data.makeData(abs(args[0].real)**2+abs(args[0].imag)**2)
            if isinstance(args[0],_scalar.Complex64):
                ans=_Float32(ans)
            elif isinstance(args[0],_scalar.Complex128):
                ans=_Float64(ans)
            elif isinstance(args[0],_array.Complex64Array):
                ans=_array.Float32Array(ans)
            elif isinstance(args[0],_array.Complex64Array):
                ans=_array.Float64Array(ans)
            else:
                ans=type(args[0])(ans)
            if hasattr(args[0],'_units'):
                ans=ans.setUnits(args[0].units+"*"+args[0].units)
        elif isinstance(args[0],_compound._Signal):
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
        if isinstance(args[0],_scalar.Scalar) or isinstance(args[0],_array.Array):
            ans = _data.makeData(arccos(args[0].value))
        elif isinstance(args[0],_compound._Signal):
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
        if isinstance(args[0],_scalar.Scalar) or isinstance(args[0],_array.Array):
            ans = type(args[0])(arccos(args[0].value)/pi*180.)
        elif isinstance(args[0],_compound._Signal):
            args[0].value=ACOSD(args[0].value).evaluate()
            ans = args[0]
        else:
            raise Exception("Invalid argument type for ABS function: %s" % (str(type(args[0])),))
        return ans
