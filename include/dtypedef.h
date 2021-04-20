/*
 *	atomic data types 0 - 37, 51-55
 */
DEFINE(MISSING, 0)      /* no data */
DEFINE(V_deprecated, 1) /* aligned bit string */
DEFINE(BU, 2)           /* byte (unsigned);  8-bit unsigned quantity */
DEFINE(WU, 3)           /* word (unsigned);  16-bit unsigned quantity */
DEFINE(LU, 4)           /* longword (unsigned);  32-bit unsigned quantity */
DEFINE(QU, 5)           /* quadword (unsigned);  64-bit unsigned quantity */
DEFINE(B, 6)            /* byte integer (signed);  8-bit signed 2's-complement integer */
DEFINE(W, 7)            /* word integer (signed);  16-bit signed 2's-complement integer */
DEFINE(L,
       8) /* longword integer (signed);  32-bit signed 2's-complement integer */
DEFINE(Q,
       9)                  /* quadword integer (signed);  64-bit signed 2's-complement integer */
DEFINE(F, 10)              /* F_floating;	32-bit single-precision floating point */
DEFINE(D, 11)              /* D_floating;	64-bit double-precision floating point */
DEFINE(FC, 12)             /* F_floating complex */
DEFINE(DC, 13)             /* D_floating complex */
DEFINE(T, 14)              /* character string;  a single 8-bit character or a sequence of
                  characters */
DEFINE(NU_deprecated, 15)  /* numeric string) unsigned */
DEFINE(NL_deprecated, 16)  /* numeric string) left separate sign */
DEFINE(NLO_deprecated, 17) /* numeric string) left overpunched sign */
DEFINE(NR_deprecated, 18)  /* numeric string) right separate sign */
DEFINE(NRO_deprecated, 19) /* numeric string) right overpunched sign */
DEFINE(NZ_deprecated, 20)  /* numeric string) zoned sign */
DEFINE(P_deprecated, 21)   /* packed decimal string */
DEFINE(ZI_deprecated, 22)  /* sequence of instructions */
DEFINE(ZEM_deprecated, 23) /* procedure entry mask */
DEFINE(DSC, 24)            /* descriptor */
DEFINE(OU, 25)             /* octaword (unsigned);  128-bit unsigned quantity */
DEFINE(
    O,
    26)                    /* octaword integer (signed);  128-bit signed 2's-complement integer */
DEFINE(G, 27)              /* G_floating;	64-bit double-precision floating point */
DEFINE(H, 28)              /* H_floating;	128-bit quadruple-precision floating point */
DEFINE(GC, 29)             /* G_floating complex */
DEFINE(HC, 30)             /* H_floating complex */
DEFINE(CIT_deprecated, 31) /* COBOL Intermediate Temporary */
DEFINE(BPV_deprecated, 32) /* bound procedure value */
DEFINE(BLV_deprecated, 33) /* bound label value */
DEFINE(VU_deprecated, 34)  /* unaligned bit string */
DEFINE(ADT_deprecated, 35) /* absolute date and time */
DEFINE(VT_deprecated,
       37) /* varying character string;  16-bit count) followed by a string */
/*
 *	38 - 50 might have been used for RESERVED purposes
 */
DEFINE(POINTER, 51)
DEFINE(FS, 52)  /* IEEE float basic single S */
DEFINE(FT, 53)  /* IEEE float basic double T */
DEFINE(FSC, 54) /* IEEE float basic single S complex */
DEFINE(FTC, 55) /* IEEE float basic double T complex */
DEFINE(C, 56)   /* used for TdiCall returned char* that must be freed */

DEFINE(IDENT, 191)
DEFINE(NID, 192)
DEFINE(PATH, 193)
DEFINE(PARAM, 194)      /* class R */
DEFINE(SIGNAL, 195)     /* class R */
DEFINE(DIMENSION, 196)  /* class R */
DEFINE(WINDOW, 197)     /* class R */
DEFINE(SLOPE, 198)      /* deprecated class R*/
DEFINE(FUNCTION, 199)   /* class R */
DEFINE(CONGLOM, 200)    /* class R */
DEFINE(RANGE, 201)      /* class R */
DEFINE(ACTION, 202)     /* class R */
DEFINE(DISPATCH, 203)   /* class R */
DEFINE(PROGRAM, 204)    /* deprecated class R*/
DEFINE(ROUTINE, 205)    /* class R */
DEFINE(PROCEDURE, 206)  /* deprecated class R*/
DEFINE(METHOD, 207)     /* class R */
DEFINE(DEPENDENCY, 208) /* deprecated class R*/
DEFINE(CONDITION, 209)  /* deprecated class R*/
DEFINE(EVENT, 210)      /* class R */
DEFINE(WITH_UNITS, 211) /* class R */
DEFINE(CALL, 212)       /* class R */
DEFINE(WITH_ERROR, 213) /* class R */
DEFINE(LIST, 214)       /* class APD */
DEFINE(TUPLE, 215)      /* class APD */
DEFINE(DICTIONARY, 216) /* class APD */
DEFINE(OPAQUE, 217)     /* class R */
