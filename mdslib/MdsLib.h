#include <mdsdescrip.h>
#include <stdio.h>            
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ipdesc.h>


#define NDESCRIP_CACHE 256

static struct descriptor descrs[NDESCRIP_CACHE];

int descr (int *dtype, void *data, int *dim1, ...);

SOCKET mdsSocket;


void NewMdsConnect(char *host);
struct descrip *MakeIpDescrip(struct descrip *arg, struct descriptor *dsc);
int NewMdsValue(char *expression, ...); /**** NOTE: NULL terminated argument list expected ****/

