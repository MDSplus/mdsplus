#include <inttypes.h>

#include <socket_port.h>
#include <pthread_port.h>

#include "Job.h"
#include <mdsmsg.h>

typedef struct _client
{
  struct _client *next;
  SOCKET reply_sock;
  int conid;
  uint32_t addr;
  uint16_t port;
} Client;
#ifdef _WIN32
#define CLIENT_PRI "Client(conid=%d, addr=" IPADDRPRI ", port=%d, reply_sock=%" PRIdPTR ")"
#define CLIENT_VAR(c) (c)->conid, IPADDRVAR(&(c)->addr), (c)->port, (intptr_t)(c)->reply_sock
#else
#define CLIENT_PRI "Client(conid=%d, addr=" IPADDRPRI ", port=%d, reply_sock=%d)"
#define CLIENT_VAR(c) (c)->conid, IPADDRVAR(&(c)->addr), (c)->port, (c)->reply_sock
#endif
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t clients_cond = PTHREAD_COND_INITIALIZER;
#define LOCK_CLIENTS MUTEX_LOCK_PUSH(&clients_mutex)
#define UNLOCK_CLIENTS MUTEX_LOCK_POP(&clients_mutex)
#define UNLOCK_CLIENTS_REV MUTEX_UNLOCK_PUSH(&clients_mutex)
#define LOCK_CLIENTS_REV MUTEX_UNLOCK_POP(&clients_mutex)
static Client *Clients = NULL;

Client *newClient(uint32_t addr, uint16_t port, int conid)
{
  Client *client = (Client *)calloc(1, sizeof(Client));
  client->reply_sock = INVALID_SOCKET;
  client->conid = conid;
  client->addr = addr;
  client->port = port;
  return client;
}

static void Client_push_locked(Client *client)
{
  // should add to tail
  if (!Clients)
  {
    Clients = client;
  }
  else
  {
    Client *c = Clients;
    while (c->next)
      c = c->next;
    c->next = client;
  }
}

static inline void Client_push(Client *client)
{
  LOCK_CLIENTS;
  Client_push_locked(client);
  UNLOCK_CLIENTS;
}

static inline Client *Client_pop_locked(Client *client)
{
  Client *c = Clients;
  Client **p = &Clients; // points to previous->next field
  for (; c; p = &c->next, c = c->next)
  {
    if (c == client)
    {
      *p = c->next;
      pthread_cond_signal(&clients_cond);
      return c;
    }
  }
  return NULL;
}

static inline int Client_free(Client *client, fd_set *fdactive)
{
  if (client)
  {
    const int conid = client->conid;
    MDSDBG(CLIENT_PRI, CLIENT_VAR(client));
    if (client->reply_sock != INVALID_SOCKET)
    {
      shutdown(client->reply_sock, 2);
      close(client->reply_sock);
      if (fdactive)
        FD_CLR(client->reply_sock, fdactive);
    }
    DisconnectFromMds(client->conid);
    free(client);
    return conid;
  }
  return INVALID_CONNECTION_ID;
}

static void Client_cleanup_jobs(Client *c, fd_set *fdactive)
{

  MDSDBG(CLIENT_PRI, CLIENT_VAR(c));
  int conid = Client_free(c, fdactive);
  for (;;)
  {
    Job *j = Job_pop_by_conid(conid);
    if (j)
    {
      Job_callback_done(j, ServerPATH_DOWN, FALSE);
      free(j);
    }
    else
      break;
  }
}

static Client *Client_get_by_addr_and_port_locked(uint32_t addr, uint16_t port)
{
  Client *c;
  for (c = Clients; c; c = c->next)
  {
    if (c->addr == addr && c->port == port)
    {
      break;
    }
  }
  return c;
}

static void Client_remove_locked(Client *c, fd_set *fdactive)
{
  MDSDBG(CLIENT_PRI " removed", CLIENT_VAR(c));
  c = Client_pop_locked(c);
  Client_cleanup_jobs(c, fdactive);
}

static void Client_remove(Client *client, fd_set *fdactive)
{
  Client *c;
  LOCK_CLIENTS;
  c = Client_pop_locked(client);
  UNLOCK_CLIENTS;
  Client_cleanup_jobs(c, fdactive);
}

static void Client_do_message(Client *c, fd_set *fdactive)
{
  char reply[60];
  char *msg = 0;
  int jobid;
  int replyType;
  int status;
  int msglen;
  int num;
  int nbytes;
  nbytes = recv(c->reply_sock, reply, 60, MSG_WAITALL);
  if (nbytes != 60)
  {
    if (nbytes != 0)
      MDSWRN(CLIENT_PRI " Invalid read %d/60", CLIENT_VAR(c), nbytes);
    else
      MDSDBG(CLIENT_PRI " Clint disconnected", CLIENT_VAR(c));
    Client_remove(c, fdactive);
    return;
  }
  num = sscanf(reply, "%d %d %d %d", &jobid, &replyType, &status, &msglen);
  if (num != 4)
  {
    MDSWRN(CLIENT_PRI " Invalid reply format '%-*s'", CLIENT_VAR(c), msglen, reply);
    Client_remove(c, fdactive);
    return;
  }
  FREE_ON_EXIT(msg);
  if (msglen != 0)
  {
    msg = (char *)malloc(msglen + 1);
    msg[msglen] = 0;
    nbytes = recv(c->reply_sock, msg, msglen, MSG_WAITALL);
    if (nbytes != msglen)
    {
      MDSWRN(CLIENT_PRI " Incomplete reply.", CLIENT_VAR(c));
      free(msg);
      Client_remove(c, fdactive);
      return;
    }
  }
  switch (replyType)
  {
  case SrvJobFINISHED:
  {
    Job *j = Job_get_by_jobid(jobid);
    if (!j)
      j = Job_get_by_jobid(MonJob);
    if (j)
    {
      Job_callback_done(j, status, TRUE);
    }
    else
    {
      MDSWRN(CLIENT_PRI " no job to finish", CLIENT_VAR(c));
    }
    break;
  }
  case SrvJobSTARTING:
  {
    Job *j = Job_get_by_jobid(jobid);
    if (!j)
      j = Job_get_by_jobid(MonJob);
    if (j)
    {
      Job_callback_before(j);
    }
    else
    {
      MDSWRN(CLIENT_PRI " no job to start", CLIENT_VAR(c));
    }
    break;
  }
  case SrvJobCHECKEDIN:
    break;
  default:
    MDSWRN(CLIENT_PRI " Invalid reply type %d.", CLIENT_VAR(c), replyType);
    Client_remove(c, fdactive);
  }
  FREE_NOW(msg);
}