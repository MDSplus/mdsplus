#ifdef linux
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64
#endif
#define _GNU_SOURCE /* glibc2 needs this */
#if defined(__sparc__)
#include "/usr/include/sys/types.h"
#elif !defined(HAVE_WINDOWS_H)
#include <sys/types.h>
#endif
#ifndef HAVE_VXWORKS_H
#include <config.h>
#endif
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include "cvtdef.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mds_stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#if (!defined(HAVE_WINDOWS_H) && !defined(HAVE_VXWORKS_H))
#include <pthread.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <taskLib.h>
#endif

#include "mdsip.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_RANDOM
#define O_RANDOM 0
#endif

#define MDS_IO_OPEN_K   1
#define MDS_IO_CLOSE_K  2
#define MDS_IO_LSEEK_K  3
#define MDS_IO_READ_K   4
#define MDS_IO_WRITE_K  5
#define MDS_IO_LOCK_K   6
#define MDS_IO_EXISTS_K 7
#define MDS_IO_REMOVE_K 8
#define MDS_IO_RENAME_K 9

#define ___max(a,b) (((a) > (b)) ? (a) : (b))
#define ___min(a,b) (((a) < (b)) ? (a) : (b))
#define MakeDesc(name) memcpy(malloc(sizeof(name)),&name,sizeof(name))

extern int TdiExecute();
extern int TdiSaveContext();
extern int TdiRestoreContext();
extern int TdiDebug();
extern int TdiData();

typedef ARRAY_COEFF(char,7) ARRAY_7;

static void ClientEventAst(MdsEventList *e, int data_len, char *data);

static void ResetErrors()
{
  static int four = 4;
  static DESCRIPTOR_LONG(clear_messages,&four);
  static DESCRIPTOR(messages,"");
  TdiDebug(&clear_messages,&messages MDS_END_ARG);
}

static void GetErrorText(int status, struct descriptor_xd *xd)
{
  static int one = 1;
  static DESCRIPTOR_LONG(get_messages,&one);
  TdiDebug(&get_messages,xd MDS_END_ARG);
  if (!xd->length)
  {
    static DESCRIPTOR(unknown,"unknown error occured");
    struct descriptor message = {0, DTYPE_T, CLASS_S, 0};
    if ((message.pointer = MdsGetMsg(status)) != NULL)
    {
	message.length = strlen(message.pointer);
        MdsCopyDxXd(&message,xd);
    }
    else
      MdsCopyDxXd((struct descriptor *)&unknown,xd);
  }
}

static void FreeDescriptors(mdsip_client_t *c)
{
  int i;
  for (i=0;i<MAX_ARGS;i++)
  {
    if (c->descrip[i])
    {
      if (c->descrip[i] != MdsEND_ARG)
      {
        if (c->descrip[i]->pointer)
          free(c->descrip[i]->pointer);
        free(c->descrip[i]);
      }
      c->descrip[i] = NULL;
    }
  }
}

static void ConvertFloat(int num, int in_type, char in_length, char *in_ptr, int out_type, char out_length, char *out_ptr)
{
  int i;
  char *in_p;
  char *out_p;
  for (i=0,in_p=in_ptr,out_p=out_ptr;i<num;i++,in_p += in_length, out_p += out_length)
  {
    char *ptr = in_p;
    char cray_f[8];
    if (in_type == CvtCRAY)
    {
      int j,k;
      for (j=0;j<2;j++)
        for (k=0;k<4;k++)
#ifdef _big_endian
          cray_f[j * 4 + k] = ptr[j * 4 + 3 - k];
#else
          cray_f[(1-j)*4 + k] = ptr[j * 4 + 3 - k];
#endif
      ptr = cray_f;
    }
    CvtConvertFloat(ptr, in_type, out_p, out_type, 0);
#ifdef _big_endian
    if (out_type == CvtCRAY)
    {
      int j,k;
      ptr = out_p;
      for (j=0;j<2;j++)
        for (k=0;k<4;k++)
          cray_f[j * 4 + k] = ptr[j * 4 + 3 - k];
      for (j=0;j<8;j++) ptr[j] = cray_f[j];
    }
#endif
  }
}

static void ConvertBinary(int num, int sign_extend, short in_length, char *in_ptr, short out_length, char *out_ptr)
{
  int i;
  int j;
  signed char *in_p = (signed char *)in_ptr;
  signed char *out_p = (signed char *)out_ptr;
  short min_len = ___min(out_length,in_length);
  for (i=0; i<num; i++,in_p += in_length, out_p += out_length)
  {
    for (j=0;j < min_len; j++) 
      out_p[j] = in_p[j];
    for (;j < out_length; j++)
      out_p[j] = sign_extend ? (in_p[min_len-1] < 0 ? -1 : 0) : 0;
  }
}



static void SendResponse(void *io_handle, mdsip_client_t *c, int status, struct descriptor *d)
{
  mdsip_message_t *m = 0;
  int nbytes = (d->class == CLASS_S) ? d->length : ((ARRAY_7 *)d)->arsize;
  int num = nbytes/___max(1,d->length);
  short length = d->length;
  if (CType(c->client_type) == CRAY_CLIENT)
  {
    switch (d->dtype)
    {
        case DTYPE_USHORT: 
        case DTYPE_ULONG:
        case DTYPE_SHORT:
        case DTYPE_LONG:
        case DTYPE_F:
        case DTYPE_FS: length = 8; break;
        case DTYPE_FC: 
        case DTYPE_FSC:
        case DTYPE_D: 
        case DTYPE_G: 
        case DTYPE_FT: length = 16; break;
        default: length = d->length; break;
    }
    nbytes = num * length;
  }
  else if (CType(c->client_type) == CRAY_IEEE_CLIENT)
  {
    switch (d->dtype)
    {
        case DTYPE_USHORT: 
        case DTYPE_SHORT: length = 4; break;
        case DTYPE_ULONG:
        case DTYPE_LONG: length = 8; break;
        default: length = d->length; break;
    }
    nbytes = num * length;
  }
  m = malloc(sizeof(mdsip_message_header_t) + nbytes);
  m->h.msglen = sizeof(mdsip_message_header_t) + nbytes;
  m->h.client_type = c->client_type;
  m->h.message_id = c->message_id;
  m->h.status = status;
  m->h.dtype = d->dtype;
  m->h.length = length;
  if (d->class == CLASS_S)
    m->h.ndims = 0;
  else
  {
    int i;
    array_coeff *a = (array_coeff *)d;
    m->h.ndims = a->dimct;
    if (a->aflags.coeff)
      for (i=0;i<m->h.ndims && i<7;i++)
        m->h.dims[i] = a->m[i];
    else
      m->h.dims[0] = a->length ? a->arsize/a->length : 0;
    for (i=m->h.ndims; i<7; i++)
      m->h.dims[i] = 0;
  }
  switch (CType(c->client_type))
  {
    case IEEE_CLIENT:
    case JAVA_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_F:  ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, 
                                               CvtIEEE_S, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                               CvtIEEE_S, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FS: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: ConvertFloat(num * 2, CvtVAX_D, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: ConvertFloat(num * 2, CvtVAX_G, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FT: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    case CRAY_CLIENT:
      switch (d->dtype)
      {
      case DTYPE_USHORT:
      case DTYPE_ULONG: 
        ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes);
        break;
      case DTYPE_SHORT:
      case DTYPE_LONG:  
        ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_F:
        ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer,CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_FS: 
        ConvertFloat(num, CvtIEEE_S, (char)d->length, d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_FC: 
        ConvertFloat(num*2,CvtVAX_F, (char)(d->length/2), d->pointer,CvtCRAY,(char)(m->h.length/2), m->bytes); 
        break;
      case DTYPE_FSC: 
        ConvertFloat(num*2,CvtIEEE_S,(char)(d->length/2),d->pointer,CvtCRAY,(char)(m->h.length/2), m->bytes);
        break;
      case DTYPE_D:
        ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_G:
        ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      case DTYPE_FT: 
        ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, CvtCRAY, (char)m->h.length, m->bytes); 
        break;
      default: 
        memcpy(m->bytes,d->pointer,nbytes); 
        break;
      }
      break;
    case CRAY_IEEE_CLIENT:
      switch (d->dtype) 
      {
        case DTYPE_USHORT:
        case DTYPE_ULONG: ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes); break;
        case DTYPE_SHORT:
        case DTYPE_LONG:  ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes); break;
        case DTYPE_F:  ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer, 
                                               CvtIEEE_S, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                               CvtIEEE_S, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FS: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: ConvertFloat(num * 2, CvtVAX_D, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: ConvertFloat(num * 2, CvtVAX_G, (char)(d->length/2), d->pointer, 
                                              CvtIEEE_T, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FT: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    case VMSG_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_F:  memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: ConvertFloat(num * 2, CvtVAX_D, (char)(d->length/2), d->pointer, 
                                              CvtVAX_G, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: ConvertFloat(num * 2, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_G, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: ConvertFloat(num * 2, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    default:
      switch (d->dtype)
      {
        case DTYPE_F:  memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_D: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_DC: memcpy(m->bytes,d->pointer,nbytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_GC: ConvertFloat(num * 2, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_FLOAT; break;
        case DTYPE_FSC: ConvertFloat(num * 2, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX; break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_DOUBLE; break;
        case DTYPE_FTC: ConvertFloat(num * 2, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)(m->h.length/2), m->bytes); m->h.dtype = DTYPE_COMPLEX_DOUBLE; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
  }
  mdsip_write(io_handle, c, m, 0);
}


static void ExecuteMessage(void *io_handle, mdsip_client_t *c)
{
  int status=1;
  static EMPTYXD(emptyxd);
  struct descriptor_xd *xd;
  char *evname;
  static DESCRIPTOR(eventastreq,EVENTASTREQUEST);
  static DESCRIPTOR(eventcanreq,EVENTCANREQUEST);
  int java = CType(c->client_type) == JAVA_CLIENT;
  if (c->options->port_name == NULL)
  {
    if (!mdsip_become_user(c))
    {
      printf("Error becoming user, %s/%s\n",c->local_user,c->remote_user);
      globus_xio_register_close(io_handle,NULL,mdsip_close_cb,c);
      return;
    }
  }
  if (StrCompare(c->descrip[0],&eventastreq) == 0)
  {
    DESCRIPTOR_LONG(eventiddsc,0);
    MdsEventList *newe = (MdsEventList *)malloc(sizeof(MdsEventList));
    struct descriptor_a *info = (struct descriptor_a *)c->descrip[2];

    evname = malloc(c->descrip[1]->length + 1);
    memcpy(evname, c->descrip[1]->pointer, c->descrip[1]->length);
    evname[c->descrip[1]->length] = 0;
    newe->io_handle = io_handle;
    newe->client = c;
 /**/
        if (java)
        {
          newe->info = 0;
          newe->info_len = 0;
          newe->jeventid = *c->descrip[2]->pointer;
        }
        else
        {
          newe->info = (struct _eventinfo *)memcpy(malloc(info->arsize),info->pointer,info->arsize);
          newe->info_len = info->arsize;
          newe->info->eventid = newe->eventid;
        }
        newe->next = 0;
        if (!(status & 1))
        {
          free(newe->info);
          free(newe);
        }
        else
        {
          MdsEventList *e;
          if (c->event)
          {
            for(e=c->event;e->next;e=e->next);
            e->next = newe;
          }
          else
            c->event = newe;
        }
#ifndef HAVE_VXWORKS_H
    status = MDSEventAst(evname,(void (*)(void *,int,char *))ClientEventAst,newe,&newe->eventid);
#endif
    free(evname); 
    if (!java)
    {
      eventiddsc.pointer = (char *)&newe->eventid;
      SendResponse(io_handle,c,status,&eventiddsc);
    }
  }
  else if (StrCompare(c->descrip[0],&eventcanreq) == 0)
  {
    int eventid;
    DESCRIPTOR_LONG(eventiddsc,0);
    eventiddsc.pointer = (char *)&eventid;
    MdsEventList *e;
    MdsEventList **p;
    if (!java)
      eventid = *(int *)c->descrip[1]->pointer;
    if (c->event)
    {
      if (java)
	for(p=&c->event,e=c->event;e && (*c->descrip[1]->pointer != e->jeventid) ;p=&e->next,e=e->next);
      else
    	for(p=&c->event,e=c->event;e && (e->eventid != eventid) ;p=&e->next,e=e->next);
      if (e)
      {
	/**/
#ifndef HAVE_VXWORKS_H
	status = MDSEventCan(e->eventid);
#endif
	/**/
	*p = e->next;
	free(e);
      }
    }
    if (!java)
      SendResponse(io_handle,c,status,&eventiddsc);
  }
  else
  {
    void  *old_context=0;
    void *tdi_context[6]={0,0,0,0,0,0};
    EMPTYXD(ans);
    if (c->options->shared_ctx)
    {
      old_context = TreeSwitchDbid(c->tree);
      TdiSaveContext(tdi_context);
      TdiRestoreContext(c->tdicontext);
    }
    c->descrip[c->nargs++] = (struct descriptor *)(xd = (struct descriptor_xd *)memcpy(malloc(sizeof(emptyxd)),&emptyxd,sizeof(emptyxd)));
    c->descrip[c->nargs++] = MdsEND_ARG;
    MdsSetClientAddr(c->addr);
    ResetErrors();
    status = LibCallg(&c->nargs, TdiExecute);
    if (status & 1) status = TdiData(xd,&ans MDS_END_ARG);
    if (!(status & 1)) 
      GetErrorText(status,&ans);
    SendResponse(io_handle,c,status,ans.pointer);
    MdsFree1Dx(xd,NULL);
    MdsFree1Dx(&ans,NULL);
    if (c->options->shared_ctx)
    {
      TdiSaveContext(c->tdicontext);
      TdiRestoreContext(tdi_context);
      c->tree = TreeSwitchDbid(old_context);
    }
  }
  FreeDescriptors(c);
}



void mdsip_process_message(void *io_handle, mdsip_client_t *c, mdsip_message_t *message)
{
  if (c->message_id != message->h.message_id)
  {
    FreeDescriptors(c);
    if (message->h.nargs < MAX_ARGS-1)
    {
      c->message_id = message->h.message_id;
      c->nargs = message->h.nargs;
    }
    else
      return;
  }
  if (message->h.descriptor_idx < c->nargs)
  {
    struct descriptor *d = c->descrip[message->h.descriptor_idx];
    c->client_type = message->h.client_type;
    if (!d)
    {
      static short lengths[] = {0,0,1,2,4,8,1,2,4,8,4,8,8,16,0};
      switch (message->h.ndims)
      {
        static struct descriptor scalar = {0, 0, CLASS_S, 0};
        static DESCRIPTOR_A_COEFF(array_1,0,0,0,1,0);
        static DESCRIPTOR_A_COEFF(array_2,0,0,0,2,0);
        static DESCRIPTOR_A_COEFF(array_3,0,0,0,3,0);
        static DESCRIPTOR_A_COEFF(array_4,0,0,0,4,0);
        static DESCRIPTOR_A_COEFF(array_5,0,0,0,5,0);
        static DESCRIPTOR_A_COEFF(array_6,0,0,0,6,0);
        static DESCRIPTOR_A_COEFF(array_7,0,0,0,7,0);
        case 0:  d = (struct descriptor *)MakeDesc(scalar);  break;
        case 1:  d = (struct descriptor *)MakeDesc(array_1); break;
        case 2:  d = (struct descriptor *)MakeDesc(array_2); break;
        case 3:  d = (struct descriptor *)MakeDesc(array_3); break;
        case 4:  d = (struct descriptor *)MakeDesc(array_4); break;
        case 5:  d = (struct descriptor *)MakeDesc(array_5); break;
        case 6:  d = (struct descriptor *)MakeDesc(array_6); break;
        case 7:  d = (struct descriptor *)MakeDesc(array_7); break;
      }
      d->length = message->h.dtype == DTYPE_CSTRING ? message->h.length : lengths[___min(message->h.dtype,13)];
      d->dtype = message->h.dtype;
      if (d->class == CLASS_A)
      {
        ARRAY_7 *a = (ARRAY_7 *)d;
        int num = 1;
        int i;
        for (i=0;i<message->h.ndims;i++)
        {
          a->m[i] = message->h.dims[i];
          num *= a->m[i];
        }
        a->arsize = a->length * num;
        a->pointer = a->a0 = malloc(a->arsize);
      }
      else
        d->pointer = d->length ? malloc(d->length) : 0;
      c->descrip[message->h.descriptor_idx] = d;
    }
    if (d)
    {
      int dbytes = d->class == CLASS_S ? (int)d->length : (int)((ARRAY_7 *)d)->arsize;
      int num = dbytes/___max(1,d->length);
      switch (CType(c->client_type))
      {
        case IEEE_CLIENT:
        case JAVA_CLIENT:
          memcpy(d->pointer,message->bytes,dbytes); break;
          break;
        case CRAY_IEEE_CLIENT:
          switch (d->dtype)
          {
            case DTYPE_USHORT:
            case DTYPE_ULONG: ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_SHORT:
            case DTYPE_LONG:  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer); break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
          break;
        case CRAY_CLIENT:
          switch (d->dtype)
          {
            case DTYPE_USHORT:
            case DTYPE_ULONG: ConvertBinary(num, 0, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_SHORT:
            case DTYPE_LONG:  ConvertBinary(num, 1, message->h.length, message->bytes, d->length, d->pointer); break;
            case DTYPE_FLOAT: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtIEEE_S, (char)d->length, d->pointer); break;
            case DTYPE_COMPLEX: ConvertFloat(num * 2, CvtCRAY, (char)(message->h.length/2), message->bytes, 
                                                  CvtIEEE_S, (char)(d->length/2), d->pointer); break;
            case DTYPE_DOUBLE: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtIEEE_T, sizeof(double), d->pointer); break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
          break;
        default:
          switch (d->dtype)
          {
            case DTYPE_FLOAT:   ConvertFloat(num, CvtVAX_F, (char)message->h.length, message->bytes,
                                                  CvtIEEE_S, sizeof(float), d->pointer); break;
            case DTYPE_COMPLEX: ConvertFloat(num * 2, CvtVAX_F, (char)message->h.length, message->bytes,
                                                  CvtIEEE_S, sizeof(float), d->pointer); break;
            case DTYPE_DOUBLE:  if (CType(c->client_type) == VMSG_CLIENT)
                                   ConvertFloat(num, CvtVAX_G, (char)message->h.length, message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer); 
                                else
                                   ConvertFloat(num, CvtVAX_D, (char)message->h.length, message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer);
                                break;

            case DTYPE_COMPLEX_DOUBLE: if (CType(c->client_type) == VMSG_CLIENT)
                                          ConvertFloat(num * 2, CvtVAX_G, (char)(message->h.length/2), message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer); 
                                       else
                                          ConvertFloat(num * 2, CvtVAX_D, (char)(message->h.length/2), message->bytes,
                                                  CvtIEEE_T, sizeof(double), d->pointer);
                          break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
      }
      switch (d->dtype)
      {
      case DTYPE_FLOAT: d->dtype = DTYPE_FS; break;
      case DTYPE_COMPLEX: d->dtype = DTYPE_FSC; break;
      case DTYPE_DOUBLE: d->dtype = DTYPE_FT; break;
      case DTYPE_COMPLEX_DOUBLE: d->dtype = DTYPE_FTC; break;
      }
      if (message->h.descriptor_idx == (message->h.nargs - 1))
      {
        ExecuteMessage(io_handle,c);
      }
    }
  }
  else
  {
    c->client_type=message->h.client_type;
    switch (message->h.descriptor_idx)
    {
    case MDS_IO_OPEN_K:
      {
        int fd;
        char *filename = (char *)message->bytes;
        char *ptr;
        int fopts = message->h.dims[1];
        mode_t mode = message->h.dims[2];
        DESCRIPTOR_LONG(fd_d,0);
        fd_d.pointer = (char *)&fd;
        if (O_CREAT == 0x0200) /* BSD */
	{
          if (fopts & 0100)
            fopts = (fopts & ~0100) | O_CREAT;
          if (fopts & 0200)
            fopts = (fopts & ~0200) | O_EXCL;
          if (fopts & 01000)
            fopts = (fopts & ~01000) | O_TRUNC;
        }
        fd = open(filename,fopts | O_BINARY | O_RANDOM,mode);
        if (fd == -1)
	{
          while (fd == -1 && ((ptr = index(filename,'\\')) != 0)) *ptr='/';
          fd = open(filename,fopts | O_BINARY | O_RANDOM,mode);
        }
#ifndef HAVE_WINDOWS_H
        if ((fd != -1) && ((fopts & O_CREAT) != 0)) {
          char *cmd=(char *)malloc(64+strlen(filename));
          sprintf(cmd,"SetMdsplusFileProtection %s 2> /dev/null",filename);
          system(cmd);
	  free(cmd);
	}
#endif
        SendResponse(io_handle,c, 1, (struct descriptor *)&fd_d);
	      break;
      }
    case MDS_IO_CLOSE_K:
      {
	      int stat = close(message->h.dims[1]);
	      DESCRIPTOR_LONG(stat_d,0);
        stat_d.pointer = (char *)&stat;
        SendResponse(io_handle,c, 1, (struct descriptor *)&stat_d);
	      break;
      }
    case MDS_IO_LSEEK_K:
      {
        int fd = message->h.dims[1];
        off_t offset;
        int whence = message->h.dims[4];
      	_int64 ans;
        struct descriptor ans_d = {8, DTYPE_Q, CLASS_S, 0};
#ifdef _big_endian
        int tmp;
        tmp = message->h.dims[2];
        message->h.dims[2] = message->h.dims[3];
        message->h.dims[3] = tmp;
#endif
        offset = (off_t)*(_int64 *)&message->h.dims[2];
        ans = (_int64)lseek(fd,offset,whence);
        ans_d.pointer = (char *)&ans;
        SendResponse(io_handle,c, 1, (struct descriptor *)&ans_d);
	      break;
      }
    case MDS_IO_READ_K:
      {
        int fd = message->h.dims[1];
        void *buf = malloc(message->h.dims[2]);
        size_t num = (size_t)message->h.dims[2];
      	ssize_t nbytes = read(fd,buf,num);
        if (nbytes > 0)
        {
          DESCRIPTOR_A(ans_d,1,DTYPE_B,0,0);
          ans_d.pointer=buf;
          ans_d.arsize=nbytes;
          SendResponse(io_handle,c,1,(struct descriptor *)&ans_d);
        }
        else
        { 
	        DESCRIPTOR(ans_d,"");
          SendResponse(io_handle,c,1,(struct descriptor *)&ans_d);
        }
        free(buf);
        break;
      }
    case MDS_IO_WRITE_K:
      {
        ssize_t nbytes = write(message->h.dims[1],message->bytes,(size_t)message->h.dims[0]);
        DESCRIPTOR_LONG(ans_d,0);
        ans_d.pointer = (char *)&nbytes;
        SendResponse(io_handle,c,1,(struct descriptor *)&ans_d);
	break;
      }
    case MDS_IO_LOCK_K:
      {
        int fd = message->h.dims[1];
        int status;
        _int64 offset;
        int size = message->h.dims[4];
        int mode_in = message->h.dims[5];
        int mode = mode_in & 0x3;
        int nowait = mode_in & 0x8;
        DESCRIPTOR_LONG(ans_d,0);
        offset = *(_int64 *)&(message->h.dims[2]);
#if defined (_WIN32)
        if (mode > 0)
          status = ( (LockFile((HANDLE)_get_osfhandle(fd), (int)offset, (int)(offset >> 32), size, 0) == 0) && 
                     (GetLastError() != ERROR_LOCK_VIOLATION) ) ? TreeFAILURE : TreeSUCCESS;
        else
          status = UnlockFile((HANDLE)_get_osfhandle(fd),(int)offset, (int)(offset >> 32), size, 0) == 0 ? TreeFAILURE : TreeSUCCESS;
#elif defined (HAVE_VXWORKS_H)
        status = TreeSUCCESS;
#else
        struct flock flock_info;
#ifdef _big_endian
        status = message->h.dims[2];
        message->h.dims[2] = message->h.dims[3];
        message->h.dims[3] = status;
#endif
        offset = *(_int64 *)&(message->h.dims[2]);
        flock_info.l_type = (mode == 0) ? F_UNLCK : ((mode == 1) ? F_RDLCK : F_WRLCK);
        flock_info.l_whence = (mode == 0) ? SEEK_SET : ((offset >= 0) ? SEEK_SET : SEEK_END);
        flock_info.l_start = (mode == 0) ? 0 : ((offset >= 0) ? offset : 0);
        flock_info.l_len = (mode == 0) ? 0 : size;
        status = (fcntl(fd,nowait ? F_SETLK : F_SETLKW, &flock_info) != -1) ? TreeSUCCESS : TreeLOCK_FAILURE;
#endif
        ans_d.pointer = (char *)&status;
        SendResponse(io_handle,c,1,(struct descriptor *)&ans_d);
        break;
      }
    case MDS_IO_EXISTS_K:
      { 
        struct stat statbuf;
        int status = (stat(message->bytes,&statbuf) == 0);
        DESCRIPTOR_LONG(status_d,0);
        status_d.pointer = (char *)&status;
        SendResponse(io_handle,c, 1, (struct descriptor *)&status_d);
        break;
      }
    case MDS_IO_REMOVE_K:
      { 
        int status = remove(message->bytes);
        DESCRIPTOR_LONG(status_d,0);
        status_d.pointer = (char *)&status;
        SendResponse(io_handle,c, 1, (struct descriptor *)&status_d);
        break;
      }
    case MDS_IO_RENAME_K:
      {
        DESCRIPTOR_LONG(status_d,0);
#ifdef HAVE_VXWORKS_H
        int status = copy(message->bytes,message->bytes+strlen(message->bytes)+1);
        if (status == 0)
        {
          status = remove(message->bytes);
          if (status != 0)
            remove(message->bytes+strlen(message->bytes)+1);
        }
#else
        int status = rename(message->bytes,message->bytes+strlen(message->bytes)+1);
#endif
        status_d.pointer = (char *)&status;
        SendResponse(io_handle,c, 1, (struct descriptor *)&status_d);
	break;
      }
    }
  }
}

static void ClientEventAst(MdsEventList *e, int data_len, char *data)
{
    int i;
    if (CType(e->client->client_type) == JAVA_CLIENT)
    {
      JMdsEventInfo *info;
      int len = sizeof(mdsip_message_header_t) + sizeof(JMdsEventInfo);
      mdsip_message_t *m = malloc(len);
      m->h.ndims = 0;
      m->h.client_type = e->client->client_type;
      m->h.msglen = len;
      m->h.dtype = DTYPE_EVENT_NOTIFY;
      info = (JMdsEventInfo *)m->bytes;
      memcpy(info->data, data, (data_len<12)?data_len:12);
      for(i = data_len; i < 12; i++)
        info->data[i] = 0;
      info->eventid = e->jeventid;
      mdsip_write(e->io_handle, e->client, m, 0);
    }
    else
    {
      mdsip_message_t *m = malloc(sizeof(mdsip_message_header_t) + e->info_len);
      m->h.ndims = 0;
      m->h.client_type = e->client->client_type;
      m->h.msglen = sizeof(mdsip_message_header_t) + e->info_len;
      m->h.dtype = DTYPE_EVENT_NOTIFY;
      memcpy(e->info->data, data, (data_len<12)?data_len:12); 
      for(i = data_len; i < 12; i++)
        e->info->data[i] = 0;
      memcpy(m->bytes,e->info,e->info_len);
      mdsip_write(e->io_handle, e->client, m, /* 1 */ 0);
    }
}
