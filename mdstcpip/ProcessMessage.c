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
#include "mdsip_connections.h"
#include <mdstypes.h>
#include "cvtdef.h"
#ifdef _WIN32
#include <io.h>
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif
#include <fcntl.h>
#include "mdsIo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <treeshr.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined(linux) && !defined(_LARGEFILE_SOURCE)
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64
#endif
#include "mdsip_connections.h"
#include <treeshr.h>
#include <stdlib.h>
#include <string.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <strroutines.h>
#include "cvtdef.h"

extern int TdiExecute();
extern int TdiRestoreContext();
extern int TreePerfRead();
extern int TreePerfWrite();
extern int TdiDebug();
extern int TdiSaveContext();
extern int TdiData();

#ifdef min
#undef min
#endif
#define min(a,b) (((a) < (b)) ? (a) : (b))
#ifdef max
#undef max
#endif
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define MakeDesc(name) memcpy(malloc(sizeof(name)),&name,sizeof(name))

typedef ARRAY_COEFF(char, 7) ARRAY_7;


static int lock_file(int fd, int64_t offset, int size, int mode_in, int *deleted)
{
  int status;
  int mode = mode_in & MDS_IO_LOCK_MASK;
  int nowait = mode_in & MDS_IO_LOCK_NOWAIT;
#ifdef _WIN32
  OVERLAPPED overlapped;
  int flags;
  *deleted = 0;
  offset = ((offset >= 0)
	    && (nowait == 0)) ? offset : (lseek(fd, 0, SEEK_END));
  overlapped.Offset = (int)(offset & 0xffffffff);
  overlapped.OffsetHigh = (int)(offset >> 32);
  overlapped.hEvent = 0;
  if (mode > 0) {
    HANDLE h = (HANDLE) _get_osfhandle(fd);
    flags = ((mode == MDS_IO_LOCK_RD)
	     && (nowait == 0)) ? 0 : LOCKFILE_EXCLUSIVE_LOCK;
    if (nowait)
      flags |= LOCKFILE_FAIL_IMMEDIATELY;
    status = UnlockFileEx(h, 0, size, 0, &overlapped);
    status = LockFileEx(h, flags, 0, size, 0, &overlapped) == 0 ? TreeFAILURE : TreeNORMAL;
  } else {
    HANDLE h = (HANDLE) _get_osfhandle(fd);
    status = UnlockFileEx(h, 0, size, 0, &overlapped) == 0 ? TreeFAILURE : TreeNORMAL;
  }
#else
  struct stat stat;
  struct flock flock_info;
  flock_info.l_type = (mode == 0) ? F_UNLCK : ((mode == 1) ? F_RDLCK : F_WRLCK);
  flock_info.l_whence = (mode == 0) ? SEEK_SET : ((offset >= 0) ? SEEK_SET : SEEK_END);
  flock_info.l_start = (mode == 0) ? 0 : ((offset >= 0) ? offset : 0);
  flock_info.l_len = (mode == 0) ? 0 : size;
  status =
      (fcntl(fd, nowait ? F_SETLK : F_SETLKW, &flock_info) != -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
  if (!(status & 1))
    perror("Error in lock_file");
  fstat(fd, &stat);
  *deleted = stat.st_nlink <= 0;
#endif
  return status;
}

static void
ConvertBinary(int num, int sign_extend, short in_length, char *in_ptr,
	      short out_length, char *out_ptr)
{
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

static void
ConvertFloat(int num, int in_type, char in_length, char *in_ptr,
	     int out_type, char out_length, char *out_ptr)
{
  int i;
  char *in_p;
  char *out_p;
  for (i = 0, in_p = in_ptr, out_p = out_ptr; i < num; i++, in_p += in_length, out_p += out_length) {
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
			      int status, struct descriptor *d)
{
  Message *m = 0;
  int nbytes = (d->class == CLASS_S) ? d->length : ((ARRAY_7 *) d)->arsize;
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
  m = memset(malloc(sizeof(MsgHdr) + nbytes), 0, sizeof(MsgHdr) + nbytes);
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
    array_coeff *a = (array_coeff *) d;
    m->h.ndims = a->dimct;
    if (a->aflags.coeff)
      for (i = 0; i < m->h.ndims && i < 7; i++)
	m->h.dims[i] = a->m[i];
    else
      m->h.dims[0] = a->length ? a->arsize / a->length : 0;
    for (i = m->h.ndims; i < 7; i++)
      m->h.dims[i] = 0;
  }
  switch (CType(client_type)) {
  case IEEE_CLIENT:
  case JAVA_CLIENT:
    switch (d->dtype) {
    case DTYPE_F:
      ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer,
		   CvtIEEE_S, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FC:
      ConvertFloat(num * 2, CvtVAX_F, (char)(d->length / 2),
		   d->pointer, CvtIEEE_S, (char)(m->h.length / 2), m->bytes);
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
      ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer,
		   CvtIEEE_T, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      ConvertFloat(num * 2, CvtVAX_D, (char)(d->length / 2),
		   d->pointer, CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer,
		   CvtIEEE_T, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      ConvertFloat(num * 2, CvtVAX_G, (char)(d->length / 2),
		   d->pointer, CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
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
      ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes);
      break;
    case DTYPE_F:
      ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer,
		   CvtCRAY, (char)m->h.length, m->bytes);
      break;
    case DTYPE_FS:
      ConvertFloat(num, CvtIEEE_S, (char)d->length,
		   d->pointer, CvtCRAY, (char)m->h.length, m->bytes);
      break;
    case DTYPE_FC:
      ConvertFloat(num * 2, CvtVAX_F, (char)(d->length / 2),
		   d->pointer, CvtCRAY, (char)(m->h.length / 2), m->bytes);
      break;
    case DTYPE_FSC:
      ConvertFloat(num * 2, CvtIEEE_S, (char)(d->length / 2),
		   d->pointer, CvtCRAY, (char)(m->h.length / 2), m->bytes);
      break;
    case DTYPE_D:
      ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes);
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes);
      break;
    case DTYPE_FT:
      ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer,
		   CvtCRAY, (char)m->h.length, m->bytes);
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
      ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes);
      break;
    case DTYPE_F:
      ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer,
		   CvtIEEE_S, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FC:
      ConvertFloat(num * 2, CvtVAX_F, (char)(d->length / 2),
		   d->pointer, CvtIEEE_S, (char)(m->h.length / 2), m->bytes);
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
      ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer,
		   CvtIEEE_T, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      ConvertFloat(num * 2, CvtVAX_D, (char)(d->length / 2),
		   d->pointer, CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_G:
      ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer,
		   CvtIEEE_T, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      ConvertFloat(num * 2, CvtVAX_G, (char)(d->length / 2),
		   d->pointer, CvtIEEE_T, (char)(m->h.length / 2), m->bytes);
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
      ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer,
		   CvtVAX_G, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_DC:
      ConvertFloat(num * 2, CvtVAX_D, (char)(d->length / 2),
		   d->pointer, CvtVAX_G, (char)(m->h.length / 2), m->bytes);
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
      ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer,
		   CvtVAX_F, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FSC:
      ConvertFloat(num * 2, CvtIEEE_S, sizeof(float),
		   d->pointer, CvtVAX_F, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_FT:
      ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer,
		   CvtVAX_G, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FTC:
      ConvertFloat(num * 2, CvtIEEE_T, sizeof(double),
		   d->pointer, CvtVAX_G, (char)(m->h.length / 2), m->bytes);
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
      ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer,
		   CvtVAX_D, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_GC:
      ConvertFloat(num * 2, CvtVAX_G, sizeof(double),
		   d->pointer, CvtVAX_D, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX_DOUBLE;
      break;
    case DTYPE_FS:
      ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer,
		   CvtVAX_F, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_FLOAT;
      break;
    case DTYPE_FSC:
      ConvertFloat(num * 2, CvtIEEE_S, sizeof(float),
		   d->pointer, CvtVAX_F, (char)(m->h.length / 2), m->bytes);
      m->h.dtype = DTYPE_COMPLEX;
      break;
    case DTYPE_FT:
      ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer,
		   CvtVAX_D, (char)m->h.length, m->bytes);
      m->h.dtype = DTYPE_DOUBLE;
      break;
    case DTYPE_FTC:
      ConvertFloat(num * 2, CvtIEEE_T, sizeof(double),
		   d->pointer, CvtVAX_D, (char)(m->h.length / 2), m->bytes);
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

static void ResetErrors()
{
  static int four = 4;
  static DESCRIPTOR_LONG(clear_messages, &four);
  static DESCRIPTOR(messages, "");
  TdiDebug(&clear_messages, &messages MDS_END_ARG);
}

static void GetErrorText(int status, struct descriptor_xd *xd)
{
  static int one = 1;
  static DESCRIPTOR_LONG(get_messages, &one);
  TdiDebug(&get_messages, xd MDS_END_ARG);
  if (!xd->length) {
    static DESCRIPTOR(unknown, "unknown error occured");
    struct descriptor message = { 0, DTYPE_T, CLASS_S, 0 };
    if ((message.pointer = MdsGetMsg(status)) != NULL) {
      message.length = strlen(message.pointer);
      MdsCopyDxXd(&message, xd);
    } else
      MdsCopyDxXd((struct descriptor *)&unknown, xd);
  }
}

static void ClientEventAst(MdsEventList * e, int data_len, char *data)
{
  int conid = e->conid;
  Connection *c = FindConnection(e->conid, 0);
  int i;
  char client_type = c->client_type;
  Message *m;
  JMdsEventInfo *info;
  int len;
  LockAsts();
  if (CType(client_type) == JAVA_CLIENT) {
    len = sizeof(MsgHdr) + sizeof(JMdsEventInfo);
    m = memset(malloc(len), 0, len);
    m->h.ndims = 0;
    m->h.client_type = client_type;
    m->h.msglen = len;
    m->h.dtype = DTYPE_EVENT_NOTIFY;
    info = (JMdsEventInfo *) m->bytes;
    if (data_len > 0)
      memcpy(info->data, data, (data_len < 12) ? data_len : 12);
    for (i = data_len; i < 12; i++)
      info->data[i] = 0;
    info->eventid = e->jeventid;
  } else {
    m = memset(malloc(sizeof(MsgHdr) + e->info_len), 0, sizeof(MsgHdr) + e->info_len);
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

///
/// Executes TDI expression held by a connecion instance. This first searches if
/// connection message corresponds to AST or CAN requests, if no asyncronous ops
/// are requested the TDI actual expression is parsed through tdishr library.
/// In this case the current TDI context and tree is switched to the connection
/// ones stored in the connection context field.
///
/// ### AST and CAN
/// AST and CAN stands for "Asynchronous System Trap" and "CANcel event request".
/// This is an asyncronous message mechanism taken from the OpenVMS system.
/// The event handler is passed inside the message arguments and executed by the
/// MDSEventAst() function from mdslib.
///
/// \param connection the Connection instance filled with proper descriptor arguments
/// \return the execute message answer built using BuildAnswer()
///
static Message *ExecuteMessage(Connection * connection)
{
  Message *ans = 0;		// return message instance //
  int status = 1;		// return status           //

  static EMPTYXD(emptyxd);
  struct descriptor_xd *xd;
  char *evname;
  static DESCRIPTOR(eventastreq, EVENTASTREQUEST);	// AST request descriptor //
  static DESCRIPTOR(eventcanreq, EVENTCANREQUEST);	// Can request descriptor //

  int client_type = connection->client_type;
  int java = CType(client_type) == JAVA_CLIENT;
  int message_id = connection->message_id;

  // AST REQUEST //
  if (StrCompare(connection->descrip[0], (struct descriptor *)&eventastreq) == 0) {
    static int eventid = -1;
    static DESCRIPTOR_LONG(eventiddsc, &eventid);
    MdsEventList *newe = (MdsEventList *) malloc(sizeof(MdsEventList));
    struct descriptor_a *info = (struct descriptor_a *)connection->descrip[2];
    newe->conid = connection->id;

    evname = malloc(connection->descrip[1]->length + 1);
    memcpy(evname, connection->descrip[1]->pointer, connection->descrip[1]->length);
    evname[connection->descrip[1]->length] = 0;

    // Manage AST Event //
    status = MDSEventAst(evname,
			 (void (*)(void *, int, char *))ClientEventAst, newe, &newe->eventid);

    free(evname);
    if (java) {
      newe->info = 0;
      newe->info_len = 0;
      newe->jeventid = *connection->descrip[2]->pointer;
    } else {
      newe->info = (struct _eventinfo *)memcpy(malloc(info->arsize), info->pointer, info->arsize);
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
	for (e = connection->event; e->next; e = e->next) ;
	e->next = newe;
      } else
	connection->event = newe;
    }
    if (!java)
      ans = BuildResponse(client_type, message_id, status, &eventiddsc);
  }
  // CAN REQUEST //
  else if (StrCompare(connection->descrip[0], (struct descriptor *)&eventcanreq) == 0) {
    static int eventid;
    static DESCRIPTOR_LONG(eventiddsc, &eventid);
    MdsEventList *e;
    MdsEventList **p;
    if (!java)
      eventid = *(int *)connection->descrip[1]->pointer;
    else
      eventid = (int)*connection->descrip[1]->pointer;
    if (connection->event) {
      for (p = &connection->event, e = connection->event; e && (e->eventid != eventid);
	   p = &e->next, e = e->next) ;
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
    void *old_dbid;
    void *tdi_context[6];
    EMPTYXD(ans_xd);

    int contextSwitch = GetContextSwitching();
    if (contextSwitch) {
      old_dbid = TreeSwitchDbid(connection->DBID);
      TdiSaveContext(tdi_context);
      TdiRestoreContext(connection->tdicontext);
    }
    connection->descrip[connection->nargs++] = (struct descriptor *)(xd = (struct descriptor_xd *)
								     memcpy(malloc
									    (sizeof
									     (emptyxd)), &emptyxd,
									    sizeof(emptyxd)));
    connection->descrip[connection->nargs++] = MdsEND_ARG;
    ResetErrors();
    SetCompressionLevel(connection->compression_level);
    status = (char *)LibCallg(&connection->nargs, TdiExecute) - (char *)0;
    if (status & 1)
      status = TdiData(xd, &ans_xd MDS_END_ARG);
    if (!(status & 1))
      GetErrorText(status, &ans_xd);
    else if (GetCompressionLevel() != connection->compression_level) {
      connection->compression_level = GetCompressionLevel();
      if (connection->compression_level > GetMaxCompressionLevel())
	connection->compression_level = GetMaxCompressionLevel();
      SetCompressionLevel(connection->compression_level);
    }
    ans = BuildResponse(connection->client_type, connection->message_id, status, ans_xd.pointer);
    MdsFree1Dx(xd, NULL);
    MdsFree1Dx(&ans_xd, NULL);
    if (contextSwitch) {
      TdiSaveContext(connection->tdicontext);
      TdiRestoreContext(tdi_context);
      connection->DBID = TreeSwitchDbid(old_dbid);
    }
  }
  FreeDescriptors(connection);
  return ans;
}

///
/// Handle message from server listen routine. A new descriptor instance is created
/// with the message buffer size and the message memory is copyed inside. A proper
/// conversion of memory structure is applied if neede for the type of client
/// connected.
///
/// \param connection the connection instance to handle
/// \param message the message to process
/// \return message answer
///
Message *ProcessMessage(Connection * connection, Message * message)
{

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
      ans = BuildResponse(connection->client_type,
			  connection->message_id, 1, (struct descriptor *)&status_d);
      return ans;
    }
  }

  // STANADARD COMMANDS ////////////////////////////////////////////////////////
  // idx < nargs        ////////////////////////////////////////////////////////
  if (message->h.descriptor_idx < connection->nargs) {

    // set connection to the message client_type  //
    connection->client_type = message->h.client_type;

    // d -> reference to curent idx argument desctriptor  //
    struct descriptor *d = connection->descrip[message->h.descriptor_idx];

    if (!d) {
      // instance the connection descriptor field //
      static short lengths[] = { 0, 0, 1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 8, 16, 0 };
      switch (message->h.ndims) {
	static struct descriptor scalar = { 0, 0, CLASS_S, 0 };
	static DESCRIPTOR_A_COEFF(array_1, 0, 0, 0, 1, 0);
	static DESCRIPTOR_A_COEFF(array_2, 0, 0, 0, 2, 0);
	static DESCRIPTOR_A_COEFF(array_3, 0, 0, 0, 3, 0);
	static DESCRIPTOR_A_COEFF(array_4, 0, 0, 0, 4, 0);
	static DESCRIPTOR_A_COEFF(array_5, 0, 0, 0, 5, 0);
	static DESCRIPTOR_A_COEFF(array_6, 0, 0, 0, 6, 0);
	static DESCRIPTOR_A_COEFF(array_7, 0, 0, 0, 7, 0);
      case 0:
	d = (struct descriptor *)MakeDesc(scalar);
	break;
      case 1:
	d = (struct descriptor *)MakeDesc(array_1);
	break;
      case 2:
	d = (struct descriptor *)MakeDesc(array_2);
	break;
      case 3:
	d = (struct descriptor *)MakeDesc(array_3);
	break;
      case 4:
	d = (struct descriptor *)MakeDesc(array_4);
	break;
      case 5:
	d = (struct descriptor *)MakeDesc(array_5);
	break;
      case 6:
	d = (struct descriptor *)MakeDesc(array_6);
	break;
      case 7:
	d = (struct descriptor *)MakeDesc(array_7);
	break;
      }
      d->length =
	  message->h.dtype < DTYPE_CSTRING ? lengths[message->h.dtype] : message->h.length;
      d->dtype = message->h.dtype;
      if (d->class == CLASS_A) {
	ARRAY_7 *a = (ARRAY_7 *) d;
	int num = 1;
	int i;
	for (i = 0; i < message->h.ndims; i++) {
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

      int dbytes = d->class == CLASS_S ? (int)d->length : (int)((ARRAY_7 *) d)->arsize;
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
	  ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer);
	  break;
	case DTYPE_SHORT:
	case DTYPE_LONG:
	  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer);
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
	  ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer);
	  break;
	case DTYPE_SHORT:
	case DTYPE_LONG:
	  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer);
	  break;
	case DTYPE_FLOAT:
	  ConvertFloat(num, CvtCRAY,
		       (char)message->h.length,
		       message->bytes, CvtIEEE_S, (char)d->length, d->pointer);
	  break;
	case DTYPE_COMPLEX:
	  ConvertFloat(num * 2, CvtCRAY,
		       (char)(message->h.length /
			      2), message->bytes, CvtIEEE_S, (char)(d->length / 2), d->pointer);
	  break;
	case DTYPE_DOUBLE:
	  ConvertFloat(num, CvtCRAY,
		       (char)message->h.length,
		       message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
	  break;
	default:
	  memcpy(d->pointer, message->bytes, dbytes);
	  break;
	}
	break;
      default:
	switch (d->dtype) {
	case DTYPE_FLOAT:
	  ConvertFloat(num, CvtVAX_F,
		       (char)message->h.length,
		       message->bytes, CvtIEEE_S, sizeof(float), d->pointer);
	  break;
	case DTYPE_COMPLEX:
	  ConvertFloat(num * 2, CvtVAX_F,
		       (char)message->h.length,
		       message->bytes, CvtIEEE_S, sizeof(float), d->pointer);
	  break;
	case DTYPE_DOUBLE:
	  if (CType(connection->client_type) == VMSG_CLIENT)
	    ConvertFloat(num, CvtVAX_G,
			 (char)message->h.length,
			 message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
	  else
	    ConvertFloat(num, CvtVAX_D,
			 (char)message->h.length,
			 message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
	  break;

	case DTYPE_COMPLEX_DOUBLE:
	  if (CType(connection->client_type) == VMSG_CLIENT)
	    ConvertFloat(num * 2, CvtVAX_G,
			 (char)(message->h.length / 2),
			 message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
	  else
	    ConvertFloat(num * 2, CvtVAX_D,
			 (char)(message->h.length / 2),
			 message->bytes, CvtIEEE_T, sizeof(double), d->pointer);
	  break;
	default:
	  memcpy(d->pointer, message->bytes, dbytes);
	  break;
	}
      }
      switch (d->dtype) {
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
    switch (message->h.descriptor_idx) {
    case MDS_IO_OPEN_K:
      {
	int fd;
	char *filename = (char *)message->bytes;
	char *ptr;
	int options = message->h.dims[1];
	int fopts;
	mode_t mode = message->h.dims[2];
	DESCRIPTOR_LONG(fd_d, 0);
	fd_d.pointer = (char *)&fd;
	fopts =
	    (options & MDS_IO_O_CREAT ? O_CREAT : 0) |
	    (options & MDS_IO_O_TRUNC ? O_TRUNC : 0) |
	    (options & MDS_IO_O_EXCL ? O_EXCL : 0) |
	    (options & MDS_IO_O_WRONLY ? O_WRONLY : 0) |
	    (options & MDS_IO_O_RDONLY ? O_RDONLY : 0) | (options & MDS_IO_O_RDWR ? O_RDWR : 0);
	fd = open(filename, fopts | O_BINARY | O_RANDOM, mode);
	if (fd == -1) {
	  int retry_open = 0;
	  while (fd == -1 && ((ptr = strchr(filename, '\\')) != 0)) {
	    retry_open = 1;
	    *ptr = '/';
	  }
	  if (retry_open)
	    fd = open(filename, fopts | O_BINARY | O_RANDOM, mode);
	}
#ifndef _WIN32
	if ((fd != -1) && ((fopts & O_CREAT) != 0)) {
	  char *cmd = (char *)malloc(64 + strlen(filename));
	  int num = snprintf(cmd, 64 + strlen(filename), "SetMdsplusFileProtection %s 2> /dev/null", filename);
	  if (num > 0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	    system(cmd);
#pragma GCC diagnostic pop
	  free(cmd);
	}
#endif
	ans =
	    BuildResponse(connection->client_type,
			  connection->message_id, 3, (struct descriptor *)&fd_d);
	break;
      }
    case MDS_IO_CLOSE_K:
      {
	int stat = close(message->h.dims[1]);
	DESCRIPTOR_LONG(stat_d, 0);
	stat_d.pointer = (char *)&stat;
	ans =
	    BuildResponse(connection->client_type,
			  connection->message_id, 1, (struct descriptor *)&stat_d);
	break;
      }
    case MDS_IO_LSEEK_K:
      {
	int fd = message->h.dims[1];
	off_t offset;
	int whence = message->h.dims[4];
	int64_t ans_o;
	struct descriptor ans_d = { 8, DTYPE_Q, CLASS_S, 0 };
#ifdef WORDS_BIGENDIAN
	int tmp;
	tmp = message->h.dims[2];
	message->h.dims[2] = message->h.dims[3];
	message->h.dims[3] = tmp;
#endif
	offset = (off_t) * (int64_t *) & message->h.dims[2];
	ans_o = lseek(fd, offset, whence);
	ans_d.pointer = (char *)&ans_o;
	ans =
	    BuildResponse(connection->client_type,
			  connection->message_id, 1, (struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_READ_K:
      {
	int fd = message->h.dims[1];
	void *buf = malloc(message->h.dims[2]);
	size_t num = (size_t) message->h.dims[2];
	ssize_t nbytes = read(fd, buf, num);
#ifdef USE_PERF
	TreePerfRead(nbytes);
#endif
	if (nbytes > 0) {
	  DESCRIPTOR_A(ans_d, 1, DTYPE_B, 0, 0);
	  if ((size_t)nbytes != num)
	    perror("READ_K wrong byte count");
	  ans_d.pointer = buf;
	  ans_d.arsize = nbytes;
	  ans =
	      BuildResponse
	      (connection->client_type, connection->message_id, 1, (struct descriptor *)
	       &ans_d);
	} else {
	  DESCRIPTOR(ans_d, "");
	  ans =
	      BuildResponse
	      (connection->client_type, connection->message_id, 1, (struct descriptor *)
	       &ans_d);
	}
	free(buf);
	break;
      }
    case MDS_IO_WRITE_K:
      {
	ssize_t nbytes = write(message->h.dims[1], message->bytes,
			       (size_t) message->h.dims[0]);
	DESCRIPTOR_LONG(ans_d, 0);
#ifdef USE_PERF
	TreePerfWrite(nbytes);
#endif
	ans_d.pointer = (char *)&nbytes;
	if (nbytes != (ssize_t) message->h.dims[0])
	  perror("WRITE_K wrong byte count");
	ans =
	    BuildResponse(connection->client_type,
			  connection->message_id, 1, (struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_LOCK_K:
      {
	int fd = message->h.dims[1];
	int status;
	int64_t offset;
	int size = message->h.dims[4];
	int mode_in = message->h.dims[5];
	int mode = mode_in & 0x3;
	int nowait = mode_in & 0x8;
	int deleted;
	DESCRIPTOR_LONG(ans_d, 0);
#ifdef WORDS_BIGENDIAN
	offset = ((int64_t) message->h.dims[2]) << 32 | message->h.dims[3];
#else
	offset = ((int64_t) message->h.dims[3]) << 32 | message->h.dims[2];
#endif
	status = lock_file(fd, offset, size, mode | nowait, &deleted);
	ans_d.pointer = (char *)&status;
	ans =
	    BuildResponse(connection->client_type,
			  connection->message_id, deleted ? 3 : 1, (struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_EXISTS_K:
      {
	struct stat statbuf;
	int status = (stat(message->bytes, &statbuf) == 0);
	DESCRIPTOR_LONG(status_d, 0);
	status_d.pointer = (char *)&status;
	ans =
	    BuildResponse(connection->client_type, connection->message_id, 1, (struct descriptor *)
			  &status_d);
	break;
      }
    case MDS_IO_REMOVE_K:
      {
	int status = remove(message->bytes);
	DESCRIPTOR_LONG(status_d, 0);
	status_d.pointer = (char *)&status;
	ans =
	    BuildResponse(connection->client_type, connection->message_id, 1, (struct descriptor *)
			  &status_d);
	break;
      }
    case MDS_IO_RENAME_K:
      {
	DESCRIPTOR_LONG(status_d, 0);
	int status = rename(message->bytes,
			    message->bytes + strlen(message->bytes) + 1);
	status_d.pointer = (char *)&status;
	ans =
	    BuildResponse(connection->client_type, connection->message_id, 1, (struct descriptor *)
			  &status_d);
	break;
      }
    case MDS_IO_READ_X_K:
      {
	int fd = message->h.dims[1];
	off_t offset;
	void *buf = malloc(message->h.dims[4]);
	size_t num = (size_t) message->h.dims[4];
	ssize_t nbytes;
	int deleted;
#ifdef WORDS_BIGENDIAN
	int tmp;
	tmp = message->h.dims[2];
	message->h.dims[2] = message->h.dims[3];
	message->h.dims[3] = tmp;
#endif
	offset = (off_t) * (int64_t *) & message->h.dims[2];

	lock_file(fd, offset, num, 1, &deleted);
	lseek(fd, offset, SEEK_SET);
	nbytes = read(fd, buf, num);
	if (nbytes != (ssize_t)num)
	  perror("READ_X wrong byte count");
#ifdef USE_PERF
	TreePerfRead(nbytes);
#endif
	lock_file(fd, offset, num, 0, &deleted);
	if (nbytes > 0) {
	  DESCRIPTOR_A(ans_d, 1, DTYPE_B, 0, 0);
	  ans_d.pointer = buf;
	  ans_d.arsize = nbytes;
	  ans =
	      BuildResponse
	      (connection->client_type,
	       connection->message_id, deleted ? 3 : 1, (struct descriptor *)
	       &ans_d);
	} else {
	  DESCRIPTOR(ans_d, "");
	  ans =
	      BuildResponse
	      (connection->client_type,
	       connection->message_id, deleted ? 3 : 1, (struct descriptor *)
	       &ans_d);
	}
	free(buf);
	break;
      }
      {
	DESCRIPTOR_LONG(status_d, 0);
	int status = 0;
	status_d.pointer = (char *)&status;
	ans =
	    BuildResponse(connection->client_type, connection->message_id, 1, (struct descriptor *)
			  &status_d);
	break;
      }
    }
  }
  return ans;
}
