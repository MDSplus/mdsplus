import ctypes as _C
import numpy as _N

DTYPE_Z=0
DTYPE_V=1
DTYPE_BU=2
DTYPE_WU=3
DTYPE_LU=4
DTYPE_QU=5
DTYPE_B=6
DTYPE_W=7
DTYPE_L=8
DTYPE_Q=9
DTYPE_F=10
DTYPE_D=11
DTYPE_FC=12
DTYPE_DC=13
DTYPE_T=14
DTYPE_NU=15
DTYPE_NL=16
DTYPE_NLO=17
DTYPE_NR=18
DTYPE_NRO=19
DTYPE_NZ=20
DTYPE_P=21
DTYPE_ZI=22
DTYPE_ZEM=23
DTYPE_DSC=24
DTYPE_OU=25
DTYPE_O=26
DTYPE_G=27
DTYPE_H=28
DTYPE_GC=29
DTYPE_HC=30
DTYPE_CIT=31
DTYPE_BPV=32
DTYPE_BLV=33
DTYPE_VU=34
DTYPE_ADT=35
DTYPE_VT=37
DTYPE_FS=52
DTYPE_FT=53
DTYPE_FSC=54
DTYPE_FTC=55
DTYPE_IDENT=191
DTYPE_NID=192
DTYPE_PATH=193
DTYPE_PARAM=194
DTYPE_SIGNAL=195
DTYPE_DIMENSION=196
DTYPE_WINDOW=197
DTYPE_SLOPE=198
DTYPE_FUNCTION=199
DTYPE_CONGLOM=200
DTYPE_RANGE=201
DTYPE_ACTION=202
DTYPE_DISPATCH=203
DTYPE_PROGRAM=204
DTYPE_ROUTINE=205
DTYPE_PROCEDURE=206
DTYPE_METHOD=207
DTYPE_DEPENDENCY=208
DTYPE_CONDITION=209
DTYPE_EVENT=210
DTYPE_WITH_UNITS=211
DTYPE_CALL=212
DTYPE_WITH_ERROR=213
DTYPE_LIST=214
DTYPE_TUPLE=215
DTYPE_DICTIONARY=216
DTYPE_MISSING=0
DTYPE_NATIVE_DOUBLE=DTYPE_FT
DTYPE_NATIVE_FLOAT=DTYPE_FS
DTYPE_DOUBLE_COMPLEX=DTYPE_FTC
DTYPE_DOUBLE=DTYPE_NATIVE_DOUBLE
DTYPE_FLOAT_COMPLEX=DTYPE_FSC
DTYPE_FLOAT=DTYPE_NATIVE_FLOAT

class mdsdtypes:
    ctypes={2:_C.c_ubyte,3:_C.c_ushort,4:_C.c_uint,5:_C.c_ulonglong,6:_C.c_byte,7:_C.c_short,8:_C.c_int,9:_C.c_longlong,14:_C.c_void_p,52:_C.c_float,53:_C.c_double}
    numpytypes={2:_N.uint8,3:_N.uint16,4:_N.uint32,5:_N.uint64,6:_N.int8,7:_N.int16,8:_N.int32,9:_N.int64,14:_N.string_,DTYPE_NATIVE_FLOAT:_N.float32,DTYPE_NATIVE_DOUBLE:_N.float64}
    m_to_n_types={}
    dtype=-1

    def __init__(self,dtype):
        self.dtype=dtype
    
    def __str__(self):
        names={0:'DTYPE_Z',1:'DTYPE_V',2:'DTYPE_BU',3:'DTYPE_WU',4:'DTYPE_LU',5:'DTYPE_QU',6:'DTYPE_B',7:'DTYPE_W',8:'DTYPE_L',9:'DTYPE_Q',
               10:'DTYPE_F',11:'DTYPE_D',12:'DTYPE_FC',13:'DTYPE_DC',14:'DTYPE_T',15:'DTYPE_NU',16:'DTYPE_NL',17:'DTYPE_NLO',18:'DTYPE_NR',
               19:'DTYPE_NRO',20:'DTYPE_NZ',21:'DTYPE_P',22:'DTYPE_ZI',23:'DTYPE_ZEM',24:'DTYPE_DSC',25:'DTYPE_OU',26:'DTYPE_O',27:'DTYPE_G',
               28:'DTYPE_H',29:'DTYPE_GC',30:'DTYPE_HC',31:'DTYPE_CIT',32:'DTYPE_BPV',33:'DTYPE_BLV',34:'DTYPE_VU',35:'DTYPE_ADT',
               37:'DTYPE_VT',52:'DTYPE_FS',53:'DTYPE_FT',54:'DTYPE_FSC',55:'DTYPE_FTC',191:'DTYPE_IDENT',192:'DTYPE_NID',193:'DTYPE_PATH',
               194:'DTYPE_PARAM',195:'DTYPE_SIGNAL',196:'DTYPE_DIMENSION',197:'DTYPE_WINDOW',198:'DTYPE_SLOPE',199:'DTYPE_FUNCTION',200:'DTYPE_CONGLOM',
               201:'DTYPE_RANGE',202:'DTYPE_ACTION',203:'DTYPE_DISPATCH',204:'DTYPE_PROGRAM',205:'DTYPE_ROUTINE',206:'DTYPE_PROCEDURE',
               207:'DTYPE_METHOD',208:'DTYPE_DEPENDENCY',209:'DTYPE_CONDITION',210:'DTYPE_EVENT',211:'DTYPE_WITH_UNITS',212:'DTYPE_CALL',
               213:'DTYPE_WITH_ERROR',214:'DTYPE_LIST',215:'DTYPE_TUPLE',216:'DTYPE_DICTIONARY'}
        if (self.dtype in names):
            return names[self.dtype]
        else:
            return 'DTYPE_?'

    def toCtype(self):
        if self.dtype in mdsdtypes.ctypes:
            return mdsdtypes.ctypes[self.dtype]
        else:
            raise TypeError,'Cannot convert '+str(self.dtype)+' to ctype'

    def toNumpy(self):
        numpytypes={2:_N.uint8,3:_N.uint16,4:_N.uint32,5:_N.uint64,6:_N.int8,7:_N.int16,8:_N.int32,9:_N.int64,DTYPE_NATIVE_FLOAT:_N.float32,DTYPE_NATIVE_DOUBLE:_N.float64}
        if self.dtype in mdsdtypes.numpytypes:
            return mdsdtypes.numpytypes[self.dtype]
        else:
            raise TypeError,'Cannot convert '+str(self.dtype)+' to numpy type'

    def fromNumpy(cls,value):
        if not mdsdtypes.m_to_n_types:
            for d in mdsdtypes.numpytypes:
                mdsdtypes.m_to_n_types.setdefault(str((mdsdtypes.numpytypes[d])().dtype),d)
        if str(value.dtype)[1:2]=='S':
            return DTYPE_T
        try:
            return mdsdtypes.m_to_n_types[str(value.dtype)]
        except KeyError:
            print mdsdtypes.m_to_n_types
            raise TypeError, 'Cannot convert '+str(value.dtype)+' to mdsplus type'
    fromNumpy=classmethod(fromNumpy)
