/*  CMS REPLACEMENT HISTORY, Element MDSIPSHR.C */
/*  *101  15-JUN-1998 15:49:03 TWF "Select buffer size" */
/*  *100  21-APR-1998 11:16:39 TWF "Zero length" */
/*  *99   17-APR-1998 10:08:48 TWF "Pass length to sendarg" */
/*  *98   14-APR-1998 14:17:43 TWF "fix negative bytes_remaining" */
/*  *97   14-APR-1998 11:58:43 TWF "Reduce maxdims and return length field" */
/*  *96    7-APR-1998 10:10:59 TWF "Support VAXG client (from epfl)" */
/*  *95   10-MAR-1998 09:20:53 TWF "Fix mdsvaluec for zero length strings" */
/*  *94    4-MAR-1998 10:25:10 TWF "Fix sending user info" */
/*  *93    3-MAR-1998 14:10:54 TWF "Add TCP_NODELY" */
/*  *92    3-MAR-1998 14:08:38 TWF "Add TCP_NODELY" */
/*  *91    3-MAR-1998 13:53:17 TWF "Add ack again" */
/*  *90    3-MAR-1998 13:35:14 TWF "Add ack again" */
/*  *89    3-MAR-1998 12:57:26 TWF "Try no oob's" */
/*  *88    3-MAR-1998 10:33:40 TWF "remove ack" */
/*  *87    3-MAR-1998 10:03:12 TWF "Add ack to SendMsg" */
/*  *86    3-MAR-1998 10:02:24 TWF "Add ack to SendMsg" */
/*  *85    3-MAR-1998 09:45:06 TWF "Update from GA" */
/*  *84    6-JAN-1998 09:29:46 TWF "Use larger window" */
/*  *83    6-JAN-1998 09:28:25 TWF "Use larger window" */
/*  *82   23-SEP-1997 14:29:56 TWF "Might not always get full header" */
/*  *81   23-SEP-1997 10:01:32 TWF "Avoid interrupts from IDL" */
/*  *80   19-AUG-1997 11:22:26 TWF "Fix selectstat" */
/*  *79   16-OCT-1995 13:24:54 TWF "Cleaner windows version" */
/*  *78   22-FEB-1995 15:27:42 TWF "Fix hpux problem" */
/*  *77   22-FEB-1995 15:26:40 TWF "Fix hpux problem" */
/*  *76   18-JAN-1995 16:56:38 TWF "Remove #ifdef on IDL entry points" */
/*  *75    6-JAN-1995 12:12:04 TWF "Compute client type" */
/*  *74    6-JAN-1995 11:43:57 TWF "Compute client type" */
/*  *73    6-JAN-1995 11:43:09 TWF "Compute client type" */
/*  *72   23-DEC-1994 09:43:47 TWF "Change eventinfo" */
/*  *71   23-DEC-1994 09:32:07 TWF "Change includes" */
/*  *70   23-DEC-1994 09:13:08 TWF "Change how events are handled" */
/*  *69   21-DEC-1994 14:17:34 TWF "Add event can" */
/*  *68   21-DEC-1994 14:16:28 TWF "Add event can" */
/*  *67   21-DEC-1994 12:03:01 TWF "Events on unix systems" */
/*  *66   21-DEC-1994 11:39:27 TWF "Events on unix systems" */
/*  *65   21-DEC-1994 11:14:56 TWF "Events on unix systems" */
/*  *64   20-DEC-1994 17:46:10 TWF "support events on unix" */
/*  *63   20-DEC-1994 17:32:19 TWF "support events on unix" */
/*  *62   20-DEC-1994 17:29:41 TWF "support events on unix" */
/*  *61   20-DEC-1994 16:19:09 TWF "support events on unix" */
/*  *60   20-DEC-1994 15:12:20 TWF "support events on unix" */
/*  *59   20-DEC-1994 15:11:14 TWF "support events on unix" */
/*  *58   14-SEP-1994 13:56:09 TWF "Take out timer" */
/*  *57   12-SEP-1994 15:14:44 TWF "Add MDS$SET_DEF" */
/*  *56    1-SEP-1994 08:51:20 TWF "Check for DECC" */
/*  *55    7-JUL-1994 11:21:02 TWF "Add user verification" */
/*  *54    7-JUL-1994 11:13:10 TWF "Add user verification" */
/*  *53    7-JUL-1994 11:07:50 TWF "Add user verification" */
/*  *52    7-JUL-1994 11:05:47 TWF "Add user verification" */
/*  *51    7-JUL-1994 11:02:57 TWF "Add user verification" */
/*  *50    7-JUL-1994 11:02:06 TWF "Add user verification" */
/*  *49    7-JUL-1994 11:00:34 TWF "Send user" */
/*  *48    6-JUL-1994 14:29:42 TWF "Fix access check" */
/*  *47    6-JUL-1994 14:22:09 TWF "Fix access check" */
/*  *46    6-JUL-1994 14:19:09 TWF "Fix access check" */
/*  *45    6-JUL-1994 14:17:50 TWF "Fix access check" */
/*  *44   28-JUN-1994 08:32:56 TWF "access check" */
/*  *43   27-JUN-1994 16:47:05 TWF "Check access" */
/*  *42   15-JUN-1994 11:37:46 TWF "Fix flip" */
/*  *41   15-JUN-1994 09:24:40 TWF "Fix flip" */
/*  *40   15-JUN-1994 08:57:10 TWF "Add more debug" */
/*  *39   14-JUN-1994 16:23:40 TWF "Add more debug" */
/*  *38   14-JUN-1994 16:20:07 TWF "Take out debug print" */
/*  *37   14-JUN-1994 16:19:04 TWF "Fix for CRAY" */
/*  *36   14-JUN-1994 16:09:38 TWF "Fix for CRAY" */
/*  *35   14-JUN-1994 14:06:55 TWF "Fix for CRAY" */
/*  *34   14-JUN-1994 13:27:56 TWF "handle dims differently on Cray" */
/*  *33   14-JUN-1994 13:20:17 TWF "handle dims differently on Cray" */
/*  *32   14-JUN-1994 12:34:53 TWF "handle dims differently on Cray" */
/*  *31   14-JUN-1994 12:22:42 TWF "handle dims differently on Cray" */
/*  *30   14-JUN-1994 11:56:05 TWF "Add support for CRAY's 64 bit values" */
/*  *29   14-JUN-1994 10:20:00 TWF "put client_type in message" */
/*  *28   14-JUN-1994 10:07:26 TWF "put client_type in message" */
/*  *27   14-JUN-1994 09:57:25 TWF "Fix compile error" */
/*  *26   14-JUN-1994 09:55:30 TWF "Fix complete test" */
/*  *25   14-JUN-1994 09:32:12 TWF "add CRAY" */
/*  *24   14-JUN-1994 09:22:26 TWF "add CRAY" */
/*  *23   13-JUN-1994 16:00:43 TWF "add CRAY" */
/*  *22   13-JUN-1994 15:06:27 TWF "add HP" */
/*  *21   10-JUN-1994 12:16:50 TWF "Add idl entry points to osf" */
/*  *20   10-JUN-1994 11:57:37 TWF "Handle 64bit pointers" */
/*  *19   10-JUN-1994 11:23:03 TWF "Take out multinet" */
/*  *18   10-JUN-1994 09:55:49 TWF "fix" */
/*  *17    9-JUN-1994 16:19:04 TWF "Use int32 instead of long" */
/*  *16    7-JUN-1994 15:46:30 TWF "ultrix" */
/*  *15    7-JUN-1994 15:23:57 TWF "Add ultrix support" */
/*  *14    6-JUN-1994 10:18:48 TWF "Flush pipe if possible" */
/*  *13    3-JUN-1994 10:02:55 TWF "Make timeout much longer" */
/*  *12    3-JUN-1994 08:30:24 TWF "Fix float on aix" */
/*  *11    3-JUN-1994 08:18:19 TWF "Fix float on aix" */
/*  *10    3-JUN-1994 08:16:55 TWF "Fix float on aix" */
/*  *9     2-JUN-1994 12:15:08 TWF "Change GetAnswerInfo to return pointer to data" */
/*  *8     2-JUN-1994 10:06:29 TWF "Create Idl entry points for IBM" */
/*  *7     1-JUN-1994 14:55:33 TWF "Add client type to SendMdsMsg" */
/*  *6     1-JUN-1994 14:39:59 TWF "Flip bytes" */
/*  *5     1-JUN-1994 12:35:05 TWF "Temporary fix for RS6000" */
/*  *4    31-MAY-1994 10:43:14 TWF "PC work" */
/*  *3    17-MAY-1994 16:59:15 TWF "Add sock to some of the Idl stubs" */
/*  *2    17-MAY-1994 10:00:52 TWF "Take out // comments" */
/*  *1    17-MAY-1994 09:30:50 TWF "MDSplus TCPIP RTL" */
/*  CMS REPLACEMENT HISTORY, Element MDSIPSHR.C */
#include "mdsip.h"
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define xxxxUNIX_SERVER

static unsigned char message_id = 1;
Message *GetMdsMsg(SOCKET sock, int *status);
#ifdef _WIN32
Message *GetMdsMsgOOB(SOCKET sock, int *status);
#endif
int SendMdsMsg(SOCKET sock, Message *m, int oob);

static int initialized = 0;

static char ClientType(void)
{
  static char ctype = 0;
  if (!ctype)
  {
    union { int i;
            char  c[sizeof(double)];
            float x;
            double d;
          } client_test;
    client_test.x = 1.;
    if (client_test.i == 0x4080) {
      client_test.d=12345678;
      if(client_test.c[5])
       ctype = VMSG_CLIENT;
      else
      ctype = VMS_CLIENT;
    }
    else if (client_test.i == 0x3F800000)
      ctype = IEEE_CLIENT;
    else
      ctype = CRAY_CLIENT;
    client_test.i = 1;
    if (!client_test.c[0]) ctype |= BigEndian;
  }
  return ctype;
}

struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length, char ndims, int *dims, void *ptr)
{
  int i;
  in_descrip->dtype = dtype;
  in_descrip->ndims = ndims;
  in_descrip->length = length;
  for (i=0;i<ndims;i++) in_descrip->dims[i] = dims[i];
  for (i=ndims; i<MAX_DIMS; i++) in_descrip->dims[i] = 0;
  in_descrip->ptr = ptr;
  return in_descrip;
}

struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims, void *ptr)
{
  int i;
  in_descrip->dtype = dtype;
  in_descrip->ndims = ndims;
  in_descrip->length = 0;
  for (i=0;i<ndims;i++) in_descrip->dims[i] = dims[i];
  for (i=ndims; i<MAX_DIMS; i++) in_descrip->dims[i] = 0;
  in_descrip->ptr = ptr;
  return in_descrip;
}

static short ArgLen(struct descrip *d)
{
  short len;
  switch (d->dtype)
  {
    case DTYPE_CSTRING :  len = d->length ? d->length : (d->ptr ? strlen(d->ptr) : 0); break;
    case DTYPE_UCHAR   :
    case DTYPE_CHAR    :  len = sizeof(char); break;
    case DTYPE_USHORT  :
    case DTYPE_SHORT   :  len = sizeof(short); break;
    case DTYPE_ULONG   :  
    case DTYPE_LONG    :  len = sizeof(long); break;
    case DTYPE_FLOAT   :  len = sizeof(float); break;
    case DTYPE_DOUBLE  :  len = sizeof(double); break;
    case DTYPE_COMPLEX :  len = sizeof(float) * 2; break;
  }
  return len;
}

int MdsValue(SOCKET sock, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  struct descrip exparg;
  struct descrip *arg = &exparg;
  va_start(incrmtr, expression);
  for (a_count = 1; arg != NULL; a_count++)
  {
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_start(incrmtr, expression);
  nargs = a_count - 2;
  arg = MakeDescrip((struct descrip *)&exparg,DTYPE_CSTRING,0,0,expression);
  for (i=1;i<a_count-1 && (status & 1);i++)
  {
    idx = i - 1;
    status = SendArg(sock, idx, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  if (status & 1)
  {
    short len;
    int numbytes;
    void *dptr;
    status = GetAnswerInfo(sock, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr);
    arg->length = len;
    if (numbytes)
    {
      if (arg->dtype == DTYPE_CSTRING)
      {
        arg->ptr = malloc(numbytes+1);
        ((char *)arg->ptr)[numbytes] = 0;
      }
      else if (numbytes > 0)
        arg->ptr = malloc(numbytes);
      if (numbytes > 0)
        memcpy(arg->ptr,dptr,numbytes);
    }
    else
      arg->ptr = NULL;
  }
  else
    arg->ptr = NULL;
  return status;
}

int MdsPut(SOCKET sock, char *node, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx = 0;
  int status = 1;
#ifdef _UNIX_SERVER
  static char *putexpprefix = "TreePutRecord(";
  static char *argplace = "$,";
#else
  static char *putexpprefix = "MDSLIB->MDS$PUT(";
  static char *argplace = "descr($),";
#endif
  char *putexp;
  struct descrip putexparg;
  struct descrip exparg;
  struct descrip *arg;
  va_start(incrmtr, expression);
  for (a_count = 3; va_arg(incrmtr, struct descrip *); a_count++);
  putexp = malloc(strlen(putexpprefix) + (a_count - 1) * strlen(argplace) + 1);
  strcpy(putexp,putexpprefix);
  for (i=0;i<a_count - 1;i++) strcat(putexp,argplace);
  putexp[strlen(putexp)-1] = ')';
  va_start(incrmtr, expression);
  nargs = a_count;
  arg = MakeDescrip(&putexparg,DTYPE_CSTRING,0,0,putexp);
  status = SendArg(sock, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
  free(putexp);
  arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,node);
  status = SendArg(sock, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
  arg = MakeDescrip(&exparg,DTYPE_CSTRING,0,0,expression);
  for (i=idx;i<a_count && (status & 1);i++)
  {
    status = SendArg(sock, (char)i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  if (status & 1)
  {
    char dtype;
    int dims[MAX_DIMS];
    char ndims;
    short len;
    int numbytes;
    void *dptr;
    status = GetAnswerInfo(sock, &dtype, &len, &ndims, dims, &numbytes, &dptr);
    if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
      memcpy(&status,dptr,numbytes);
  }
  return status;
}

/*JMS--->*/


int MdsValueFtotSize (struct descrip *dataarg)
{
  int totsize = 0;
  if (dataarg->ndims == 0) {
    printf("MDSvalueF: scalar\n");
    totsize=1;
  } else { 
    int i;
    for (i=0;i<dataarg->ndims;i++) {
      totsize=totsize+dataarg->dims[i];
    }
  }
  return totsize;
}

int  MdsValueF(SOCKET sock, char *expression, double *data, int maxsize, int *retsize)
{

  struct descrip dataarg;

  int status = MdsValue(sock, expression, (struct descrip *)&dataarg, (struct descrip *)NULL);

  /*printf("EXPRESSION IN MDSVALUEF: %s %u \n",expression,strlen(expression)); */
  if (status & 1) {

    int fullsize;
    float *newdata;
    int totsize = MdsValueFtotSize( (struct descrip *)&dataarg );
    memcpy(retsize,&totsize,sizeof(int));


    if (totsize > maxsize) totsize=maxsize;

    fullsize=totsize*ArgLen(&dataarg);

    switch (dataarg.dtype) {
    int i;
    case DTYPE_FLOAT : 
	memcpy(data, (float *) dataarg.ptr, fullsize);
	break;
    case DTYPE_DOUBLE : 
	memcpy(data, (double *) dataarg.ptr, fullsize);
	break;
    case DTYPE_LONG : 

        newdata = calloc (totsize, sizeof(float));

        for (i=0;i<totsize;i++) {
	  *(newdata+i) = ((float) *((long *)dataarg.ptr+i));
        } 

        memcpy(data, newdata, fullsize);
	free(newdata);
	break;
    default :
	printf ("MDSValueF: Can't handle type: %u\n", dataarg.dtype);
	status=0;
	break;
    }
  }
  if (dataarg.ptr) free(dataarg.ptr);
  return status;

}

int  MdsValueC(SOCKET sock, char *expression, char *data, int *retsize)
{

  struct descrip dataarg;
  int status = MdsValue(sock, expression, (struct descrip *)&dataarg, (struct descrip *)NULL);
  /* printf("EXPRESSION IN MDSVALUEC: -->%s<--\n",expression); */
  if (status & 1) {
    int size = ArgLen(&dataarg);
    memcpy(retsize,&size,sizeof(int));
    switch (dataarg.dtype) {
    case DTYPE_CSTRING : 
	memcpy(data, (char *) dataarg.ptr, ArgLen(&dataarg)*sizeof(char));
	break;
    default :
	printf ("MDSValueC: Can't handle type: %u\n", dataarg.dtype);
	status=0;
	break;
    }
  }
  if (dataarg.ptr) free(dataarg.ptr);
  return status;

}

int  MdsValueI(SOCKET sock, char *expression, int *data);
int  MdsValueI(SOCKET sock, char *expression, int *data) 
{

  struct descrip dataarg;
  int status = MdsValue(sock, expression, (struct descrip *)&dataarg, (struct descrip *)NULL);

  if (status & 1) {
    switch (dataarg.dtype) {
    case DTYPE_SHORT : case DTYPE_LONG :
	memcpy(data, (int *) dataarg.ptr, ArgLen(&dataarg));
	break;
    default :
	printf ("MDSValueI: Can't handle type: %u\n", dataarg.dtype);
	status=0;
	break;
    }
  }
  if (dataarg.ptr) free(dataarg.ptr);
  return status;

}



int  MdsValueAll(SOCKET sock, char *expression, int *ireturn,
		       double *freturn, char *creturn,
		       int maxsize, int *retsize, int *type)
{

  struct descrip ansarg;

  int status = MdsValue(sock, expression, (struct descrip *)&ansarg, (struct descrip *)NULL);

  if (status & 1) {

    int fullsize;
    int totsize=0;
    printf ("ndims: %u \n",ansarg.ndims);
    printf ("value: %d \n",*(int *)ansarg.ptr);
    if (ansarg.ndims == 0) {
      totsize=1;
    } else { 
      int i;
      for (i=0;i<ansarg.ndims;i++) {
	totsize=totsize+ansarg.dims[i];
      }
    }
    memcpy(retsize,&totsize,sizeof(int));
    fullsize=totsize*ArgLen(&ansarg);

    printf ("total size: %u \n",totsize);
    memcpy(type,&ansarg.dtype,sizeof(char));
    printf ("type: %u\n",*type);

    switch (ansarg.dtype) {
    case DTYPE_CHAR : 
          printf("Character returned: %s \n",(char *)ansarg.ptr);
          break;
    case DTYPE_FLOAT : 
          memcpy(freturn, (float *) ansarg.ptr, fullsize);
          break;
    case DTYPE_DOUBLE : 
          memcpy(freturn, (double *) ansarg.ptr, fullsize);
          break;
	  /*    case DTYPE_UCHAR : 
          memcpy(ireturn, (uchar *) ansarg.ptr, fullsize);
          break; */
    case DTYPE_USHORT : 
          memcpy(ireturn, (unsigned short *) ansarg.ptr, fullsize);
          break;
	  /* case DTYPE_ULONG : 
          memcpy(ireturn, (ulong *) ansarg.ptr, fullsize);
          break; */
    case DTYPE_SHORT : 
          memcpy(ireturn, (short *) ansarg.ptr, fullsize);
          break;
    case DTYPE_LONG : 
          memcpy(ireturn, (long *) ansarg.ptr, fullsize);
          break;
    default :
          printf ("Can't handle type: %u\n", *type);
          break;
    }


    /*    int size = maxsize*4; 
    printf("ansarg.ptr val: %u \n",*(int *)ansarg.ptr);
    memcpy(returnval, (int *) ansarg.ptr, size); */
  }
  if (ansarg.ptr) free(ansarg.ptr);
  printf ("STATUS in MDSVALUEALL: %d \n",status);
  return status;

}

/*JMS<---*/

int  MdsOpen(SOCKET sock, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
#ifdef _UNIX_SERVER
  static char *expression = "TreeOpen($,$)";
#else
  static char *expression = "MDSLIB->MDS$OPEN($,$)";
#endif
  int status = MdsValue(sock, expression, MakeDescrip((struct descrip *)&treearg,DTYPE_CSTRING,0,0,tree), 
			      MakeDescrip((struct descrip *)&shotarg,DTYPE_LONG,0,0,&shot),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);

  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsClose(SOCKET sock)
{
  struct descrip ansarg;
#ifdef _UNIX_SERVER
  static char *expression = "TreeClose()";
#else
  static char *expression = "MDSLIB->MDS$CLOSE()";
#endif
  int status = MdsValue(sock, expression, &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsSetDefault(SOCKET sock, char *node)
{
  struct descrip nodearg;
  struct descrip ansarg;
#ifdef _UNIX_SERVER
  static char *expression = "TreeSetDefault($)";
#else
  static char *expression = "MDSLIB->MDS$SET_DEFAULT($)";
#endif
  int status = MdsValue(sock, expression, MakeDescrip(&nodearg,DTYPE_CSTRING,0,0,node), &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

#ifdef _WIN32
typedef struct _EventThread {
	SOCKET sock;
	HANDLE thread_handle;
	DWORD  thread_id;
	int    eventid;
	int    event_count;
	struct _EventThread *next;
} EventThread;

static EventThread *eThreads = NULL;

unsigned long WINAPI MdsDispatchEvent(SOCKET sock);
#endif

int  MdsEventAst(SOCKET sock, char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
  struct descrip eventnamarg;
  struct descrip infoarg;
  struct descrip ansarg;
  MdsEventInfo info;
  int size = sizeof(info);
  int status;
#ifdef _WIN32
  for (et=eThreads; eThreads && et->sock != sock; et = et->next);
  if (et == NULL) {
	  et =  (EventThread *)malloc(sizeof(EventThread));
      et->sock = sock;
	  et->event_count = 0;
	  et->next = eThreads;
	  et->thread_handle = NULL;
	  eThreads = et;
  }
#endif
  info.astadr = (void (*)(void *,int ,char *))astadr;
  info.astprm = astprm;
  status = MdsValue(sock, EVENTASTREQUEST, MakeDescrip((struct descrip *)&eventnamarg,DTYPE_CSTRING,0,0,eventnam), 
			      MakeDescrip((struct descrip *)&infoarg,DTYPE_UCHAR,1,&size,&info),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) {
    *eventid = *(int *)ansarg.ptr;
#ifdef _WIN32
    et->eventid = *(int *)ansarg.ptr;
    if (et->thread_handle == NULL)
    et->thread_handle = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)MdsDispatchEvent, (LPVOID)sock, (DWORD)NULL, &et->thread_id);
    et->event_count++;
#endif
  }
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsEventCan(SOCKET sock, int eventid)
{
#ifdef _WIN32
  EventThread *et, *prev;
#endif
  struct descrip eventarg;
  struct descrip ansarg = {0,0,0,0};
  int status = MdsValue(sock, EVENTCANREQUEST, MakeDescrip((struct descrip *)&eventarg,DTYPE_LONG,0,0,&eventid), 
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if (ansarg.ptr) free(ansarg.ptr);
#ifdef _WIN32
  for (prev=NULL,et=eThreads; et && (et->sock != sock); prev = et,et = et->next);
  if (et) {
	et->event_count--;
	if (et->event_count == 0)
	  closesocket(et->sock);
      if (prev)
		  prev->next = et->next;
	  else
		  eThreads = et->next;
  }
#endif
  return status;
}

#ifdef _WIN32
unsigned long WINAPI MdsDispatchEvent(SOCKET sock)
#else
void MdsDispatchEvent(SOCKET sock)
#endif
{
  int status;
  Message  *m;
#ifdef _WIN32
  while ((m = GetMdsMsgOOB(sock,&status)) != 0)
#else
  if ((m = GetMdsMsg(sock,&status)) != 0)
#endif
  {
    if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo)))
    {
      MdsEventInfo *event = (MdsEventInfo *)m->bytes;
      (*event->astadr)(event->astprm, event->eventid, event->data);
    }
#ifdef MULTINET
    sys$qiow(0,sock,IO$_SETMODE | IO$M_ATTNAST,0,0,0,MdsDispatchEvent,sock,0,0,0,0);
#endif
  }
#ifdef _WIN32
  return 0;
#endif
}

static SOCKET ConnectToPort(char *host, char *service)
{
  int status;
  SOCKET s;
  static struct sockaddr_in sin;
  struct hostent *hp;
  struct servent *sp;
  static int one=1;
  long sendbuf = SEND_BUF_SIZE,recvbuf = RECV_BUF_SIZE;

  
  hp = gethostbyname(host);
#ifdef _WIN32
  if ((hp == NULL) && (WSAGetLastError() == WSANOTINITIALISED))
  {
	  WSADATA wsaData;
	  WORD wVersionRequested;
	  wVersionRequested = MAKEWORD(1,1);
	  WSAStartup(wVersionRequested,&wsaData);
	  hp = gethostbyname(host);
  }
#endif
  if (hp == NULL)
  {
    printf("Error: %s unknown\n",host);
    return INVALID_SOCKET;
  }
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET) return INVALID_SOCKET;
  sp = getservbyname(service,"tcp");
  if (sp == NULL)
  {
    char *port = getenv(service);
	port = (port == NULL) ? "8000" : port;
    sp = malloc(sizeof(*sp));
    sp->s_port = htons((short)atoi(port));
  }
  if (sp == NULL || sp->s_port == 0)
  {
    printf("Unknown service: %s\nSet environment variable %s if port is known\n",service,service);
    return INVALID_SOCKET;
  }
  sin.sin_family = AF_INET;
  sin.sin_port = sp->s_port;
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
  status = connect(s, (struct sockaddr *)&sin, sizeof(sin));
  if (status < 0)
  {
    perror("Error in connect to service\n");
    return INVALID_SOCKET;
  }
  else
  {
    int status;
    Message *m;
#ifdef _WIN32
	static char user[128];
	int bsize=128;
	char *user_p = GetUserName(user,&bsize) ? user : "Windows User";
#else
    static char user[L_cuserid];
    char *user_p;
    user_p = (cuserid(user) && strlen(user)) ? user : "?";
#endif
    m = malloc(sizeof(MsgHdr) + strlen(user_p));
    m->h.length = strlen(user_p);
    m->h.msglen = sizeof(MsgHdr) + m->h.length;
    m->h.dtype = DTYPE_CSTRING;
    m->h.ndims = 0;
    memcpy(m->bytes,user_p,m->h.length);
    status = SendMdsMsg(s,m,0);
    free(m);
    if (status & 1)
    {
      m = GetMdsMsg(s,&status);
      if (!(status & 1))
      {
        printf("Error in connect to service\n");
        return INVALID_SOCKET;
      }
      else
      {
        if (!m->h.status)
        {
          printf("Access denied\n");
          free(m);
          DisconnectFromMds(s);
          return INVALID_SOCKET;
        }
      }
    }
    else
    {
      printf("Error in user verification\n");
      return INVALID_SOCKET;
    }
  }
  setsockopt(s, SOL_SOCKET,SO_RCVBUF,(void *)&recvbuf,sizeof(recvbuf));
  setsockopt(s, SOL_SOCKET,SO_SNDBUF,(void *)&sendbuf,sizeof(sendbuf));
#ifndef _WIN32
  setsockopt(s, SOL_SOCKET,SO_OOBINLINE,(void *)&one,sizeof(one));
#endif
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
#ifdef MULTINET
  sys$qiow(0,s,IO$_SETMODE | IO$M_ATTNAST,0,0,0,MdsDispatchEvent,s,0,0,0,0);
#endif
  return s;
}

SOCKET  ConnectToMds(char *host)
{
  return ConnectToPort(host, "mdsip");
}

SOCKET  ConnectToMdsEvents(char *host)
{
  return ConnectToPort(host, "mdsipe");
}

int  GetAnswerInfo(SOCKET sock, char *dtype, short *length, char *ndims, int *dims,
 int *numbytes, void * *dptr)
{
  int status;
  Message *m;
  int i;
  *numbytes = 0;
  m = GetMdsMsg(sock, &status);
  if (status != 1) return 0;
  if (m->h.ndims)
  {
    *numbytes = m->h.length;
    for (i=0;i<m->h.ndims;i++)
    {
#ifdef __CRAY
      dims[i] = i % 2 ? m->h.dims[i/2] >> 32 : m->h.dims[i/2] & 0xffffffff;
#else
      dims[i] = m->h.dims[i];
#endif
      *numbytes *= dims[i];
#ifdef DEBUG
      printf("dim[%d] = %d\n",i,dims[i]);
#endif
    }
    for (i=m->h.ndims;i < MAX_DIMS; i++)
      dims[i] = 0;
  }
  else
  {
    *numbytes = m->h.length;
    for (i=0;i<MAX_DIMS;i++)
      dims[i] = 0;
  }
  if ((int)(sizeof(MsgHdr) + *numbytes) != m->h.msglen)
  {
    *numbytes = 0;
    return 0;
  }
  *dtype = m->h.dtype;
  *length = m->h.length;
  *ndims = m->h.ndims;
  *dptr = m->bytes;
  return m->h.status;
}

int  DisconnectFromMds(SOCKET sock)
{
  int status;
  shutdown(sock,2);
  status = close(sock);
#ifdef __MSDOS__
  WSACleanup();
#endif
  return status;
}

int  SendArg(SOCKET sock, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
int *dims, char *bytes)
{
  int status;
  int msglen;
  int i;
  int nbytes = length;
  Message *m;
  for (i=0;i<ndims;i++)
    nbytes *= dims[i];
  msglen = sizeof(MsgHdr) + nbytes;
  m = malloc(msglen);
  m->h.msglen = msglen;
  m->h.message_id = message_id;
  m->h.descriptor_idx = idx;
  m->h.dtype = dtype;
  m->h.nargs = nargs;
  m->h.length = length;
  m->h.ndims = ndims;
#ifdef __CRAY
  for (i=0;i<MAX_DIMS;i++)
  {
    if (i % 2)
      m->h.dims[i/2] |= i < ndims ? (dims[i] << 32) : 0;
    else
      m->h.dims[i/2] = dims[i];
  }
#else
  for (i=0;i<MAX_DIMS;i++) m->h.dims[i] = i < ndims ? dims[i] : 0;
#endif
  memcpy(m->bytes,bytes,nbytes);
  status = SendMdsMsg(sock, m, 0);
  if (idx == (nargs -1)) message_id++;
  free(m);
  return status;
}

int SendMdsMsg(SOCKET sock, Message *m, int oob)
{
  char *bptr = (char *)m;
  int bytes_to_send = m->h.msglen;
  m->h.client_type = ClientType();
  while (bytes_to_send > 0)
  {
    int bytes_this_time = min(bytes_to_send,BUFSIZ);
    int bytes_sent;
    bytes_sent = send(sock, bptr, bytes_this_time, oob);
    if (bytes_sent <= 0)
      return 0;
    else
    {
      bytes_to_send -= bytes_sent;
      bptr += bytes_sent;
    }
  }
  return 1;
}

static void FlipBytes(int n, char *ptr)
{
  int i;
  for (i=0;i<n/2;i++)
  {
    char tmp = ptr[i];
    ptr[i] = ptr[n - i - 1];
    ptr[n - i - 1] = tmp;
  }
}
  
static void FlipHeader(MsgHdr *header)
{
  int i;
#ifdef __CRAY
#define Flip32(n) n = ((n >> 24) & 0xff) | ((n >> 8) & 0xff00) | ((n << 8) & 0xff0000) | ((n << 24) & 0xff000000)
#define Flip16(n) n = ((n >> 8) & 0xff) | ((n << 8) & 0xff00)
  Flip32(header->msglen);
  Flip32(header->status);
  Flip16(header->length);
  for (i=0;i<MAX_DIMS/2;i++) FlipBytes(sizeof(header->dims[i]),(char *)&header->dims[i]);
#else
  FlipBytes(4,(char *)&header->msglen);
  FlipBytes(4,(char *)&header->status);
  FlipBytes(2,(char *)&header->length);
  for (i=0;i<MAX_DIMS;i++) FlipBytes(sizeof(header->dims[i]),(char *)&header->dims[i]);
#endif
}

static void FlipData(Message *m)
{
  int num = 1;
  int i;
  char *ptr;
  int dims[MAX_DIMS];
  for (i=0;i<MAX_DIMS;i++)
  {
#ifdef __CRAY
    dims[i] = i % 2 ? m->h.dims[i/2] >> 32 : m->h.dims[i/2] & 0xffffffff;
#else
    dims[i] = m->h.dims[i];
#endif
  }
  if (m->h.ndims) for (i=0;i<m->h.ndims;i++) num *= dims[i];
#ifdef DEBUG
  printf("num to flip = %d\n",num);
#endif
  switch (m->h.dtype)
  {
#ifndef __CRAY
    case DTYPE_FLOAT:   
    case DTYPE_DOUBLE:
    case DTYPE_COMPLEX:
#endif
    case DTYPE_USHORT:
    case DTYPE_SHORT:  
    case DTYPE_ULONG:
    case DTYPE_LONG:  for (i=0,ptr=m->bytes;i<num;i++,ptr += m->h.length) FlipBytes(m->h.length,ptr); break;
  }
}

Message *GetMdsMsg(SOCKET sock, int *status)
{
  static MsgHdr header;
  static Message *msg = 0;
  int msglen = 0;
/*
  static struct timeval timer = {10,0};
  int tablesize = FD_SETSIZE;
*/
  int nbytes;
  int selectstat=0;
  char *bptr = (char *)&header;
  fd_set readfds;
  fd_set exceptfds;
  int bytes_remaining = sizeof(MsgHdr);
  FD_ZERO(&readfds);
  FD_SET(sock,&readfds);
  FD_ZERO(&exceptfds);
  FD_SET(sock,&exceptfds);
  *status = 0;
  if (msg)
  {
    free(msg);
    msg = 0;
  }
/*
#ifdef MULTINET
  while (((msglen == 0) || bytes_remaining) && 
          (lib$ast_in_prog() || ((selectstat = select(tablesize, &readfds, 0, &exceptfds, &timer)) > 0)))
#else
  while (((msglen == 0) || bytes_remaining) && 
          ((selectstat = select(tablesize, &readfds, 0, &exceptfds, &timer)) != 0))
#endif
*/
  while (bytes_remaining > 0)
  {
    unsigned short flags = 0;
    if (selectstat == -1 && errno == 4) continue;
    if (msglen == 0)
    {
      nbytes = recv(sock, bptr, bytes_remaining, flags);
      if (nbytes == 0)
      {
        *status = 0;
        return 0;
      }
      else if (nbytes > 0)
      {
        bytes_remaining -= nbytes;
        bptr += nbytes;
        if (bytes_remaining == 0)
        {
          if ( Endian(header.client_type) != Endian(ClientType()) ) FlipHeader(&header);
#ifdef DEBUG
          printf("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
               header.msglen,header.status,header.length,header.nargs,header.descriptor_idx,header.message_id,header.dtype);
          printf("client_type = %d\nndims = %d\n",header.client_type,header.ndims);
#endif
          if (CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS)
          {
            *status = 0;
            return 0;
          }  
          msglen = header.msglen;
          bytes_remaining = msglen - sizeof(MsgHdr);
          msg = malloc(msglen);
          msg->h = header;
          bptr = msg->bytes;
        }
      }
      else
      {
        perror("GETMSG recv error");
        printf("errno = %d\n",errno);
      }
    }
    else
    {
      int bytes_this_time = min(bytes_remaining,BUFSIZ);
      nbytes = recv(sock, bptr, bytes_this_time, flags);
#ifdef DEBUG
      {int i;
      for (i=0;i<nbytes;i++)
        printf("byte(%d) = 0x%x\n",i,bptr[i]);
      }
#endif
      if (nbytes > 0)
      {
        bytes_remaining -= nbytes;
        bptr += nbytes;
      }
      else
      {
	*status = 0;
        return 0;
      }
    }
  }
  if (msglen && !bytes_remaining)
  {
    if ( Endian(header.client_type) != Endian(ClientType()) ) FlipData(msg);
    *status = 1;
  }
  else
  {
    perror("Select problem");
  }
  return msg;
}

#ifdef _WIN32
Message *GetMdsMsgOOB(SOCKET sock, int *status)
{
  static MsgHdr header;
  static Message *msg = 0;
  int msglen = 0;
  static struct timeval timer = {10,0};

  int tablesize = FD_SETSIZE;

  int nbytes;
  int selectstat=0;
  char last;
  char *bptr = (char *)&header;
  fd_set readfds;
  fd_set exceptfds;
  int bytes_remaining = sizeof(MsgHdr);
    unsigned short flags = MSG_OOB;
	DWORD oob_data;
	int stat;
  FD_ZERO(&readfds);
  FD_SET(sock,&readfds);
  FD_ZERO(&exceptfds);
  FD_SET(sock,&exceptfds);
  *status = 0;
  if (msg)
  {
    free(msg);
    msg = 0;
  }

/*
#ifdef MULTINET
  while (((msglen == 0) || bytes_remaining) && 
          (lib$ast_in_prog() || ((selectstat = select(tablesize, 0, 0, &exceptfds, &timer)) > 0)))
#else
  while (((msglen == 0) || bytes_remaining) && 
          ((selectstat = select(tablesize, 0, 0, &exceptfds, NULL)) != 0))
#endif
*/
  selectstat = select(tablesize, 0, 0, &exceptfds, NULL);
  nbytes = recv(sock, &last, 1, flags);
  stat = 	ioctlsocket(sock,SIOCATMARK,&oob_data);
  while (bytes_remaining > 1) 
  {
/*    unsigned short flags = 0; */

    if (selectstat == -1 && errno == 4) continue;
    if (msglen == 0)
    {
        nbytes = recv(sock, bptr, bytes_remaining, 0);
      if (nbytes == 0)
	  { 
        *status = 0;
        return 0;
      }
      else if (nbytes > 0)
      {
        bytes_remaining -= nbytes;
        bptr += nbytes;
        if (bytes_remaining == 0)
        {
          if ( Endian(header.client_type) != Endian(ClientType()) ) FlipHeader(&header);
#ifdef DEBUG
          printf("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
               header.msglen,header.status,header.length,header.nargs,header.descriptor_idx,header.message_id,header.dtype);
          printf("client_type = %d\nndims = %d\n",header.client_type,header.ndims);
#endif
          if (CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS)
          {
            *status = 0;
            return 0;
          }  
          msglen = header.msglen;
          bytes_remaining = msglen - sizeof(MsgHdr);
          msg = malloc(msglen);
          msg->h = header;
          bptr = msg->bytes;
        }
      }
      else
      {
        perror("GETMSG recv error");
        printf("errno = %d\n",errno);
      }
    }
    else
    {
      int bytes_this_time = min(bytes_remaining,BUFSIZ);
      nbytes = recv(sock, bptr, bytes_this_time, 0);
#ifdef DEBUG
      {int i;
      for (i=0;i<nbytes;i++)
        printf("byte(%d) = 0x%x\n",i,bptr[i]);
      }
#endif
      if (nbytes > 0)
      {
        bytes_remaining -= nbytes;
        bptr += nbytes;
      }
      else
      {
	*status = 0;
        return 0;
      }
    }
  }
  if (msglen && (bytes_remaining==1))
  {
	*bptr = last;
    if ( Endian(header.client_type) != Endian(ClientType()) ) FlipData(msg);
    *status = 1;
  }
  else
  {
    perror("Select problem");
  }
  return msg;
}
#endif

int  IdlMdsClose(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsClose', socket, value=[1b])
*/
  int status;
  sighold(SIGALRM);
  status = MdsClose((SOCKET)lpvArgv[0]);
  sigrelse(SIGALRM);
  return status;
}

int  IdlConnectToMds(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlConnectToMds', 'host-name')
*/
  int status;
  sighold(SIGALRM);
  status = ConnectToMds((char *)lpvArgv[0]);
  sigrelse(SIGALRM);
  return status;
}

int  IdlDisconnectFromMds(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlDisconnectFromMds', socket, value=[1b])
*/
  int status;
  sighold(SIGALRM);
  status = DisconnectFromMds((SOCKET)lpvArgv[0]);
  sigrelse(SIGALRM);
  return status;
}

int  IdlMdsOpen(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsOpen', sock, 'tree-name', shot, value = [1b,0b,1b]) 
*/
  int status;
  sighold(SIGALRM);
  status = MdsOpen((int)lpvArgv[0],(char *)lpvArgv[1],(int)lpvArgv[2]);
  sigrelse(SIGALRM);
  return status;
}

int  IdlMdsSetDefault(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsSetDefault', sock, 'node', value = [1b,0b]) 
*/
  int status;
  sighold(SIGALRM);
  status = MdsSetDefault((int)lpvArgv[0],(char *)lpvArgv[1]);
  sigrelse(SIGALRM);
  return status;
}

int  IdlGetAnsInfo(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlGetAnsInfo', socket_l, dtype_b, length_w, ndims_b, dims_l[7], numbytes_l, 
                               value=[1b,0b,0b,0b,0b,0b,0b])
*/
  int status;
  sighold(SIGALRM);
  status = GetAnswerInfo((SOCKET)lpvArgv[0], (char *)lpvArgv[1], (short *)lpvArgv[2], (char *)lpvArgv[3],
                       (int *)lpvArgv[4], (int *)lpvArgv[5], (void **)lpvArgv[6]);
  sigrelse(SIGALRM);
  return status;
}

int  Idlmemcpy(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','Idlmemcpy', answer, answer_ptr, nbytes, value=[0b,1b,1b])
*/
#ifdef __alpha
  memcpy((void *)lpvArgv[0],*(void **)lpvArgv[1], (int)lpvArgv[2]);
#else
  memcpy((void *)lpvArgv[0],(void *)lpvArgv[1], (int)lpvArgv[2]);
#endif
  return 1;
}

int  IdlSendArg(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlSendArg', sock_l, idx_l, dtype_b, nargs_w, length_w, ndims_b, dims_l[7], 
			    bytes, value=[1b,1b,1b,1b,1b,1b,1b,0b,0b])
*/
  unsigned char idx    = (unsigned char)lpvArgv[1];
  unsigned char dtype  = (unsigned char)lpvArgv[2];
  unsigned char nargs  = (unsigned char)lpvArgv[3];
  short         length = (short)lpvArgv[4];
  char          ndims  = (char)lpvArgv[5];
  int status;
  sighold(SIGALRM);
  status = SendArg((SOCKET)lpvArgv[0], idx, dtype, nargs, length, ndims, (int *)lpvArgv[6], (char *)lpvArgv[7]);
  sigrelse(SIGALRM);
  return status;
}
