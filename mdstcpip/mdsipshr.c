/*  CMS REPLACEMENT HISTORY, Element MDSIPSHR.C */
/*  *99   12-JAN-1999 16:15:22 JMS "removed all code used by MdsLib - placed into mdsiputil.c */
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
/*  *17    9-JUN-1994 16:19:04 TWF "Use int instead of long" */
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
#ifdef _USE_VARARGS
int MdsValue(va_alist) va_dcl
{
  SOCKET sock;
  char *expression;
#else
int MdsValue(SOCKET sock, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  struct descrip exparg;
  struct descrip *arg = &exparg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 1; arg != NULL; a_count++)
  {
    arg = va_arg(incrmtr, struct descrip *);
  }
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  nargs = a_count - 2;
  arg = MakeDescrip((struct descrip *)&exparg,DTYPE_CSTRING,0,0,expression);
  for (i=1;i<a_count-1 && (status & 1);i++)
  {
    idx = i - 1;
    status = SendArg(sock, idx, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
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

#ifdef _USE_VARARGS
int MdsPut(va_alist) va_dcl
{
  SOCKET sock;
  char *node;
  char *expression;
#else
int MdsPut(SOCKET sock, char *node, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx = 0;
  int status = 1;
  static char *putexpprefix = "TreePut(";
  static char *argplace = "$,";
  char *putexp;
  struct descrip putexparg;
  struct descrip exparg;
  struct descrip *arg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  node = va_arg(incrmtr, char *);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 3; va_arg(incrmtr, struct descrip *); a_count++);
  putexp = malloc(strlen(putexpprefix) + (a_count - 1) * strlen(argplace) + 1);
  strcpy(putexp,putexpprefix);
  for (i=0;i<a_count - 1;i++) strcat(putexp,argplace);
  putexp[strlen(putexp)-1] = ')';
#ifdef _USE_VARARGS
  va_start(incrmtr);
  sock = va_arg(incrmtr, SOCKET);
  node = va_arg(incrmtr, char *);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
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
  va_end(incrmtr);
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

int  MdsOpen(SOCKET sock, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
  static char *expression = "TreeOpen($,$)";
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
  static char *expression = "TreeClose()";
  int status = MdsValue(sock, expression, &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

int  MdsSetDefault(SOCKET sock, char *node)
{
  struct descrip nodearg;
  struct descrip ansarg;
  static char *expression = "TreeSetDefault($)";
  int status = MdsValue(sock, expression, MakeDescrip(&nodearg,DTYPE_CSTRING,0,0,node), &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}


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
