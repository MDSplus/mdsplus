#ifndef MdsLib_H
#define MdsLib_H

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

#ifndef _WIN32
#define INVALID_SOCKET -1
#endif

#include <mdsdescrip.h>
#include <stdio.h>            
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>


#define NDESCRIP_CACHE 256

static struct descriptor *descrs[NDESCRIP_CACHE];

int descr (int *dtype, void *data, int *dim1, ...);


SOCKET MdsConnect(char *host);
int MdsValue(char *expression, ...); /**** NOTE: NULL terminated argument list expected ****/

SOCKET mdsSocket=INVALID_SOCKET;

#endif
