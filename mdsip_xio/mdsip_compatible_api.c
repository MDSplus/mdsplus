#define IPDESC_H
#include "mdsip.h"

#define MAX_CONNECTIONS 1024
void *MDSIP_IO_HANDLES[MAX_CONNECTIONS];
static int client_addr = 0;
static char *server_portname = NULL;

int ConnectToMds(char *server)
{
  int i;
  for (i=0;i<MAX_CONNECTIONS && MDSIP_IO_HANDLES[i] != NULL;i++);
  if (i==MAX_CONNECTIONS)
  {
    fprintf(stderr,"Exceeded maximum active connections of %d\n",MAX_CONNECTIONS);
    return -1;
  }
  MDSIP_IO_HANDLES[i]=mdsip_connect(server);
  return (MDSIP_IO_HANDLES[i] != NULL) ? i+1 : -1;
}

int SendArg(int sock, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
	     int *dims, char *bytes)
{
  return mdsip_send_arg(MDSIP_IO_HANDLES[sock-1], idx, dtype, nargs, length, ndims, dims, bytes);
}

int DisconnectFromMds(int sock)
{
  int status = globus_xio_close(MDSIP_IO_HANDLES[sock-1],NULL);
  MDSIP_IO_HANDLES[sock-1]=NULL;
  return status == GLOBUS_SUCCESS;
}

int HostToIp(char *host, int *addr, short *port)
{
  return mdsip_host_to_ipaddr(host, addr, port);
}

void MdsIpFree(void *buff)
{
  free(buff);
}

int GetAnswerInfoTS(int sock, char *dtype, short *length, char *ndims, 
		     int *dims, int *numbytes, void * *dptr, void **m)
{
  return mdsip_get_result(MDSIP_IO_HANDLES[sock-1], dtype, length, ndims, dims, numbytes, dptr, m);
}

int  GetAnswerInfo(int sock, char *dtype, short *length, char *ndims, int *dims, int *numbytes, void * *dptr)
{
  static void *m = 0;
  if (m)
  {    
    free(m);
    m = 0;
  }
  return GetAnswerInfoTS(sock,dtype,length,ndims,dims,numbytes,dptr,&m);
}

short ArgLen(struct descrip *d)
{
  return mdsip_arglen(d);
}

struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims, void *ptr)
{
  return mdsip_make_descrip(in_descrip, dtype, 0, ndims, dims, ptr);
}

struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length, char ndims, int *dims, void *ptr)
{
  return mdsip_make_descrip(in_descrip, dtype, length, ndims, dims, ptr);
}

int SendMdsMsg(int sock, mdsip_message_t *m, int oob)
{
  return mdsip_send_message(MDSIP_IO_HANDLES[sock-1], m, oob);
}

mdsip_message_t *GetMdsMsg(int sock, int *status)
{
  return mdsip_get_message(MDSIP_IO_HANDLES[sock-1],status);
}

int  IdlMdsClose(int lArgc, int *sock)
{
/*  status = call_external('mdsipshr','IdlMdsClose', socket, value=[1b])
*/
  return MdsClose(*sock);
}

int  IdlConnectToMds(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlConnectToMds', 'host-name')
*/
  return ConnectToMds((char *)lpvArgv[0]);
}

int  IdlDisconnectFromMds(int lArgc, int *sock)
{
/*  status = call_external('mdsipshr','IdlDisconnectFromMds', socket, value=[1b])
*/
  DisconnectFromMds(*sock);
  return 1;
}

int  IdlMdsOpen(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsOpen', sock, 'tree-name', shot, value = [1b,0b,1b]) 
*/
  int *sock = (int *)&lpvArgv[0];
  return MdsOpen(*sock,(char *)lpvArgv[1],*((int *)&lpvArgv[2]));
}

int  IdlMdsSetDefault(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsSetDefault', sock, 'node', value = [1b,0b]) 
*/
  int *sock = (int *)&lpvArgv[0];
  return MdsSetDefault(*sock,(char *)lpvArgv[1]);
}

int  IdlGetAnsInfo(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlGetAnsInfo', socket_l, dtype_b, length_w, ndims_b, dims_l[7], numbytes_l, 
                               value=[1b,0b,0b,0b,0b,0b,0b])
*/
  int status;
  static void *mem = NULL;
  int *sock = (int *)&lpvArgv[0];
  if (mem != NULL)
  {
    free(mem);
    mem = NULL;
  }
  status = mdsip_get_result(MDSIP_IO_HANDLES[(*sock)-1], (char *)lpvArgv[1], (short *)lpvArgv[2], (char *)lpvArgv[3],
                       (int *)lpvArgv[4], (int *)lpvArgv[5], (void **)lpvArgv[6], &mem);
  return status;
}

int  Idlmemcpy(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','Idlmemcpy', answer, answer_ptr, nbytes, value=[0b,1b,1b])
*/
  memcpy((void *)lpvArgv[0],(void *)lpvArgv[1], (int)((char *)lpvArgv[2] - (char *)0));
  return 1;
}

int  IdlSendArg(int lArgc, void * * lpvArgv)
{
/*  status = call_external('mdsipshr','IdlSendArg', sock_l, idx_l, dtype_b, nargs_w, length_w, ndims_b, dims_l[7], 
			    bytes, value=[1b,1b,1b,1b,1b,1b,1b,0b,0b])
*/
  int *sock = (int *)&lpvArgv[0];
  unsigned char idx    = *((unsigned char *)&lpvArgv[1]);
  unsigned char dtype  = *((unsigned char *)&lpvArgv[2]);
  unsigned char nargs  = *((unsigned char *)&lpvArgv[3]);
  short         length = *((short *)&lpvArgv[4]);
  char          ndims  = *((char *)&lpvArgv[5]);
  return mdsip_send_arg(MDSIP_IO_HANDLES[(*sock)-1], idx, dtype, nargs, length, ndims, (int *)lpvArgv[6], (char *)lpvArgv[7]);
}

void MdsSetClientAddr(int addr)
{
  client_addr=addr;
}

int MdsGetClientAddr()
{
  return client_addr;
}

void MdsSetServerPortname(char *portname)
{
  server_portname=portname;
}

char *MdsGetServerPortname()
{
  return server_portname;
}

int  MdsEventAst(int sock, char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
  return mdsip_event_ast(MDSIP_IO_HANDLES[sock-1],eventnam,astadr,astprm,eventid);
}

int MdsEventCan(int sock, int eventid)
{
  return mdsip_event_can(MDSIP_IO_HANDLES[sock-1],eventid);
}

void MdsDispatchEvent(int sock)
{
  mdsip_dispatch_event(MDSIP_IO_HANDLES[sock-1]);
}

int ConnectToMdsEvents(char *host)
{
  char hostpart[256] = {0};
  char portpart[256] = {0};
  sscanf(host,"%[^:]:%s",hostpart,portpart);
  if (strlen(portpart) == 0)
    strcpy(portpart,"mdsipe");
  strcat(hostpart,":");
  strcat(hostpart,portpart);
  return ConnectToMds(hostpart);
}

