#include <config.h>
#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#include <mdsshr.h>
#include <libroutines.h>
#include "mdsshrthreadsafe.h"
extern int UdpEventGetPort(unsigned short *port);
extern int UdpEventGetAddress(char **addr_format, unsigned char *arange);
extern int UdpEventGetTtl(unsigned char *ttl);
extern int UdpEventGetLoop(unsigned char *loop);
extern int UdpEventGetInterface(struct in_addr **interface_addr);

static int releaseEventInfo(void *ptr);

#define MAX_MSG_LEN 4096
#define MAX_EVENTS 1000000	/*Maximum number of events handled by a single process */

static int sendSocket = 0;
static int eventTopIdx = 0;
static void *eventInfos[MAX_EVENTS];

static pthread_mutex_t eventIdMutex;
static int eventIdMutex_initialized = 0;
static pthread_mutex_t sendEventMutex;
static int sendEventMutex_initialized = 0;
static pthread_mutex_t getSocketMutex;
static int getSocketMutex_initialized = 0;
static pthread_mutex_t initializeMutex;
static int initializeMutex_initialized = 0;

extern void InitializeEventSettings();

struct EventInfo {
  int socket;
  pthread_t thread;
  char *eventName;
  void *arg;
  void (*astadr) (void *, int, char *);
};

/**********************
 Message structure:

- event name len (4 byte int)
- event name (event name len bytes)
- buf len (4 byte int)
- buf (buf len chars)

***********************/

static void *handleMessage(void *arg)
{
  int recBytes;
  char recBuf[MAX_MSG_LEN];
  struct sockaddr clientAddr;
  int addrSize = sizeof(clientAddr);
  int nameLen, bufLen;
  char *eventName;
  char *currPtr;
  int thisNameLen;

  struct EventInfo *eventInfo = (struct EventInfo *)arg;
  thisNameLen = strlen(eventInfo->eventName);
  while (1) {
#ifdef _WIN32
    if ((recBytes = recvfrom(eventInfo->socket, (char *)recBuf, MAX_MSG_LEN, 0,
			     (struct sockaddr *)&clientAddr, &addrSize)) < 0) {
      int error = WSAGetLastError();
      if (error == WSAESHUTDOWN || error == WSAEINTR || error == WSAENOTSOCK) {
	break;
      } else {
	fprintf(stderr,"Error getting data - %d\n", error);

      }
      continue;
    }
#else
    if ((recBytes = recvfrom(eventInfo->socket, (char *)recBuf, MAX_MSG_LEN, 0,
			     (struct sockaddr *)&clientAddr, (socklen_t *) & addrSize)) < 0) {
      perror("Error receiving UDP messages\n");
      continue;
    }
#endif
    if (recBytes < (int)(sizeof(int) * 2 + thisNameLen))
      continue;
    currPtr = recBuf;
    nameLen = ntohl(*((unsigned int *)currPtr));
    if (nameLen != thisNameLen)
      continue;
    currPtr += sizeof(int);
    eventName = currPtr;
    currPtr += nameLen;
    bufLen = ntohl(*((unsigned int *)currPtr));
    currPtr += sizeof(int);
    if (recBytes != (nameLen + bufLen + 8)) /*** check for invalid buffer ***/
      continue;
    if (strncmp(eventInfo->eventName, eventName, nameLen))   /*** check to see if this message matches the event name ***/
      continue;
    eventInfo->astadr(eventInfo->arg, bufLen, currPtr);
  }
  return 0;
}

static void initialize()
{
  static int initialized = 0;
  if (!initialized) {

#ifdef _WIN32

    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1, 1);

    WSAStartup(wVersionRequested, &wsaData);
#endif
  }
  LockMdsShrMutex(&initializeMutex, &initializeMutex_initialized);
  InitializeEventSettings();
  UnlockMdsShrMutex(&initializeMutex);
  initialized = 1;
}

static int releaseEventInfo(void *ptr)
{
  int i, status = 0;
  LockMdsShrMutex(&eventIdMutex, &eventIdMutex_initialized);

  for (i = 0; i < eventTopIdx; i++) {
    if (eventInfos[i] == ptr) {
      free(((struct EventInfo *)ptr)->eventName);
      free(ptr);
      eventInfos[i] = 0;
      status = 1;
    }
  }
  UnlockMdsShrMutex(&eventIdMutex);
  return status;
}

static int getEventId(void *ptr)
{
  int i, ans;
  LockMdsShrMutex(&eventIdMutex, &eventIdMutex_initialized);

  if (eventTopIdx >= MAX_EVENTS - 1)	//If top reached, find some hole
  {
    for (i = 0; i < MAX_EVENTS; i++) {
      if (!eventInfos[i]) {
	eventInfos[i] = ptr;
	ans = i + 1;
      }
    }
    if (i == MAX_EVENTS) {
      printf("Too Many events!!");
      ans = 0;
    }
  } else {
    eventInfos[eventTopIdx] = ptr;
    eventTopIdx++;
    ans = eventTopIdx;
  }
  UnlockMdsShrMutex(&eventIdMutex);
  return ans;
}

static struct EventInfo *getEventInfo(int id)
{
  return (id > MAX_EVENTS) ? 0 : (struct EventInfo *)eventInfos[id - 1];
}

static int getSocket()
{
  LockMdsShrMutex(&getSocketMutex, &getSocketMutex_initialized);
  if (!sendSocket) {
    if ((sendSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
#ifdef _WIN32
      int error = WSAGetLastError();
      fprintf(stderr, "Error creating socket - errcode=%d\n", error);
#else
      perror("Error creating socket\n");
#endif
      sendSocket = -1;
    }
  }
  UnlockMdsShrMutex(&getSocketMutex);
  return sendSocket;
}


static void getMulticastAddr(char const *eventName, char *retIp)
{
  char *addr_format;
  int i;
  int len = strlen(eventName);
  unsigned char arange[2];
  unsigned int hash = 0, hashnew;
  UdpEventGetAddress(&addr_format,arange);
  for (i = 0; i < len; i++)
    hash += eventName[i];
  hashnew =
      (unsigned int)((float)arange[0] +
		     ((float)(hash % 256) / 256.) * ((float)arange[1] - (float)arange[0] + 1.));
  sprintf(retIp, addr_format, hashnew);
  free(addr_format);
}

int MDSUdpEventAst(char const *eventName, void (*astadr) (void *, int, char *), void *astprm,
		   int *eventid)
{
  struct sockaddr_in serverAddr;
#ifdef _WIN32
  char flag = 1;
#else
  int flag = 1;
  int const SOCKET_ERROR = -1;
#endif
  int udpSocket;
  char ipAddress[64];
  struct ip_mreq ipMreq;
  struct EventInfo *currInfo;
  unsigned short port;
  memset(&ipMreq, 0, sizeof(ipMreq));

  initialize();

  if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
#ifdef _WIN32
    int error = WSAGetLastError();
    fprintf(stderr, "Error creating socket - errcode=%d\n", error);
#else
    perror("Error creating socket\n");
#endif
    return 0;
  }
  //    serverAddr.sin_len = sizeof(serverAddr);
  serverAddr.sin_family = AF_INET;
  UdpEventGetPort(&port);
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Allow multiple connections
  if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == SOCKET_ERROR) {
    printf("Cannot set REUSEADDR option\n");
#ifdef _WIN32
    switch (WSAGetLastError()) {
    case WSANOTINITIALISED:
      printf("WSAENETDOWN\n");
      break;
    case WSAENETDOWN:
      printf("WSAENETDOWN\n");
      break;
    case WSAEADDRINUSE:
      printf("WSAEADDRINUSE\n");
      break;
    case WSAEINTR:
      printf("WSAEINTR\n");
      break;
    case WSAEINPROGRESS:
      printf("WSAEINPROGRESS\n");
      break;
    case WSAEALREADY:
      printf("WSAEALREADY\n");
      break;
    case WSAEADDRNOTAVAIL:
      printf("WSAEADDRNOTAVAIL\n");
      break;
    case WSAEAFNOSUPPORT:
      printf("WSAEAFNOSUPPORT\n");
      break;
    case WSAECONNREFUSED:
      printf("WSAECONNREFUSED\n");
      break;
    case WSAEFAULT:
      printf("WSAEFAULT\n");
      break;
    default:
      printf("BOH\n");
    }
#else
    perror("\n");
#endif
    return 0;
  }
#ifdef _WIN32
  if (bind(udpSocket, (SOCKADDR *) & serverAddr, sizeof(serverAddr)) != 0)
#else
  if (bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) != 0)
#endif
  {
    perror("Cannot bind socket\n");
    return 0;
  }

  getMulticastAddr(eventName, ipAddress);
  ipMreq.imr_multiaddr.s_addr = inet_addr(ipAddress);
  ipMreq.imr_interface.s_addr = INADDR_ANY;
  if (setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq)) < 0) {
#ifdef _WIN32
    int error = WSAGetLastError();
    char *errorstr = 0;
    switch (error) {
    case WSANOTINITIALISED:
      errorstr = "WSAENETDOWN";
      break;
    case WSAENETDOWN:
      errorstr = "WSAENETDOWN";
      break;
    case WSAEADDRINUSE:
      errorstr = "WSAEADDRINUSE";
      break;
    case WSAEINTR:
      errorstr = "WSAEINTR";
      break;
    case WSAEINPROGRESS:
      errorstr = "WSAEINPROGRESS";
      break;
    case WSAEALREADY:
      errorstr = "WSAEALREADY";
      break;
    case WSAEADDRNOTAVAIL:
      errorstr = "WSAEADDRNOTAVAIL";
      break;
    case WSAEAFNOSUPPORT:
      errorstr = "WSAEAFNOSUPPORT";
      break;
    case WSAECONNREFUSED:
      errorstr = "WSAECONNREFUSED";
      break;
    case WSAENOPROTOOPT:
      errorstr = "WSAENOPROTOOPT";
      break;
    case WSAEFAULT:
      errorstr = "WSAEFAULT";
      break;
    default:
      errorstr = 0;
    }
    if (errorstr)
      fprintf(stderr, "Error setting socket options IP_ADD_MEMBERSHIP in udpStartReceiver - %s\n",
	      errorstr);
    else
      fprintf(stderr,
	      "Error setting socket options IP_ADD_MEMBERSHIP in udpStartReceiver - error code %d\n",
	      error);
#else
    perror("Error setting socket options IP_ADD_MEMBERSHIP in udpStartReceiver\n");
#endif
    return 0;
  }

  currInfo = (struct EventInfo *)malloc(sizeof(struct EventInfo));
  currInfo->eventName = malloc(strlen(eventName) + 1);
  strcpy(currInfo->eventName, eventName);
  currInfo->socket = udpSocket;
  currInfo->arg = astprm;
  currInfo->astadr = astadr;

  pthread_create(&currInfo->thread, 0, handleMessage, (void *)currInfo);
  pthread_detach(currInfo->thread);
  *eventid = getEventId((void *)currInfo);
  return 1;
}

int MDSUdpEventCan(int eventid)
{
  struct EventInfo *currInfo = getEventInfo(eventid);
  if (currInfo) {
    pthread_cancel(currInfo->thread);
#ifndef _WIN32
    close(currInfo->socket);
#else
    shutdown(currInfo->socket, 2);
    closesocket(currInfo->socket);
#endif
    releaseEventInfo(currInfo);
    return 1;
  } else
    return 0;
}

int MDSUdpEvent(char const *eventName, int bufLen, char const *buf)
{
  char multiIp[64];
  int udpSocket;
  struct sockaddr_in sin;
  char *msg=0, *currPtr;
  int msgLen, nameLen, actBufLen;
  int status;
  struct hostent *hp = (struct hostent *)NULL;
  unsigned short port;
  unsigned char ttl,loop;
  struct in_addr *interface_addr=0;

  initialize();
  getMulticastAddr(eventName, multiIp);
  udpSocket = getSocket();

  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  hp = gethostbyname(multiIp);
  if (hp == NULL) {
    unsigned int addr = inet_addr(multiIp);
    if (addr != 0xffffffff)
      hp = gethostbyaddr((const char *)&addr, (int)sizeof(addr), AF_INET);
  }
  if (hp != NULL)
    memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
  UdpEventGetPort(&port);
  sin.sin_port = htons(port);
  nameLen = strlen(eventName);
  if (bufLen < MAX_MSG_LEN - (4 + 4 + nameLen))
    actBufLen = bufLen;
  else
    actBufLen = MAX_MSG_LEN - (4 + 4 + nameLen);
  msgLen = 4 + nameLen + 4 + actBufLen;
  msg = malloc(msgLen);
  currPtr = msg;
  *((unsigned int *)currPtr) = htonl(nameLen);
  currPtr += 4;
  memcpy(currPtr, eventName, nameLen);
  currPtr += nameLen;

  *((unsigned int *)currPtr) = htonl(bufLen);
  currPtr += 4;
  memcpy(currPtr, buf, bufLen);

  LockMdsShrMutex(&sendEventMutex, &sendEventMutex_initialized);
  if (UdpEventGetTtl(&ttl))
    setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
  if (UdpEventGetLoop(&loop))
    setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
  if (UdpEventGetInterface(&interface_addr)) {
    status = setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_IF, interface_addr, sizeof(*interface_addr));
    free(interface_addr);
  } 
  if (sendto(udpSocket, msg, msgLen, 0, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("Error sending UDP message!\n");
#ifdef _WIN32
    switch (WSAGetLastError()) {
    case WSANOTINITIALISED:
      printf("WSAENETDOWN\n");
      break;
    case WSAENETDOWN:
      printf("WSAENETDOWN\n");
      break;
    case WSAEADDRINUSE:
      printf("WSAEADDRINUSE\n");
      break;
    case WSAEINTR:
      printf("WSAEINTR\n");
      break;
    case WSAEINPROGRESS:
      printf("WSAEINPROGRESS\n");
      break;
    case WSAEALREADY:
      printf("WSAEALREADY\n");
      break;
    case WSAEADDRNOTAVAIL:
      printf("WSAEADDRNOTAVAIL\n");
      break;
    case WSAEAFNOSUPPORT:
      printf("WSAEAFNOSUPPORT\n");
      break;
    case WSAECONNREFUSED:
      printf("WSAECONNREFUSED\n");
      break;
    case WSAEFAULT:
      printf("WSAEFAULT\n");
      break;
    default:
      printf("Unknown error occurred while sending event msg.\n");
    }
#endif
    status = 0;
  } else
    status = 1;
  if (msg)
    free(msg);
  UnlockMdsShrMutex(&sendEventMutex);
  return status;
}

/**********************
 Message structure:

- event name len (4 byte int)
- event name (event name len bytes)
- buf len (4 byte int)
- buf (buf len chars)

***********************/

