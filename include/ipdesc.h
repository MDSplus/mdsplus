/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef IPDESC_H
#define IPDESC_H

#ifdef _WIN32
#ifndef _WS2DEF_
#include "windows.h"
#endif
#else
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif

#define MAX_DIMS 7
#define DTYPE_UCHAR   2
#define DTYPE_USHORT  3
#define DTYPE_ULONG   4
#define DTYPE_ULONGLONG 5
#define DTYPE_CHAR    6
#define DTYPE_SHORT   7
#define DTYPE_LONG    8
#define DTYPE_LONGLONG 9
#ifdef DTYPE_FLOAT
#undef DTYPE_FLOAT
#endif
#define DTYPE_FLOAT   10
#ifdef DTYPE_DOUBLE
#undef DTYPE_DOUBLE
#endif
#define DTYPE_DOUBLE  11
#define DTYPE_COMPLEX 12
#define DTYPE_COMPLEX_DOUBLE 13
#define DTYPE_CSTRING 14
#define DTYPE_EVENT_NOTIFY   99
#ifndef DTYPE_EVENT
#define DTYPE_EVENT DTYPE_EVENT_NOTIFY
#endif

struct descrip {
  char dtype;
  char ndims;
  int dims[MAX_DIMS];
  int length;
  void *ptr;
};
#define EndOfArgs ((struct descrip *)0)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MDSIP_H__
  extern int ConnectToMds(char *host);
  extern int SendArg(int s, unsigned char i, char dtype, unsigned char nargs, short len, char ndims,
		     int *dims, char *ptr);
  extern int GetAnswerInfo(int s, char *dtype, short *len, char *ndims, int *dims, int *nbytes,
			   void * *dptr);
  extern int DisconnectFromMds(int conid);
  extern struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims,
				     void *ptr);
  extern struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length,
					       char ndims, int *dims, void *ptr);
  extern int MdsEventAst(int conid, char *eventnam, void (*astadr) (), void *astprm, int *eventid);
  extern int MdsEventCan(int conid, int eventid);
  extern int HostToIp(char *host, int *addr, short *port);
#ifndef MdsLib_H
  extern int MdsValue(int conid, char *, ...);
  extern int MdsPut(int conid, char *node, char *expression, ...);
  extern int MdsOpen(int conid, char *tree, int shot);
  extern int MdsSetDefault(int conid, char *node);
  extern int MdsClose(int conid);
  extern void FreeMessage(void *m);
#endif
#endif

#ifdef __cplusplus
}				// extern "C"
#endif
#endif
