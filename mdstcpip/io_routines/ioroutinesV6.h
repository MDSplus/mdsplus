#define AF_T AF_INET6
#define PF_T AF_INET6
#define PORTDELIM '#'
#define SOCKADDR_IN sockaddr_in6
#define SIN_FAMILY sin6_family
#define SIN_PORT sin6_port
#define _INADDR_ANY in6addr_any
#define GET_IPHOST(sin)          \
  char iphost[INET6_ADDRSTRLEN]; \
  inet_ntop(AF_INET6, &sin.sin6_addr, iphost, INET6_ADDRSTRLEN)

#include <mdsplus/mdsconfig.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#include <socket_port.h>
DEFINE_INITIALIZESOCKETS;

#include "../mdsip_connections.h"
#include <mdsmsg.h>

static int GetHostAndPort(char *hostin, struct sockaddr *sin);

static int io_reuseCheck(char *host, char *unique, size_t buflen)
{
  struct sockaddr_in6 sin;
  if (IS_OK(GetHostAndPort(host, (struct sockaddr *)&sin)))
  {
    uint16_t *addr = (uint16_t *)&sin.sin6_addr;
    snprintf(unique, buflen,
             "%s://%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x#%u", PROT, addr[0],
             addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7],
             (unsigned)ntohs(sin.sin6_port));
    return C_OK;
  }
  else
  {
    *unique = 0;
    return C_ERROR;
  }
}
