#ifndef MdsLib_H
#define MdsLib_H

#include <mdsdescrip.h>
#ifdef __VMS 
#include "ipdesc.h"
#else
#include <ipdesc.h>
#endif

#ifndef _WIN32
#define INVALID_SOCKET -1
#endif

#include <stdio.h>            
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>


#define NDESCRIP_CACHE 256

static struct descriptor *descrs[NDESCRIP_CACHE];

int descr (int *dtype, void *data, int *dim1, ...);


#ifdef __VMS
SOCKET MdsConnect(struct dsc$descriptor *host);
int MdsValue(struct dsc$descriptor *expression, ...); /**** NOTE: NULL terminated argument list expected ****/
#else
SOCKET MdsConnect(char *host);
int MdsValue(char *expression, ...); /**** NOTE: NULL terminated argument list expected ****/
#endif

extern SOCKET mdsSocket;

#endif
