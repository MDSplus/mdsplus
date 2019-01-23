/*	the dtypes
 *
 *	Reserved data type codes:
 *	codes 38-191 are reserved to DIGITAL;
 *	codes 160-191 are reserved to DIGITAL facilities for facility-specific purposes;
 *	codes 192-255 are reserved for DIGITAL's Computer Special Systems Group
 *	  and for customers for their own use.
 */
DEFINE(MISSING		,0)
DEFINE(BU		,2)	/* byte (unsigned);  8-bit unsigned quantity */
DEFINE(WU		,3)	/* word (unsigned);  16-bit unsigned quantity */
DEFINE(LU		,4)	/* longword (unsigned);  32-bit unsigned quantity */
DEFINE(QU		,5)	/* quadword (unsigned);  64-bit unsigned quantity */
DEFINE(OU		,25)	/* octaword (unsigned);  128-bit unsigned quantity */
DEFINE(B		,6)	/* byte integer (signed);  8-bit signed 2's-complement integer */
DEFINE(W		,7)	/* word integer (signed);  16-bit signed 2's-complement integer */
DEFINE(L		,8)	/* longword integer (signed);  32-bit signed 2's-complement integer */
DEFINE(Q		,9)	/* quadword integer (signed);  64-bit signed 2's-complement integer */
DEFINE(O		,26)	/* octaword integer (signed);  128-bit signed 2's-complement integer */
DEFINE(F		,10)	/* F_floating;  32-bit single-precision floating point */
DEFINE(D		,11)	/* D_floating;  64-bit double-precision floating point */
DEFINE(G		,27)	/* G_floating;  64-bit double-precision floating point */
DEFINE(H		,28)	/* H_floating;  128-bit quadruple-precision floating point */
DEFINE(FC		,12)	/* F_floating complex */
DEFINE(DC		,13)	/* D_floating complex */
DEFINE(GC		,29)	/* G_floating complex */
DEFINE(HC		,30)	/* H_floating complex */
DEFINE(CIT		,31)	/* COBOL Intermediate Temporary */
/*
 *	String data types:
 */
DEFINE(T		,14)	/* character string;  a single 8-bit character or a sequence of characters */
DEFINE(VT		,37)	/* varying character string;  16-bit count) followed by a string */
DEFINE(NU		,15)	/* numeric string) unsigned */
DEFINE(NL		,16)	/* numeric string) left separate sign */
DEFINE(NLO		,17)	/* numeric string) left overpunched sign */
DEFINE(NR		,18)	/* numeric string) right separate sign */
DEFINE(NRO		,19)	/* numeric string) right overpunched sign */
DEFINE(NZ		,20)	/* numeric string) zoned sign */
DEFINE(P		,21)	/* packed decimal string */
DEFINE(V		,1)	/* aligned bit string */
DEFINE(VU		,34)	/* unaligned bit string */
/*
 *	IEEE data types:
 */
DEFINE(FS		,52)	/* IEEE float basic single S */
DEFINE(FT		,53)	/* IEEE float basic double T */
DEFINE(FSC		,54)	/* IEEE float basic single S complex */
DEFINE(FTC		,55)	/* IEEE float basic double T complex */
/*
 *	Miscellaneous data types:
 */
DEFINE(ZI		,22)	/* sequence of instructions */
DEFINE(ZEM		,23)	/* procedure entry mask */
DEFINE(DSC		,24)	/* descriptor */
DEFINE(BPV		,32)	/* bound procedure value */
DEFINE(BLV		,33)	/* bound label value */
DEFINE(ADT		,35)	/* absolute date and time */
DEFINE(POINTER		,51)
/*
 *	class R
 */
DEFINE(LIST		,214)
DEFINE(TUPLE		,215)
DEFINE(DICTIONARY	,216)
DEFINE(IDENT		,191)
DEFINE(NID		,192)
DEFINE(PATH		,193)
DEFINE(PARAM		,194)
DEFINE(SIGNAL		,195)
DEFINE(DIMENSION	,196)
DEFINE(WINDOW		,197)
DEFINE(SLOPE		,198)	/* Do not use this deprecated type */
DEFINE(FUNCTION		,199)
DEFINE(CONGLOM		,200)
DEFINE(RANGE		,201)
DEFINE(ACTION		,202)
DEFINE(DISPATCH		,203)
DEFINE(PROGRAM		,204)
DEFINE(ROUTINE		,205)
DEFINE(PROCEDURE	,206)
DEFINE(METHOD		,207)
DEFINE(DEPENDENCY	,208)
DEFINE(CONDITION	,209)
DEFINE(EVENT		,210)
DEFINE(WITH_UNITS	,211)
DEFINE(CALL		,212)
DEFINE(WITH_ERROR	,213)
DEFINE(OPAQUE		,217)
