#include <stdlib.h>
#include <string.h>
#include <ipdesc.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#define min(a,b) (((a)<(b)) ? (a) : (b))


/* this decl will be int ipdesc.h */
extern int HostToIp(char *host, int *ip, short *port);

typedef struct _connection {
  int  ip;
  short port;
  int  sock;
  struct _connection *next;
} Connection;

static Connection *Connections = NULL;

static int PopConnection(int ip, short port)
{
  Connection *cptr, *p_cptr;
  for (p_cptr=NULL,cptr=Connections; (cptr != NULL); p_cptr=cptr,cptr=cptr->next)
  {
     if ((cptr->port == port) && (ip ==  cptr->ip))
     break;
  }
  if (cptr) {
    if (cptr != Connections) {
      p_cptr->next = cptr->next;
      cptr->next=Connections;
      Connections = cptr;
    } 
  }
  return (cptr != NULL);
}

static int PushConnection(char *host, int ip, short port)
{
  Connection *cptr;
  int sock = 0;
  if (ip != 0)
    sock = ConnectToMds(host);
  if ((sock >= 0)) {
    cptr = (Connection *)malloc(sizeof(Connection));
    cptr->port=port;
    cptr->sock = sock;
    cptr->ip = ip;
    cptr->next = Connections;
    Connections = cptr;
  }
  return (sock >= 0);
}

static void CloseConnection()
{
  Connection *tmp;
  if (Connections->ip != 0)
    DisconnectFromMds(Connections->sock);
  tmp = Connections->next;
  free (Connections);
  Connections = tmp;
}

int MdsConnect(char *host)
{
  int status, ip;
  short port;

  status = HostToIp(host, &ip, &port);
  if (status&1)
    if (!PopConnection(ip, port))
      status = PushConnection(host, ip, port);
  return status;
}

int MdsDisconnect(int all, char *host)
{
  Connection *tmp;
  int status = 1;
  if (all) {
    for (tmp=Connections; tmp!=NULL; tmp=Connections) {
      CloseConnection();
    }
  }
  else if (host == NULL)
    CloseConnection();
  else 
  {
    int ip;
    short port;
    status = HostToIp(host, &ip, &port);
    if (status > 0)
      if (PopConnection(ip, port))
        CloseConnection();
  }
  return 1;
}

int MdsIsRemote() 
{
  if (Connections != NULL)
    return (Connections->ip != 0);
  else
    return 0;
}

struct descriptor_xd *RemoteMdsValue(char *expr)
{
  static const DESCRIPTOR(IT_IS_LOCAL, 
                           "Mds Access is local MdsRemote->RemoteMdsValue should not be called");
  static EMPTYXD(ans_xd);
  struct descrip ans;
  int status;
  int ndim;
  int sock;

  if (Connections != NULL)
    if (Connections->ip != 0) 
    {
      sock = Connections->sock;
      status = MdsValue(sock, expr, &ans, 0);
      /* 
         even if it is not successful copy the 
         error tring to the answer.
       */
      for(ndim=0; (ndim < MAX_DIMS) && (ans.dims[ndim] != 0); ndim++);
      if (ndim==0) {
        struct descriptor_s ans_dsc={0, 0, CLASS_S, 0};
        ans_dsc.pointer = ans.ptr;
        ans_dsc.dtype = ans.dtype;
        ans_dsc.length = ans.length;
        MdsCopyDxXd((struct descriptor *)&ans_dsc, &ans_xd);
      }  
      else {
        DESCRIPTOR_A_COEFF(a_dsc, 0, 0,0,MAX_DIMS, 0);
        int i;
        a_dsc.pointer = ans.ptr;
        a_dsc.dimct = ndim;
        a_dsc.dtype = ans.dtype;
        a_dsc.length = ans.length;
        a_dsc.a0 = ans.ptr;
        a_dsc.arsize=ans.length;
        for (i=0; i<ndim; i++) {
          a_dsc.m[i] = ans.dims[i];
          a_dsc.arsize *= ans.dims[i];
        }
        MdsCopyDxXd((struct descriptor *)&a_dsc, &ans_xd);
      }
      free(ans.ptr);
      return(&ans_xd);
    }
    else
      return (struct descriptor_xd *)&IT_IS_LOCAL;
  else
    return (struct descriptor_xd *)&IT_IS_LOCAL;
}


