/*  CMS REPLACEMENT HISTORY, Element TDI$MESSAGES.H */
/*  *10    4-SEP-1996 11:59:02 TWF "Support vax" */
/*  *9     4-SEP-1996 11:54:03 TWF "Support vax" */
/*  *8    26-JUL-1996 09:20:16 TWF "Remove extern_model pragmas" */
/*  *7    26-JUL-1996 08:52:44 TWF "Remove extern_model pragmas" */
/*  *6    18-JUN-1996 09:30:15 TWF "Port to Unix/Windows" */
/*  *5    18-JUN-1996 09:03:21 TWF "Port to Unix/Windows" */
/*  *4    17-JUN-1996 08:18:26 TWF "Port to Unix/Windows" */
/*  *3    12-JUN-1996 11:04:06 TWF "Port to Unix/Windows" */
/*  *2    26-OCT-1994 11:14:00 TWF "no vaxc" */
/*  *1    19-OCT-1994 11:34:05 TWF "Declaration of TDI messages" */
/*  CMS REPLACEMENT HISTORY, Element TDI$MESSAGES.H */
/* Declare TDI message values */
#ifdef __VMS
#pragma extern_model save
#pragma extern_model globalvalue
#ifdef __VAX
#define TdiBREAK TDI##$_BREAK
#define TdiCASE TDI##$_CASE
#define TdiCONTINUE TDI##$_CONTINUE
#define TdiEXTRANEOUS TDI##$_EXTRANEOUS
#define TdiRETURN TDI##$_RETURN
#define TdiABORT TDI##$_ABORT
#define TdiBAD_INDEX TDI##$_BAD_INDEX
#define TdiBOMB TDI##$_BOMB
#define TdiEXTRA_ARG TDI##$_EXTRA_ARG
#define TdiGOTO TDI##$_GOTO
#define TdiINVCLADSC TDI##$_INVCLADSC
#define TdiINVCLADTY TDI##$_INVCLADTY
#define TdiINVDTYDSC TDI##$_INVDTYDSC
#define TdiINV_OPC TDI##$_INV_OPC
#define TdiINV_SIZE TDI##$_INV_SIZE
#define TdiMISMATCH TDI##$_MISMATCH
#define TdiMISS_ARG TDI##$_MISS_ARG
#define TdiNDIM_OVER TDI##$_NDIM_OVER
#define TdiNO_CMPLX TDI##$_NO_CMPLX
#define TdiNO_OPC TDI##$_NO_OPC
#define TdiNO_OUTPTR TDI##$_NO_OUTPTR
#define TdiNO_SELF_PTR TDI##$_NO_SELF_PTR
#define TdiNOT_NUMBER TDI##$_NOT_NUMBER
#define TdiNULL_PTR TDI##$_NULL_PTR
#define TdiRECURSIVE TDI##$_RECURSIVE
#define TdiSIG_DIM TDI##$_SIG_DIM
#define TdiSTRTOOLON TDI##$_STRTOOLON
#define TdiSYNTAX TDI##$_SYNTAX
#define TdiTIMEOUT TDI##$_TIMEOUT
#define TdiTOO_BIG TDI##$_TOO_BIG
#define TdiUNBALANCE TDI##$_UNBALANCE
#define TdiUNKNOWN_VAR TDI##$_UNKNOWN_VAR
#endif
extern TdiBREAK;
extern TdiCASE;
extern TdiCONTINUE;
extern TdiEXTRANEOUS;
extern TdiRETURN;
extern TdiABORT;
extern TdiBAD_INDEX;
extern TdiBOMB;
extern TdiEXTRA_ARG;
extern TdiGOTO;
extern TdiINVCLADSC;
extern TdiINVCLADTY;
extern TdiINVDTYDSC;
extern TdiINV_OPC;
extern TdiINV_SIZE;
extern TdiMISMATCH;
extern TdiMISS_ARG;
extern TdiNDIM_OVER;
extern TdiNO_CMPLX;
extern TdiNO_OPC;
extern TdiNO_OUTPTR;
extern TdiNO_SELF_PTR;
extern TdiNOT_NUMBER;
extern TdiNULL_PTR;
extern TdiRECURSIVE;
extern TdiSIG_DIM;
extern TdiSTRTOOLON;
extern TdiSYNTAX;
extern TdiTIMEOUT;
extern TdiTOO_BIG;
extern TdiUNBALANCE;
extern TdiUNKNOWN_VAR;
#pragma extern_model restore
#else
#ifndef TdiBREAK
#define TdiBREAK 0xfd38008
#define TdiCASE 0xfd38010
#define TdiCONTINUE 0xfd38018
#define TdiEXTRANEOUS 0xfd38020
#define TdiRETURN 0xfd38028
#define TdiABORT 0xfd38032
#define TdiBAD_INDEX 0xfd3803a
#define TdiBOMB 0xfd38042
#define TdiEXTRA_ARG 0xfd3804a
#define TdiGOTO 0xfd38052
#define TdiINVCLADSC 0xfd3805a
#define TdiINVCLADTY 0xfd38062
#define TdiINVDTYDSC 0xfd3806a
#define TdiINV_OPC 0xfd38072
#define TdiINV_SIZE 0xfd3807a
#define TdiMISMATCH 0xfd38082
#define TdiMISS_ARG 0xfd3808a
#define TdiNDIM_OVER 0xfd38092
#define TdiNO_CMPLX 0xfd3809a
#define TdiNO_OPC 0xfd380a2
#define TdiNO_OUTPTR 0xfd380aa
#define TdiNO_SELF_PTR 0xfd380b2
#define TdiNOT_NUMBER 0xfd380ba
#define TdiNULL_PTR 0xfd380c2
#define TdiRECURSIVE 0xfd380ca
#define TdiSIG_DIM 0xfd380d2
#define TdiSTRTOOLON 0xfd380fc
#define TdiSYNTAX 0xfd380da
#define TdiTIMEOUT 0xfd38104
#define TdiTOO_BIG 0xfd380e2
#define TdiUNBALANCE 0xfd380ea
#define TdiUNKNOWN_VAR 0xfd380f2
#endif
#endif

#ifdef CREATE_STS_TEXT
#include        "facility_list.h"

static struct stsText  tdi_stsText[] = {
    STS_TEXT(TdiBREAK,"BREAK was not in DO FOR SWITCH or WHILE")
   ,STS_TEXT(TdiCASE,"CASE was not in SWITCH statement")
   ,STS_TEXT(TdiCONTINUE,"CONTINUE was not in DO FOR or WHILE")
   ,STS_TEXT(TdiEXTRANEOUS,"Some characters were unused, bad number maybe")
   ,STS_TEXT(TdiRETURN,"Extraneous RETURN statement, not from a FUN")
   ,STS_TEXT(TdiABORT,"Program requested abort")
   ,STS_TEXT(TdiBAD_INDEX,"Index or subscript is too small or too big")
   ,STS_TEXT(TdiBOMB,"Bad punctuation, could not compile the text")
   ,STS_TEXT(TdiEXTRA_ARG,"Too many arguments for function, watch commas")
   ,STS_TEXT(TdiGOTO,"GOTO target label not found")
   ,STS_TEXT(TdiINVCLADSC,"Storage class not valid, must be scalar or array")
   ,STS_TEXT(TdiINVCLADTY,"Invalid mixture of storage class and data type")
   ,STS_TEXT(TdiINVDTYDSC,"Storage data type is not valid")
   ,STS_TEXT(TdiINV_OPC,"Invalid operator code in a function")
   ,STS_TEXT(TdiINV_SIZE,"Number of elements does not match declaration")
   ,STS_TEXT(TdiMISMATCH,"Shape of arguments does not match")
   ,STS_TEXT(TdiMISS_ARG,"Missing argument is required for function")
   ,STS_TEXT(TdiNDIM_OVER,"Number of dimensions is over the allowed 8")
   ,STS_TEXT(TdiNO_CMPLX,"There are no complex forms of this function")
   ,STS_TEXT(TdiNO_OPC,"No support for this function, today")
   ,STS_TEXT(TdiNO_OUTPTR,"An output pointer is required")
   ,STS_TEXT(TdiNO_SELF_PTR,"No $VALUE is defined for signal or validation")
   ,STS_TEXT(TdiNOT_NUMBER,"Value is not a scalar number and must be")
   ,STS_TEXT(TdiNULL_PTR,"Null pointer where value needed")
   ,STS_TEXT(TdiRECURSIVE,"Overly recursive function, calls itself maybe")
   ,STS_TEXT(TdiSIG_DIM,"Signal dimension does not match data shape")
   ,STS_TEXT(TdiSYNTAX,"Bad punctuation or misspelled word or number")
   ,STS_TEXT(TdiTOO_BIG,"Conversion of number lost significant digits")
   ,STS_TEXT(TdiUNBALANCE,"Unbalanced () [] {} '' "" or /**/")
   ,STS_TEXT(TdiUNKNOWN_VAR,"Unknown/undefined variable name")
   ,STS_TEXT(TdiSTRTOOLON,"string is too long (greater than 65535)")
   ,STS_TEXT(TdiTIMEOUT,"task did not complete in alotted time")
   };
#endif
