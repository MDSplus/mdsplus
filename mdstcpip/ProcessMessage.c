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
#include <mdsplus/mdsconfig.h>

#if defined(linux) && !defined(_LARGEFILE_SOURCE)
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64
#endif
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <io.h>
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

#include <condition.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <strroutines.h>
#include <tdishr.h>
#include <treeshr.h>

#include "../tdishr/tdithreadstatic.h"
#include "../treeshr/treeshrp.h"
#include "cvtdef.h"
#include "mdsIo.h"
#include "mdsip_connections.h"

extern int TdiRestoreContext();
extern int TdiSaveContext();

#ifdef min
#undef min
#endif
#define min(a, b) (((a) < (b)) ? (a) : (b))
#ifdef max
#undef max
#endif
#define max(a, b) (((a) > (b)) ? (a) : (b))

static void ConvertBinary(int num, int sign_extend, short in_length,
                          char *in_ptr, short out_length, char *out_ptr) {
  int i;
  int j;
  signed char *in_p = (signed char *)in_ptr;
  signed char *out_p = (signed char *)out_ptr;
  short min_len = min(out_length, in_length);
  for (i = 0; i < num; i++, in_p += in_length, out_p += out_length) {
    for (j = 0; j < min_len; j++)
      out_p[j] = in_p[j];
    for (; j < out_length; j++)
      out_p[j] = sign_extend ? (in_p[min_len - 1] < 0 ? -1 : 0) : 0;
  }
}

static void ConvertFloat(int num, int in_type, char in_length, char *in_ptr,
                         int out_type, char out_length, char *out_ptr) {
  int i;
  char *in_p;
  char *out_p;
  for (i = 0, in_p = in_ptr, out_p = out_ptr; i < num;
       i++, in_p += in_length, out_p += out_length) {
    char *ptr = in_p;
    char cray_f[8];
    if (in_type == CvtCRAY) {
      int j, k;
      for (j = 0; j < 2; j++)
        for (k = 0; k < 4; k++)
#ifdef WORDS_BIGENDIAN
          cray_f[j * 4 + k] = ptr[j * 4 + 3 - k];
#else
          cray_f[(1 - j) * 4 + k] = ptr[j * 4 + 3 - k];
#endif
      ptr = cray_f;
    }
    CvtConvertFloat(ptr, in_type, out_p, out_type, 0);
#ifdef WORDS_BIGENDIAN
    if (out_type == CvtCRAY) {
      int j, k;
      ptr = out_p;
      for (j = 0; j < 2; j++)
        for (k = 0; k < 4; k++)
          cray_f[j * 4 + k] = ptr[j * 4 + 3 - k];
      for (j = 0; j < 8; j++)
        ptr[j] = cray_f[j];
    }
#endif
  }
}

///
///
/// \param client_type
/// \param message_id
/// \param status
/// \param d
/// \return
///

static Message *BuildResponse(int client_type, unsigned char message_id,
                              int status, struct descriptor *d) {
  Message *m = NULL;
  /*
  if (SupportsCompression(client_type)) {
    INIT_AND_FREEXD_ON_EXIT(out);
    if IS_OK(MdsSerializeDscOut(d, &out)) {
      struct descriptor_a* array = (struct descriptor_a*)out.pointer;
      m = malloc(sizeof(MsgHdr) + array->arsize);
      memset(&m->h,0,sizeof(MsgHdr));
      m->h.msglen = sizeof(MsgHdr) + array->arsize;
      m->h.client_type= client_type;
      m->h.message_id = message_id;
      m->h.status     = status;
      m->h.dtype      = DTYPE_SERIAL;
      m->h.length     = 1;
      memcpy(m->bytes, array->pointer, array->arsize);
    }
    FREEXD_NOW(out);
    return m;
  }
  */
  int nbytes = (d->class == CLASS_S) ? d->length : ((array_coeff *)d)->arsize;
  int num = nbytes / ((d->length < 1) ? 1 : d->length);
  short length = d->length;
  if (CType(client_type) == CRAY_CLIENT) {
    switch (d->dtype) {
    case DTYPE_USHORT:
    case DTYPE_ULONG:
    case DTYPE_SHORT:
    case DTYPE_LONG:
    case DTYPE_F:
    case DTYPE_FS:
      length = 8;
      break;
    case DTYPE_FC:
    case DTYPE_FSC:
    case DTYPE_D:
    case DTYPE_G:
    case DTYPE_FT:
      length = 16;
      break;
    default:
      length = d->length;
      break;
    }
    nbytes = num * length;
  } else if (CType(client_type) == CRAY_IEEE_CLIENT) {
    switch (d->dtype) {
    case DTYPE_USHORT:
    case DTYPE_SHORT:
      length = 4;
      break;
    case DTYPE_ULONG:
    case DTYPE_LONG:
      length = 8;
      break;
    default:
      length = d->length;
      break;
    }
    nbytes = num * length;
  }
  m = malloc(sizeof(MsgHdr) + nbytes);
  memset(&m->h, 0, sizeof(MsgHdr));
  m->h.msglen = sizeof(MsgHdr) + nbytes;
  m->h.client_type = client_type;
  m->h.message_id = message_id;
  m->h.status = status;
  m->h.dtype = d->dtype;
  m->h.length = length;
  if (d->class == CLASS_S)
    m->h.ndims = 0;
  else {
    int i;
    array_coeff *a = (array_coeff *)d;
    m->h.ndims = a->dimct;
    if (a->aflags.coeff)
      for (i = 0; i < m->h.ndims && i < MAX_DIMS; i++)
        m->h.dims[i] = a->m[i];
    else
      m->h.dims[0] = a->length ? a->arsize / a->length : 0;
    for (i = m->h.ndims; i < MAX_DIMS; i++)
      m->h.dims[i] = 0;
  }
  switch (CType(client_type)) {
  case IEEE_CLIENT:
  case JAVA_CLIENT:
    switch (d->dtype) {
    case DTYPE_F:
      ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, CvtIEEE_S,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FC:
      ConvertFloat(num * 2, CvtVAX_F, (char)(d->length / 2), d->pointer,
                   CvtIEEE_S, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_FS:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FSC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_D:
      ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, CvtIEEE_T,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      ConvertFloat(num * 2, CvtVAX_D, (char)(d->length / 2), d->pointer,
                   CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, CvtIEEE_T,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      ConvertFloat(num * 2, CvtVAX_G, (char)(d->length / 2), d->pointer,
                   CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_FT:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FTC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    default:
      memcpy(m->bytes, d->pointer, nbytes);
      break;
    }
    break;
  case CRAY_CLIENT:
    switch (d->dtype) {
    case DTYPE_USHORT:
    case DTYPE_ULONG:
      ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes);
      break;
    case DTYPE_SHORT:
    case DTYPE_LONG:
      ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length,
                    m->bytes);
      break;
    case DTYPE_F:
      ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, CvtCRAY,
                   (char)m->h.length, m->bytes);
      break;
    case DTYPE_FS:
      ConvertFloat(num, CvtIEEE_S, (char)d->length, d->pointer, CvtCRAY,
                   (char)m->h.length, m->bytes);
      break;
    case DTYPE_FC:
      ConvertFloat(num * 2, CvtVAX_F, (char)(d->length / 2), d->pointer,
                   CvtCRAY, (char)(m->h.length / 2), m->bytes);
      break;
    case DTYPE_FSC:
      ConvertFloat(num * 2, CvtIEEE_S, (char)(d->length / 2), d->pointer,
                   CvtCRAY, (char)(m->h.length / 2), m->bytes);
      break;
    case DTYPE_D:
      ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, CvtCRAY,
                   (char)m->h.length, m->bytes);
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, CvtCRAY,
                   (char)m->h.length, m->bytes);
      break;
    case DTYPE_FT:
      ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, CvtCRAY,
                   (char)m->h.length, m->bytes);
      break;
    default:
      memcpy(m->bytes, d->pointer, nbytes);
      break;
    }
    break;
  case CRAY_IEEE_CLIENT:
    switch (d->dtype) {
    case DTYPE_USHORT:
    case DTYPE_ULONG:
      ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes);
      break;
    case DTYPE_SHORT:
    case DTYPE_LONG:
      ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length,
                    m->bytes);
      break;
    case DTYPE_F:
      ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, CvtIEEE_S,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FC:
      ConvertFloat(num * 2, CvtVAX_F, (char)(d->length / 2), d->pointer,
                   CvtIEEE_S, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_FS:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FSC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_D:
      ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, CvtIEEE_T,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      ConvertFloat(num * 2, CvtVAX_D, (char)(d->length / 2), d->pointer,
                   CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, CvtIEEE_T,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      ConvertFloat(num * 2, CvtVAX_G, (char)(d->length / 2), d->pointer,
                   CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_FT:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FTC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    default:
      memcpy(m->bytes, d->pointer, nbytes);
      break;
    }
    break;
  case VMSG_CLIENT:
    switch (d->dtype) {
    case DTYPE_F:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_D:
      ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, CvtVAX_G,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      ConvertFloat(num * 2, CvtVAX_D, (char)(d->length / 2), d->pointer,
                   CvtVAX_G, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_G:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_FS:
      ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, CvtVAX_F,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FSC:
      ConvertFloat(num * 2, CvtIEEE_S, sizeof(float), d->pointer, CvtVAX_F,
                   (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_FT:
      ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, CvtVAX_G,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FTC:
      ConvertFloat(num * 2, CvtIEEE_T, sizeof(double), d->pointer, CvtVAX_G,
                   (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    default:
      memcpy(m->bytes, d->pointer, nbytes);
      break;
    }
    break;
  default:
    switch (d->dtype) {
    case DTYPE_F:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_D:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      memcpy(m->bytes, d->pointer, nbytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, CvtVAX_D,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      ConvertFloat(num * 2, CvtVAX_G, sizeof(double), d->pointer, CvtVAX_D,
                   (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_FS:
      ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, CvtVAX_F,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FSC:
      ConvertFloat(num * 2, CvtIEEE_S, sizeof(float), d->pointer, CvtVAX_F,
                   (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_FT:
      ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, CvtVAX_D,
                   (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FTC:
      ConvertFloat(num * 2, CvtIEEE_T, sizeof(double), d->pointer, CvtVAX_D,
                   (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    default:
      memcpy(m->bytes, d->pointer, nbytes);
      break;
    }
    break;
  }
  return m;
}

static void GetErrorText(int status, mdsdsc_xd_t *xd) {
  static DESCRIPTOR(unknown, "unknown error occured");
  struct descriptor message = {0, DTYPE_T, CLASS_S, 0};
  if ((message.pointer = MdsGetMsg(status)) != NULL) {
    message.length = strlen(message.pointer);
    MdsCopyDxXd(&message, xd);
  } else
    MdsCopyDxXd((struct descriptor *)&unknown, xd);
}

static void ClientEventAst(MdsEventList *e, int data_len, char *data) {
  int conid = e->conid;
  Connection *c = FindConnection(e->conid, 0);
  int i;
  char client_type;
  Message *m;
  JMdsEventInfo *info;
  int len;
  // Check Connection: if down, cancel the event and return
  if (!c) {
    MDSEventCan(e->eventid);
    return;
  }
  client_type = c->client_type;
  LockAsts();
  if (CType(client_type) == JAVA_CLIENT) {
    len = sizeof(MsgHdr) + sizeof(JMdsEventInfo);
    m = memset(malloc(len), 0, len);
    m->h.ndims = 0;
    m->h.client_type = client_type;
    m->h.msglen = len;
    m->h.dtype = DTYPE_EVENT_NOTIFY;
    info = (JMdsEventInfo *)m->bytes;
    if (data_len > 0)
      memcpy(info->data, data, (data_len < 12) ? data_len : 12);
    for (i = data_len; i < 12; i++)
      info->data[i] = 0;
    info->eventid = e->jeventid;
  } else {
    m = memset(malloc(sizeof(MsgHdr) + e->info_len), 0,
               sizeof(MsgHdr) + e->info_len);
    m->h.ndims = 0;
    m->h.client_type = client_type;
    m->h.msglen = sizeof(MsgHdr) + e->info_len;
    m->h.dtype = DTYPE_EVENT_NOTIFY;
    if (data_len > 0)
      memcpy(e->info->data, data, (data_len < 12) ? data_len : 12);
    for (i = data_len; i < 12; i++)
      e->info->data[i] = 0;
    memcpy(m->bytes, e->info, e->info_len);
  }
  SendMdsMsg(conid, m, MSG_DONTWAIT);
  free(m);
  UnlockAsts();
}

/* The following implementation of a cancelable worker is used in:
 * tdishr/TdiDoTask.c
 * mdstcpip/ProcessMessage.c
 * Problems with the implementation are likely to be fixed in all locations.
 */
typedef struct {
  void **ctx;
  int status;
#ifndef _WIN32
  Condition *const condition;
#endif
  MDSplusThreadStatic_t *mts;
  Connection *const connection;
  mdsdsc_xd_t *const xd_out;
} worker_args_t;

typedef struct {
  worker_args_t *const wa;
  mdsdsc_xd_t *const xdp;
} worker_cleanup_t;

static void WorkerCleanup(worker_cleanup_t *const wc) {
#ifndef _WIN32
  CONDITION_SET(wc->wa->condition);
#endif
  free_xd(wc->xdp);
}

static int WorkerThread(void *args) {
  EMPTYXD(xd);
  worker_cleanup_t wc = {(worker_args_t *)args, &xd};
  pthread_cleanup_push((void *)WorkerCleanup, (void *)&wc);
  TDITHREADSTATIC(MDSplusThreadStatic(wc.wa->mts));
  --TDI_INTRINSIC_REC;
  wc.wa->status = TdiIntrinsic(OPC_EXECUTE, wc.wa->connection->nargs,
                               wc.wa->connection->descrip, &xd);
  ++TDI_INTRINSIC_REC;
  if
    IS_OK(wc.wa->status)
  wc.wa->status = TdiData(xd.pointer, wc.wa->xd_out MDS_END_ARG);
  pthread_cleanup_pop(1);
  return wc.wa->status;
}

static inline int executeCommand(Connection *connection, mdsdsc_xd_t *ans_xd) {
  // fprintf(stderr,"starting task for connection %d\n",connection->id);
  void *tdicontext[6], *pc = NULL;
  MDSplusThreadStatic_t *mts = MDSplusThreadStatic(NULL);
#ifdef _WIN32
  worker_args_t wa = {
#else
  Condition WorkerRunning = CONDITION_INITIALIZER;
  worker_args_t wa = {.condition = &WorkerRunning,
#endif
    .mts = mts,
    .status = -1,
    .connection = connection,
    .xd_out = ans_xd
  };
  if (GetContextSwitching()) {
    pc = TreeCtxPush(&connection->DBID);
    TdiSaveContext(tdicontext);
    TdiRestoreContext(connection->tdicontext);
  }
#ifdef _WIN32
  HANDLE hWorker = CreateThread(NULL, DEFAULT_STACKSIZE * 16,
                                (void *)WorkerThread, &wa, 0, NULL);
  if (!hWorker) {
    errno = GetLastError();
    perror("ERROR CreateThread");
    wa.status = MDSplusFATAL;
    goto end;
  }
  int canceled = B_FALSE;
  if (connection->io->check)
    while (WaitForSingleObject(hWorker, 100) == WAIT_TIMEOUT) {
      if (!connection->io->check(connection))
        continue;
      fflush(stdout);
      fprintf(stderr, "Client disconnected, terminating Worker\n");
      TerminateThread(hWorker, 2);
      canceled = B_TRUE;
      break;
    }
  WaitForSingleObject(hWorker, INFINITE);
  if (canceled)
    wa.status = TdiABORT;
#else
  pthread_t Worker;
  _CONDITION_LOCK(wa.condition);
  CREATE_THREAD(Worker, *16, WorkerThread, &wa);
  if (c_status) {
    perror("ERROR pthread_create");
    _CONDITION_UNLOCK(wa.condition);
    pthread_cond_destroy(&WorkerRunning.cond);
    pthread_mutex_destroy(&WorkerRunning.mutex);
    wa.status = MDSplusFATAL;
    goto end;
  }
  int canceled = B_FALSE;
  for (;;) {
    _CONDITION_WAIT_1SEC(wa.condition, );
    if (WorkerRunning.value)
      break;
    if (canceled)
      continue; // skip check if already canceled
    if (!connection->io->check)
      continue; // if no io->check def
    if (!connection->io->check(connection))
      continue;
    fflush(stdout);
    fprintf(stderr, "Client disconnected, canceling Worker ..");
    pthread_cancel(Worker);
    canceled = B_TRUE;
    _CONDITION_WAIT_1SEC(wa.condition, );
    fflush(stdout);
    if (WorkerRunning.value)
      fprintf(stderr, " ok\n");
    else {
      fprintf(stderr, " failed - sending SIGCHLD\n");
      pthread_kill(Worker, SIGCHLD);
    }
    break;
  }
  _CONDITION_UNLOCK(wa.condition);
  void *result;
  pthread_join(Worker, &result);
  pthread_cond_destroy(&WorkerRunning.cond);
  pthread_mutex_destroy(&WorkerRunning.mutex);
  if (canceled && result == PTHREAD_CANCELED)
    wa.status = TdiABORT;
#endif
end:;
  if (pc) {
    TdiSaveContext(connection->tdicontext);
    TdiRestoreContext(tdicontext);
    TreeCtxPop(pc);
  }
  return wa.status;
}

///
/// Executes TDI expression held by a connecion instance. This first searches if
/// connection message corresponds to AST or CAN requests, if no asyncronous ops
/// are requested the TDI actual expression is parsed through tdishr library.
/// In this case the current TDI context and tree is switched to the connection
/// ones stored in the connection context field.
///
/// ### AST and CAN
/// AST and CAN stands for "Asynchronous System Trap" and "CANcel event
/// request". This is an asyncronous message mechanism taken from the OpenVMS
/// system. The event handler is passed inside the message arguments and
/// executed by the MDSEventAst() function from mdslib.
///
/// \param connection the Connection instance filled with proper descriptor
/// arguments \return the execute message answer built using BuildAnswer()
///
static Message *ExecuteMessage(Connection *connection) {
  Message *ans = 0; // return message instance //
  int status = 1;   // return status           //

  char *evname;
  static DESCRIPTOR(eventastreq, EVENTASTREQUEST); // AST request descriptor //
  static DESCRIPTOR(eventcanreq, EVENTCANREQUEST); // Can request descriptor //

  int client_type = connection->client_type;
  int java = CType(client_type) == JAVA_CLIENT;
  int message_id = connection->message_id;

  // AST REQUEST //
  if (StrCompare(connection->descrip[0], (struct descriptor *)&eventastreq) ==
      0) {
    static int eventid = -1;
    static DESCRIPTOR_LONG(eventiddsc, &eventid);
    MdsEventList *newe = (MdsEventList *)malloc(sizeof(MdsEventList));
    struct descriptor_a *info = (struct descriptor_a *)connection->descrip[2];
    newe->conid = connection->id;

    evname = malloc(connection->descrip[1]->length + 1);
    memcpy(evname, connection->descrip[1]->pointer,
           connection->descrip[1]->length);
    evname[connection->descrip[1]->length] = 0;

    // Manage AST Event //
    status = MDSEventAst(evname, (void (*)(void *, int, char *))ClientEventAst,
                         newe, &newe->eventid);

    free(evname);
    if (java) {
      newe->info = 0;
      newe->info_len = 0;
      newe->jeventid = *connection->descrip[2]->pointer;
    } else {
      newe->info = (MdsEventInfo *)memcpy(malloc(info->arsize), info->pointer,
                                          info->arsize);
      newe->info_len = info->arsize;
      newe->info->eventid = newe->eventid;
    }
    newe->next = 0;
    if (!(status & 1)) {
      eventiddsc.pointer = (void *)&eventid;
      free(newe->info);
      free(newe);
    } else {
      MdsEventList *e;
      eventiddsc.pointer = (void *)&newe->eventid;
      if (connection->event) {
        for (e = connection->event; e->next; e = e->next)
          ;
        e->next = newe;
      } else
        connection->event = newe;
    }
    if (!java)
      ans = BuildResponse(client_type, message_id, status, &eventiddsc);
  }
  // CAN REQUEST //
  else if (StrCompare(connection->descrip[0],
                      (struct descriptor *)&eventcanreq) == 0) {
    static int eventid;
    static DESCRIPTOR_LONG(eventiddsc, &eventid);
    MdsEventList *e;
    MdsEventList **p;
    if (!java)
      eventid = *(int *)connection->descrip[1]->pointer;
    else
      eventid = (int)*connection->descrip[1]->pointer;
    if (connection->event) {
      for (p = &connection->event, e = connection->event;
           e && (e->eventid != eventid); p = &e->next, e = e->next)
        ;
      if (e) {
        /**/ MDSEventCan(e->eventid);
        /**/ *p = e->next;
        free(e);
      }
    }
    if (!java)
      ans = BuildResponse(client_type, message_id, status, &eventiddsc);
  }
  // NORMAL TDI COMMAND //
  else {
    INIT_AND_FREEXD_ON_EXIT(ans_xd);
    status = executeCommand(connection, &ans_xd);
    if
      STATUS_NOT_OK
    GetErrorText(status, &ans_xd);
    if (GetCompressionLevel() != connection->compression_level) {
      connection->compression_level = GetCompressionLevel();
      if (connection->compression_level > GetMaxCompressionLevel())
        connection->compression_level = GetMaxCompressionLevel();
      SetCompressionLevel(connection->compression_level);
    }
    ans = BuildResponse(connection->client_type, connection->message_id, status,
                        ans_xd.pointer);
    FREEXD_NOW(ans_xd);
  }
  FreeDescriptors(connection);
  return ans;
}

static inline char *replaceBackslashes(char *filename) {
  char *ptr;
  while ((ptr = strchr(filename, '\\')) != NULL)
    *ptr = '/';
  return filename;
}

///
/// Handle message from server listen routine. A new descriptor instance is
/// created with the message buffer size and the message memory is copyed
/// inside. A proper conversion of memory structure is applied if neede for the
/// type of client connected.
///
/// \param connection the connection instance to handle
/// \param message the message to process
/// \return message answer
///
Message *ProcessMessage(Connection *connection, Message *message) {

  Message *ans = 0;

  // COMING NEW MESSAGE  //
  // reset connection id //
  if (connection->message_id != message->h.message_id) {
    FreeDescriptors(connection);
    if (message->h.nargs < MDSIP_MAX_ARGS - 1) {
      connection->message_id = message->h.message_id;
      connection->nargs = message->h.nargs;
    } else {
      DESCRIPTOR_LONG(status_d, 0);
      int status = 0;
      status_d.pointer = (char *)&status;
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          (struct descriptor *)&status_d);
      return ans;
    }
  }

  // STANADARD COMMANDS ////////////////////////////////////////////////////////
  // idx < nargs        ////////////////////////////////////////////////////////
  if (message->h.descriptor_idx < connection->nargs) {

    // set connection to the message client_type  //
    connection->client_type = message->h.client_type;

#define COPY_DESC(name, GENERATOR, ...)                                        \
  do {                                                                         \
    const GENERATOR(__VA_ARGS__);                                              \
    *(void **)&d = memcpy(malloc(sizeof(tmp)), &tmp, sizeof(tmp));             \
  } while (0)

    // d -> reference to curent idx argument desctriptor  //
    struct descriptor *d = connection->descrip[message->h.descriptor_idx];
    if (message->h.dtype == DTYPE_SERIAL) {
      if (d && d->class != CLASS_XD) {
        if (d->class == CLASS_D && d->pointer)
          free(d->pointer);
        free(d);
      }
      COPY_DESC(d, EMPTYXD, tmp);
      connection->descrip[message->h.descriptor_idx] = d;
      return ans;
    }
    if (!d) {
      // instance the connection descriptor field //
      const short lengths[] = {0, 0, 1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 8, 16, 0};
      if (message->h.ndims == 0) {
        d = calloc(1, sizeof(struct descriptor_s));
        d->class = CLASS_S;
      } else
        COPY_DESC(d, DESCRIPTOR_A_COEFF, tmp, 0, 0, 0, MAX_DIMS, 0);
      d->length = message->h.dtype < DTYPE_CSTRING ? lengths[message->h.dtype]
                                                   : message->h.length;
      d->dtype = message->h.dtype;
      if (d->class == CLASS_A) {
        array_coeff *a = (array_coeff *)d;
        int num = 1;
        int i;
        a->dimct = message->h.ndims;
        for (i = 0; i < a->dimct; i++) {
          a->m[i] = message->h.dims[i];
          num *= a->m[i];
        }
        a->arsize = a->length * num;
        a->pointer = a->a0 = malloc(a->arsize);
      } else
        d->pointer = d->length ? malloc(d->length) : 0;
      // set new instance //
      connection->descrip[message->h.descriptor_idx] = d;
    }
    if (d) {
      // have valid connection descriptor instance     //
      // copy the message buffer into the descriptor   //

      int dbytes = d->class == CLASS_S ? (int)d->length
                                       : (int)((array_coeff *)d)->arsize;
      int num = dbytes / max(1, d->length);

      switch (CType(connection->client_type)) {
      case IEEE_CLIENT:
      case JAVA_CLIENT:
        memcpy(d->pointer, message->bytes, dbytes);
        break;
      case CRAY_IEEE_CLIENT:
        switch (d->dtype) {
        case DTYPE_USHORT:
        case DTYPE_ULONG:
          ConvertBinary(num, 0, message->h.length, message->bytes, d->length,
                        d->pointer);
          break;
        case DTYPE_SHORT:
        case DTYPE_LONG:
          ConvertBinary(num, 1, message->h.length, message->bytes, d->length,
                        d->pointer);
          break;
        default:
          memcpy(d->pointer, message->bytes, dbytes);
          break;
        }
        break;
      case CRAY_CLIENT:
        switch (d->dtype) {
        case DTYPE_USHORT:
        case DTYPE_ULONG:
          ConvertBinary(num, 0, message->h.length, message->bytes, d->length,
                        d->pointer);
          break;
        case DTYPE_SHORT:
        case DTYPE_LONG:
          ConvertBinary(num, 1, message->h.length, message->bytes, d->length,
                        d->pointer);
          break;
        case DTYPE_FLOAT:
          ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes,
                       CvtIEEE_S, (char)d->length, d->pointer);
          break;
        case DTYPE_COMPLEX:
          ConvertFloat(num * 2, CvtCRAY, (char)(message->h.length / 2),
                       message->bytes, CvtIEEE_S, (char)(d->length / 2),
                       d->pointer);
          break;
        case DTYPE_DOUBLE:
          ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes,
                       CvtIEEE_T, sizeof(double), d->pointer);
          break;
        default:
          memcpy(d->pointer, message->bytes, dbytes);
          break;
        }
        break;
      default:
        switch (d->dtype) {
        case DTYPE_FLOAT:
          ConvertFloat(num, CvtVAX_F, (char)message->h.length, message->bytes,
                       CvtIEEE_S, sizeof(float), d->pointer);
          break;
        case DTYPE_COMPLEX:
          ConvertFloat(num * 2, CvtVAX_F, (char)message->h.length,
                       message->bytes, CvtIEEE_S, sizeof(float), d->pointer);
          break;
        case DTYPE_DOUBLE:
          if (CType(connection->client_type) == VMSG_CLIENT)
            ConvertFloat(num, CvtVAX_G, (char)message->h.length, message->bytes,
                         CvtIEEE_T, sizeof(double), d->pointer);
          else
            ConvertFloat(num, CvtVAX_D, (char)message->h.length, message->bytes,
                         CvtIEEE_T, sizeof(double), d->pointer);
          break;

        case DTYPE_COMPLEX_DOUBLE:
          if (CType(connection->client_type) == VMSG_CLIENT)
            ConvertFloat(num * 2, CvtVAX_G, (char)(message->h.length / 2),
                         message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
          else
            ConvertFloat(num * 2, CvtVAX_D, (char)(message->h.length / 2),
                         message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
          break;
        default:
          memcpy(d->pointer, message->bytes, dbytes);
          break;
        }
      }
      switch (d->dtype) {
      default:
        break;
      case DTYPE_FLOAT:
        d->dtype = DTYPE_FS;
        break;
      case DTYPE_COMPLEX:
        d->dtype = DTYPE_FSC;
        break;
      case DTYPE_DOUBLE:
        d->dtype = DTYPE_FT;
        break;
      case DTYPE_COMPLEX_DOUBLE:
        d->dtype = DTYPE_FTC;
        break;
      }

      // CALL EXECUTE MESSAGE //
      if (message->h.descriptor_idx == (message->h.nargs - 1)) {
        ans = ExecuteMessage(connection);
      }
    }
  }

  // SPECIAL I/O MESSAGES //////////////////////////////////////////////////////
  // idx >= nargs         //////////////////////////////////////////////////////
  else {

    connection->client_type = message->h.client_type;
    const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
    switch (message->h.descriptor_idx) {
    case MDS_IO_OPEN_K: {
      char *filename = (char *)message->bytes;
      int options = mdsio->open.options;
      mode_t mode = mdsio->open.mode;
      int fopts = 0;
      if (options & MDS_IO_O_CREAT)
        fopts |= O_CREAT;
      if (options & MDS_IO_O_TRUNC)
        fopts |= O_TRUNC;
      if (options & MDS_IO_O_EXCL)
        fopts |= O_EXCL;
      if (options & MDS_IO_O_WRONLY)
        fopts |= O_WRONLY;
      if (options & MDS_IO_O_RDONLY)
        fopts |= O_RDONLY;
      if (options & MDS_IO_O_RDWR)
        fopts |= O_RDWR;
      int fd = MDS_IO_OPEN(filename, fopts, mode);
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&fd};
      ans = BuildResponse(connection->client_type, connection->message_id, 3,
                          &ans_d);
      break;
    }
    case MDS_IO_CLOSE_K: {
      int fd = mdsio->close.fd;
      int ans_o = MDS_IO_CLOSE(fd);
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          &ans_d);
      break;
    }
    case MDS_IO_LSEEK_K: {
      int fd = mdsio->lseek.fd;
      int64_t offset = mdsio->lseek.offset;
      SWAP_INT_IF_BIGENDIAN(&offset);
      int whence = mdsio->lseek.whence;
      int64_t ans_o = MDS_IO_LSEEK(fd, offset, whence);
      struct descriptor ans_d = {8, DTYPE_Q, CLASS_S, 0};
      ans_d.pointer = (char *)&ans_o;
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          (struct descriptor *)&ans_d);
      SWAP_INT_IF_BIGENDIAN(ans_d.pointer);
      break;
    }
    case MDS_IO_READ_K: {
      int fd = mdsio->read.fd;
      size_t count = mdsio->read.count;
      void *buf = malloc(count);
      ssize_t nbytes = MDS_IO_READ(fd, buf, count);
#ifdef USE_PERF
      TreePerfRead(nbytes);
#endif
      if (nbytes > 0) {
        DESCRIPTOR_A(ans_d, 1, DTYPE_B, buf, nbytes);
        if ((size_t)nbytes != count)
          perror("READ_K wrong byte count");
        ans = BuildResponse(connection->client_type, connection->message_id, 1,
                            (struct descriptor *)&ans_d);
      } else {
        DESCRIPTOR(ans_d, "");
        ans = BuildResponse(connection->client_type, connection->message_id, 1,
                            (struct descriptor *)&ans_d);
      }
      free(buf);
      break;
    }
    case MDS_IO_WRITE_K: {
      /* from http://man7.org/linux/man-pages/man2/write.2.html
       * On Linux, write() (and similar system calls) will transfer at most
       * 0x7ffff000 (2,147,479,552) bytes, returning the number of bytes
       * actually transferred.  (This is true on both 32-bit and 64-bit
       * systems.) => ergo uint32 will suffice
       */
      int fd = mdsio->write.fd;
      size_t count = mdsio->write.count;
      uint32_t ans_o =
          (uint32_t)(MDS_IO_WRITE(fd, message->bytes, count) & 0xFFFFFFFFL);
      struct descriptor ans_d = {sizeof(ans_o), DTYPE_LU, CLASS_S,
                                 (char *)&ans_o};
      SWAP_INT_IF_BIGENDIAN(ans_d.pointer);
      if (ans_o != mdsio->write.count)
        perror("WRITE_K wrong byte count");
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          &ans_d);
      break;
    }
    case MDS_IO_LOCK_K: {
      int fd = mdsio->lock.fd;
      off_t offset = mdsio->lock.offset;
      SWAP_INT_IF_BIGENDIAN(&offset);
      int size = mdsio->lock.size;
      int mode_in = mdsio->lock.mode;
      int mode = mode_in & 0x3;
      int nowait = mode_in & 0x8;
      int deleted;
      int ans_o = MDS_IO_LOCK(fd, offset, size, mode | nowait, &deleted);
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
      ans = BuildResponse(connection->client_type, connection->message_id,
                          deleted ? 3 : 1, &ans_d);
      break;
    }
    case MDS_IO_EXISTS_K: {
      char *filename = message->bytes;
      int ans_o = MDS_IO_EXISTS(filename);
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          &ans_d);
      break;
    }
    case MDS_IO_REMOVE_K: {
      char *filename = message->bytes;
      int ans_o = MDS_IO_REMOVE(filename);
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          &ans_d);
      break;
    }
    case MDS_IO_RENAME_K: {
      char *old = message->bytes;
      char *new = message->bytes + strlen(old) + 1;
      int ans_o = MDS_IO_RENAME(old, new);
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          &ans_d);
      break;
    }
    case MDS_IO_READ_X_K: {
      int fd = mdsio->read_x.fd;
      off_t offset = mdsio->read_x.offset;
      SWAP_INT_IF_BIGENDIAN(&offset);
      size_t count = mdsio->read_x.count;
      void *buf = malloc(count);
      int deleted;
      size_t nbytes = MDS_IO_READ_X(fd, offset, buf, count, &deleted);
      if (nbytes > 0) {
        DESCRIPTOR_A(ans_d, 1, DTYPE_B, buf, nbytes);
        if ((size_t)nbytes != count)
          perror("READ_X_K wrong byte count");
        ans = BuildResponse(connection->client_type, connection->message_id,
                            deleted ? 3 : 1, (struct descriptor *)&ans_d);
      } else {
        DESCRIPTOR(ans_d, "");
        ans = BuildResponse(connection->client_type, connection->message_id,
                            deleted ? 3 : 1, (struct descriptor *)&ans_d);
      }
      free(buf);
      break;
    }
    case MDS_IO_OPEN_ONE_K: {
      char *treename = message->bytes;
      char *filepath = message->bytes + strlen(treename) + 1;
      int shot = mdsio->open_one.shot;
      tree_type_t type = (tree_type_t)mdsio->open_one.type;
      int new = mdsio->open_one.new;
      int edit_flag = mdsio->open_one.edit;
      int fd;
      char *fullpath = NULL;
      int status = MDS_IO_OPEN_ONE(filepath, treename, shot, type, new,
                                   edit_flag, &fullpath, NULL, &fd);
      int msglen = fullpath ? strlen(fullpath) + 9 : 8;
      char *msg = malloc(msglen);
      DESCRIPTOR_A(ans_d, sizeof(char), DTYPE_B, msg, msglen);
      memcpy(msg, &status, 4);
      memcpy(msg + 4, &fd, 4);
      if (fullpath) {
        memcpy(msg + 8, fullpath, msglen - 8);
        free(fullpath);
      }
      ans = BuildResponse(connection->client_type, connection->message_id, 3,
                          (struct descriptor *)&ans_d);
      free(msg);
      break;
    }
    default: {
      int ans_o = 0;
      struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
      ans = BuildResponse(connection->client_type, connection->message_id, 1,
                          &ans_d);
      break;
    }
    }
  }
  return ans;
}
