/*      TDI$$DEF_FUNCTION.C
        Internal/intrinsic function table definition.
        Yacc tokens are put into TdiRefFunction table here.
        Precedence is associated with a token.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <STATICdef.h>
#include <mds_stdarg.h>
#include <libroutines.h>
#include <tdishr_messages.h>
#include <mdsdescrip.h>
#define COM



extern int TdiIntrinsic();
#define MdsEND_ARG_64 (void *)-1
#ifndef va_count
#define  va_count(narg) if (first != MdsEND_ARG && first != MdsEND_ARG_64) \
{ struct descriptor *arg;\
  va_start(incrmtr, first); \
  arg = va_arg(incrmtr, struct descriptor *); \
  for (narg=1; (narg < 256) && \
               (arg != MdsEND_ARG) && \
               (arg != MdsEND_ARG_64); \
               narg++,arg=va_arg(incrmtr, struct descriptor *)); \
} else narg=0

#endif				/* va_count */

#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
extern EXPORT int Tdi##name ( struct descriptor *first, ... ) \
{ \
  int narg; \
  va_list incrmtr; \
  va_count(narg); \
  if (narg && narg < 256) \
  { \
    int i; \
    struct descriptor *args[256]; \
    va_start(incrmtr, first); \
    args[0] = first; \
    for (i=1;i<narg;i++) \
      args[i] = va_arg(incrmtr, struct descriptor *); \
    return TdiIntrinsic(__LINE__ - 25,narg-1,args,args[narg-1]); \
  } \
  else \
    return TdiNO_OUTPTR; \
}
#include <opcbuiltins.h>
