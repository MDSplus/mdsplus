#ifndef MdsLib_H
#define MdsLib_H

#include <ipdesc.h>

#ifndef _WIN32
#define INVALID_SOCKET -1
#endif

#include <stdio.h>            
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

SOCKET MdsConnect(char *host);
int descr (int *dtype, void *data, int *dim1, ...);
int MdsOpen(char *tree, int* shot);
int MdsClose(char *tree, int* shot);
int MdsSetDefault(char *node);
int MdsValue(char *expression, ...);
int MdsPut(char *node, char *expression, ...); 
SOCKET MdsSetSocket( SOCKET *socket );
void MdsDisconnect();

#ifdef __cplusplus
} // extern "C"
#endif

extern SOCKET mdsSocket;

#endif
