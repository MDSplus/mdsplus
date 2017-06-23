def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_builtin=_mimport('builtin')
Builtin=_builtin.Builtin

class dPLACEHOLDER(Builtin):
    min_args=0
    max_args=0
    name='$'
    opcode=0
dPLACEHOLDER.__name__='$'

class dAMU(Builtin):
    min_args=0
    max_args=0
    name='$AMU'
    opcode=3
dAMU.__name__='$AMU'

class dHBAR(Builtin):
    min_args=0
    max_args=0
    name='$HBAR'
    opcode=13
dHBAR.__name__='$HBAR'

class dVALUE(Builtin):
    min_args=0
    max_args=0
    name='$VALUE'
    opcode=30
dVALUE.__name__='$VALUE'

class ABORT(Builtin):
    min_args=0
    max_args=255
    name='ABORT'
    opcode=31
ABORT.__name__='ABORT'

class ACHAR(Builtin):
    min_args=1
    max_args=2
    name='ACHAR'
    opcode=35
ACHAR.__name__='ACHAR'

class ADD(Builtin):
    min_args=2
    max_args=2
    name='ADD'
    opcode=38
ADD.__name__='ADD'

class ADJUSTL(Builtin):
    min_args=1
    max_args=1
    name='ADJUSTL'
    opcode=39
ADJUSTL.__name__='ADJUSTL'

class ADJUSTR(Builtin):
    min_args=1
    max_args=1
    name='ADJUSTR'
    opcode=40
ADJUSTR.__name__='ADJUSTR'

class AIMAG(Builtin):
    min_args=1
    max_args=1
    name='AIMAG'
    opcode=41
AIMAG.__name__='AIMAG'

class AINT(Builtin):
    min_args=1
    max_args=2
    name='AINT'
    opcode=42
AINT.__name__='AINT'

class ALL(Builtin):
    min_args=1
    max_args=2
    name='ALL'
    opcode=43
ALL.__name__='ALL'

class ALLOCATED(Builtin):
    min_args=1
    max_args=1
    name='ALLOCATED'
    opcode=44
ALLOCATED.__name__='ALLOCATED'

class AND(Builtin):
    min_args=2
    max_args=2
    name='AND'
    opcode=45
AND.__name__='AND'

class AND_NOT(Builtin):
    min_args=2
    max_args=2
    name='AND_NOT'
    opcode=46
AND_NOT.__name__='AND_NOT'

class ANINT(Builtin):
    min_args=1
    max_args=2
    name='ANINT'
    opcode=47
ANINT.__name__='ANINT'

class ANY(Builtin):
    min_args=1
    max_args=2
    name='ANY'
    opcode=48
ANY.__name__='ANY'

class ARG(Builtin):
    min_args=1
    max_args=1
    name='ARG'
    opcode=49
ARG.__name__='ARG'

class ARGD(Builtin):
    min_args=1
    max_args=1
    name='ARGD'
    opcode=50
ARGD.__name__='ARGD'

class ARG_OF(Builtin):
    min_args=1
    max_args=2
    name='ARG_OF'
    opcode=51
ARG_OF.__name__='ARG_OF'

class ARRAY(Builtin):
    min_args=0
    max_args=2
    name='ARRAY'
    opcode=52
ARRAY.__name__='ARRAY'

class ASIN(Builtin):
    min_args=1
    max_args=1
    name='ASIN'
    opcode=53
ASIN.__name__='ASIN'

class ASIND(Builtin):
    min_args=1
    max_args=1
    name='ASIND'
    opcode=54
ASIND.__name__='ASIND'

class AS_IS(Builtin):
    min_args=1
    max_args=1
    name='AS_IS'
    opcode=55
AS_IS.__name__='AS_IS'

class ATAN(Builtin):
    min_args=1
    max_args=1
    name='ATAN'
    opcode=56
ATAN.__name__='ATAN'

class ATAN2(Builtin):
    min_args=2
    max_args=2
    name='ATAN2'
    opcode=57
ATAN2.__name__='ATAN2'

class ATAN2D(Builtin):
    min_args=2
    max_args=2
    name='ATAN2D'
    opcode=58
ATAN2D.__name__='ATAN2D'

class ATAND(Builtin):
    min_args=1
    max_args=1
    name='ATAND'
    opcode=59
ATAND.__name__='ATAND'

class ATANH(Builtin):
    min_args=1
    max_args=1
    name='ATANH'
    opcode=60
ATANH.__name__='ATANH'

class AXIS_OF(Builtin):
    min_args=1
    max_args=1
    name='AXIS_OF'
    opcode=61
AXIS_OF.__name__='AXIS_OF'

class BACKSPACE(Builtin):
    min_args=1
    max_args=1
    name='BACKSPACE'
    opcode=62
BACKSPACE.__name__='BACKSPACE'

class IBCLR(Builtin):
    min_args=2
    max_args=2
    name='IBCLR'
    opcode=63
IBCLR.__name__='IBCLR'

class BEGIN_OF(Builtin):
    min_args=1
    max_args=2
    name='BEGIN_OF'
    opcode=64
BEGIN_OF.__name__='BEGIN_OF'

class IBITS(Builtin):
    min_args=3
    max_args=3
    name='IBITS'
    opcode=65
IBITS.__name__='IBITS'

class BREAK(Builtin):
    min_args=0
    max_args=0
    name='BREAK'
    opcode=66
BREAK.__name__='BREAK'

class BSEARCH(Builtin):
    min_args=2
    max_args=4
    name='BSEARCH'
    opcode=67
BSEARCH.__name__='BSEARCH'

class IBSET(Builtin):
    min_args=2
    max_args=2
    name='IBSET'
    opcode=68
IBSET.__name__='IBSET'

class BTEST(Builtin):
    min_args=2
    max_args=2
    name='BTEST'
    opcode=69
BTEST.__name__='BTEST'

class BUILD_ACTION(Builtin):
    min_args=2
    max_args=5
    name='BUILD_ACTION'
    opcode=70
BUILD_ACTION.__name__='BUILD_ACTION'

class BUILD_CONDITION(Builtin):
    min_args=2
    max_args=2
    name='BUILD_CONDITION'
    opcode=71
BUILD_CONDITION.__name__='BUILD_CONDITION'

class BUILD_CONGLOM(Builtin):
    min_args=4
    max_args=4
    name='BUILD_CONGLOM'
    opcode=72
BUILD_CONGLOM.__name__='BUILD_CONGLOM'

class BUILD_DEPENDENCY(Builtin):
    min_args=3
    max_args=3
    name='BUILD_DEPENDENCY'
    opcode=73
BUILD_DEPENDENCY.__name__='BUILD_DEPENDENCY'

class BUILD_DIM(Builtin):
    min_args=2
    max_args=2
    name='BUILD_DIM'
    opcode=74
BUILD_DIM.__name__='BUILD_DIM'

class BUILD_DISPATCH(Builtin):
    min_args=5
    max_args=5
    name='BUILD_DISPATCH'
    opcode=75
BUILD_DISPATCH.__name__='BUILD_DISPATCH'

class BUILD_EVENT(Builtin):
    min_args=1
    max_args=1
    name='BUILD_EVENT'
    opcode=76
BUILD_EVENT.__name__='BUILD_EVENT'

class BUILD_FUNCTION(Builtin):
    min_args=1
    max_args=254
    name='BUILD_FUNCTION'
    opcode=77
BUILD_FUNCTION.__name__='BUILD_FUNCTION'

class BUILD_METHOD(Builtin):
    min_args=3
    max_args=254
    name='BUILD_METHOD'
    opcode=78
BUILD_METHOD.__name__='BUILD_METHOD'

class BUILD_PARAM(Builtin):
    min_args=3
    max_args=3
    name='BUILD_PARAM'
    opcode=79
BUILD_PARAM.__name__='BUILD_PARAM'

class BUILD_PATH(Builtin):
    min_args=1
    max_args=1
    name='BUILD_PATH'
    opcode=80
BUILD_PATH.__name__='BUILD_PATH'

class BUILD_PROCEDURE(Builtin):
    min_args=3
    max_args=254
    name='BUILD_PROCEDURE'
    opcode=81
BUILD_PROCEDURE.__name__='BUILD_PROCEDURE'

class BUILD_PROGRAM(Builtin):
    min_args=2
    max_args=2
    name='BUILD_PROGRAM'
    opcode=82
BUILD_PROGRAM.__name__='BUILD_PROGRAM'

class BUILD_RANGE(Builtin):
    min_args=2
    max_args=3
    name='BUILD_RANGE'
    opcode=83
BUILD_RANGE.__name__='BUILD_RANGE'

class BUILD_ROUTINE(Builtin):
    min_args=3
    max_args=254
    name='BUILD_ROUTINE'
    opcode=84
BUILD_ROUTINE.__name__='BUILD_ROUTINE'

class BUILD_SIGNAL(Builtin):
    min_args=2
    max_args=10
    name='BUILD_SIGNAL'
    opcode=85
BUILD_SIGNAL.__name__='BUILD_SIGNAL'

class BUILD_SLOPE(Builtin):
    min_args=1
    max_args=254
    name='BUILD_SLOPE'
    opcode=86
BUILD_SLOPE.__name__='BUILD_SLOPE'

class BUILD_WINDOW(Builtin):
    min_args=3
    max_args=3
    name='BUILD_WINDOW'
    opcode=87
BUILD_WINDOW.__name__='BUILD_WINDOW'

class BUILD_WITH_UNITS(Builtin):
    min_args=2
    max_args=2
    name='BUILD_WITH_UNITS'
    opcode=88
BUILD_WITH_UNITS.__name__='BUILD_WITH_UNITS'

class BUILTIN_OPCODE(Builtin):
    min_args=1
    max_args=1
    name='BUILTIN_OPCODE'
    opcode=89
BUILTIN_OPCODE.__name__='BUILTIN_OPCODE'

class BYTE(Builtin):
    min_args=1
    max_args=1
    name='BYTE'
    opcode=90
BYTE.__name__='BYTE'

class BYTE_UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='BYTE_UNSIGNED'
    opcode=91
BYTE_UNSIGNED.__name__='BYTE_UNSIGNED'

class CASE(Builtin):
    min_args=2
    max_args=254
    name='CASE'
    opcode=92
CASE.__name__='CASE'

class CEILING(Builtin):
    min_args=1
    max_args=1
    name='CEILING'
    opcode=93
CEILING.__name__='CEILING'

class CHAR(Builtin):
    min_args=1
    max_args=2
    name='CHAR'
    opcode=94
CHAR.__name__='CHAR'

class CLASS(Builtin):
    min_args=1
    max_args=1
    name='CLASS'
    opcode=95
CLASS.__name__='CLASS'

class FCLOSE(Builtin):
    min_args=1
    max_args=1
    name='FCLOSE'
    opcode=96
FCLOSE.__name__='FCLOSE'

class CMPLX(Builtin):
    min_args=1
    max_args=3
    name='CMPLX'
    opcode=97
CMPLX.__name__='CMPLX'

class COMMA(Builtin):
    min_args=2
    max_args=254
    name='COMMA'
    opcode=98
COMMA.__name__='COMMA'

class COMPILE(Builtin):
    min_args=1
    max_args=254
    name='COMPILE'
    opcode=99
COMPILE.__name__='COMPILE'

class COMPLETION_OF(Builtin):
    min_args=1
    max_args=1
    name='COMPLETION_OF'
    opcode=100
COMPLETION_OF.__name__='COMPLETION_OF'

class CONCAT(Builtin):
    min_args=2
    max_args=254
    name='CONCAT'
    opcode=101
CONCAT.__name__='CONCAT'

class CONDITIONAL(Builtin):
    min_args=3
    max_args=3
    name='CONDITIONAL'
    opcode=102
CONDITIONAL.__name__='CONDITIONAL'

class CONJG(Builtin):
    min_args=1
    max_args=1
    name='CONJG'
    opcode=103
CONJG.__name__='CONJG'

class CONTINUE(Builtin):
    min_args=0
    max_args=0
    name='CONTINUE'
    opcode=104
CONTINUE.__name__='CONTINUE'

class CONVOLVE(Builtin):
    min_args=2
    max_args=2
    name='CONVOLVE'
    opcode=105
CONVOLVE.__name__='CONVOLVE'

class COS(Builtin):
    min_args=1
    max_args=1
    name='COS'
    opcode=106
COS.__name__='COS'

class COSD(Builtin):
    min_args=1
    max_args=1
    name='COSD'
    opcode=107
COSD.__name__='COSD'

class COSH(Builtin):
    min_args=1
    max_args=1
    name='COSH'
    opcode=108
COSH.__name__='COSH'

class COUNT(Builtin):
    min_args=1
    max_args=2
    name='COUNT'
    opcode=109
COUNT.__name__='COUNT'

class CSHIFT(Builtin):
    min_args=2
    max_args=3
    name='CSHIFT'
    opcode=110
CSHIFT.__name__='CSHIFT'

class CVT(Builtin):
    min_args=2
    max_args=2
    name='CVT'
    opcode=111
CVT.__name__='CVT'

class DATA(Builtin):
    min_args=1
    max_args=1
    name='DATA'
    opcode=112
DATA.__name__='DATA'

class DATE_AND_TIME(Builtin):
    min_args=0
    max_args=1
    name='DATE_AND_TIME'
    opcode=113
DATE_AND_TIME.__name__='DATE_AND_TIME'

class DATE_TIME(Builtin):
    min_args=0
    max_args=1
    name='DATE_TIME'
    opcode=114
DATE_TIME.__name__='DATE_TIME'

class DBLE(Builtin):
    min_args=1
    max_args=1
    name='DBLE'
    opcode=115
DBLE.__name__='DBLE'

class DEALLOCATE(Builtin):
    min_args=0
    max_args=254
    name='DEALLOCATE'
    opcode=116
DEALLOCATE.__name__='DEALLOCATE'

class DEBUG(Builtin):
    min_args=0
    max_args=1
    name='DEBUG'
    opcode=117
DEBUG.__name__='DEBUG'

class DECODE(Builtin):
    min_args=1
    max_args=2
    name='DECODE'
    opcode=118
DECODE.__name__='DECODE'

class DECOMPILE(Builtin):
    min_args=1
    max_args=2
    name='DECOMPILE'
    opcode=119
DECOMPILE.__name__='DECOMPILE'

class DECOMPRESS(Builtin):
    min_args=4
    max_args=4
    name='DECOMPRESS'
    opcode=120
DECOMPRESS.__name__='DECOMPRESS'

class DEFAULT(Builtin):
    min_args=1
    max_args=254
    name='DEFAULT'
    opcode=121
DEFAULT.__name__='DEFAULT'

class DERIVATIVE(Builtin):
    min_args=2
    max_args=3
    name='DERIVATIVE'
    opcode=122
DERIVATIVE.__name__='DERIVATIVE'

class DESCR(Builtin):
    min_args=1
    max_args=1
    name='DESCR'
    opcode=123
DESCR.__name__='DESCR'

class DIAGONAL(Builtin):
    min_args=1
    max_args=2
    name='DIAGONAL'
    opcode=124
DIAGONAL.__name__='DIAGONAL'

class DIGITS(Builtin):
    min_args=1
    max_args=1
    name='DIGITS'
    opcode=125
DIGITS.__name__='DIGITS'

class DIM(Builtin):
    min_args=2
    max_args=2
    name='DIM'
    opcode=126
DIM.__name__='DIM'

class DIM_OF(Builtin):
    min_args=1
    max_args=2
    name='DIM_OF'
    opcode=127
DIM_OF.__name__='DIM_OF'

class DISPATCH_OF(Builtin):
    min_args=1
    max_args=1
    name='DISPATCH_OF'
    opcode=128
DISPATCH_OF.__name__='DISPATCH_OF'

class DIVIDE(Builtin):
    min_args=2
    max_args=2
    name='DIVIDE'
    opcode=129
DIVIDE.__name__='DIVIDE'

class LBOUND(Builtin):
    min_args=1
    max_args=2
    name='LBOUND'
    opcode=130
LBOUND.__name__='LBOUND'

class DO(Builtin):
    min_args=2
    max_args=254
    name='DO'
    opcode=131
DO.__name__='DO'

class DOT_PRODUCT(Builtin):
    min_args=2
    max_args=2
    name='DOT_PRODUCT'
    opcode=132
DOT_PRODUCT.__name__='DOT_PRODUCT'

class DPROD(Builtin):
    min_args=2
    max_args=2
    name='DPROD'
    opcode=133
DPROD.__name__='DPROD'

class DSCPTR(Builtin):
    min_args=1
    max_args=2
    name='DSCPTR'
    opcode=134
DSCPTR.__name__='DSCPTR'

class SHAPE(Builtin):
    min_args=1
    max_args=2
    name='SHAPE'
    opcode=135
SHAPE.__name__='SHAPE'

class SIZE(Builtin):
    min_args=1
    max_args=2
    name='SIZE'
    opcode=136
SIZE.__name__='SIZE'

class KIND(Builtin):
    min_args=1
    max_args=1
    name='KIND'
    opcode=137
KIND.__name__='KIND'

class UBOUND(Builtin):
    min_args=1
    max_args=2
    name='UBOUND'
    opcode=138
UBOUND.__name__='UBOUND'

class D_COMPLEX(Builtin):
    min_args=1
    max_args=2
    name='D_COMPLEX'
    opcode=139
D_COMPLEX.__name__='D_COMPLEX'

class D_FLOAT(Builtin):
    min_args=1
    max_args=1
    name='D_FLOAT'
    opcode=140
D_FLOAT.__name__='D_FLOAT'

class RANGE(Builtin):
    min_args=1
    max_args=1
    name='RANGE'
    opcode=141
RANGE.__name__='RANGE'

class PRECISION(Builtin):
    min_args=1
    max_args=1
    name='PRECISION'
    opcode=142
PRECISION.__name__='PRECISION'

class ELBOUND(Builtin):
    min_args=1
    max_args=2
    name='ELBOUND'
    opcode=143
ELBOUND.__name__='ELBOUND'

class ELSE(Builtin):
    min_args=0
    max_args=0
    name='ELSE'
    opcode=144
ELSE.__name__='ELSE'

class ELSEWHERE(Builtin):
    min_args=0
    max_args=0
    name='ELSEWHERE'
    opcode=145
ELSEWHERE.__name__='ELSEWHERE'

class ENCODE(Builtin):
    min_args=1
    max_args=254
    name='ENCODE'
    opcode=146
ENCODE.__name__='ENCODE'

class ENDFILE(Builtin):
    min_args=1
    max_args=1
    name='ENDFILE'
    opcode=147
ENDFILE.__name__='ENDFILE'

class END_OF(Builtin):
    min_args=1
    max_args=2
    name='END_OF'
    opcode=148
END_OF.__name__='END_OF'

class EOSHIFT(Builtin):
    min_args=3
    max_args=4
    name='EOSHIFT'
    opcode=149
EOSHIFT.__name__='EOSHIFT'

class EPSILON(Builtin):
    min_args=1
    max_args=1
    name='EPSILON'
    opcode=150
EPSILON.__name__='EPSILON'

class EQ(Builtin):
    min_args=2
    max_args=2
    name='EQ'
    opcode=151
EQ.__name__='EQ'

class EQUALS(Builtin):
    min_args=2
    max_args=2
    name='EQUALS'
    opcode=152
EQUALS.__name__='EQUALS'

class EQUALS_FIRST(Builtin):
    min_args=1
    max_args=1
    name='EQUALS_FIRST'
    opcode=153
EQUALS_FIRST.__name__='EQUALS_FIRST'

class EQV(Builtin):
    min_args=2
    max_args=2
    name='EQV'
    opcode=154
EQV.__name__='EQV'

class ESHAPE(Builtin):
    min_args=1
    max_args=2
    name='ESHAPE'
    opcode=155
ESHAPE.__name__='ESHAPE'

class ESIZE(Builtin):
    min_args=1
    max_args=2
    name='ESIZE'
    opcode=156
ESIZE.__name__='ESIZE'

class EUBOUND(Builtin):
    min_args=1
    max_args=2
    name='EUBOUND'
    opcode=157
EUBOUND.__name__='EUBOUND'

class EVALUATE(Builtin):
    min_args=1
    max_args=1
    name='EVALUATE'
    opcode=158
EVALUATE.__name__='EVALUATE'

class EXECUTE(Builtin):
    min_args=1
    max_args=254
    name='EXECUTE'
    opcode=159
EXECUTE.__name__='EXECUTE'

class EXP(Builtin):
    min_args=1
    max_args=1
    name='EXP'
    opcode=160
EXP.__name__='EXP'

class EXPONENT(Builtin):
    min_args=1
    max_args=1
    name='EXPONENT'
    opcode=161
EXPONENT.__name__='EXPONENT'

class EXT_FUNCTION(Builtin):
    min_args=2
    max_args=254
    name='EXT_FUNCTION'
    opcode=162
EXT_FUNCTION.__name__='EXT_FUNCTION'

class FFT(Builtin):
    min_args=2
    max_args=2
    name='FFT'
    opcode=163
FFT.__name__='FFT'

class FIRSTLOC(Builtin):
    min_args=1
    max_args=2
    name='FIRSTLOC'
    opcode=164
FIRSTLOC.__name__='FIRSTLOC'

class FIT(Builtin):
    min_args=2
    max_args=2
    name='FIT'
    opcode=165
FIT.__name__='FIT'

class FIX_ROPRAND(Builtin):
    min_args=2
    max_args=2
    name='FIX_ROPRAND'
    opcode=166
FIX_ROPRAND.__name__='FIX_ROPRAND'

class FLOAT(Builtin):
    min_args=1
    max_args=2
    name='FLOAT'
    opcode=167
FLOAT.__name__='FLOAT'

class FLOOR(Builtin):
    min_args=1
    max_args=1
    name='FLOOR'
    opcode=168
FLOOR.__name__='FLOOR'

class FOR(Builtin):
    min_args=4
    max_args=254
    name='FOR'
    opcode=169
FOR.__name__='FOR'

class FRACTION(Builtin):
    min_args=1
    max_args=1
    name='FRACTION'
    opcode=170
FRACTION.__name__='FRACTION'

class FUN(Builtin):
    min_args=2
    max_args=254
    name='FUN'
    opcode=171
FUN.__name__='FUN'

class F_COMPLEX(Builtin):
    min_args=1
    max_args=2
    name='F_COMPLEX'
    opcode=172
F_COMPLEX.__name__='F_COMPLEX'

class F_FLOAT(Builtin):
    min_args=1
    max_args=1
    name='F_FLOAT'
    opcode=173
F_FLOAT.__name__='F_FLOAT'

class GE(Builtin):
    min_args=2
    max_args=2
    name='GE'
    opcode=174
GE.__name__='GE'

class GETNCI(Builtin):
    min_args=2
    max_args=3
    name='GETNCI'
    opcode=175
GETNCI.__name__='GETNCI'

class GOTO(Builtin):
    min_args=1
    max_args=1
    name='GOTO'
    opcode=176
GOTO.__name__='GOTO'

class GT(Builtin):
    min_args=2
    max_args=2
    name='GT'
    opcode=177
GT.__name__='GT'

class G_COMPLEX(Builtin):
    min_args=1
    max_args=2
    name='G_COMPLEX'
    opcode=178
G_COMPLEX.__name__='G_COMPLEX'

class G_FLOAT(Builtin):
    min_args=1
    max_args=1
    name='G_FLOAT'
    opcode=179
G_FLOAT.__name__='G_FLOAT'

class HELP_OF(Builtin):
    min_args=1
    max_args=1
    name='HELP_OF'
    opcode=180
HELP_OF.__name__='HELP_OF'

class HUGE(Builtin):
    min_args=1
    max_args=1
    name='HUGE'
    opcode=181
HUGE.__name__='HUGE'

class H_COMPLEX(Builtin):
    min_args=1
    max_args=2
    name='H_COMPLEX'
    opcode=182
H_COMPLEX.__name__='H_COMPLEX'

class H_FLOAT(Builtin):
    min_args=1
    max_args=1
    name='H_FLOAT'
    opcode=183
H_FLOAT.__name__='H_FLOAT'

class IACHAR(Builtin):
    min_args=1
    max_args=1
    name='IACHAR'
    opcode=184
IACHAR.__name__='IACHAR'

class IAND(Builtin):
    min_args=2
    max_args=2
    name='IAND'
    opcode=185
IAND.__name__='IAND'

class IAND_NOT(Builtin):
    min_args=2
    max_args=2
    name='IAND_NOT'
    opcode=186
IAND_NOT.__name__='IAND_NOT'

class ICHAR(Builtin):
    min_args=1
    max_args=1
    name='ICHAR'
    opcode=187
ICHAR.__name__='ICHAR'

class IDENT_OF(Builtin):
    min_args=1
    max_args=1
    name='IDENT_OF'
    opcode=188
IDENT_OF.__name__='IDENT_OF'

class IF(Builtin):
    min_args=2
    max_args=3
    name='IF'
    opcode=189
IF.__name__='IF'

class IF_ERROR(Builtin):
    min_args=1
    max_args=254
    name='IF_ERROR'
    opcode=190
IF_ERROR.__name__='IF_ERROR'

class IMAGE_OF(Builtin):
    min_args=1
    max_args=1
    name='IMAGE_OF'
    opcode=191
IMAGE_OF.__name__='IMAGE_OF'

class IN(Builtin):
    min_args=1
    max_args=1
    name='IN'
    opcode=192
IN.__name__='IN'

class INAND(Builtin):
    min_args=2
    max_args=2
    name='INAND'
    opcode=193
INAND.__name__='INAND'

class INAND_NOT(Builtin):
    min_args=2
    max_args=2
    name='INAND_NOT'
    opcode=194
INAND_NOT.__name__='INAND_NOT'

class INDEX(Builtin):
    min_args=2
    max_args=3
    name='INDEX'
    opcode=195
INDEX.__name__='INDEX'

class INOR(Builtin):
    min_args=2
    max_args=2
    name='INOR'
    opcode=196
INOR.__name__='INOR'

class INOR_NOT(Builtin):
    min_args=2
    max_args=2
    name='INOR_NOT'
    opcode=197
INOR_NOT.__name__='INOR_NOT'

class INOT(Builtin):
    min_args=1
    max_args=1
    name='INOT'
    opcode=198
INOT.__name__='INOT'

class INOUT(Builtin):
    min_args=1
    max_args=1
    name='INOUT'
    opcode=199
INOUT.__name__='INOUT'

class INQUIRE(Builtin):
    min_args=2
    max_args=2
    name='INQUIRE'
    opcode=200
INQUIRE.__name__='INQUIRE'

class INT(Builtin):
    min_args=1
    max_args=2
    name='INT'
    opcode=201
INT.__name__='INT'

class INTEGRAL(Builtin):
    min_args=2
    max_args=3
    name='INTEGRAL'
    opcode=202
INTEGRAL.__name__='INTEGRAL'

class INTERPOL(Builtin):
    min_args=2
    max_args=3
    name='INTERPOL'
    opcode=203
INTERPOL.__name__='INTERPOL'

class INTERSECT(Builtin):
    min_args=0
    max_args=254
    name='INTERSECT'
    opcode=204
INTERSECT.__name__='INTERSECT'

class INT_UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='INT_UNSIGNED'
    opcode=205
INT_UNSIGNED.__name__='INT_UNSIGNED'

class INVERSE(Builtin):
    min_args=1
    max_args=1
    name='INVERSE'
    opcode=206
INVERSE.__name__='INVERSE'

class IOR(Builtin):
    min_args=2
    max_args=2
    name='IOR'
    opcode=207
IOR.__name__='IOR'

class IOR_NOT(Builtin):
    min_args=2
    max_args=2
    name='IOR_NOT'
    opcode=208
IOR_NOT.__name__='IOR_NOT'

class IS_IN(Builtin):
    min_args=2
    max_args=3
    name='IS_IN'
    opcode=209
IS_IN.__name__='IS_IN'

class IEOR(Builtin):
    min_args=2
    max_args=2
    name='IEOR'
    opcode=210
IEOR.__name__='IEOR'

class IEOR_NOT(Builtin):
    min_args=2
    max_args=2
    name='IEOR_NOT'
    opcode=211
IEOR_NOT.__name__='IEOR_NOT'

class LABEL(Builtin):
    min_args=2
    max_args=254
    name='LABEL'
    opcode=212
LABEL.__name__='LABEL'

class LAMINATE(Builtin):
    min_args=2
    max_args=254
    name='LAMINATE'
    opcode=213
LAMINATE.__name__='LAMINATE'

class LANGUAGE_OF(Builtin):
    min_args=1
    max_args=1
    name='LANGUAGE_OF'
    opcode=214
LANGUAGE_OF.__name__='LANGUAGE_OF'

class LASTLOC(Builtin):
    min_args=1
    max_args=2
    name='LASTLOC'
    opcode=215
LASTLOC.__name__='LASTLOC'

class LE(Builtin):
    min_args=2
    max_args=2
    name='LE'
    opcode=216
LE.__name__='LE'

class LEN(Builtin):
    min_args=1
    max_args=1
    name='LEN'
    opcode=217
LEN.__name__='LEN'

class LEN_TRIM(Builtin):
    min_args=1
    max_args=1
    name='LEN_TRIM'
    opcode=218
LEN_TRIM.__name__='LEN_TRIM'

class LGE(Builtin):
    min_args=2
    max_args=2
    name='LGE'
    opcode=219
LGE.__name__='LGE'

class LGT(Builtin):
    min_args=2
    max_args=2
    name='LGT'
    opcode=220
LGT.__name__='LGT'

class LLE(Builtin):
    min_args=2
    max_args=2
    name='LLE'
    opcode=221
LLE.__name__='LLE'

class LLT(Builtin):
    min_args=2
    max_args=2
    name='LLT'
    opcode=222
LLT.__name__='LLT'

class LOG(Builtin):
    min_args=1
    max_args=1
    name='LOG'
    opcode=223
LOG.__name__='LOG'

class LOG10(Builtin):
    min_args=1
    max_args=1
    name='LOG10'
    opcode=224
LOG10.__name__='LOG10'

class LOG2(Builtin):
    min_args=1
    max_args=1
    name='LOG2'
    opcode=225
LOG2.__name__='LOG2'

class LOGICAL(Builtin):
    min_args=1
    max_args=2
    name='LOGICAL'
    opcode=226
LOGICAL.__name__='LOGICAL'

class LONG(Builtin):
    min_args=1
    max_args=1
    name='LONG'
    opcode=227
LONG.__name__='LONG'

class LONG_UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='LONG_UNSIGNED'
    opcode=228
LONG_UNSIGNED.__name__='LONG_UNSIGNED'

class LT(Builtin):
    min_args=2
    max_args=2
    name='LT'
    opcode=229
LT.__name__='LT'

class MATMUL(Builtin):
    min_args=2
    max_args=2
    name='MATMUL'
    opcode=230
MATMUL.__name__='MATMUL'

class MAT_ROT(Builtin):
    min_args=2
    max_args=5
    name='MAT_ROT'
    opcode=231
MAT_ROT.__name__='MAT_ROT'

class MAT_ROT_INT(Builtin):
    min_args=2
    max_args=5
    name='MAT_ROT_INT'
    opcode=232
MAT_ROT_INT.__name__='MAT_ROT_INT'

class MAX(Builtin):
    min_args=2
    max_args=254
    name='MAX'
    opcode=233
MAX.__name__='MAX'

class MAXEXPONENT(Builtin):
    min_args=1
    max_args=1
    name='MAXEXPONENT'
    opcode=234
MAXEXPONENT.__name__='MAXEXPONENT'

class MAXLOC(Builtin):
    min_args=1
    max_args=3
    name='MAXLOC'
    opcode=235
MAXLOC.__name__='MAXLOC'

class MAXVAL(Builtin):
    min_args=1
    max_args=3
    name='MAXVAL'
    opcode=236
MAXVAL.__name__='MAXVAL'

class MEAN(Builtin):
    min_args=1
    max_args=3
    name='MEAN'
    opcode=237
MEAN.__name__='MEAN'

class MEDIAN(Builtin):
    min_args=2
    max_args=2
    name='MEDIAN'
    opcode=238
MEDIAN.__name__='MEDIAN'

class MERGE(Builtin):
    min_args=3
    max_args=3
    name='MERGE'
    opcode=239
MERGE.__name__='MERGE'

class METHOD_OF(Builtin):
    min_args=1
    max_args=1
    name='METHOD_OF'
    opcode=240
METHOD_OF.__name__='METHOD_OF'

class MIN(Builtin):
    min_args=2
    max_args=254
    name='MIN'
    opcode=241
MIN.__name__='MIN'

class MINEXPONENT(Builtin):
    min_args=1
    max_args=1
    name='MINEXPONENT'
    opcode=242
MINEXPONENT.__name__='MINEXPONENT'

class MINLOC(Builtin):
    min_args=1
    max_args=3
    name='MINLOC'
    opcode=243
MINLOC.__name__='MINLOC'

class MINVAL(Builtin):
    min_args=1
    max_args=3
    name='MINVAL'
    opcode=244
MINVAL.__name__='MINVAL'

class MOD(Builtin):
    min_args=2
    max_args=2
    name='MOD'
    opcode=245
MOD.__name__='MOD'

class MODEL_OF(Builtin):
    min_args=1
    max_args=1
    name='MODEL_OF'
    opcode=246
MODEL_OF.__name__='MODEL_OF'

class MULTIPLY(Builtin):
    min_args=2
    max_args=2
    name='MULTIPLY'
    opcode=247
MULTIPLY.__name__='MULTIPLY'

class NAME_OF(Builtin):
    min_args=1
    max_args=1
    name='NAME_OF'
    opcode=248
NAME_OF.__name__='NAME_OF'

class NAND(Builtin):
    min_args=2
    max_args=2
    name='NAND'
    opcode=249
NAND.__name__='NAND'

class NAND_NOT(Builtin):
    min_args=2
    max_args=2
    name='NAND_NOT'
    opcode=250
NAND_NOT.__name__='NAND_NOT'

class NDESC(Builtin):
    min_args=1
    max_args=1
    name='NDESC'
    opcode=251
NDESC.__name__='NDESC'

class NE(Builtin):
    min_args=2
    max_args=2
    name='NE'
    opcode=252
NE.__name__='NE'

class NEAREST(Builtin):
    min_args=2
    max_args=2
    name='NEAREST'
    opcode=253
NEAREST.__name__='NEAREST'

class NEQV(Builtin):
    min_args=2
    max_args=2
    name='NEQV'
    opcode=254
NEQV.__name__='NEQV'

class NINT(Builtin):
    min_args=1
    max_args=2
    name='NINT'
    opcode=255
NINT.__name__='NINT'

class NOR(Builtin):
    min_args=2
    max_args=2
    name='NOR'
    opcode=256
NOR.__name__='NOR'

class NOR_NOT(Builtin):
    min_args=2
    max_args=2
    name='NOR_NOT'
    opcode=257
NOR_NOT.__name__='NOR_NOT'

class NOT(Builtin):
    min_args=1
    max_args=1
    name='NOT'
    opcode=258
NOT.__name__='NOT'

class OBJECT_OF(Builtin):
    min_args=1
    max_args=1
    name='OBJECT_OF'
    opcode=259
OBJECT_OF.__name__='OBJECT_OF'

class OCTAWORD(Builtin):
    min_args=1
    max_args=1
    name='OCTAWORD'
    opcode=260
OCTAWORD.__name__='OCTAWORD'

class OCTAWORD_UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='OCTAWORD_UNSIGNED'
    opcode=261
OCTAWORD_UNSIGNED.__name__='OCTAWORD_UNSIGNED'

class ON_ERROR(Builtin):
    min_args=1
    max_args=1
    name='ON_ERROR'
    opcode=262
ON_ERROR.__name__='ON_ERROR'

class OPCODE_BUILTIN(Builtin):
    min_args=1
    max_args=1
    name='OPCODE_BUILTIN'
    opcode=263
OPCODE_BUILTIN.__name__='OPCODE_BUILTIN'

class OPCODE_STRING(Builtin):
    min_args=1
    max_args=1
    name='OPCODE_STRING'
    opcode=264
OPCODE_STRING.__name__='OPCODE_STRING'

class FOPEN(Builtin):
    min_args=2
    max_args=254
    name='FOPEN'
    opcode=265
FOPEN.__name__='FOPEN'

class OPTIONAL(Builtin):
    min_args=1
    max_args=1
    name='OPTIONAL'
    opcode=266
OPTIONAL.__name__='OPTIONAL'

class OR(Builtin):
    min_args=2
    max_args=2
    name='OR'
    opcode=267
OR.__name__='OR'

class OR_NOT(Builtin):
    min_args=2
    max_args=2
    name='OR_NOT'
    opcode=268
OR_NOT.__name__='OR_NOT'

class OUT(Builtin):
    min_args=1
    max_args=1
    name='OUT'
    opcode=269
OUT.__name__='OUT'

class PACK(Builtin):
    min_args=2
    max_args=3
    name='PACK'
    opcode=270
PACK.__name__='PACK'

class PHASE_OF(Builtin):
    min_args=1
    max_args=1
    name='PHASE_OF'
    opcode=271
PHASE_OF.__name__='PHASE_OF'

class POST_DEC(Builtin):
    min_args=1
    max_args=1
    name='POST_DEC'
    opcode=272
POST_DEC.__name__='POST_DEC'

class POST_INC(Builtin):
    min_args=1
    max_args=1
    name='POST_INC'
    opcode=273
POST_INC.__name__='POST_INC'

class POWER(Builtin):
    min_args=2
    max_args=2
    name='POWER'
    opcode=274
POWER.__name__='POWER'

class PRESENT(Builtin):
    min_args=1
    max_args=1
    name='PRESENT'
    opcode=275
PRESENT.__name__='PRESENT'

class PRE_DEC(Builtin):
    min_args=1
    max_args=1
    name='PRE_DEC'
    opcode=276
PRE_DEC.__name__='PRE_DEC'

class PRE_INC(Builtin):
    min_args=1
    max_args=1
    name='PRE_INC'
    opcode=277
PRE_INC.__name__='PRE_INC'

class PRIVATE(Builtin):
    min_args=1
    max_args=1
    name='PRIVATE'
    opcode=278
PRIVATE.__name__='PRIVATE'

class PROCEDURE_OF(Builtin):
    min_args=1
    max_args=1
    name='PROCEDURE_OF'
    opcode=279
PROCEDURE_OF.__name__='PROCEDURE_OF'

class PRODUCT(Builtin):
    min_args=1
    max_args=3
    name='PRODUCT'
    opcode=280
PRODUCT.__name__='PRODUCT'

class PROGRAM_OF(Builtin):
    min_args=1
    max_args=1
    name='PROGRAM_OF'
    opcode=281
PROGRAM_OF.__name__='PROGRAM_OF'

class PROJECT(Builtin):
    min_args=3
    max_args=4
    name='PROJECT'
    opcode=282
PROJECT.__name__='PROJECT'

class PROMOTE(Builtin):
    min_args=2
    max_args=2
    name='PROMOTE'
    opcode=283
PROMOTE.__name__='PROMOTE'

class PUBLIC(Builtin):
    min_args=1
    max_args=1
    name='PUBLIC'
    opcode=284
PUBLIC.__name__='PUBLIC'

class QUADWORD(Builtin):
    min_args=1
    max_args=1
    name='QUADWORD'
    opcode=285
QUADWORD.__name__='QUADWORD'

class QUADWORD_UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='QUADWORD_UNSIGNED'
    opcode=286
QUADWORD_UNSIGNED.__name__='QUADWORD_UNSIGNED'

class QUALIFIERS_OF(Builtin):
    min_args=1
    max_args=1
    name='QUALIFIERS_OF'
    opcode=287
QUALIFIERS_OF.__name__='QUALIFIERS_OF'

class RADIX(Builtin):
    min_args=1
    max_args=1
    name='RADIX'
    opcode=288
RADIX.__name__='RADIX'

class RAMP(Builtin):
    min_args=0
    max_args=2
    name='RAMP'
    opcode=289
RAMP.__name__='RAMP'

class RANDOM(Builtin):
    min_args=0
    max_args=2
    name='RANDOM'
    opcode=290
RANDOM.__name__='RANDOM'

class RANDOM_SEED(Builtin):
    min_args=0
    max_args=1
    name='RANDOM_SEED'
    opcode=291
RANDOM_SEED.__name__='RANDOM_SEED'

class DTYPE_RANGE(Builtin):
    min_args=2
    max_args=3
    name='DTYPE_RANGE'
    opcode=292
DTYPE_RANGE.__name__='DTYPE_RANGE'

class RANK(Builtin):
    min_args=1
    max_args=1
    name='RANK'
    opcode=293
RANK.__name__='RANK'

class RAW_OF(Builtin):
    min_args=1
    max_args=1
    name='RAW_OF'
    opcode=294
RAW_OF.__name__='RAW_OF'

class READ(Builtin):
    min_args=1
    max_args=1
    name='READ'
    opcode=295
READ.__name__='READ'

class REAL(Builtin):
    min_args=1
    max_args=2
    name='REAL'
    opcode=296
REAL.__name__='REAL'

class REBIN(Builtin):
    min_args=2
    max_args=4
    name='REBIN'
    opcode=297
REBIN.__name__='REBIN'

class REF(Builtin):
    min_args=1
    max_args=1
    name='REF'
    opcode=298
REF.__name__='REF'

class REPEAT(Builtin):
    min_args=2
    max_args=2
    name='REPEAT'
    opcode=299
REPEAT.__name__='REPEAT'

class REPLICATE(Builtin):
    min_args=3
    max_args=3
    name='REPLICATE'
    opcode=300
REPLICATE.__name__='REPLICATE'

class RESHAPE(Builtin):
    min_args=2
    max_args=4
    name='RESHAPE'
    opcode=301
RESHAPE.__name__='RESHAPE'

class RETURN(Builtin):
    min_args=0
    max_args=1
    name='RETURN'
    opcode=302
RETURN.__name__='RETURN'

class REWIND(Builtin):
    min_args=1
    max_args=1
    name='REWIND'
    opcode=303
REWIND.__name__='REWIND'

class RMS(Builtin):
    min_args=1
    max_args=3
    name='RMS'
    opcode=304
RMS.__name__='RMS'

class ROUTINE_OF(Builtin):
    min_args=1
    max_args=1
    name='ROUTINE_OF'
    opcode=305
ROUTINE_OF.__name__='ROUTINE_OF'

class RRSPACING(Builtin):
    min_args=1
    max_args=1
    name='RRSPACING'
    opcode=306
RRSPACING.__name__='RRSPACING'

class SCALE(Builtin):
    min_args=2
    max_args=2
    name='SCALE'
    opcode=307
SCALE.__name__='SCALE'

class SCAN(Builtin):
    min_args=2
    max_args=3
    name='SCAN'
    opcode=308
SCAN.__name__='SCAN'

class FSEEK(Builtin):
    min_args=1
    max_args=3
    name='FSEEK'
    opcode=309
FSEEK.__name__='FSEEK'

class SET_EXPONENT(Builtin):
    min_args=2
    max_args=2
    name='SET_EXPONENT'
    opcode=310
SET_EXPONENT.__name__='SET_EXPONENT'

class SET_RANGE(Builtin):
    min_args=2
    max_args=9
    name='SET_RANGE'
    opcode=311
SET_RANGE.__name__='SET_RANGE'

class ISHFT(Builtin):
    min_args=2
    max_args=2
    name='ISHFT'
    opcode=312
ISHFT.__name__='ISHFT'

class ISHFTC(Builtin):
    min_args=3
    max_args=3
    name='ISHFTC'
    opcode=313
ISHFTC.__name__='ISHFTC'

class SHIFT_LEFT(Builtin):
    min_args=2
    max_args=2
    name='SHIFT_LEFT'
    opcode=314
SHIFT_LEFT.__name__='SHIFT_LEFT'

class SHIFT_RIGHT(Builtin):
    min_args=2
    max_args=2
    name='SHIFT_RIGHT'
    opcode=315
SHIFT_RIGHT.__name__='SHIFT_RIGHT'

class SIGN(Builtin):
    min_args=2
    max_args=2
    name='SIGN'
    opcode=316
SIGN.__name__='SIGN'

class SIGNED(Builtin):
    min_args=1
    max_args=1
    name='SIGNED'
    opcode=317
SIGNED.__name__='SIGNED'

class SIN(Builtin):
    min_args=1
    max_args=1
    name='SIN'
    opcode=318
SIN.__name__='SIN'

class SIND(Builtin):
    min_args=1
    max_args=1
    name='SIND'
    opcode=319
SIND.__name__='SIND'

class SINH(Builtin):
    min_args=1
    max_args=1
    name='SINH'
    opcode=320
SINH.__name__='SINH'

class SIZEOF(Builtin):
    min_args=1
    max_args=1
    name='SIZEOF'
    opcode=321
SIZEOF.__name__='SIZEOF'

class SLOPE_OF(Builtin):
    min_args=1
    max_args=2
    name='SLOPE_OF'
    opcode=322
SLOPE_OF.__name__='SLOPE_OF'

class SMOOTH(Builtin):
    min_args=2
    max_args=2
    name='SMOOTH'
    opcode=323
SMOOTH.__name__='SMOOTH'

class SOLVE(Builtin):
    min_args=2
    max_args=2
    name='SOLVE'
    opcode=324
SOLVE.__name__='SOLVE'

class SORTVAL(Builtin):
    min_args=1
    max_args=2
    name='SORTVAL'
    opcode=325
SORTVAL.__name__='SORTVAL'

class SPACING(Builtin):
    min_args=1
    max_args=1
    name='SPACING'
    opcode=326
SPACING.__name__='SPACING'

class SPAWN(Builtin):
    min_args=0
    max_args=3
    name='SPAWN'
    opcode=327
SPAWN.__name__='SPAWN'

class SPREAD(Builtin):
    min_args=3
    max_args=3
    name='SPREAD'
    opcode=328
SPREAD.__name__='SPREAD'

class SQRT(Builtin):
    min_args=1
    max_args=1
    name='SQRT'
    opcode=329
SQRT.__name__='SQRT'

class SQUARE(Builtin):
    min_args=1
    max_args=1
    name='SQUARE'
    opcode=330
SQUARE.__name__='SQUARE'

class STATEMENT(Builtin):
    min_args=0
    max_args=254
    name='STATEMENT'
    opcode=331
STATEMENT.__name__='STATEMENT'

class STD_DEV(Builtin):
    min_args=1
    max_args=3
    name='STD_DEV'
    opcode=332
STD_DEV.__name__='STD_DEV'

class STRING(Builtin):
    min_args=1
    max_args=254
    name='STRING'
    opcode=333
STRING.__name__='STRING'

class STRING_OPCODE(Builtin):
    min_args=1
    max_args=1
    name='STRING_OPCODE'
    opcode=334
STRING_OPCODE.__name__='STRING_OPCODE'

class SUBSCRIPT(Builtin):
    min_args=1
    max_args=9
    name='SUBSCRIPT'
    opcode=335
SUBSCRIPT.__name__='SUBSCRIPT'

class SUBTRACT(Builtin):
    min_args=2
    max_args=2
    name='SUBTRACT'
    opcode=336
SUBTRACT.__name__='SUBTRACT'

class SUM(Builtin):
    min_args=1
    max_args=3
    name='SUM'
    opcode=337
SUM.__name__='SUM'

class SWITCH(Builtin):
    min_args=2
    max_args=254
    name='SWITCH'
    opcode=338
SWITCH.__name__='SWITCH'

class SYSTEM_CLOCK(Builtin):
    min_args=1
    max_args=1
    name='SYSTEM_CLOCK'
    opcode=339
SYSTEM_CLOCK.__name__='SYSTEM_CLOCK'

class TAN(Builtin):
    min_args=1
    max_args=1
    name='TAN'
    opcode=340
TAN.__name__='TAN'

class TAND(Builtin):
    min_args=1
    max_args=1
    name='TAND'
    opcode=341
TAND.__name__='TAND'

class TANH(Builtin):
    min_args=1
    max_args=1
    name='TANH'
    opcode=342
TANH.__name__='TANH'

class TASK_OF(Builtin):
    min_args=1
    max_args=1
    name='TASK_OF'
    opcode=343
TASK_OF.__name__='TASK_OF'

class TEXT(Builtin):
    min_args=1
    max_args=2
    name='TEXT'
    opcode=344
TEXT.__name__='TEXT'

class TIME_OUT_OF(Builtin):
    min_args=1
    max_args=1
    name='TIME_OUT_OF'
    opcode=345
TIME_OUT_OF.__name__='TIME_OUT_OF'

class TINY(Builtin):
    min_args=1
    max_args=1
    name='TINY'
    opcode=346
TINY.__name__='TINY'

class TRANSFER(Builtin):
    min_args=2
    max_args=3
    name='TRANSFER'
    opcode=347
TRANSFER.__name__='TRANSFER'

class TRANSPOSE_(Builtin):
    min_args=1
    max_args=1
    name='TRANSPOSE_'
    opcode=348
TRANSPOSE_.__name__='TRANSPOSE_'

class TRIM(Builtin):
    min_args=1
    max_args=1
    name='TRIM'
    opcode=349
TRIM.__name__='TRIM'

class UNARY_MINUS(Builtin):
    min_args=1
    max_args=1
    name='UNARY_MINUS'
    opcode=350
UNARY_MINUS.__name__='UNARY_MINUS'

class UNARY_PLUS(Builtin):
    min_args=1
    max_args=1
    name='UNARY_PLUS'
    opcode=351
UNARY_PLUS.__name__='UNARY_PLUS'

class UNION(Builtin):
    min_args=0
    max_args=254
    name='UNION'
    opcode=352
UNION.__name__='UNION'

class UNITS(Builtin):
    min_args=1
    max_args=1
    name='UNITS'
    opcode=353
UNITS.__name__='UNITS'

class UNITS_OF(Builtin):
    min_args=1
    max_args=1
    name='UNITS_OF'
    opcode=354
UNITS_OF.__name__='UNITS_OF'

class UNPACK(Builtin):
    min_args=3
    max_args=3
    name='UNPACK'
    opcode=355
UNPACK.__name__='UNPACK'

class UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='UNSIGNED'
    opcode=356
UNSIGNED.__name__='UNSIGNED'

class VAL(Builtin):
    min_args=1
    max_args=1
    name='VAL'
    opcode=357
VAL.__name__='VAL'

class VALIDATION_OF(Builtin):
    min_args=1
    max_args=1
    name='VALIDATION_OF'
    opcode=358
VALIDATION_OF.__name__='VALIDATION_OF'

class VALUE_OF(Builtin):
    min_args=1
    max_args=1
    name='VALUE_OF'
    opcode=359
VALUE_OF.__name__='VALUE_OF'

class VAR(Builtin):
    min_args=1
    max_args=2
    name='VAR'
    opcode=360
VAR.__name__='VAR'

class VECTOR(Builtin):
    min_args=0
    max_args=254
    name='VECTOR'
    opcode=361
VECTOR.__name__='VECTOR'

class VERIFY(Builtin):
    min_args=2
    max_args=3
    name='VERIFY'
    opcode=362
VERIFY.__name__='VERIFY'

class WAIT(Builtin):
    min_args=1
    max_args=1
    name='WAIT'
    opcode=363
WAIT.__name__='WAIT'

class WHEN_OF(Builtin):
    min_args=1
    max_args=1
    name='WHEN_OF'
    opcode=364
WHEN_OF.__name__='WHEN_OF'

class WHERE(Builtin):
    min_args=2
    max_args=3
    name='WHERE'
    opcode=365
WHERE.__name__='WHERE'

class WHILE(Builtin):
    min_args=2
    max_args=254
    name='WHILE'
    opcode=366
WHILE.__name__='WHILE'

class WINDOW_OF(Builtin):
    min_args=1
    max_args=1
    name='WINDOW_OF'
    opcode=367
WINDOW_OF.__name__='WINDOW_OF'

class WORD(Builtin):
    min_args=1
    max_args=1
    name='WORD'
    opcode=368
WORD.__name__='WORD'

class WORD_UNSIGNED(Builtin):
    min_args=1
    max_args=1
    name='WORD_UNSIGNED'
    opcode=369
WORD_UNSIGNED.__name__='WORD_UNSIGNED'

class WRITE(Builtin):
    min_args=1
    max_args=254
    name='WRITE'
    opcode=370
WRITE.__name__='WRITE'

class ZERO(Builtin):
    min_args=0
    max_args=2
    name='ZERO'
    opcode=371
ZERO.__name__='ZERO'

class d2PI(Builtin):
    min_args=0
    max_args=0
    name='$2PI'
    opcode=372
d2PI.__name__='$2PI'

class dNARG(Builtin):
    min_args=0
    max_args=0
    name='$NARG'
    opcode=373
dNARG.__name__='$NARG'

class ELEMENT(Builtin):
    min_args=3
    max_args=3
    name='ELEMENT'
    opcode=374
ELEMENT.__name__='ELEMENT'

class RC_DROOP(Builtin):
    min_args=3
    max_args=3
    name='RC_DROOP'
    opcode=375
RC_DROOP.__name__='RC_DROOP'

class RESET_PRIVATE(Builtin):
    min_args=0
    max_args=0
    name='RESET_PRIVATE'
    opcode=376
RESET_PRIVATE.__name__='RESET_PRIVATE'

class RESET_PUBLIC(Builtin):
    min_args=0
    max_args=0
    name='RESET_PUBLIC'
    opcode=377
RESET_PUBLIC.__name__='RESET_PUBLIC'

class SHOW_PRIVATE(Builtin):
    min_args=0
    max_args=254
    name='SHOW_PRIVATE'
    opcode=378
SHOW_PRIVATE.__name__='SHOW_PRIVATE'

class SHOW_PUBLIC(Builtin):
    min_args=0
    max_args=254
    name='SHOW_PUBLIC'
    opcode=379
SHOW_PUBLIC.__name__='SHOW_PUBLIC'

class SHOW_VM(Builtin):
    min_args=0
    max_args=2
    name='SHOW_VM'
    opcode=380
SHOW_VM.__name__='SHOW_VM'

class TRANSLATE(Builtin):
    min_args=3
    max_args=3
    name='TRANSLATE'
    opcode=381
TRANSLATE.__name__='TRANSLATE'

class TRANSPOSE_MUL(Builtin):
    min_args=2
    max_args=2
    name='TRANSPOSE_MUL'
    opcode=382
TRANSPOSE_MUL.__name__='TRANSPOSE_MUL'

class UPCASE(Builtin):
    min_args=1
    max_args=1
    name='UPCASE'
    opcode=383
UPCASE.__name__='UPCASE'

class USING(Builtin):
    min_args=2
    max_args=4
    name='USING'
    opcode=384
USING.__name__='USING'

class VALIDATION(Builtin):
    min_args=1
    max_args=1
    name='VALIDATION'
    opcode=385
VALIDATION.__name__='VALIDATION'

class dDEFAULT(Builtin):
    min_args=0
    max_args=0
    name='$DEFAULT'
    opcode=386
dDEFAULT.__name__='$DEFAULT'

class dEXPT(Builtin):
    min_args=0
    max_args=0
    name='$EXPT'
    opcode=387
dEXPT.__name__='$EXPT'

class dSHOT(Builtin):
    min_args=0
    max_args=0
    name='$SHOT'
    opcode=388
dSHOT.__name__='$SHOT'

class GETDBI(Builtin):
    min_args=1
    max_args=2
    name='GETDBI'
    opcode=389
GETDBI.__name__='GETDBI'

class CULL(Builtin):
    min_args=1
    max_args=4
    name='CULL'
    opcode=390
CULL.__name__='CULL'

class EXTEND(Builtin):
    min_args=1
    max_args=4
    name='EXTEND'
    opcode=391
EXTEND.__name__='EXTEND'

class I_TO_X(Builtin):
    min_args=1
    max_args=2
    name='I_TO_X'
    opcode=392
I_TO_X.__name__='I_TO_X'

class X_TO_I(Builtin):
    min_args=1
    max_args=2
    name='X_TO_I'
    opcode=393
X_TO_I.__name__='X_TO_I'

class MAP(Builtin):
    min_args=2
    max_args=2
    name='MAP'
    opcode=394
MAP.__name__='MAP'

class COMPILE_DEPENDENCY(Builtin):
    min_args=1
    max_args=1
    name='COMPILE_DEPENDENCY'
    opcode=395
COMPILE_DEPENDENCY.__name__='COMPILE_DEPENDENCY'

class DECOMPILE_DEPENDENCY(Builtin):
    min_args=1
    max_args=1
    name='DECOMPILE_DEPENDENCY'
    opcode=396
DECOMPILE_DEPENDENCY.__name__='DECOMPILE_DEPENDENCY'

class BUILD_CALL(Builtin):
    min_args=3
    max_args=254
    name='BUILD_CALL'
    opcode=397
BUILD_CALL.__name__='BUILD_CALL'

class ERRORLOGS_OF(Builtin):
    min_args=1
    max_args=1
    name='ERRORLOGS_OF'
    opcode=398
ERRORLOGS_OF.__name__='ERRORLOGS_OF'

class PERFORMANCE_OF(Builtin):
    min_args=1
    max_args=1
    name='PERFORMANCE_OF'
    opcode=399
PERFORMANCE_OF.__name__='PERFORMANCE_OF'

class XD(Builtin):
    min_args=1
    max_args=1
    name='XD'
    opcode=400
XD.__name__='XD'

class CONDITION_OF(Builtin):
    min_args=1
    max_args=1
    name='CONDITION_OF'
    opcode=401
CONDITION_OF.__name__='CONDITION_OF'

class SORT(Builtin):
    min_args=1
    max_args=2
    name='SORT'
    opcode=402
SORT.__name__='SORT'

class dTHIS(Builtin):
    min_args=0
    max_args=0
    name='$THIS'
    opcode=403
dTHIS.__name__='$THIS'

class DATA_WITH_UNITS(Builtin):
    min_args=1
    max_args=1
    name='DATA_WITH_UNITS'
    opcode=404
DATA_WITH_UNITS.__name__='DATA_WITH_UNITS'

class dATM(Builtin):
    min_args=0
    max_args=0
    name='$ATM'
    opcode=405
dATM.__name__='$ATM'

class dEPSILON0(Builtin):
    min_args=0
    max_args=0
    name='$EPSILON0'
    opcode=406
dEPSILON0.__name__='$EPSILON0'

class dGN(Builtin):
    min_args=0
    max_args=0
    name='$GN'
    opcode=407
dGN.__name__='$GN'

class dMU0(Builtin):
    min_args=0
    max_args=0
    name='$MU0'
    opcode=408
dMU0.__name__='$MU0'

class EXTRACT(Builtin):
    min_args=3
    max_args=3
    name='EXTRACT'
    opcode=409
EXTRACT.__name__='EXTRACT'

class FINITE(Builtin):
    min_args=1
    max_args=1
    name='FINITE'
    opcode=410
FINITE.__name__='FINITE'

class BIT_SIZE(Builtin):
    min_args=1
    max_args=1
    name='BIT_SIZE'
    opcode=411
BIT_SIZE.__name__='BIT_SIZE'

class MODULO(Builtin):
    min_args=2
    max_args=2
    name='MODULO'
    opcode=412
MODULO.__name__='MODULO'

class SELECTED_INT_KIND(Builtin):
    min_args=1
    max_args=1
    name='SELECTED_INT_KIND'
    opcode=413
SELECTED_INT_KIND.__name__='SELECTED_INT_KIND'

class SELECTED_REAL_KIND(Builtin):
    min_args=1
    max_args=2
    name='SELECTED_REAL_KIND'
    opcode=414
SELECTED_REAL_KIND.__name__='SELECTED_REAL_KIND'

class DSQL(Builtin):
    min_args=1
    max_args=254
    name='DSQL'
    opcode=415
DSQL.__name__='DSQL'

class ISQL(Builtin):
    min_args=1
    max_args=1
    name='ISQL'
    opcode=416
ISQL.__name__='ISQL'

class FTELL(Builtin):
    min_args=1
    max_args=1
    name='FTELL'
    opcode=417
FTELL.__name__='FTELL'

class MAKE_ACTION(Builtin):
    min_args=2
    max_args=5
    name='MAKE_ACTION'
    opcode=418
MAKE_ACTION.__name__='MAKE_ACTION'

class MAKE_CONDITION(Builtin):
    min_args=2
    max_args=2
    name='MAKE_CONDITION'
    opcode=419
MAKE_CONDITION.__name__='MAKE_CONDITION'

class MAKE_CONGLOM(Builtin):
    min_args=4
    max_args=4
    name='MAKE_CONGLOM'
    opcode=420
MAKE_CONGLOM.__name__='MAKE_CONGLOM'

class MAKE_DEPENDENCY(Builtin):
    min_args=3
    max_args=3
    name='MAKE_DEPENDENCY'
    opcode=421
MAKE_DEPENDENCY.__name__='MAKE_DEPENDENCY'

class MAKE_DIM(Builtin):
    min_args=2
    max_args=2
    name='MAKE_DIM'
    opcode=422
MAKE_DIM.__name__='MAKE_DIM'

class MAKE_DISPATCH(Builtin):
    min_args=5
    max_args=5
    name='MAKE_DISPATCH'
    opcode=423
MAKE_DISPATCH.__name__='MAKE_DISPATCH'

class MAKE_FUNCTION(Builtin):
    min_args=1
    max_args=254
    name='MAKE_FUNCTION'
    opcode=424
MAKE_FUNCTION.__name__='MAKE_FUNCTION'

class MAKE_METHOD(Builtin):
    min_args=3
    max_args=254
    name='MAKE_METHOD'
    opcode=425
MAKE_METHOD.__name__='MAKE_METHOD'

class MAKE_PARAM(Builtin):
    min_args=3
    max_args=3
    name='MAKE_PARAM'
    opcode=426
MAKE_PARAM.__name__='MAKE_PARAM'

class MAKE_PROCEDURE(Builtin):
    min_args=3
    max_args=254
    name='MAKE_PROCEDURE'
    opcode=427
MAKE_PROCEDURE.__name__='MAKE_PROCEDURE'

class MAKE_PROGRAM(Builtin):
    min_args=2
    max_args=2
    name='MAKE_PROGRAM'
    opcode=428
MAKE_PROGRAM.__name__='MAKE_PROGRAM'

class MAKE_RANGE(Builtin):
    min_args=2
    max_args=3
    name='MAKE_RANGE'
    opcode=429
MAKE_RANGE.__name__='MAKE_RANGE'

class MAKE_ROUTINE(Builtin):
    min_args=3
    max_args=254
    name='MAKE_ROUTINE'
    opcode=430
MAKE_ROUTINE.__name__='MAKE_ROUTINE'

class MAKE_SIGNAL(Builtin):
    min_args=2
    max_args=10
    name='MAKE_SIGNAL'
    opcode=431
MAKE_SIGNAL.__name__='MAKE_SIGNAL'

class MAKE_WINDOW(Builtin):
    min_args=3
    max_args=3
    name='MAKE_WINDOW'
    opcode=432
MAKE_WINDOW.__name__='MAKE_WINDOW'

class MAKE_WITH_UNITS(Builtin):
    min_args=2
    max_args=2
    name='MAKE_WITH_UNITS'
    opcode=433
MAKE_WITH_UNITS.__name__='MAKE_WITH_UNITS'

class MAKE_CALL(Builtin):
    min_args=3
    max_args=254
    name='MAKE_CALL'
    opcode=434
MAKE_CALL.__name__='MAKE_CALL'

class CLASS_OF(Builtin):
    min_args=1
    max_args=1
    name='CLASS_OF'
    opcode=435
CLASS_OF.__name__='CLASS_OF'

class DSCPTR_OF(Builtin):
    min_args=1
    max_args=2
    name='DSCPTR_OF'
    opcode=436
DSCPTR_OF.__name__='DSCPTR_OF'

class KIND_OF(Builtin):
    min_args=1
    max_args=1
    name='KIND_OF'
    opcode=437
KIND_OF.__name__='KIND_OF'

class NDESC_OF(Builtin):
    min_args=1
    max_args=1
    name='NDESC_OF'
    opcode=438
NDESC_OF.__name__='NDESC_OF'

class ACCUMULATE(Builtin):
    min_args=1
    max_args=3
    name='ACCUMULATE'
    opcode=439
ACCUMULATE.__name__='ACCUMULATE'

class MAKE_SLOPE(Builtin):
    min_args=1
    max_args=254
    name='MAKE_SLOPE'
    opcode=440
MAKE_SLOPE.__name__='MAKE_SLOPE'

class REM(Builtin):
    min_args=1
    max_args=254
    name='REM'
    opcode=441
REM.__name__='REM'

class COMPLETION_MESSAGE_OF(Builtin):
    min_args=1
    max_args=1
    name='COMPLETION_MESSAGE_OF'
    opcode=442
COMPLETION_MESSAGE_OF.__name__='COMPLETION_MESSAGE_OF'

class INTERRUPT_OF(Builtin):
    min_args=1
    max_args=1
    name='INTERRUPT_OF'
    opcode=443
INTERRUPT_OF.__name__='INTERRUPT_OF'

class dSHOTNAME(Builtin):
    min_args=0
    max_args=0
    name='$SHOTNAME'
    opcode=444
dSHOTNAME.__name__='$SHOTNAME'

class BUILD_WITH_ERROR(Builtin):
    min_args=2
    max_args=2
    name='BUILD_WITH_ERROR'
    opcode=445
BUILD_WITH_ERROR.__name__='BUILD_WITH_ERROR'

class ERROR_OF(Builtin):
    min_args=1
    max_args=1
    name='ERROR_OF'
    opcode=446
ERROR_OF.__name__='ERROR_OF'

class MAKE_WITH_ERROR(Builtin):
    min_args=2
    max_args=2
    name='MAKE_WITH_ERROR'
    opcode=447
MAKE_WITH_ERROR.__name__='MAKE_WITH_ERROR'

class DO_TASK(Builtin):
    min_args=1
    max_args=1
    name='DO_TASK'
    opcode=448
DO_TASK.__name__='DO_TASK'

class ISQL_SET(Builtin):
    min_args=1
    max_args=3
    name='ISQL_SET'
    opcode=449
ISQL_SET.__name__='ISQL_SET'

class FS_FLOAT(Builtin):
    min_args=1
    max_args=1
    name='FS_FLOAT'
    opcode=450
FS_FLOAT.__name__='FS_FLOAT'

class FS_COMPLEX(Builtin):
    min_args=1
    max_args=2
    name='FS_COMPLEX'
    opcode=451
FS_COMPLEX.__name__='FS_COMPLEX'

class FT_FLOAT(Builtin):
    min_args=1
    max_args=1
    name='FT_FLOAT'
    opcode=452
FT_FLOAT.__name__='FT_FLOAT'

class FT_COMPLEX(Builtin):
    min_args=1
    max_args=2
    name='FT_COMPLEX'
    opcode=453
FT_COMPLEX.__name__='FT_COMPLEX'

class BUILD_OPAQUE(Builtin):
    min_args=2
    max_args=2
    name='BUILD_OPAQUE'
    opcode=454
BUILD_OPAQUE.__name__='BUILD_OPAQUE'

class MAKE_OPAQUE(Builtin):
    min_args=2
    max_args=2
    name='MAKE_OPAQUE'
    opcode=455
MAKE_OPAQUE.__name__='MAKE_OPAQUE'

globs=list(globals().values())
for c in globs:
    if hasattr(c,'__name__') and hasattr(c,'name'):
        try:
            del globals()[c.__name__]
        except:
            pass
        globals()[c.name]=c
        c.__name__=c.name
