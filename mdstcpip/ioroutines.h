#define AF_T AF_INET
#define PF_T PF_INET
#define PORTDELIM ':'
#define SOCKADDR_IN sockaddr_in
#define SIN_FAMILY sin_family
#define SIN_ADDR sin_addr.s_addr
#define SIN_PORT sin_port
#define _INADDR_ANY INADDR_ANY
#define GET_IPHOST(sin) char *iphost = inet_ntoa(sin.sin_addr)

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

#include <STATICdef.h>
#include <socket_port.h>

#include "mdsip_connections.h"

static int GetHostAndPort(char *hostin, struct sockaddr_in *sin);

static int io_reuseCheck(char *host, char *unique, size_t buflen) {
  struct sockaddr_in sin;
  if
    IS_OK(GetHostAndPort(host, &sin)) {
      uint8_t *addr = (uint8_t *)&sin.sin_addr;
      snprintf(unique, buflen, "%s://%u.%u.%u.%u:%u", PROT, addr[0], addr[1],
               addr[2], addr[3], (unsigned)ntohs(sin.sin_port));
      return C_OK;
    }
  *unique = 0;
  return C_ERROR;
}
