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
  int MdsConnect(char *host);
  int descr(int *dtype, void *data, int *dim1, ...);
  int descr2(int *dtype, int *dim1, ...);
  int MdsOpen(char *tree, int *shot);
  int MdsClose(char *tree, int *shot);
  int MdsSetDefault(char *node);
  int MdsValue(char *expression, ...);
  int MdsPut(char *node, char *expression, ...);
  int MdsValue2(char *expression, ...);
  int MdsPut2(char *node, char *expression, ...);
  int MdsSetSocket(int * socket);
  void MdsDisconnect();
  int MdsConnectR(char *host);
  int MdsOpenR(int *conid, char *tree, int *shot);
  int MdsCloseR(int *conid, char *tree, int *shot);
  int MdsSetDefaultR(int *conid, char *node);
  int MdsValueR(int *conid, char *expression, ...);
  int MdsPutR(int *conid, char *node, char *expression, ...);
  int MdsValue2R(int *conid, char *expression, ...);
  int MdsPut2R(int *conid, char *node, char *expression, ...);
  void MdsDisconnectR(int *conid);
#endif
#ifdef __cplusplus
}				// extern "C"
#endif
extern int mdsSocket;

#endif
