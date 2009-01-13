#from dollar_value import dollar_value
import sys
import numpy as _N
import os

def parenthesize(obj,prec):
    from opc import opc
    if (isinstance(obj,opc)):
      if (obj.opcode.precedence < prec):
          ans = "(" + obj.opcode.str(obj.args) + ")"
      else:
          ans = obj.opcode.str(obj.args)
    else:
        ans = str(obj)
    return ans

def __array(args):
    if len(args) == 2:
        return _N.empty(args[0],type(args[1]))
    else:
        return _N.empty(args[0])

def __dim_of_str(args):
    if len(args)>1:
        return "dim_of("+str(args[0])+","+str(args[1])+")"
    else:
        return "dim_of("+str(args[0])+")"
            
class opcode(object):
    def __init__(self,name,str_code,evaluate_code,min_args,max_args,class_of=None,precedence=999):
        self.name=name
        self.str_code=compile(str_code,"mdsplus_function.py","eval")
        if evaluate_code is None:
            self.evaluate_code=None
        else:
            self.evaluate_code=compile(evaluate_code,"mdsplus_function.py","eval")
        self.min_args=min_args
        self.max_args=max_args
        self.class_of=class_of
        self.precedence=precedence

    def __str__(self):
        return "opcode-%s" % self.name

    def str(self,args):
        return eval(self.str_code)
    
    def evaluate(self,args):
        from tdishr import TdiExecute
        if self.evaluate_code is None:
            exp='TdiExecute("'+self.name+'('
            for i in range(len(args)-1):
                exp=exp+'$,'
            exp=exp+'$)",args)'
            return eval(exp)
        else:
            print "xxxxxx----"+str(self.name)
            return eval(self.evaluate_code)
    def check_args(self,args):
        if self.min_args != None and self.min_args > len(args):
            raise Exception("Insufficient arguments supplied. "+str(len(args))+" provided. Requires " + str(self.min_args))
        if self.max_args != None and self.max_args < len(args):
            raise Exception("Too many arguments supplied. "+str(len(args))+" provided. Max " + str(self.min_args))

    def __add__(self,other):
        try:
            other = other.evaluate()
        except AttributeError:
            pass
        return self.evaluate()+other

    __radd__=__add__

    def __sub__(self,other):
        try:
            other = other.evalute()
        except AttributeError:
            pass
        return self.evaluate()-other

    def __rsub__(self,other):
        try:
            other = other.evaluate()
        except AttributeError:
            pass
        return other-self.evaluate()

    def __mul__(self,other):
        try:
            other = other.evaluate()
        except AttributeError:
            pass
        return self.evaluate()*other
    __rmul__=__mul__

    def __div__(self,other):
        try:
            other = other.evaluate()
        except AttributeError:
            pass
        return self.evaluate()/other

    def __rdiv__(self,other):
        try:
            other = other.evaluate()
        except AttributeError:
            pass
        return other/self.evaluate()


opcodes_by_number={}
opcodes_by_name={}

def find_opcode(name):
    if not opcodes_by_number:
        opcodes_by_number[  0]=opcode("$",'"$"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[  1]=opcode("$A0",'"$A0"','build_with_units(build_with_error(52.9177E-12,2400E-21),"m")',0,0)
        opcodes_by_number[  2]=opcode("$ALPHA",'"$ALPHA"','build_with_error(7.29735308e-3,0.00000033e-3)',0,0)
        opcodes_by_number[  3]=opcode("$AMU",'"$AMU"','build_with_error(1.6605402e-27,0.0000010e-27)',0,0)
        opcodes_by_number[  4]=opcode("$C",'"$C"','build_with_units(299792458.,"m/s")',0,0)
        opcodes_by_number[  5]=opcode("$CAL",'"$CAL"','build_with_units(4.1868,"J")',0,0)
        opcodes_by_number[  6]=opcode("$DEGREE",'"$DEGREE"','.01745329252',0,0)
        opcodes_by_number[  7]=opcode("$EV",'"$EV"','build_with_units(build_with_error(1.60217733e-19,0.00000049e-19),"J/eV")',0,0)
        opcodes_by_number[  8]=opcode("$FALSE",'"$FALSE"','False',0,0)
        opcodes_by_number[  9]=opcode("$FARADAY",'"$FARADAY"','build_with_units(build_with_error(9.6485309e4,0.0000029e4),"C/mol")',0,0)
        opcodes_by_number[ 10]=opcode("$G",'"$G"','build_with_units(build_with_error(6.67259e-11,0.00085),"m^3/s^2/kg")',0,0)
        opcodes_by_number[ 11]=opcode("$GAS",'"$GAS"','build_with_units(build_with_error(8.314510,0.000070),"J/K/mol")',0,0)
        opcodes_by_number[ 12]=opcode("$H",'"$H"','build_with_units(build_with_error(6.6260755e-34,0.0000040),"J*s")',0,0)
        opcodes_by_number[ 13]=opcode("$HBAR",'"$HBAR"','build_with_units(build_with_error(1.05457266e-34,0.00000063),"J*s")',0,0)
        opcodes_by_number[ 14]=opcode("$I",'"$I"','1j',0,0)
        opcodes_by_number[ 15]=opcode("$K",'"$K"','build_with_units(build_with_error(1.380658e-23,0.000012e-23),"J/K")',0,0)
        opcodes_by_number[ 16]=opcode("$ME",'"$ME"','build_with_units(build_with_error(9.1093897e-31,0.0000054e-31),"kg")',0,0)
        opcodes_by_number[ 17]=opcode("$MISSING",'"$MISSING"','0',0,0)
        opcodes_by_number[ 18]=opcode("$MP",'"$MP"','build_with_units(build_with_error(1.6726231e-27,0.0000010e-27),"kg")',0,0)
        opcodes_by_number[ 19]=opcode("$N0",'"$N0"','build_with_units(build_with_error(2.686763e25,0.000023e25),"/m^3")',0,0)
        opcodes_by_number[ 20]=opcode("$NA",'"$NA"','build_with_units(build_with_error(6.0221367e23,0.0000036e23),"/mol")',0,0)
        opcodes_by_number[ 21]=opcode("$P0",'"$P0"','build_with_units(1.01325e5,"Pa")',0,0)
        opcodes_by_number[ 22]=opcode("$PI",'"$PI"','3.1415926536',0,0)
        opcodes_by_number[ 23]=opcode("$QE",'"$QE"','build_with_units(build_with_error(1.60217733e-19,0.000000493-19),"C")',0,0)
        opcodes_by_number[ 24]=opcode("$RE",'"$RE"','build_with_units(build_with_error(2.81794092e-15,0.00000038e-15),"m")',0,0)
        opcodes_by_number[ 25]=opcode("$ROPRAND",'"$ROPRAND"','_N.nan',0,0)
        opcodes_by_number[ 26]=opcode("$RYDBERG",'"$RYDBERG"','build_with_units(build_with_error(1.0973731534e7,0.0000000013e7),"/m")',0,0)
        opcodes_by_number[ 27]=opcode("$T0",'"$T0"','build_with_units(273.16,"K")',0,0)
        opcodes_by_number[ 28]=opcode("$TORR",'"$TORR"','build_with_units(1.3332e2,"Pa")',0,0)
        opcodes_by_number[ 29]=opcode("$TRUE",'"$TRUE"','True',0,0)
        opcodes_by_number[ 30]=opcode("$VALUE",'"$VALUE"','dollar_value.get()',0,0)
        opcodes_by_number[ 31]=opcode("ABORT",'"ABORT"','abort()',0,255)
        opcodes_by_number[ 32]=opcode("ABS",'"abs("+str(args[0])+")"','_N.abs(data(args[0]))',1,1)
        opcodes_by_number[ 33]=opcode("ABS1",'"ABS1"',None,1,1)################ NOT IMPLEMENTED
        opcodes_by_number[ 34]=opcode("ABSSQ",'"ABSSQ"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 35]=opcode("ACHAR",'"ACHAR"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 36]=opcode("ACOS",'"ACOS"','_N.arccos(data(args[0]))',1,1)
        opcodes_by_number[ 37]=opcode("ACOSD",'"ACOSD"','_N.arccos(data(args[0]))/_N.pi*180.',1,1)
        opcodes_by_number[ 38]=opcode("ADD",'parenthesize(args[0],0) + " + " + parenthesize(args[1],0)','args[0] + args[1]',2,2,precedence=0)
        opcodes_by_number[ 39]=opcode("ADJUSTL",'"ADJUSTL"','args[0].lstrip().ljust(len(args[0]))',1,1)
        opcodes_by_number[ 40]=opcode("ADJUSTR",'"ADJUSTR"',None,1,1)################ NOT IMPLEMENTED
        opcodes_by_number[ 41]=opcode("AIMAG",'"AIMAG"','_N.array(data(args[0])).imag',1,1)
        opcodes_by_number[ 42]=opcode("AINT",'"AINT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 43]=opcode("ALL",'"ALL"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 44]=opcode("ALLOCATED",'"ALLOCATED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 45]=opcode("AND",'"AND"','data(args[0]) & data(args[1])',2,2)
        opcodes_by_number[ 46]=opcode("AND_NOT",'"AND_NOT"','data(args[0]) & (~ data(args[1]))',2,2)
        opcodes_by_number[ 47]=opcode("ANINT",'"ANINT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 48]=opcode("ANY",'"ANY"','_N.data(args[0])',1,2)
        opcodes_by_number[ 49]=opcode("ARG",'"ARG"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 50]=opcode("ARGD",'"ARGD"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 51]=opcode("ARG_OF",'"ARG_OF"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 52]=opcode("ARRAY",'"ARRAY"','__array(args)',0,2)
        opcodes_by_number[ 53]=opcode("ASIN",'"ASIN"','_N.arcsin(data(args[0]))',1,1)
        opcodes_by_number[ 54]=opcode("ASIND",'"ASIND"','_N.arcsin(data(args[0]))/_N.pi*180.',1,1)
        opcodes_by_number[ 55]=opcode("AS_IS",'"AS_IS"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 56]=opcode("ATAN",'"ATAN"','_N.arctan(data(args[0]))',1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 57]=opcode("ATAN2",'"ATAN2"','_N.arctan2(data(args[0]),data(args[1]))',2,2)
        opcodes_by_number[ 58]=opcode("ATAN2D",'"ATAN2D"','_N.arctan2(data(args[0]),data(args[1]))/_N.pi*180.',2,2)
        opcodes_by_number[ 59]=opcode("ATAND",'"ATAND"','_N.arctan(data(args[0]))/_N.pi*180.',1,1)
        opcodes_by_number[ 60]=opcode("ATANH",'"ATANH"','_N.arctanh(data(args[0]))',1,1)
        opcodes_by_number[ 61]=opcode("AXIS_OF",'"AXIS_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 62]=opcode("BACKSPACE",'"BACKSPACE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 63]=opcode("IBCLR",'"IBCLR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 64]=opcode("BEGIN_OF",'"BEGIN_OF"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 65]=opcode("IBITS",'"IBITS"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[ 66]=opcode("BREAK",'"BREAK"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[ 67]=opcode("BSEARCH",'"BSEARCH"',None,2,4) ################ NOT IMPLEMENTED
        opcodes_by_number[ 68]=opcode("IBSET",'"IBSET"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 69]=opcode("BTEST",'"BTEST"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 70]=opcode("BUILD_ACTION",'"BUILD_ACTION"',None,2,5) ################ NOT IMPLEMENTED
        opcodes_by_number[ 71]=opcode("BUILD_CONDITION",'"BUILD_CONDITION"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 72]=opcode("BUILD_CONGLOM",'"BUILD_CONGLOM"',None,4,4) ################ NOT IMPLEMENTED
        opcodes_by_number[ 73]=opcode("BUILD_DEPENDENCY",'"BUILD_DEPENDENCY"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[ 74]=opcode("BUILD_DIM",'"BUILD_DIM"','build_dim(*args)',2,2)
        opcodes_by_number[ 75]=opcode("BUILD_DISPATCH",'"BUILD_DISPATCH"',None,5,5) ################ NOT IMPLEMENTED
        opcodes_by_number[ 76]=opcode("BUILD_EVENT",'"BUILD_EVENT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 77]=opcode("BUILD_FUNCTION",'"BUILD_FUNCTION"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 78]=opcode("BUILD_METHOD",'"BUILD_METHOD"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 79]=opcode("BUILD_PARAM",'"BUILD_PARAM"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[ 80]=opcode("BUILD_PATH",'"BUILD_PATH"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 81]=opcode("BUILD_PROCEDURE",'"BUILD_PROCEDURE"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 82]=opcode("BUILD_PROGRAM",'"BUILD_PROGRAM"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 83]=opcode("BUILD_RANGE",'"BUILD_RANGE"',None,2,3)
        opcodes_by_number[ 84]=opcode("BUILD_ROUTINE",'"BUILD_ROUTINE"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 85]=opcode("BUILD_SIGNAL",'"BUILD_SIGNAL"',None,2,10)
        opcodes_by_number[ 86]=opcode("BUILD_SLOPE",'"BUILD_SLOPE"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 87]=opcode("BUILD_WINDOW",'"BUILD_WINDOW"','build_window(*args)',3,3)
        opcodes_by_number[ 88]=opcode("BUILD_WITH_UNITS",'"BUILD_WITH_UNITS"','build_with_units(*args)',2,2)
        opcodes_by_number[ 89]=opcode("BUILTIN_OPCODE",'"BUILTIN_OPCODE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 90]=opcode("BYTE",'"BYTE"','_N.int8(data(args[0]))',1,1)
        opcodes_by_number[ 91]=opcode("BYTE_UNSIGNED",'"BYTE_UNSIGNED"','_N.uint8(data(args[0]))',1,1)
        opcodes_by_number[ 92]=opcode("CASE",'"CASE"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 93]=opcode("CEILING",'"CEILING"','_N.ceil(data(args[0]))',1,1)
        opcodes_by_number[ 94]=opcode("CHAR",'"CHAR"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[ 95]=opcode("CLASS",'"CLASS"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 96]=opcode("FCLOSE",'"FCLOSE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[ 97]=opcode("CMPLX",'"CMPLX"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[ 98]=opcode("COMMA",'"COMMA"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[ 99]=opcode("COMPILE",'"COMPILE"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[100]=opcode("COMPLETION_OF",'"COMPLETION_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[101]=opcode("CONCAT",'"CONCAT"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[102]=opcode("CONDITIONAL",'"CONDITIONAL"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[103]=opcode("CONJG",'"CONJG"','_N.conjugate(data(args[0]))',1,1)
        opcodes_by_number[104]=opcode("CONTINUE",'"CONTINUE"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[105]=opcode("CONVOLVE",'"CONVOLVE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[106]=opcode("COS",'"COS"','_N.cos(data(args[0]))',1,1)
        opcodes_by_number[107]=opcode("COSD",'"COSD"','_N.cos(data(args[0])*_N.pi/180.)',1,1)
        opcodes_by_number[108]=opcode("COSH",'"COSH"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[109]=opcode("COUNT",'"COUNT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[110]=opcode("CSHIFT",'"CSHIFT"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[111]=opcode("CVT",'"CVT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[112]=opcode("DATA",'"DATA"',None,1,1)
        opcodes_by_number[113]=opcode("DATE_AND_TIME",'"DATE_AND_TIME"',None,0,1) ################ NOT IMPLEMENTED
        opcodes_by_number[114]=opcode("DATE_TIME",'"DATE_TIME"',None,0,1) ################ NOT IMPLEMENTED
        opcodes_by_number[115]=opcode("DBLE",'"DBLE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[116]=opcode("DEALLOCATE",'"DEALLOCATE"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[117]=opcode("DEBUG",'"DEBUG"',None,0,1) ################ NOT IMPLEMENTED
        opcodes_by_number[118]=opcode("DECODE",'"DECODE"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[119]=opcode("DECOMPILE",'"DECOMPILE"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[120]=opcode("DECOMPRESS",'"DECOMPRESS"',None,4,4) ################ NOT IMPLEMENTED
        opcodes_by_number[121]=opcode("DEFAULT",'"DEFAULT"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[122]=opcode("DERIVATIVE",'"DERIVATIVE"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[123]=opcode("DESCR",'"DESCR"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[124]=opcode("DIAGONAL",'"DIAGONAL"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[125]=opcode("DIGITS",'"DIGITS"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[126]=opcode("DIM",'"DIM"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[127]=opcode("DIM_OF",'__dim_of_str(args)',None,1,2) ####### NOT IMPLEMENTED
        opcodes_by_number[128]=opcode("DISPATCH_OF",'"DISPATCH_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[129]=opcode("DIVIDE",'parenthesize(args[0],1)+" / "+parenthesize(args[1],1)','args[0]/args[1]',2,2,precedence=1)
        opcodes_by_number[130]=opcode("LBOUND",'"LBOUND"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[131]=opcode("DO",'"DO"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[132]=opcode("DOT_PRODUCT",'"DOT_PRODUCT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[133]=opcode("DPROD",'"DPROD"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[134]=opcode("DSCPTR",'"DSCPTR"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[135]=opcode("SHAPE",'"SHAPE"','_N.array(_N.shape(data(args[0])))',1,2)
        opcodes_by_number[136]=opcode("SIZE",'"SIZE"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[137]=opcode("KIND",'"KIND"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[138]=opcode("UBOUND",'"UBOUND"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[139]=opcode("D_COMPLEX",'"D_COMPLEX"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[140]=opcode("D_FLOAT",'"D_FLOAT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[141]=opcode("RANGE",'"RANGE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[142]=opcode("PRECISION",'"PRECISION"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[143]=opcode("ELBOUND",'"ELBOUND"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[144]=opcode("ELSE",'"ELSE"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[145]=opcode("ELSEWHERE",'"ELSEWHERE"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[146]=opcode("ENCODE",'"ENCODE"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[147]=opcode("ENDFILE",'"ENDFILE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[148]=opcode("END_OF",'"END_OF"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[149]=opcode("EOSHIFT",'"EOSHIFT"',None,3,4) ################ NOT IMPLEMENTED
        opcodes_by_number[150]=opcode("EPSILON",'"EPSILON"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[151]=opcode("EQ",'"EQ"','data(args[0])==data(args[1])',2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[152]=opcode("EQUALS",'"EQUALS"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[153]=opcode("EQUALS_FIRST",'"EQUALS_FIRST"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[154]=opcode("EQV",'"EQV"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[155]=opcode("ESHAPE",'"ESHAPE"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[156]=opcode("ESIZE",'"ESIZE"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[157]=opcode("EUBOUND",'"EUBOUND"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[158]=opcode("EVALUATE",'"EVALUATE"','evaluate(args[0])',1,1)
        opcodes_by_number[159]=opcode("EXECUTE",'"EXECUTE"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[160]=opcode("EXP",'"EXP"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[161]=opcode("EXPONENT",'"EXPONENT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[162]=opcode("EXT_FUNCTION",'"EXT_FUNCTION"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[163]=opcode("FFT",'"FFT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[164]=opcode("FIRSTLOC",'"FIRSTLOC"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[165]=opcode("FIT",'"FIT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[166]=opcode("FIX_ROPRAND",'"FIX_ROPRAND"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[167]=opcode("FLOAT",'"FLOAT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[168]=opcode("FLOOR",'"FLOOR"','_N.floor(data(args[0]))',1,1)
        opcodes_by_number[169]=opcode("FOR",'"FOR"',None,4,254) ################ NOT IMPLEMENTED
        opcodes_by_number[170]=opcode("FRACTION",'"FRACTION"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[171]=opcode("FUN",'"FUN"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[172]=opcode("F_COMPLEX",'"F_COMPLEX"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[173]=opcode("F_FLOAT",'"F_FLOAT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[174]=opcode("GE",'"GE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[175]=opcode("GETNCI",'"GETNCI"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[176]=opcode("GOTO",'"GOTO"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[177]=opcode("GT",'"GT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[178]=opcode("G_COMPLEX",'"G_COMPLEX"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[179]=opcode("G_FLOAT",'"G_FLOAT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[180]=opcode("HELP_OF",'"HELP_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[181]=opcode("HUGE",'"HUGE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[182]=opcode("H_COMPLEX",'"H_COMPLEX"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[183]=opcode("H_FLOAT",'"H_FLOAT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[184]=opcode("IACHAR",'"IACHAR"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[185]=opcode("IAND",'"IAND"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[186]=opcode("IAND_NOT",'"IAND_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[187]=opcode("ICHAR",'"ICHAR"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[188]=opcode("IDENT_OF",'"IDENT_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[189]=opcode("IF",'"IF"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[190]=opcode("IF_ERROR",'"IF_ERROR"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[191]=opcode("IMAGE_OF",'"IMAGE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[192]=opcode("IN",'"IN"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[193]=opcode("INAND",'"INAND"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[194]=opcode("INAND_NOT",'"INAND_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[195]=opcode("INDEX",'"INDEX"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[196]=opcode("INOR",'"INOR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[197]=opcode("INOR_NOT",'"INOR_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[198]=opcode("INOT",'"INOT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[199]=opcode("INOUT",'"INOUT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[200]=opcode("INQUIRE",'"INQUIRE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[201]=opcode("INT",'"INT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[202]=opcode("INTEGRAL",'"INTEGRAL"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[203]=opcode("INTERPOL",'"INTERPOL"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[204]=opcode("INTERSECT",'"INTERSECT"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[205]=opcode("INT_UNSIGNED",'"INT_UNSIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[206]=opcode("INVERSE",'"INVERSE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[207]=opcode("IOR",'"IOR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[208]=opcode("IOR_NOT",'"IOR_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[209]=opcode("IS_IN",'"IS_IN"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[210]=opcode("IEOR",'"IEOR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[211]=opcode("IEOR_NOT",'"IEOR_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[212]=opcode("LABEL",'"LABEL"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[213]=opcode("LAMINATE",'"LAMINATE"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[214]=opcode("LANGUAGE_OF",'"LANGUAGE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[215]=opcode("LASTLOC",'"LASTLOC"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[216]=opcode("LE",'"LE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[217]=opcode("LEN",'"LEN"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[218]=opcode("LEN_TRIM",'"LEN_TRIM"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[219]=opcode("LGE",'"LGE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[220]=opcode("LGT",'"LGT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[221]=opcode("LLE",'"LLE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[222]=opcode("LLT",'"LLT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[223]=opcode("LOG",'"LOG"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[224]=opcode("LOG10",'"LOG10"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[225]=opcode("LOG2",'"LOG2"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[226]=opcode("LOGICAL",'"LOGICAL"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[227]=opcode("LONG",'"LONG"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[228]=opcode("LONG_UNSIGNED",'"LONG_UNSIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[229]=opcode("LT",'"LT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[230]=opcode("MATMUL",'"MATMUL"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[231]=opcode("MAT_ROT",'"MAT_ROT"',None,2,5) ################ NOT IMPLEMENTED
        opcodes_by_number[232]=opcode("MAT_ROT_INT",'"MAT_ROT_INT"',None,2,5) ################ NOT IMPLEMENTED
        opcodes_by_number[233]=opcode("MAX",'"MAX"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[234]=opcode("MAXEXPONENT",'"MAXEXPONENT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[235]=opcode("MAXLOC",'"MAXLOC"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[236]=opcode("MAXVAL",'"MAXVAL"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[237]=opcode("MEAN",'"MEAN"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[238]=opcode("MEDIAN",'"MEDIAN"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[239]=opcode("MERGE",'"MERGE"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[240]=opcode("METHOD_OF",'"METHOD_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[241]=opcode("MIN",'"MIN"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[242]=opcode("MINEXPONENT",'"MINEXPONENT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[243]=opcode("MINLOC",'"MINLOC"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[244]=opcode("MINVAL",'"MINVAL"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[245]=opcode("MOD",'"MOD"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[246]=opcode("MODEL_OF",'"MODEL_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[247]=opcode("MULTIPLY",'parenthesize(args[0],1)+" * "+parenthesize(args[1],1)','args[0]*args[1]',2,2,precedence=1)
        opcodes_by_number[248]=opcode("NAME_OF",'"NAME_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[249]=opcode("NAND",'"NAND"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[250]=opcode("NAND_NOT",'"NAND_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[251]=opcode("NDESC",'"NDESC"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[252]=opcode("NE",'"NE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[253]=opcode("NEAREST",'"NEAREST"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[254]=opcode("NEQV",'"NEQV"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[255]=opcode("NINT",'"NINT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[256]=opcode("NOR",'"NOR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[257]=opcode("NOR_NOT",'"NOR_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[258]=opcode("NOT",'"NOT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[259]=opcode("OBJECT_OF",'"OBJECT_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[260]=opcode("OCTAWORD",'"OCTAWORD"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[261]=opcode("OCTAWORD_UNSIGNED",'"OCTAWORD_UNSIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[262]=opcode("ON_ERROR",'"ON_ERROR"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[263]=opcode("OPCODE_BUILTIN",'"OPCODE_BUILTIN"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[264]=opcode("OPCODE_STRING",'"OPCODE_STRING("+str(args[0])+")"','opcodes_by_number[args[0]].name',1,1)
        opcodes_by_number[265]=opcode("FOPEN",'"FOPEN"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[266]=opcode("OPTIONAL",'"OPTIONAL"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[267]=opcode("OR",'"OR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[268]=opcode("OR_NOT",'"OR_NOT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[269]=opcode("OUT",'"OUT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[270]=opcode("PACK",'"PACK"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[271]=opcode("PHASE_OF",'"PHASE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[272]=opcode("POST_DEC",'"POST_DEC"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[273]=opcode("POST_INC",'"POST_INC"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[274]=opcode("POWER",'"POWER"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[275]=opcode("PRESENT",'"PRESENT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[276]=opcode("PRE_DEC",'"PRE_DEC"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[277]=opcode("PRE_INC",'"PRE_INC"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[278]=opcode("PRIVATE",'"PRIVATE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[279]=opcode("PROCEDURE_OF",'"PROCEDURE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[280]=opcode("PRODUCT",'"PRODUCT"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[281]=opcode("PROGRAM_OF",'"PROGRAM_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[282]=opcode("PROJECT",'"PROJECT"',None,3,4) ################ NOT IMPLEMENTED
        opcodes_by_number[283]=opcode("PROMOTE",'"PROMOTE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[284]=opcode("PUBLIC",'"PUBLIC"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[285]=opcode("QUADWORD",'"QUADWORD"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[286]=opcode("QUADWORD_UNSIGNED",'"QUADWORD_UNSIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[287]=opcode("QUALIFIERS_OF",'"QUALIFIERS_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[288]=opcode("RADIX",'"RADIX"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[289]=opcode("RAMP",'"RAMP"',None,0,2) ################ NOT IMPLEMENTED
        opcodes_by_number[290]=opcode("RANDOM",'"RANDOM("+str(args[0])+")"','rand(data(args[0]))',0,2)
        opcodes_by_number[291]=opcode("RANDOM_SEED",'"RANDOM_SEED"',None,0,1) ################ NOT IMPLEMENTED
        opcodes_by_number[292]=opcode("DTYPE_RANGE",'"DTYPE_RANGE"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[293]=opcode("RANK",'"RANK"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[294]=opcode("RAW_OF",'"RAW_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[295]=opcode("READ",'"READ"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[296]=opcode("REAL",'"REAL"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[297]=opcode("REBIN",'"REBIN"',None,2,4) ################ NOT IMPLEMENTED
        opcodes_by_number[298]=opcode("REF",'"REF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[299]=opcode("REPEAT",'"REPEAT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[300]=opcode("REPLICATE",'"REPLICATE"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[301]=opcode("RESHAPE",'"RESHAPE"',None,2,4) ################ NOT IMPLEMENTED
        opcodes_by_number[302]=opcode("RETURN",'"RETURN"',None,0,1) ################ NOT IMPLEMENTED
        opcodes_by_number[303]=opcode("REWIND",'"REWIND"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[304]=opcode("RMS",'"RMS"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[305]=opcode("ROUTINE_OF",'"ROUTINE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[306]=opcode("RRSPACING",'"RRSPACING"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[307]=opcode("SCALE",'"SCALE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[308]=opcode("SCAN",'"SCAN"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[309]=opcode("FSEEK",'"FSEEK"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[310]=opcode("SET_EXPONENT",'"SET_EXPONENT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[311]=opcode("SET_RANGE",'"SET_RANGE"',None,2,9) ################ NOT IMPLEMENTED
        opcodes_by_number[312]=opcode("ISHFT",'"ISHFT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[313]=opcode("ISHFTC",'"ISHFTC"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[314]=opcode("SHIFT_LEFT",'"SHIFT_LEFT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[315]=opcode("SHIFT_RIGHT",'"SHIFT_RIGHT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[316]=opcode("SIGN",'"SIGN"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[317]=opcode("SIGNED",'"SIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[318]=opcode("SIN",'"sin(" + str(args[0]) + ")"','_N.sin(data(args[0]))',1,1)
        opcodes_by_number[319]=opcode("SIND",'"sind(" + str(args[0]) + ")"','_N.sin(data(args[0])*_N.pi/180.)',1,1)
        opcodes_by_number[320]=opcode("SINH",'"SINH"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[321]=opcode("SIZEOF",'"SIZEOF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[322]=opcode("SLOPE_OF",'"SLOPE_OF"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[323]=opcode("SMOOTH",'"SMOOTH"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[324]=opcode("SOLVE",'"SOLVE"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[325]=opcode("SORTVAL",'"SORTVAL"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[326]=opcode("SPACING",'"SPACING"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[327]=opcode("SPAWN",'"SPAWN"',None,0,3) ################ NOT IMPLEMENTED
        opcodes_by_number[328]=opcode("SPREAD",'"SPREAD"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[329]=opcode("SQRT",'"SQRT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[330]=opcode("SQUARE",'"SQUARE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[331]=opcode("STATEMENT",'"STATEMENT"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[332]=opcode("STD_DEV",'"STD_DEV"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[333]=opcode("STRING",'"STRING"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[334]=opcode("STRING_OPCODE",'"STRING_OPCODE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[335]=opcode("SUBSCRIPT",'"SUBSCRIPT"',None,1,9) ################ NOT IMPLEMENTED
        opcodes_by_number[336]=opcode("SUBTRACT",'"SUBTRACT"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[337]=opcode("SUM",'"SUM"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[338]=opcode("SWITCH",'"SWITCH"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[339]=opcode("SYSTEM_CLOCK",'"SYSTEM_CLOCK"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[340]=opcode("TAN",'"TAN"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[341]=opcode("TAND",'"TAND"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[342]=opcode("TANH",'"TANH"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[343]=opcode("TASK_OF",'"TASK_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[344]=opcode("TEXT",'"TEXT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[345]=opcode("TIME_OUT_OF",'"TIME_OUT_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[346]=opcode("TINY",'"TINY"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[347]=opcode("TRANSFER",'"TRANSFER"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[348]=opcode("TRANSPOSE_",'"TRANSPOSE_"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[349]=opcode("TRIM",'"TRIM"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[350]=opcode("UNARY_MINUS",'"UNARY_MINUS"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[351]=opcode("UNARY_PLUS",'"UNARY_PLUS"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[352]=opcode("UNION",'"UNION"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[353]=opcode("UNITS",'"units("+str(args[0])+")"','units(args[0])',1,1)
        opcodes_by_number[354]=opcode("UNITS_OF",'"units_of("+str(args[0])+")"','units_of(args[0])',1,1)
        opcodes_by_number[355]=opcode("UNPACK",'"UNPACK"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[356]=opcode("UNSIGNED",'"UNSIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[357]=opcode("VAL",'"VAL"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[358]=opcode("VALIDATION_OF",'"VALIDATION_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[359]=opcode("VALUE_OF",'"value_of("+str(args[0])+")"','value_of(args[0])',1,1)
        opcodes_by_number[360]=opcode("VAR",'"VAR"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[361]=opcode("VECTOR",'"VECTOR"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[362]=opcode("VERIFY",'"VERIFY"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[363]=opcode("WAIT",'"WAIT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[364]=opcode("WHEN_OF",'"WHEN_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[365]=opcode("WHERE",'"WHERE"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[366]=opcode("WHILE",'"WHILE"',None,2,254) ################ NOT IMPLEMENTED
        opcodes_by_number[367]=opcode("WINDOW_OF",'"WINDOW_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[368]=opcode("WORD",'"WORD"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[369]=opcode("WORD_UNSIGNED",'"WORD_UNSIGNED"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[370]=opcode("WRITE",'"WRITE"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[371]=opcode("ZERO",'"ZERO"',None,0,2) ################ NOT IMPLEMENTED
        opcodes_by_number[372]=opcode("$2PI",'"$2PI"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[373]=opcode("$NARG",'"$NARG"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[374]=opcode("ELEMENT",'"ELEMENT"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[375]=opcode("RC_DROOP",'"RC_DROOP"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[376]=opcode("RESET_PRIVATE",'"RESET_PRIVATE"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[377]=opcode("RESET_PUBLIC",'"RESET_PUBLIC"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[378]=opcode("SHOW_PRIVATE",'"SHOW_PRIVATE"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[379]=opcode("SHOW_PUBLIC",'"SHOW_PUBLIC"',None,0,254) ################ NOT IMPLEMENTED
        opcodes_by_number[380]=opcode("SHOW_VM",'"SHOW_VM"',None,0,2) ################ NOT IMPLEMENTED
        opcodes_by_number[381]=opcode("TRANSLATE",'"TRANSLATE"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[382]=opcode("TRANSPOSE_MUL",'"TRANSPOSE_MUL"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[383]=opcode("UPCASE",'"UPCASE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[384]=opcode("USING",'"USING"',None,2,4) ################ NOT IMPLEMENTED
        opcodes_by_number[385]=opcode("VALIDATION",'"VALIDATION"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[386]=opcode("$DEFAULT",'"$DEFAULT"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[387]=opcode("$EXPT",'"$EXPT"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[388]=opcode("$SHOT",'"$SHOT"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[389]=opcode("GETDBI",'"GETDBI"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[390]=opcode("CULL",'"CULL"',None,1,4) ################ NOT IMPLEMENTED
        opcodes_by_number[391]=opcode("EXTEND",'"EXTEND"',None,1,4) ################ NOT IMPLEMENTED
        opcodes_by_number[392]=opcode("I_TO_X",'"I_TO_X"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[393]=opcode("X_TO_I",'"X_TO_I"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[394]=opcode("MAP",'"MAP"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[395]=opcode("COMPILE_DEPENDENCY",'"COMPILE_DEPENDENCY"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[396]=opcode("DECOMPILE_DEPENDENCY",'"DECOMPILE_DEPENDENCY"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[397]=opcode("BUILD_CALL",'"BUILD_CALL"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[398]=opcode("ERRORLOGS_OF",'"ERRORLOGS_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[399]=opcode("PERFORMANCE_OF",'"PERFORMANCE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[400]=opcode("XD",'"XD"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[401]=opcode("CONDITION_OF",'"CONDITION_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[402]=opcode("SORT",'"SORT"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[403]=opcode("$THIS",'"$THIS"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[404]=opcode("DATA_WITH_UNITS",'"DATA_WITH_UNITS"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[405]=opcode("$ATM",'"$ATM"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[406]=opcode("$EPSILON0",'"$EPSILON0"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[407]=opcode("$GN",'"$GN"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[408]=opcode("$MU0",'"$MU0"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[409]=opcode("EXTRACT",'"EXTRACT"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[410]=opcode("FINITE",'"FINITE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[411]=opcode("BIT_SIZE",'"BIT_SIZE"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[412]=opcode("MODULO",'"MODULO"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[413]=opcode("SELECTED_INT_KIND",'"SELECTED_INT_KIND"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[414]=opcode("SELECTED_REAL_KIND",'"SELECTED_REAL_KIND"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[415]=opcode("DSQL",'"DSQL"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[416]=opcode("ISQL",'"ISQL"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[417]=opcode("FTELL",'"FTELL"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[418]=opcode("MAKE_ACTION",'"MAKE_ACTION"',None,2,5) ################ NOT IMPLEMENTED
        opcodes_by_number[419]=opcode("MAKE_CONDITION",'"MAKE_CONDITION"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[420]=opcode("MAKE_CONGLOM",'"MAKE_CONGLOM"',None,4,4) ################ NOT IMPLEMENTED
        opcodes_by_number[421]=opcode("MAKE_DEPENDENCY",'"MAKE_DEPENDENCY"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[422]=opcode("MAKE_DIM",'"MAKE_DIM"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[423]=opcode("MAKE_DISPATCH",'"MAKE_DISPATCH"',None,5,5) ################ NOT IMPLEMENTED
        opcodes_by_number[424]=opcode("MAKE_FUNCTION",'"MAKE_FUNCTION"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[425]=opcode("MAKE_METHOD",'"MAKE_METHOD"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[426]=opcode("MAKE_PARAM",'"MAKE_PARAM"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[427]=opcode("MAKE_PROCEDURE",'"MAKE_PROCEDURE"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[428]=opcode("MAKE_PROGRAM",'"MAKE_PROGRAM"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[429]=opcode("MAKE_RANGE",'"MAKE_RANGE"',None,2,3) ################ NOT IMPLEMENTED
        opcodes_by_number[430]=opcode("MAKE_ROUTINE",'"MAKE_ROUTINE"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[431]=opcode("MAKE_SIGNAL",'"MAKE_SIGNAL"',None,2,10) ################ NOT IMPLEMENTED
        opcodes_by_number[432]=opcode("MAKE_WINDOW",'"MAKE_WINDOW"',None,3,3) ################ NOT IMPLEMENTED
        opcodes_by_number[433]=opcode("MAKE_WITH_UNITS",'"MAKE_WITH_UNITS"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[434]=opcode("MAKE_CALL",'"MAKE_CALL"',None,3,254) ################ NOT IMPLEMENTED
        opcodes_by_number[435]=opcode("CLASS_OF",'"CLASS_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[436]=opcode("DSCPTR_OF",'"DSCPTR_OF"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[437]=opcode("KIND_OF",'"KIND_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[438]=opcode("NDESC_OF",'"NDESC_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[439]=opcode("ACCUMULATE",'"ACCUMULATE"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[440]=opcode("MAKE_SLOPE",'"MAKE_SLOPE"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[441]=opcode("REM",'"REM"',None,1,254) ################ NOT IMPLEMENTED
        opcodes_by_number[442]=opcode("COMPLETION_MESSAGE_OF",'"COMPLETION_MESSAGE_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[443]=opcode("INTERRUPT_OF",'"INTERRUPT_OF"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[444]=opcode("$SHOTNAME",'"$SHOTNAME"',None,0,0) ################ NOT IMPLEMENTED
        opcodes_by_number[445]=opcode("BUILD_WITH_ERROR",'"BUILD_WITH_ERROR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[446]=opcode("ERROR_OF",'"error_of("+str(args[0])+")"','error_of(args[0])',1,1)
        opcodes_by_number[447]=opcode("MAKE_WITH_ERROR",'"MAKE_WITH_ERROR"',None,2,2) ################ NOT IMPLEMENTED
        opcodes_by_number[448]=opcode("DO_TASK",'"DO_TASK"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[449]=opcode("ISQL_SET",'"ISQL_SET"',None,1,3) ################ NOT IMPLEMENTED
        opcodes_by_number[450]=opcode("FS_FLOAT",'"FS_FLOAT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[451]=opcode("FS_COMPLEX",'"FS_COMPLEX"',None,1,2) ################ NOT IMPLEMENTED
        opcodes_by_number[452]=opcode("FT_FLOAT",'"FT_FLOAT"',None,1,1) ################ NOT IMPLEMENTED
        opcodes_by_number[453]=opcode("FT_COMPLEX",'"FT_COMPLEX"',None,1,2) ################ NOT IMPLEMENTED
        for opc in opcodes_by_number:
            opcodes_by_name[opcodes_by_number[opc].name]=opcodes_by_number[opc]
            opcodes_by_number[opc].number=opc
            opcodes_by_number[opc].number=opc

    try:
        if isinstance(name,str):
            return opcodes_by_name[name.upper().strip()]
        else:
            return opcodes_by_number[name]
    except KeyError:
        return None
            
            
def f_float(*args):
    import sys
    print "Name = %s" %__name__
    print "Dict = %s" % sys.modules[__name__].__dict__
    return find_opcode('f_float').evaluate(args)

