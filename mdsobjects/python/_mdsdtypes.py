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
DTYPE_OPAQUE=217
DTYPE_MISSING=0

DTYPE_NATIVE_DOUBLE=DTYPE_FT
DTYPE_NATIVE_FLOAT=DTYPE_FS
DTYPE_DOUBLE_COMPLEX=DTYPE_FTC
DTYPE_DOUBLE=DTYPE_NATIVE_DOUBLE
DTYPE_FLOAT_COMPLEX=DTYPE_FSC
DTYPE_FLOAT=DTYPE_NATIVE_FLOAT

class mdsdtypes:
    ctypes={DTYPE_BU:_C.c_ubyte,DTYPE_WU:_C.c_ushort,DTYPE_LU:_C.c_uint,DTYPE_QU:_C.c_ulonglong,DTYPE_B:_C.c_byte,DTYPE_W:_C.c_short,DTYPE_L:_C.c_int32,DTYPE_Q:_C.c_longlong,DTYPE_T:_C.c_void_p,DTYPE_FS:_C.c_float,DTYPE_FT:_C.c_double,DTYPE_FSC:None,DTYPE_FTC:None}
    numpytypes={DTYPE_BU:_N.uint8,DTYPE_WU:_N.uint16,DTYPE_LU:_N.uint32,DTYPE_QU:_N.uint64,
                DTYPE_B:_N.int8,DTYPE_W:_N.int16,DTYPE_L:_N.int32,DTYPE_Q:_N.int64,
                DTYPE_T:_N.string_,DTYPE_NATIVE_FLOAT:_N.float32,DTYPE_NATIVE_DOUBLE:_N.float64,DTYPE_FLOAT_COMPLEX:_N.complex64,DTYPE_DOUBLE_COMPLEX:_N.complex128}
    dtypenames={DTYPE_Z:'DTYPE_Z',DTYPE_V:'DTYPE_V',
                DTYPE_BU:'DTYPE_BU',DTYPE_WU:'DTYPE_WU',DTYPE_LU:'DTYPE_LU',DTYPE_QU:'DTYPE_QU',
                DTYPE_B:'DTYPE_B',DTYPE_W:'DTYPE_W',DTYPE_L:'DTYPE_L',DTYPE_Q:'DTYPE_Q',
                DTYPE_F:'DTYPE_F',DTYPE_D:'DTYPE_D',DTYPE_FC:'DTYPE_FC',DTYPE_DC:'DTYPE_DC',DTYPE_T:'DTYPE_T',DTYPE_NU:'DTYPE_NU',DTYPE_NL:'DTYPE_NL',DTYPE_NLO:'DTYPE_NLO',DTYPE_NR:'DTYPE_NR',
                DTYPE_NRO:'DTYPE_NRO',DTYPE_NZ:'DTYPE_NZ',DTYPE_P:'DTYPE_P',DTYPE_ZI:'DTYPE_ZI',DTYPE_ZEM:'DTYPE_ZEM',DTYPE_DSC:'DTYPE_DSC',DTYPE_OU:'DTYPE_OU',DTYPE_O:'DTYPE_O',DTYPE_G:'DTYPE_G',
                DTYPE_H:'DTYPE_H',DTYPE_GC:'DTYPE_GC',DTYPE_HC:'DTYPE_HC',DTYPE_CIT:'DTYPE_CIT',DTYPE_BPV:'DTYPE_BPV',DTYPE_BLV:'DTYPE_BLV',DTYPE_VU:'DTYPE_VU',DTYPE_ADT:'DTYPE_ADT',
                DTYPE_VT:'DTYPE_VT',DTYPE_FS:'DTYPE_FS',DTYPE_FT:'DTYPE_FT',DTYPE_FSC:'DTYPE_FSC',DTYPE_FTC:'DTYPE_FTC',DTYPE_IDENT:'DTYPE_IDENT',DTYPE_NID:'DTYPE_NID',DTYPE_PATH:'DTYPE_PATH',
                DTYPE_PARAM:'DTYPE_PARAM',DTYPE_SIGNAL:'DTYPE_SIGNAL',DTYPE_DIMENSION:'DTYPE_DIMENSION',DTYPE_WINDOW:'DTYPE_WINDOW',DTYPE_SLOPE:'DTYPE_SLOPE',DTYPE_FUNCTION:'DTYPE_FUNCTION',DTYPE_CONGLOM:'DTYPE_CONGLOM',
                DTYPE_RANGE:'DTYPE_RANGE',DTYPE_ACTION:'DTYPE_ACTION',DTYPE_DISPATCH:'DTYPE_DISPATCH',DTYPE_PROGRAM:'DTYPE_PROGRAM',DTYPE_ROUTINE:'DTYPE_ROUTINE',DTYPE_PROCEDURE:'DTYPE_PROCEDURE',
                DTYPE_METHOD:'DTYPE_METHOD',DTYPE_DEPENDENCY:'DTYPE_DEPENDENCY',DTYPE_CONDITION:'DTYPE_CONDITION',DTYPE_EVENT:'DTYPE_EVENT',DTYPE_WITH_UNITS:'DTYPE_WITH_UNITS',DTYPE_CALL:'DTYPE_CALL',
                DTYPE_WITH_ERROR:'DTYPE_WITH_ERROR',DTYPE_LIST:'DTYPE_LIST',DTYPE_TUPLE:'DTYPE_TUPLE',DTYPE_DICTIONARY:'DTYPE_DICTIONARY',DTYPE_OPAQUE:'DTYPE_OPAQUE'}
    m_to_n_types={}
    name_to_dtype={}
    dtype=-1

    def __init__(self,dtype):
        self.dtype=dtype

    def __str__(self):
        if (self.dtype in mdsdtypes.dtypenames):
            return mdsdtypes.dtypenames[self.dtype]
        else:
            return 'DTYPE_?'

    def toCtype(self):
        if self.dtype in mdsdtypes.ctypes:
            return mdsdtypes.ctypes[self.dtype]
        else:
            raise TypeError('Cannot convert '+str(self.dtype)+' to ctype')

    def toNumpy(self):
        if self.dtype in mdsdtypes.numpytypes:
            return mdsdtypes.numpytypes[self.dtype]
        else:
            raise TypeError('Cannot convert '+str(self.dtype)+' to numpy type')

    def fromNumpy(cls,value):
        if not mdsdtypes.m_to_n_types:
            for d in mdsdtypes.numpytypes:
               mdsdtypes.m_to_n_types.setdefault(str((mdsdtypes.numpytypes[d])().dtype),d)
            mdsdtypes.m_to_n_types.setdefault("bool",DTYPE_BU)
        if str(value.dtype)[1:2]=='S' or str(value.dtype)[1:2]=='U':
            return DTYPE_T
        try:
            return mdsdtypes.m_to_n_types[str(value.dtype)]
        except KeyError:
            raise TypeError('Cannot convert '+str(value.dtype)+' to mdsplus type')
    fromNumpy=classmethod(fromNumpy)

    def fromName(name):
        if not mdsdtypes.name_to_dtype:
            tmp=mdsdtypes.dtypenames.copy()
            while 1:
                try:
                    d = tmp.popitem()
                    mdsdtypes.name_to_dtype.setdefault(d[1],d[0])
                except:
                    break
        return mdsdtypes.name_to_dtype[name]
    fromName=staticmethod(fromName)
