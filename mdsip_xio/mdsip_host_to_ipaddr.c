#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int mdsip_host_to_ipaddr(char *host, int *addr, short *port)
{
  struct hostent *hp = NULL;
  struct servent *sp;

  char hostpart[256] = {0};
  char portpart[256] = {0};
  int i;

  sscanf(host,"%[^:]:%s",hostpart,portpart);
  if (strlen(portpart) == 0)
  {
    if (host[0] == '_')
      strcpy(portpart,"mdsips");
    else
      strcpy(portpart,"mdsip");
  }
  if (strcmp(hostpart, "local") == 0) {
    *addr = 0;
    *port = 0;
    return 1;
  }
  if (hostpart[0] == '_')
  {
    for (i=0;hostpart[i] != 0;i++)
      hostpart[i]=hostpart[i+1];
  }
  hp = gethostbyname(hostpart);
  if (hp == NULL)
  {
    *addr = inet_addr(hostpart);
    if (*addr != 0xffffffff)
      hp = gethostbyaddr((void *) addr, (int) sizeof(* addr), AF_INET);
  }
  if (hp == NULL)
  {
    fprintf(stderr,"Unknown host: %s\n",hostpart);
    return -1;
  }
  if (atoi(portpart) == 0)
  {
    sp = getservbyname(portpart,"tcp");
    if (sp != NULL)
      *port = ntohs(sp->s_port);
    else
    {
      char *portc = getenv(portpart);
      *port = (portc == NULL) ? 8000 : (short)atoi(portc);
    }
  }
  else
    *port = (short)atoi(portpart);
  if (*port == 0)
  {
    fprintf(stderr,"Unknown service: %s\n",portpart);
    return -1;
  }
  memcpy(addr, hp->h_addr_list[0], sizeof(*addr));
  return 1;
}
