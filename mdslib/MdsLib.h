#ifdef __VMS 
#include "ipdesc.h"
#else
#include <ipdesc.h>
#endif
/*** undefine conflicting data types ***/
#undef DTYPE_FLOAT
#undef DTYPE_DOUBLE
#undef DTYPE_COMPLEX
#undef DTYPE_EVENT

#include <mdsdescrip.h>
#include <stdio.h>            
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>


#define NDESCRIP_CACHE 256

static struct descriptor descrs[NDESCRIP_CACHE];

int descr (int *dtype, void *data, int *dim1, ...);

SOCKET mdsSocket;


void MdsConnect(char *host);
int MdsValue(char *expression, ...); /**** NOTE: NULL terminated argument list expected ****/

