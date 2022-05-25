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

#include <libroutines.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <strroutines.h>
#include <tdishr.h>
#include <treeshr.h>
#include <socket_port.h>

#include "../tdishr/tdithreadstatic.h"
#include "../treeshr/treeshrp.h"
#include "../mdsIo.h"
#include "../mdsip_connections.h"

//#define DEBUG
#include <mdsmsg.h>

extern int TdiRestoreContext(void **);
extern int TdiSaveContext(void **);

extern int CvtConvertFloat(void *invalue, uint32_t indtype, void *outvalue,
                           uint32_t outdtype, uint32_t options);
#define VAX_F DTYPE_F   /* VAX F     Floating point data    */
#define VAX_D DTYPE_D   /* VAX D     Floating point data    */
#define VAX_G DTYPE_G   /* VAX G     Floating point data    */
#define VAX_H DTYPE_H   /* VAX H     Floating point data    */
#define IEEE_S DTYPE_FS /* IEEE S    Floating point data    */
#define IEEE_T DTYPE_FT /* IEEE T    Floating point data    */
#define IBM_LONG 6      /* IBM Long  Floating point data    */
#define IBM_SHORT 7     /* IBM Short Floating point data    */
#define CRAY 8          /* Cray      Floating point data    */
#define IEEE_X 9        /* IEEE X    Floating point data    */

static void convert_binary(int num, int sign_extend, short in_length,
                           char *in_ptr, short out_length, char *out_ptr)
{
  int i;
  int j;
  signed char *in_p = (signed char *)in_ptr;
  signed char *out_p = (signed char *)out_ptr;
  short min_len = out_length < in_length ? out_length : in_length;
  for (i = 0; i < num; i++, in_p += in_length, out_p += out_length)
  {
    for (j = 0; j < min_len; j++)
      out_p[j] = in_p[j];
    for (; j < out_length; j++)
      out_p[j] = sign_extend ? (in_p[min_len - 1] < 0 ? -1 : 0) : 0;
  }
}

static void convert_float(int num, int in_type, char in_length, char *in_ptr,
                          int out_type, char out_length, char *out_ptr)
{
  int i;
  char *in_p;
  char *out_p;
  for (i = 0, in_p = in_ptr, out_p = out_ptr; i < num;
       i++, in_p += in_length, out_p += out_length)
  {
    char *ptr = in_p;
    char cray_f[8];
    if (in_type == CRAY)
    {
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
    if (out_type == CRAY)
    {
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

static inline uint32_t get_nbytes(uint16_t *length, uint32_t *num, int client_type, mdsdsc_t *d)
{
  if (CType(client_type) == CRAY_CLIENT)
  {
    switch (d->dtype)
    {
    case DTYPE_USHORT:
    case DTYPE_ULONG:
    case DTYPE_SHORT:
    case DTYPE_LONG:
    case DTYPE_F:
    case DTYPE_FS:
      *length = 8;
      break;
    case DTYPE_FC:
    case DTYPE_FSC:
    case DTYPE_D:
    case DTYPE_G:
    case DTYPE_FT:
      *length = 16;
      break;
    default:
      *length = d->length;
      break;
    }
  }
  else if (CType(client_type) == CRAY_IEEE_CLIENT)
  {
    switch (d->dtype)
    {
    case DTYPE_USHORT:
    case DTYPE_SHORT:
      *length = 4;
      break;
    case DTYPE_ULONG:
    case DTYPE_LONG:
      *length = 8;
      break;
    default:
      *length = d->length;
      break;
    }
  }
  else
  {
    *length = d->length;
  }
  if (d->class == CLASS_S)
  {
    *num = 1;
  }
  else if (*length == 0)
  {
    *num = 0;
  }
  else
  {
    *num = ((array_coeff *)d)->arsize / (*length);
  }
  return (*num) * (*length);
}

static inline void convert_ieee(Message *m, int num, const mdsdsc_t *d, uint32_t nbytes)
{
  switch (d->dtype)
  {
  case DTYPE_F:
    convert_float(num, VAX_F, (char)d->length, d->pointer, IEEE_S,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_FLOAT;
    break;
  case DTYPE_FC:
    convert_float(num * 2, VAX_F, (char)(d->length / 2), d->pointer,
                  IEEE_S, (char)(m->h.length / 2), m->bytes);
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
    convert_float(num, VAX_D, (char)d->length, d->pointer, IEEE_T,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_DC:
    convert_float(num * 2, VAX_D, (char)(d->length / 2), d->pointer,
                  IEEE_T, (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX_DOUBLE;
    break;
  case DTYPE_G:
    convert_float(num, VAX_G, (char)d->length, d->pointer, IEEE_T,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_GC:
    convert_float(num * 2, VAX_G, (char)(d->length / 2), d->pointer,
                  IEEE_T, (char)(m->h.length / 2), m->bytes);
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
}

static inline void convert_cray(Message *m, int num, const mdsdsc_t *d, uint32_t nbytes)
{
  switch (d->dtype)
  {
  case DTYPE_USHORT:
  case DTYPE_ULONG:
    convert_binary(num, 0, d->length, d->pointer, m->h.length, m->bytes);
    break;
  case DTYPE_SHORT:
  case DTYPE_LONG:
    convert_binary(num, 1, (char)d->length, d->pointer, (char)m->h.length,
                   m->bytes);
    break;
  case DTYPE_F:
    convert_float(num, VAX_F, (char)d->length, d->pointer, CRAY,
                  (char)m->h.length, m->bytes);
    break;
  case DTYPE_FS:
    convert_float(num, IEEE_S, (char)d->length, d->pointer, CRAY,
                  (char)m->h.length, m->bytes);
    break;
  case DTYPE_FC:
    convert_float(num * 2, VAX_F, (char)(d->length / 2), d->pointer,
                  CRAY, (char)(m->h.length / 2), m->bytes);
    break;
  case DTYPE_FSC:
    convert_float(num * 2, IEEE_S, (char)(d->length / 2), d->pointer,
                  CRAY, (char)(m->h.length / 2), m->bytes);
    break;
  case DTYPE_D:
    convert_float(num, VAX_D, sizeof(double), d->pointer, CRAY,
                  (char)m->h.length, m->bytes);
    break;
  case DTYPE_G:
    convert_float(num, VAX_G, sizeof(double), d->pointer, CRAY,
                  (char)m->h.length, m->bytes);
    break;
  case DTYPE_FT:
    convert_float(num, IEEE_T, sizeof(double), d->pointer, CRAY,
                  (char)m->h.length, m->bytes);
    break;
  default:
    memcpy(m->bytes, d->pointer, nbytes);
    break;
  }
}

static inline void convert_cray_ieee(Message *m, int num, const mdsdsc_t *d, uint32_t nbytes)
{
  switch (d->dtype)
  {
  case DTYPE_USHORT:
  case DTYPE_ULONG:
    convert_binary(num, 0, d->length, d->pointer, m->h.length, m->bytes);
    break;
  case DTYPE_SHORT:
  case DTYPE_LONG:
    convert_binary(num, 1, (char)d->length, d->pointer, (char)m->h.length,
                   m->bytes);
    break;
  case DTYPE_F:
    convert_float(num, VAX_F, (char)d->length, d->pointer, IEEE_S,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_FLOAT;
    break;
  case DTYPE_FC:
    convert_float(num * 2, VAX_F, (char)(d->length / 2), d->pointer,
                  IEEE_S, (char)(m->h.length / 2), m->bytes);
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
    convert_float(num, VAX_D, (char)d->length, d->pointer, IEEE_T,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_DC:
    convert_float(num * 2, VAX_D, (char)(d->length / 2), d->pointer,
                  IEEE_T, (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX_DOUBLE;
    break;
  case DTYPE_G:
    convert_float(num, VAX_G, (char)d->length, d->pointer, IEEE_T,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_GC:
    convert_float(num * 2, VAX_G, (char)(d->length / 2), d->pointer,
                  IEEE_T, (char)(m->h.length / 2), m->bytes);
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
}

static inline void convert_vmsg(Message *m, int num, const mdsdsc_t *d, uint32_t nbytes)
{
  switch (d->dtype)
  {
  case DTYPE_F:
    memcpy(m->bytes, d->pointer, nbytes);
    m->h.dtype = DTYPE_FLOAT;
    break;
  case DTYPE_FC:
    memcpy(m->bytes, d->pointer, nbytes);
    m->h.dtype = DTYPE_COMPLEX;
    break;
  case DTYPE_D:
    convert_float(num, VAX_D, sizeof(double), d->pointer, VAX_G,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_DC:
    convert_float(num * 2, VAX_D, (char)(d->length / 2), d->pointer,
                  VAX_G, (char)(m->h.length / 2), m->bytes);
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
    convert_float(num, IEEE_S, sizeof(float), d->pointer, VAX_F,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_FLOAT;
    break;
  case DTYPE_FSC:
    convert_float(num * 2, IEEE_S, sizeof(float), d->pointer, VAX_F,
                  (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX;
    break;
  case DTYPE_FT:
    convert_float(num, IEEE_T, sizeof(double), d->pointer, VAX_G,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_FTC:
    convert_float(num * 2, IEEE_T, sizeof(double), d->pointer, VAX_G,
                  (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX_DOUBLE;
    break;
  default:
    memcpy(m->bytes, d->pointer, nbytes);
    break;
  }
}

static inline void convert_default(Message *m, int num, const mdsdsc_t *d, uint32_t nbytes)
{
  switch (d->dtype)
  {
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
    convert_float(num, VAX_G, sizeof(double), d->pointer, VAX_D,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_GC:
    convert_float(num * 2, VAX_G, sizeof(double), d->pointer, VAX_D,
                  (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX_DOUBLE;
    break;
  case DTYPE_FS:
    convert_float(num, IEEE_S, sizeof(float), d->pointer, VAX_F,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_FLOAT;
    break;
  case DTYPE_FSC:
    convert_float(num * 2, IEEE_S, sizeof(float), d->pointer, VAX_F,
                  (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX;
    break;
  case DTYPE_FT:
    convert_float(num, IEEE_T, sizeof(double), d->pointer, VAX_D,
                  (char)m->h.length, m->bytes);
    m->h.dtype = DTYPE_DOUBLE;
    break;
  case DTYPE_FTC:
    convert_float(num * 2, IEEE_T, sizeof(double), d->pointer, VAX_D,
                  (char)(m->h.length / 2), m->bytes);
    m->h.dtype = DTYPE_COMPLEX_DOUBLE;
    break;
  default:
    memcpy(m->bytes, d->pointer, nbytes);
    break;
  }
}

static inline int _send_response(Connection *connection, Message *message, Message **message_out, int status, mdsdsc_t *d)
{
  const int client_type = connection->client_type;
  const unsigned char message_id = connection->message_id;
  Message *m = NULL;
  int serial = STATUS_NOT_OK || (connection->descrip[0] && connection->descrip[0]->dtype == DTYPE_SERIAL);
  (void)message;
  if (serial && STATUS_OK && d->class == CLASS_A)
  {
    mdsdsc_a_t *array = (mdsdsc_a_t *)d;
    *message_out = m = malloc(sizeof(MsgHdr) + array->arsize);
    memset(&m->h, 0, sizeof(MsgHdr));
    m->h.msglen = sizeof(MsgHdr) + array->arsize;
    m->h.client_type = client_type;
    m->h.message_id = message_id;
    m->h.status = status;
    m->h.dtype = DTYPE_SERIAL;
    m->h.ndims = 1;
    m->h.dims[0] = array->arsize;
    m->h.length = 1;
    memcpy(m->bytes, array->pointer, array->arsize);
  }
  else
  {
    uint16_t length;
    uint32_t num;
    uint32_t nbytes = get_nbytes(&length, &num, client_type, d);
    *message_out = m = malloc(sizeof(MsgHdr) + nbytes);
    memset(&m->h, 0, sizeof(MsgHdr));
    m->h.msglen = sizeof(MsgHdr) + nbytes;
    m->h.client_type = client_type;
    m->h.message_id = message_id;
    m->h.status = status;
    m->h.dtype = d->dtype;
    m->h.length = length;
    if (d->class == CLASS_S)
    {
      m->h.ndims = 0;
    }
    else
    {
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
    switch (CType(client_type))
    {
    case IEEE_CLIENT:
    case JAVA_CLIENT:
      convert_ieee(m, num, d, nbytes);
      break;
    case CRAY_CLIENT:
      convert_cray(m, num, d, nbytes);
      break;
    case CRAY_IEEE_CLIENT:
      convert_cray_ieee(m, num, d, nbytes);
      break;
    case VMSG_CLIENT:
      convert_vmsg(m, num, d, nbytes);
      break;
    default:
      convert_default(m, num, d, nbytes);
      break;
    }
  }
  return SendMdsMsgC(connection, m, 0);
}

/// returns true if message cleanup is handled
static int send_response(Connection *connection, Message *message, const int status_in, mdsdsc_t *const d)
{
  int status;
  INIT_AND_FREE_ON_EXIT(Message *, m);
  status = _send_response(connection, message, &m, status_in, d);
  FREE_NOW(m);
  if (STATUS_NOT_OK)
    return FALSE; // no good close connection
  free(message);
  return TRUE;
}

/// returns true if message cleanup is handled
static int return_status(Connection *connection, Message *message, int status)
{
  struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&status};
  return send_response(connection, message, 1, &ans_d);
}

static void GetErrorText(int status, mdsdsc_xd_t *xd)
{
  static DESCRIPTOR(unknown, "unknown error occured");
  struct descriptor message = {0, DTYPE_T, CLASS_S, 0};
  if ((message.pointer = MdsGetMsg(status)) != NULL)
  {
    message.length = strlen(message.pointer);
    MdsCopyDxXd(&message, xd);
  }
  else
    MdsCopyDxXd((mdsdsc_t *)&unknown, xd);
}

static void _client_event_ast_cleanup(Message **m)
{
  free(*m);
  UnlockAsts();
}

static inline void _client_event_ast(MdsEventList *e, int data_len, char *data, int client_type, Message **m)
{
  int len;
  if (CType(client_type) == JAVA_CLIENT)
  {
    JMdsEventInfo *info;
    len = sizeof(MsgHdr) + sizeof(JMdsEventInfo);
    *m = calloc(1, len);
    info = (JMdsEventInfo *)(*m)->bytes;
    info->eventid = e->jeventid;
  }
  else
  {
    len = sizeof(MsgHdr) + e->info_len;
    *m = calloc(1, len);
    memcpy((*m)->bytes, e->info, e->info_len);
  }
  (*m)->h.client_type = client_type;
  (*m)->h.msglen = len;
  (*m)->h.dtype = DTYPE_EVENT_NOTIFY;
  if (data_len > 0)
    memcpy(e->info->data, data, (data_len < 12) ? data_len : 12);
  SendMdsMsgC(e->connection, *m, MSG_DONTWAIT);
}

static void client_event_ast(MdsEventList *e, int data_len, char *data)
{
  const client_t client_type = e->connection->client_type;
  // Check Connection: if down, cancel the event and return
  if (client_type == INVALID_CLIENT)
  {
    MDSEventCan(e->eventid);
    return;
  }
  LockAsts();
  Message *m = NULL;
  pthread_cleanup_push((void *)_client_event_ast_cleanup, (void *)&m);
  _client_event_ast(e, data_len, data, client_type, &m);
  pthread_cleanup_pop(1);
}

typedef struct
{
  void *tdicontext[6];
  Connection *connection;
  mdsdsc_xd_t *xdp;
  int cs;
} cleanup_command_t;

static void cleanup_command(void *args)
{
  cleanup_command_t *p = (cleanup_command_t *)args;
  MdsFree1Dx(p->xdp, NULL);
  if (p->cs)
  {
    TdiSaveContext(p->connection->tdicontext);
    TdiRestoreContext(p->tdicontext);
  }
}

static inline int execute_command(Connection *connection, mdsdsc_xd_t *ans_xd)
{
  int status;
  cleanup_command_t p;
  p.cs = !!GetContextSwitching();
  if (p.cs)
  {
    TdiSaveContext(p.tdicontext);
    TdiRestoreContext(connection->tdicontext);
  }
  p.connection = connection;
  EMPTYXD(xd);
  p.xdp = &xd;
  const int serialize_out = connection->descrip[0]->dtype == DTYPE_SERIAL;
  if (serialize_out)
  {
    connection->descrip[0]->dtype = DTYPE_T;
  }
  pthread_cleanup_push(cleanup_command, (void *)&p);
  TDITHREADSTATIC_INIT;
  --TDI_INTRINSIC_REC;
  status = TdiIntrinsic(OPC_EXECUTE, connection->nargs, connection->descrip, &xd);
  ++TDI_INTRINSIC_REC;
  if (STATUS_OK)
  {
    if (serialize_out)
    {
      connection->descrip[0]->dtype = DTYPE_SERIAL;
      status = MdsSerializeDscOut(xd.pointer, ans_xd);
    }
    else
      status = TdiData(xd.pointer, ans_xd MDS_END_ARG);
  }
  pthread_cleanup_pop(1);
  return status;
}

/// returns true if message cleanup is handled
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

static int execute_message(Connection *connection, Message *message)
{
  int status = 1;
  int freed_message = FALSE;
  char *evname;
  static const DESCRIPTOR(eventastreq, EVENTASTREQUEST); // AST request descriptor //
  static const DESCRIPTOR(eventcanreq, EVENTCANREQUEST); // Can request descriptor //
  const int java = CType(connection->client_type) == JAVA_CLIENT;
  if (!connection->descrip[0])
  {
    MDSWRN("NULL_ptr as first descrip argument");
    freed_message = return_status(connection, message, TdiNULL_PTR);
  }
  else if (StrCompare(connection->descrip[0], (mdsdsc_t *)&eventastreq) == 0)
  { // AST REQUEST //
    int eventid = -1;
    DESCRIPTOR_LONG(eventiddsc, &eventid);
    MdsEventList *newe = (MdsEventList *)malloc(sizeof(MdsEventList));
    struct descriptor_a *info = (struct descriptor_a *)connection->descrip[2];
    newe->connection = connection;

    evname = malloc(connection->descrip[1]->length + 1);
    memcpy(evname, connection->descrip[1]->pointer,
           connection->descrip[1]->length);
    evname[connection->descrip[1]->length] = 0;
    // Manage AST Event //
    status = MDSEventAst(evname, (void (*)(void *, int, char *))client_event_ast,
                         newe, &newe->eventid);
    free(evname);
    if (java)
    {
      newe->info = 0;
      newe->info_len = 0;
      newe->jeventid = *connection->descrip[2]->pointer;
    }
    else
    {
      newe->info = (MdsEventInfo *)memcpy(malloc(info->arsize), info->pointer,
                                          info->arsize);
      newe->info_len = info->arsize;
      newe->info->eventid = newe->eventid;
    }
    newe->next = 0;
    if (STATUS_NOT_OK)
    {
      eventiddsc.pointer = (void *)&eventid;
      free(newe->info);
      free(newe);
    }
    else
    {
      MdsEventList *e;
      eventiddsc.pointer = (void *)&newe->eventid;
      if (connection->event)
      {
        for (e = connection->event; e->next; e = e->next)
          ;
        e->next = newe;
      }
      else
        connection->event = newe;
    }
    if (!java)
      freed_message = send_response(connection, message, status, &eventiddsc);
    else
    {
      free(message);
      freed_message = TRUE;
    }
  }
  else if (StrCompare(connection->descrip[0], (mdsdsc_t *)&eventcanreq) == 0)
  { // CAN REQUEST //
    int eventid;
    DESCRIPTOR_LONG(eventiddsc, &eventid);
    MdsEventList *e;
    MdsEventList **p;
    if (!java)
      eventid = *(int *)connection->descrip[1]->pointer;
    else
      eventid = (int)*connection->descrip[1]->pointer;
    if (connection->event)
    {
      for (p = &connection->event, e = connection->event;
           e && (e->eventid != eventid); p = &e->next, e = e->next)
        ;
      if (e)
      {
        /**/ MDSEventCan(e->eventid);
        /**/ *p = e->next;
        free(e);
      }
    }
    if (!java)
      freed_message = send_response(connection, message, status, &eventiddsc);
    else
    {
      free(message);
      freed_message = TRUE;
    }
  }
  else // NORMAL TDI COMMAND //
  {
    INIT_AND_FREEXD_ON_EXIT(ans_xd);
    status = execute_command(connection, &ans_xd);
    if (STATUS_NOT_OK)
      GetErrorText(status, &ans_xd);
    connection->compression_level = GetCompressionLevel();
    freed_message = send_response(connection, message, status, ans_xd.pointer);
    FREEXD_NOW(ans_xd);
  }
  FreeDescriptors(connection);
  return freed_message;
}

/// returns true if message cleanup is handled
static int standard_command(Connection *connection, Message *message)
{
  if (connection->message_id != message->h.message_id)
  {
    MDSDBG("%d NewM %3d (%2d/%2d) : '%.*s'",
           connection->id, message->h.message_id, message->h.descriptor_idx, message->h.nargs,
           message->h.length, message->bytes);
    FreeDescriptors(connection);
    if (message->h.nargs < MDSIP_MAX_ARGS - 1)
    {
      connection->message_id = message->h.message_id;
      connection->nargs = message->h.nargs;
    }
    else
    {
      return return_status(connection, message, 0);
    }
  }
  // set connection to the message client_type  //
  connection->client_type = message->h.client_type;
  int status = 1;
  mdsdsc_t *d = connection->descrip[message->h.descriptor_idx];
  if (message->h.dtype == DTYPE_SERIAL)
  {
    MdsFreeDescriptor(d);
    mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
    MDSDBG("%d NewA %3d (%2d/%2d) : serial",
           connection->id, message->h.message_id, message->h.descriptor_idx + 1, message->h.nargs);
    status = MdsSerializeDscIn(message->bytes, &xd);
    connection->descrip[message->h.descriptor_idx] = d = xd.pointer;
    if (STATUS_OK && message->h.descriptor_idx == 0 && d->dtype == DTYPE_T)
    {
      d->dtype = DTYPE_SERIAL;
    }
  }
  else
  {
    if (!d)
    {
      // instance the connection descriptor field //
      const short lengths[] = {0, 0, 1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 8, 16, 0};
      length_t length = message->h.dtype < DTYPE_CSTRING
                            ? lengths[message->h.dtype]
                            : message->h.length;
      if (message->h.ndims == 0)
      {
        d = malloc(sizeof(mdsdsc_s_t) + length);
        d->class = CLASS_S;
        d->dtype = message->h.dtype;
        d->length = length;
        d->pointer = length ? (void *)d + sizeof(mdsdsc_s_t) : 0;
      }
      else
      {
        static const DESCRIPTOR_A_COEFF(empty, 0, 0, 0, MAX_DIMS, 0);
        int i;
        int num = 1;
        for (i = 0; i < message->h.ndims; i++)
        {
          num *= message->h.dims[i];
        }
        arsize_t arsize = length * num;
        d = memcpy(malloc(sizeof(empty) + arsize), &empty, sizeof(empty));
        d->dtype = message->h.dtype;
        array_coeff *a = (array_coeff *)d;
        a->dimct = message->h.ndims;
        for (i = 0; i < a->dimct; i++)
        {
          a->m[i] = message->h.dims[i];
        }
        a->length = length;
        a->arsize = arsize;
        a->pointer = a->a0 = (void *)d + sizeof(empty);
      }
      connection->descrip[message->h.descriptor_idx] = d;
    }
    if (d)
    {
      // have valid connection descriptor instance     //
      // copy the message buffer into the descriptor   //
      int dbytes = d->class == CLASS_S
                       ? (int)d->length
                       : (int)((array_coeff *)d)->arsize;
      int num = d->length > 1 ? (dbytes / d->length) : dbytes;

      switch (CType(connection->client_type))
      {
      case IEEE_CLIENT:
      case JAVA_CLIENT:
        memcpy(d->pointer, message->bytes, dbytes);
        break;
      case CRAY_IEEE_CLIENT:
        switch (d->dtype)
        {
        case DTYPE_USHORT:
        case DTYPE_ULONG:
          convert_binary(num, 0, message->h.length, message->bytes, d->length,
                         d->pointer);
          break;
        case DTYPE_SHORT:
        case DTYPE_LONG:
          convert_binary(num, 1, message->h.length, message->bytes, d->length,
                         d->pointer);
          break;
        default:
          memcpy(d->pointer, message->bytes, dbytes);
          break;
        }
        break;
      case CRAY_CLIENT:
        switch (d->dtype)
        {
        case DTYPE_USHORT:
        case DTYPE_ULONG:
          convert_binary(num, 0, message->h.length, message->bytes, d->length,
                         d->pointer);
          break;
        case DTYPE_SHORT:
        case DTYPE_LONG:
          convert_binary(num, 1, message->h.length, message->bytes, d->length,
                         d->pointer);
          break;
        case DTYPE_FLOAT:
          convert_float(num, CRAY, (char)message->h.length, message->bytes,
                        IEEE_S, (char)d->length, d->pointer);
          break;
        case DTYPE_COMPLEX:
          convert_float(num * 2, CRAY, (char)(message->h.length / 2),
                        message->bytes, IEEE_S, (char)(d->length / 2),
                        d->pointer);
          break;
        case DTYPE_DOUBLE:
          convert_float(num, CRAY, (char)message->h.length, message->bytes,
                        IEEE_T, sizeof(double), d->pointer);
          break;
        default:
          memcpy(d->pointer, message->bytes, dbytes);
          break;
        }
        break;
      default:
        switch (d->dtype)
        {
        case DTYPE_FLOAT:
          convert_float(num, VAX_F, (char)message->h.length, message->bytes,
                        IEEE_S, sizeof(float), d->pointer);
          break;
        case DTYPE_COMPLEX:
          convert_float(num * 2, VAX_F, (char)message->h.length,
                        message->bytes, IEEE_S, sizeof(float), d->pointer);
          break;
        case DTYPE_DOUBLE:
          if (CType(connection->client_type) == VMSG_CLIENT)
            convert_float(num, VAX_G, (char)message->h.length, message->bytes,
                          IEEE_T, sizeof(double), d->pointer);
          else
            convert_float(num, VAX_D, (char)message->h.length, message->bytes,
                          IEEE_T, sizeof(double), d->pointer);
          break;

        case DTYPE_COMPLEX_DOUBLE:
          if (CType(connection->client_type) == VMSG_CLIENT)
            convert_float(num * 2, VAX_G, (char)(message->h.length / 2),
                          message->bytes, IEEE_T, sizeof(double), d->pointer);
          else
            convert_float(num * 2, VAX_D, (char)(message->h.length / 2),
                          message->bytes, IEEE_T, sizeof(double), d->pointer);
          break;
        default:
          memcpy(d->pointer, message->bytes, dbytes);
          break;
        }
      }
      switch (d->dtype)
      {
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
      MDSDBG("%d NewA %3d (%2d/%2d) : simple",
             connection->id, message->h.message_id, message->h.descriptor_idx + 1, message->h.nargs);
    }
    else
    {
      status = LibINSVIRMEM;
    }
  }
  if (STATUS_OK)
  {
    // CALL EXECUTE MESSAGE //
    if (message->h.descriptor_idx == (message->h.nargs - 1))
    {
      MDSDBG("%d Call %3d (%2d/%2d)",
             connection->id, message->h.message_id, message->h.descriptor_idx + 1, message->h.nargs);
      int freed_message = execute_message(connection, message);
      UnlockConnection(connection);
      return freed_message;
    }
    free(message);
    return TRUE;
  }
  else
  {
    return return_status(connection, message, status);
  }
}

/// returns true if message cleanup is handled
static inline int mdsio_open_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
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
  return send_response(connection, message, 3, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_close_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
  int fd = mdsio->close.fd;
  int ans_o = MDS_IO_CLOSE(fd);
  struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
  return send_response(connection, message, 1, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_lseek_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
  int fd = mdsio->lseek.fd;
  int64_t offset = mdsio->lseek.offset;
  SWAP_INT_IF_BIGENDIAN(&offset);
  int whence = mdsio->lseek.whence;
  int64_t ans_o = MDS_IO_LSEEK(fd, offset, whence);
  struct descriptor ans_d = {8, DTYPE_Q, CLASS_S, 0};
  ans_d.pointer = (char *)&ans_o;
  SWAP_INT_IF_BIGENDIAN(ans_d.pointer);
  return send_response(connection, message, 1, (mdsdsc_t *)&ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_read_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
  int fd = mdsio->read.fd;
  size_t count = mdsio->read.count;
  void *buf = malloc(count);
  ssize_t nbytes = MDS_IO_READ(fd, buf, count);
#ifdef USE_PERF
  TreePerfRead(nbytes);
#endif
  int freed_message;
  if (nbytes > 0)
  {
    DESCRIPTOR_A(ans_d, 1, DTYPE_B, buf, nbytes);
    if ((size_t)nbytes != count)
      MDSWRN("READ_K wrong byte count");
    freed_message = send_response(connection, message, 1, (mdsdsc_t *)&ans_d);
  }
  else
  {
    DESCRIPTOR(ans_d, "");
    freed_message = send_response(connection, message, 1, (mdsdsc_t *)&ans_d);
  }
  free(buf);
  return freed_message;
}

/// returns true if message cleanup is handled
static inline int mdsio_write_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
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
    MDSWRN("WRITE_K wrong byte count");
  return send_response(connection, message, 1, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_lock_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
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
  return send_response(connection, message, deleted ? 3 : 1, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_exists_k(Connection *connection, Message *message)
{
  char *filename = message->bytes;
  int ans_o = MDS_IO_EXISTS(filename);
  struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
  return send_response(connection, message, 1, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_remove_k(Connection *connection, Message *message)
{
  char *filename = message->bytes;
  int ans_o = MDS_IO_REMOVE(filename);
  struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
  return send_response(connection, message, 1, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_rename_k(Connection *connection, Message *message)
{
  char *old = message->bytes;
  char *new = message->bytes + strlen(old) + 1;
  int ans_o = MDS_IO_RENAME(old, new);
  struct descriptor ans_d = {4, DTYPE_L, CLASS_S, (char *)&ans_o};
  return send_response(connection, message, 1, &ans_d);
}

/// returns true if message cleanup is handled
static inline int mdsio_read_x_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
  int fd = mdsio->read_x.fd;
  off_t offset = mdsio->read_x.offset;
  SWAP_INT_IF_BIGENDIAN(&offset);
  size_t count = mdsio->read_x.count;
  void *buf = malloc(count);
  int deleted;
  size_t nbytes = MDS_IO_READ_X(fd, offset, buf, count, &deleted);
  int freed_message;
  if (nbytes > 0)
  {
    DESCRIPTOR_A(ans_d, 1, DTYPE_B, buf, nbytes);
    if ((size_t)nbytes != count)
      MDSWRN("READ_X_K wrong byte count");
    freed_message = send_response(connection, message, deleted ? 3 : 1, (mdsdsc_t *)&ans_d);
  }
  else
  {
    DESCRIPTOR(ans_d, "");
    freed_message = send_response(connection, message, deleted ? 3 : 1, (mdsdsc_t *)&ans_d);
  }
  free(buf);
  return freed_message;
}

/// returns true if message cleanup is handled
static inline int mdsio_open_one_k(Connection *connection, Message *message)
{
  const mdsio_t *mdsio = (mdsio_t *)message->h.dims;
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
  if (fullpath)
  {
    memcpy(msg + 8, fullpath, msglen - 8);
    free(fullpath);
  }
  int freed_message = send_response(connection, message, 3, (mdsdsc_t *)&ans_d);
  free(msg);
  return freed_message;
}

/// returns true if message cleanup is handled
static int mdsio_command(Connection *connection, Message *message)
{
  connection->client_type = message->h.client_type;
  switch (message->h.descriptor_idx)
  {
  case MDS_IO_OPEN_K:
    return mdsio_open_k(connection, message);
  case MDS_IO_CLOSE_K:
    return mdsio_close_k(connection, message);
  case MDS_IO_LSEEK_K:
    return mdsio_lseek_k(connection, message);
  case MDS_IO_READ_K:
    return mdsio_read_k(connection, message);
  case MDS_IO_WRITE_K:
    return mdsio_write_k(connection, message);
  case MDS_IO_LOCK_K:
    return mdsio_lock_k(connection, message);
  case MDS_IO_EXISTS_K:
    return mdsio_exists_k(connection, message);
  case MDS_IO_REMOVE_K:
    return mdsio_remove_k(connection, message);
  case MDS_IO_RENAME_K:
    return mdsio_rename_k(connection, message);
  case MDS_IO_READ_X_K:
    return mdsio_read_x_k(connection, message);
  case MDS_IO_OPEN_ONE_K:
    return mdsio_open_one_k(connection, message);
  default:
    return return_status(connection, message, 0);
  }
}

//#define THREADED_IO
#ifdef THREADED_IO
struct command
{
  int (*method)(Connection *, Message *);
  Connection *connection;
  Message *message;
};

static void *thread_command(void *args)
{
  struct command *cm = (struct command *)args;
  if (!cm->method(cm->connection, cm->message))
    free(cm->message);
  return NULL;
}

/// returns true if message cleanup is handled
static int dispatch_command(
    int (*method)(Connection *, Message *),
    Connection *connection,
    Message *message)
{
  struct command *args = (struct command *)malloc(sizeof(struct command));
  if (args)
  {
    args->method = method;
    args->connection = connection;
    args->message = message;
    pthread_t thread;
    if ((errno = pthread_create(&thread, NULL, thread_command, (void *)args)))
      MDSERR("pthread_create");
    else if ((errno = pthread_detach(thread)))
      MDSERR("pthread_detach");
    else
      return TRUE;
  }
  return return_status(connection, message, MDSplusFATAL);
}
#endif

/// Handle message from server listen routine. A new descriptor instance is
/// created with the message buffer size and the message memory is copyed
/// inside. A proper conversion of memory structure is applied if neede for the
/// type of client connected.
///
/// \param connection the connection instance to handle
/// \param message the message to process
/// \return true if message cleanup is handled
///
int ProcessMessage(Connection *connection, Message *message)
{
  if (message->h.descriptor_idx < message->h.nargs)
  {
#ifdef THREADED_IO
    return dispatch_command(standard_command, connection, message);
#else
    return standard_command(connection, message);
#endif
  }
  else
  {
#ifdef THREADED_IO
    return dispatch_command(mdsio_command, connection, message);
#else
    return mdsio_command(connection, message);
#endif
  }
}
