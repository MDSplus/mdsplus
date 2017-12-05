#ifndef MdsLib_H
#define MdsLib_H

#include <ipdesc.h>

#include <stdio.h>
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MDSLIB_NO_PROTOS
  SOCKET MdsConnect(char *host);
  int descr(int *dtype, void *data, int *dim1, ...);
  int MdsOpen(char *tree, int *shot);
  int MdsClose(char *tree, int *shot);
  int MdsSetDefault(char *node);
  int MdsValue(char *expression, ...);
  int MdsPut(char *node, char *expression, ...);
  SOCKET MdsSetSocket(int * socket);
  void MdsDisconnect();
#endif
#ifdef __cplusplus
}				// extern "C"
#endif
extern int mdsSocket;

#endif
