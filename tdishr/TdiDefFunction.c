/*	TDI$$DEF_FUNCTION.C
 	Internal/intrinsic function table definition.
	Yacc tokens are put into TdiRefFunction table here.
	Precedence is associated with a token.

	Ken Klare, LANL CTR-7	(c)1989,1990
*/

#include <mds_stdarg.h>
#include <libroutines.h>
#include <tdimessages.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#define COM 


extern int TdiIntrinsic();

#ifndef va_count
#define  va_count(narg) va_start(incrmtr, first); \
                        for (narg=1; (narg < 256) && (va_arg(incrmtr, struct descriptor *) != MdsEND_ARG); narg++)
#endif /* va_count */

#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
int Tdi##name ( struct descriptor *first, ... ) \
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
#include "opcbuiltins.h"
