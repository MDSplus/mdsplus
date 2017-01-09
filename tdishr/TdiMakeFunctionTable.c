/*  CMS REPLACEMENT HISTORY, Element TDI$$MAKE_FUNCTION_TABLE.C */
/*  *8    26-JUL-1996 13:44:09 TWF "Fix extern initialization" */
/*  *7    26-JUL-1996 08:53:01 TWF "Remove extern_model pragmas" */
/*  *6    27-JUN-1996 09:10:22 TWF "Port to Unix/Windows" */
/*  *5    21-JUN-1996 15:29:36 TWF "Port to Unix/Windows" */
/*  *4    18-OCT-1995 11:51:22 TWF "make all includes the same case" */
/*  *3     2-NOV-1994 08:58:17 TWF "Change extern model" */
/*  *2    25-OCT-1994 08:21:07 TWF "Add TdiMAX_FUNCTION" */
/*  *1    25-OCT-1994 08:18:33 TWF "Make TdiRefFunction table" */
/*  CMS REPLACEMENT HISTORY, Element TDI$$MAKE_FUNCTION_TABLE.C */
#include <STATICdef.h>
#include <mdsdescrip.h>
#define COM

#define OPC(name,builtin,f1,f2,f3,i1,i2,o1,o2,m1,m2,token) extern int Tdi1##f1(), Tdi2##f2(), Tdi3##f3();
#include "opcbuiltins.h"
#undef OPC



	/******************************
        Shorthand for conversions:
        Destinctive forms.
        VV=logical 0/1
        XX=lowest support.
        YY=highest.
        ******************************/

#define DTYPE_SUBSCRIPT DTYPE_L
#define DTYPE_UNITS     DTYPE_T
#define DTYPE_VV                DTYPE_BU
#define DTYPE_XX                DTYPE_T
#define DTYPE_YY                DTYPE_HC

#include "tdirefzone.h"

#include "tdiyacc.h"
YYSTYPE YYLVAL = { {0} };

#define LEX_OK 0
#define OK 0
#define C LEX_K_C
#define I LEX_K_IMMED
#define N LEX_K_NAMED
#define S LEX_K_SYMBOL
#define U LEX_K_UNUSUAL

#include "tdireffunction.h"

const struct TdiFunctionStruct TdiRefFunction[] = {
#define OPC(name,builtin,f1,f2,f3,i1,i2,o1,o2,m1,m2,token) {#builtin,&Tdi1##f1,&Tdi2##f2,&Tdi3##f3, \
           DTYPE_##i1,DTYPE_##i2,DTYPE_##o1,DTYPE_##o2,m1,m2,token},
#include "opcbuiltins.h"
#undef OPC
  {0}
};

const int TdiFUNCTION_MAX = sizeof(TdiRefFunction) / sizeof(struct TdiFunctionStruct) - 1;
