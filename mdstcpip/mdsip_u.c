#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include "../tdishr/cvtdef.h"
#undef DTYPE_FLOAT
#undef DTYPE_DOUBLE
#undef DTYPE_EVENT
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mds_stdarg.h>
#ifdef THREADS
#include <pthread.h>
#endif
#include "mdsip.h"
extern char *ctime();
#define MAX_ARGS 256
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

typedef ARRAY_COEFF(char,7) ARRAY_7;

typedef struct _context { void *tree;
                        } Context;

typedef struct _client { SOCKET sock;
                         Context context;
                         unsigned char message_id;
                         int client_type;
                         int nargs;
                         struct descriptor *descrip[MAX_ARGS];
                         MdsEventList *event;
			 void         *tdicontext[6];
                         struct _client *next;
                       } Client;

static Client *ClientList = 0;
static fd_set fdactive;
static int multi;

extern int TdiSaveContext();
extern int TdiRestoreContext();
extern int TdiExecute();
extern int TdiData();
extern int TdiDebug();
extern int TdiResetPublic();
extern int TdiResetPrivate();

static int ConnectToInet();
static int CreateMdsPort(char *service, int multi);
static void AddClient(int socket, struct sockaddr_in *sin);
static void RemoveClient(Client *c);
static void ProcessMessage(Client *c, Message *message);
static void ExecuteMessage(Client *c);
static void SendResponse(Client *c,int status, struct descriptor *xd);
static int DoMessage(Client *c);
static void ConvertFloat(int nbytes, int in_type, char in_length, char *in_ptr, int out_type, char out_length, char *out_ptr);
extern Message *GetMdsMsg(int sock, int *status);
extern int SendMdsMsg(int sock, Message *m, int oob);
void GetErrorText(int status, struct descriptor_xd *xd);
void ResetErrors();

static int zero = 0;
static int one = 1;
static int two = 2;

#define MakeDesc(name) memcpy(malloc(sizeof(name)),&name,sizeof(name))

int main(int argc, char **argv)
{
  static struct sockaddr_in sin;
  int sock;
  int shut = 0;
  int tablesize = FD_SETSIZE;
  fd_set readfds;
#ifdef _WIN32
  WSADATA wsaData;
  WORD wVersionRequested;
  wVersionRequested = MAKEWORD(1,1);
  WSAStartup(wVersionRequested,&wsaData);
#endif
  multi = argc > 2;  
  FD_ZERO(&fdactive);
  if (multi)
    sock = CreateMdsPort(argv[1],1);
  else
  {
    sock = ConnectToInet(argv[1]);
    shut = (ClientList == NULL);
  }    
  readfds = fdactive;
  while (!shut && (select(tablesize, &readfds, 0, 0, 0) > 0))
  {
    if (FD_ISSET(sock, &readfds))
    {
      int len = sizeof(struct sockaddr_in);
      AddClient(accept(sock, (struct sockaddr *)&sin, &len),&sin);
    }
    else
    {
      Client *c,*next;
      for (c=ClientList,next=c ? c->next : 0; c; c=next,next=c ? c->next : 0)
        if (FD_ISSET(c->sock, &readfds))
          DoMessage(c);
    }
    shut = (ClientList == NULL) && !multi;
    readfds = fdactive;
  }
  shutdown(sock,2);
  close(sock);
  return 1;
}

static int DoMessage(Client *c)
{
  int status;
  Message *msgptr = GetMdsMsg(c->sock,&status);
  if (status & 1)
  {
    send(c->sock, 0, 0, 0);
    ProcessMessage(c, msgptr);
  }
  else
    RemoveClient(c);
  return status;
}

static void CompressString(struct descriptor *in)
{
  unsigned short len;
  StrTrim(in,in,&len);
  StrUpcase(in,in);
  while(in->length && (in->pointer[0] == ' ' || in->pointer[0] == '	'))
    StrRight(in,in,&two);
}
  
static int CheckClient(char *host_c, char *user_c)
{
  FILE *f;
  int ok = 0;
#if defined(_WIN32)
  f = fopen("C:\\mdsip.hosts","r");
#else
  f = fopen("/etc/mdsip.hosts","r");
#endif
  if (f)
  {
    static char line_c[256];
    static char match_c[256];
    static struct descriptor line_d = {0, DTYPE_T, CLASS_S, line_c};
    static struct descriptor match_d = {0, DTYPE_T, CLASS_S, match_c};
    static struct descriptor match = {0, DTYPE_T, CLASS_D, 0};
    static struct descriptor local_user = {0,DTYPE_T,CLASS_D,0};
    static struct descriptor access_id = {0,DTYPE_T,CLASS_D,0};
    static DESCRIPTOR(delimiter,"|");
    if (multi)
    {
      strcpy(match_c,"MULTI");
    }
    else
    {
      strncpy(match_c,user_c,255);
      strncat(match_c,"@",255);
      strncat(match_c,host_c,255);
    }
    match_d.length = strlen(match_c);
    StrUpcase(&match,&match_d);
    while (ok==0 && fgets(line_c,255,f))
    {
      if (line_c[0] != '#')
      {
        line_d.length = strlen(line_c) - 1;
	StrElement(&access_id,&zero,&delimiter,&line_d);
        StrElement(&local_user,&one,&delimiter,&line_d);
        CompressString(&access_id);
        CompressString(&local_user);
        if (access_id.length)
        {
          if (access_id.pointer[0] != '!')
          {
            if (StrMatchWild(&match,&access_id) & 1)
            {
	      /*
              if (local_user.length)
              {
  	        int persona;
	        int status;
	        status = sys$persona_create(&persona,&local_user,0);
                if (status & 1)
                {
                  status = sys$persona_assume(&persona,IMP$M_ASSUME_SECURITY | IMP$M_ASSUME_ACCOUNT | IMP$M_ASSUME_JOB_WIDE);
                  ok = (status & 1) ? 1 : 2;
                }
              }
              else
	      */
                ok = 1;
            }
          }
          else
          {
            StrRight(&access_id,&access_id,&two);
            if (StrMatchWild(&match,&access_id) & 1)
              ok = 2;
          }
        }
      }
    }
    fclose(f);
  }
  else
  {
    printf("Unable to open mdsip.hosts file\n");
    exit(1);
  }
  return ok;
}

static void AddClient(int sock,struct sockaddr_in *sin)
{
  if (sock > 0)
  {
    static Message m;
    Message *m_user;
    char *timestr;
    char *user = 0;
    char *user_p = 0;
    struct hostent *hp;
    int i;
    int status;
    int ok = 0;
    Client *c;
    time_t tim;
    m.h.msglen = sizeof(MsgHdr);
    hp = gethostbyaddr((char *)&sin->sin_addr,sizeof(sin->sin_addr),AF_INET);
    m_user = GetMdsMsg(sock,&status);
    if ((status & 1) && (m_user) && (m_user->h.dtype == DTYPE_CSTRING))
    {
      user = malloc(m_user->h.length + 1);
      memcpy(user,m_user->bytes,m_user->h.length);
      user[m_user->h.length] = 0;
    }
    user_p = user ? user : "?";
    tim = time(0);
    timestr = ctime(&tim);
    timestr[strlen(timestr)-1] = 0;
    if (hp)
      printf("%s (%d) Connection received from %s@%s [%s]\r\n", timestr,sock, user_p, hp->h_name, inet_ntoa(sin->sin_addr));
    else
      printf("%s (%d) Connection received from %s@%s\r\n", timestr, sock, user_p, inet_ntoa(sin->sin_addr));
    if (!multi)
    {
      if (hp) ok = CheckClient(hp->h_name,user_p);
      if (ok == 0) ok = CheckClient((char *)inet_ntoa(sin->sin_addr),user_p);
    }
    else
      ok = 1;
    if (user) free(user);
    m.h.status = ok & 1;
    SendMdsMsg(sock,&m,0);
    if (m.h.status)
    {
      Client *new = malloc(sizeof(Client));
      FD_SET(sock,&fdactive);
      new->sock = sock;
      new->context.tree = 0;
      new->message_id = 0;
      new->event = 0;
      for (i=0;i<6;i++)
	new->tdicontext[i] = NULL;
      for (i=0;i<MAX_ARGS;i++)
        new->descrip[i] = NULL;
      new->next = NULL;
      for (c=ClientList; c && c->next; c = c->next);
      if (c) 
        c->next = new;
      else
        ClientList = new;
    }
    else
    {
      printf("Access denied\n");
      shutdown(sock,2);
      close(sock);
    }
  }
}

static void FreeDescriptors(Client *c)
{
  int i;
  for (i=0;i<MAX_ARGS;i++)
  {
    if (c->descrip[i] && c->descrip[i] != MdsEND_ARG)
    {
      if (c->descrip[i]->pointer)
        free(c->descrip[i]->pointer);
      free(c->descrip[i]);
      c->descrip[i] = NULL;
    }
  }
}

static void RemoveClient(Client *c)
{
  Client *p,*nc;
  MdsEventList *e,*nexte;
  FD_CLR(c->sock,&fdactive);
  shutdown(c->sock,2);
  closesocket(c->sock);
  FreeDescriptors(c);
  for (e=c->event; e; e=nexte)
  {
    nexte = e->next;
    /*
    MdsEventCan(e->eventid);
    */
    if (e->info_len > 0) free(e->info);
    free(e);
  }
  if (c->context.tree)
  {
    void  *old_context;
    void *tdi_context[6];
    old_context = TreeSwitchDbid(c->context.tree);
    TreeClose(0,0);
    c->context.tree = TreeSwitchDbid(old_context);
    TdiSaveContext(tdi_context);
    TdiRestoreContext(c->tdicontext);
    TdiResetPublic(MdsEND_ARG);
    TdiResetPrivate(MdsEND_ARG);
    TdiRestoreContext(tdi_context);
  }
  for (p=0,nc=ClientList; nc && nc!=c; p=nc,nc=nc->next);
  if (nc == c)
  {
    if (p)
      p->next = c->next;
    else
      ClientList = c->next;
  }
  free(c);
}

static void ConvertFloat(int num, int in_type, char in_length, char *in_ptr, int out_type, char out_length, char *out_ptr)
{
  int i;
  char *in_p;
  char *out_p;
  for (i=0,in_p=in_ptr,out_p=out_ptr;i<num;i++,in_p += in_length, out_p += out_length)
  {
/*
    if ((in_length == 4) && (*(int *)in_p == 32768) && (out_length == 4))
      *(int *)out_p = 32768;
    else
*/
      CvtConvertFloat(in_p, in_type, out_p, out_type, 0);
  }
}

static void ConvertBinary(int num, int sign_extend, short in_length, char *in_ptr, short out_length, char *out_ptr)
{
  int i;
  int j;
  signed char *in_p = (signed char *)in_ptr;
  signed char *out_p = (signed char *)out_ptr;
  short min_len = min(out_length,in_length);
  for (i=0; i<num; i++,in_p += in_length, out_p += out_length)
  {
    for (j=0;j < min_len; j++) 
      out_p[j] = in_p[j];
    for (;j < out_length; j++)
      out_p[j] = sign_extend ? (in_p[min_len-1] < 0 ? -1 : 0) : 0;
  }
}

static void ProcessMessage(Client *c, Message *message)
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
    int idx = message->h.descriptor_idx;
    c->client_type = message->h.client_type;
    if (!d)
    {
      static short lengths[] = {0,0,1,2,4,0,1,2,4,0,4,8,8,0,0};
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
      d->length = message->h.dtype == DTYPE_CSTRING ? message->h.length : lengths[min(message->h.dtype,12)];
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
        d->pointer = malloc(d->length);
      c->descrip[message->h.descriptor_idx] = d;
    }
    if (d)
    {
      int dbytes = d->class == CLASS_S ? (int)d->length : (int)((ARRAY_7 *)d)->arsize;
      int num = dbytes/max(1,d->length);
      switch (CType(c->client_type))
      {
        case IEEE_CLIENT:
        case JAVA_CLIENT:
          switch (d->dtype)
          {
            case DTYPE_F:
            case DTYPE_FC: ConvertFloat(num, CvtIEEE_S, (char)message->h.length, message->bytes, 
                                                  CvtVAX_F, sizeof(float), d->pointer); break;
            case DTYPE_D: ConvertFloat(num, CvtIEEE_T, (char)message->h.length, message->bytes, 
                                                  CvtVAX_D, sizeof(double), d->pointer); break;
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
            case DTYPE_F: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtVAX_F, (char)d->length, d->pointer); break;
            case DTYPE_FC: ConvertFloat(num * 2, CvtCRAY, (char)(message->h.length/2), message->bytes, 
                                                  CvtVAX_F, (char)(d->length/2), d->pointer); break;
            case DTYPE_D: ConvertFloat(num, CvtCRAY, (char)message->h.length, message->bytes, 
                                                  CvtVAX_D, sizeof(double), d->pointer); break;
            default: memcpy(d->pointer,message->bytes,dbytes); break;
          }
          break;
        default:
         if((d->dtype == DTYPE_D) && (CType(c->client_type) == VMSG_CLIENT))
          ConvertFloat(num, CvtVAX_G, (char)message->h.length, message->bytes, 
                            CvtVAX_D, sizeof(double), d->pointer);
         else
	 memcpy(d->pointer,message->bytes,dbytes);
      }
      if (message->h.descriptor_idx == (message->h.nargs - 1))
      {
#ifdef THREADS
        pthread_t thread;
        pthread_create(&thread,pthread_attr_default,(pthread_startroutine_t)ExecuteMessage,(pthread_addr_t)c);
#else
        ExecuteMessage(c);
#endif
      }
    }
  }
}

static void ClientEventAst(MdsEventList *e)
{
  Client *c;
  for (c=ClientList; c && (c->sock != e->sock); c = c->next);
  if (c)
  {
    if (CType(c->client_type) == JAVA_CLIENT)
    {
      JMdsEventInfo *info;
      int len = sizeof(MsgHdr) + sizeof(JMdsEventInfo);
      Message *m = malloc(len);
      m->h.msglen = len;
      m->h.dtype = DTYPE_EVENT;
      info = (JMdsEventInfo *)m->bytes;
      /*
      MdsGetEvi(e->eventid, info->data, 0);
      */
      info->eventid = e->jeventid;
      SendMdsMsg(c->sock, m, 0);
      free(m);
    }
    else
    {
      Message *m = malloc(sizeof(MsgHdr) + e->info_len);
      m->h.msglen = sizeof(MsgHdr) + e->info_len;
      m->h.dtype = DTYPE_EVENT;
      /*
      MdsGetEvi(e->eventid, e->info->data, 0);
      */
      memcpy(m->bytes,e->info,e->info_len);
      SendMdsMsg(c->sock, m, MSG_OOB);
      free(m);
    }
  }
  else
  {
    /*
    MdsEventCan(e->eventid);
    */
    if (e->info_len > 0) free(e->info);
    free(e);
  }
}

static void ExecuteMessage(Client *c)
{
  int status;
  static EMPTYXD(emptyxd);
  struct descriptor_xd *xd;
  static DESCRIPTOR(eventastreq,EVENTASTREQUEST);
  static DESCRIPTOR(eventcanreq,EVENTCANREQUEST);
  int java = CType(c->client_type) == JAVA_CLIENT;
  if (StrCompare(c->descrip[0],&eventastreq) == 0)
  {
    static int eventid;
    static DESCRIPTOR_LONG(eventiddsc,&eventid);
    MdsEventList *newe = (MdsEventList *)malloc(sizeof(MdsEventList));
    struct descriptor_a *info = (struct descriptor_a *)c->descrip[2];
    newe->sock = c->sock;
    /*
    status = MdsEventAst(c->descrip[1].pointer,(void (*)())ClientEventAst,newe,&newe->eventid);
    */
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
    if (!java)
      SendResponse(c,status,&eventiddsc);
  }
  else if (StrCompare(c->descrip[0],&eventcanreq) == 0)
  {
    static int eventid;
    static DESCRIPTOR_LONG(eventiddsc,&eventid);
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
	/*
        MdsEventCan((int *)e->eventid);
	*/
        free(e);
        *p = e->next;
      }
    }
    if (!java)
      SendResponse(c,status,&eventiddsc);
  }
  else
  {
    void  *old_context;
    void *tdi_context[6];
	EMPTYXD(ans);
    old_context = TreeSwitchDbid(c->context.tree);
    TdiSaveContext(tdi_context);
    TdiRestoreContext(c->tdicontext);
    c->descrip[c->nargs++] = (struct descriptor *)(xd = (struct descriptor_xd *)memcpy(malloc(sizeof(emptyxd)),&emptyxd,sizeof(emptyxd)));
    c->descrip[c->nargs++] = MdsEND_ARG;
    ResetErrors();
    status = LibCallg(&c->nargs, TdiExecute);
    if (status & 1) status = TdiData(xd,&ans MDS_END_ARG);
    if (!(status & 1)) GetErrorText(status,&ans);
    SendResponse(c,status,ans.pointer);
    MdsFree1Dx(xd,NULL);
	MdsFree1Dx(&ans,NULL);
    TdiSaveContext(c->tdicontext);
    TdiRestoreContext(tdi_context);
    c->context.tree = TreeSwitchDbid(old_context);
  }
  FreeDescriptors(c);
}

static void SendResponse(Client *c, int status, struct descriptor *d)
{
  static Message *m = 0;
  int flag = 0;
  int nbytes = (d->class == CLASS_S) ? d->length : ((ARRAY_7 *)d)->arsize;
  int num = nbytes/max(1,d->length);
  short length = d->length;
  if (CType(c->client_type) == CRAY_CLIENT && d->dtype <= DTYPE_FC)
  {
    short lengths[] = {0,0,1,8,8,0,1,8,8,0,8,8,16};
    length = lengths[d->dtype];
    nbytes = num * length;
  }
  m = malloc(sizeof(MsgHdr) + nbytes);
  m->h.msglen = sizeof(MsgHdr) + nbytes;
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
                                               CvtIEEE_S, (char)m->h.length, m->bytes); break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                               CvtIEEE_S, (char)(m->h.length/2), m->bytes); break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, (char)d->length, d->pointer, 
                                              CvtIEEE_T, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    case CRAY_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_USHORT:
        case DTYPE_ULONG: ConvertBinary(num, 0, d->length, d->pointer, m->h.length, m->bytes); break;
        case DTYPE_SHORT:
        case DTYPE_LONG:  ConvertBinary(num, 1, (char)d->length, d->pointer, (char)m->h.length, m->bytes); break;
        case DTYPE_F: ConvertFloat(num, CvtVAX_F, (char)d->length, d->pointer,
                                            CvtCRAY, (char)m->h.length, m->bytes); break;
        case DTYPE_FC: ConvertFloat(num * 2, CvtVAX_F, (char)(d->length/2), d->pointer, 
                                                  CvtCRAY, (char)(m->h.length/2), m->bytes); break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, 
                                              CvtCRAY, (char)m->h.length, m->bytes); break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtCRAY, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    case VMSG_CLIENT:
      switch (d->dtype)
      {
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)m->h.length, m->bytes); break;
        case DTYPE_D: ConvertFloat(num, CvtVAX_D, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_G, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
    default:
      switch (d->dtype)
      {
        case DTYPE_FS: ConvertFloat(num, CvtIEEE_S, sizeof(float), d->pointer, 
                                              CvtVAX_F, (char)m->h.length, m->bytes); break;
        case DTYPE_G: ConvertFloat(num, CvtVAX_G, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); break;
        case DTYPE_FT: ConvertFloat(num, CvtIEEE_T, sizeof(double), d->pointer, 
                                              CvtVAX_D, (char)m->h.length, m->bytes); m->h.dtype = DTYPE_D; break;
        default: memcpy(m->bytes,d->pointer,nbytes); break;
      }
      break;
  }
  if (m->h.dtype == DTYPE_G || m->h.dtype == DTYPE_FT)
    m->h.dtype = DTYPE_D;
  else if (m->h.dtype == DTYPE_FS)
    m->h.dtype = DTYPE_F;
  SendMdsMsg(c->sock, m, 0);
  free(m);
}

static int CreateMdsPort(char *service, int multi_in)
{
  static struct sockaddr_in sin;
  struct servent *sp;
  long sendbuf=32768,recvbuf=32768;
  int s;
  int status;
  if (multi)
    CheckClient(0,0);
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    printf("Error getting Connection Socket\n");
    return 0;
  }
  FD_SET(s,&fdactive);
  sp = getservbyname(service,"tcp");
  if (sp == NULL)
  {
    printf("unknown service: %s/tcp\n",service);
    exit(1);
  }
  setsockopt(s, SOL_SOCKET,SO_RCVBUF,(char *)&recvbuf,sizeof(long));
  setsockopt(s, SOL_SOCKET,SO_SNDBUF,(char *)&sendbuf,sizeof(long));  
  status = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&multi_in,sizeof(1));
  if (status < 0)
  {
    printf("Error setting socket options\n");
    exit(1);
  }
  if (multi_in)
  {
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = sp->s_port;
    status = bind(s, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if (status < 0)
    {
      perror("Error binding to service\n");
      exit(1);
    }
  }
  status = listen(s,5);
  if (status < 0)
  {
    printf("Error from listen\n");
    exit(1);
  }
  return s;
}

static int ConnectToInet(char *service)
{
  static struct sockaddr_in sin;
  int s=0;
  int n = sizeof(sin);
  int status = 1; /* sys$assign(&INET, &s, 0, 0); */
  if (!(status & 1)) { exit(status);}
  FD_SET(s,&fdactive);
  if ((status=getpeername(s, (struct sockaddr *)&sin, &n)) < 0)
  {
    perror("Error getting peer name");
    exit(0);
  }
  AddClient(s,&sin);
  return CreateMdsPort(service, 0);
}

void ResetErrors()
{
  static int four = 4;
  static DESCRIPTOR_LONG(clear_messages,&four);
  static DESCRIPTOR(messages,"");
  TdiDebug(&clear_messages,&messages MDS_END_ARG);
}

void GetErrorText(int status, struct descriptor_xd *xd)
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
