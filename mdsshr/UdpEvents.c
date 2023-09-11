/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define _GNU_SOURCE
#include <pthread.h>

#include <mdsplus/mdsconfig.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <sys/types.h>
#include <unistd.h>

#include <libroutines.h>
#include <mdsshr.h>
#include <socket_port.h>
#include <pthread_port.h>
#include <_mdsshr.h>

extern int UdpEventGetPort(unsigned short *port);
extern int UdpEventGetAddress(char **addr_format, unsigned char *arange);
extern int UdpEventGetTtl(char *ttl);
extern int UdpEventGetLoop(char *loop);
extern int UdpEventGetInterface(struct in_addr **interface_addr);

#define MAX_MSG_LEN 4096
#define MAX_EVENTS \
  1000000 /*Maximum number of events handled by a single process */

#ifndef EVENT_THREAD_STACK_SIZE_MIN
#define EVENT_THREAD_STACK_SIZE_MIN 102400
#endif

typedef struct _EventList
{
  int eventid;
  pthread_t thread;
  SOCKET socket;
  struct _EventList *next;
} EventList;

struct EventInfo
{
  SOCKET socket;
  char *eventName;
  void *arg;
  void (*astadr)(void *, int, char *);
};

static EventList *EVENTLIST = 0;
static pthread_mutex_t eventIdMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t sendEventMutex = PTHREAD_MUTEX_INITIALIZER;

/**********************
 Message structure:

- event name len (4 byte int)
- event name (event name len bytes)
- buf len (4 byte int)
- buf (buf len chars)

***********************/

static void *handleMessage(void *info_in)
{

  struct EventInfo *info = (struct EventInfo *)info_in;
  SOCKET socket = info->socket;
  size_t thisNameLen = strlen(info->eventName);
  char *thisEventName = strcpy(alloca(thisNameLen + 1), info->eventName);
  void *arg = info->arg;
  void (*astadr)(void *, int, char *) = info->astadr;
  free(info->eventName);
  free(info);
  INIT_AND_FREE_ON_EXIT(char *, recBuf);
  struct sockaddr clientAddr;
  recBuf = malloc(MAX_MSG_LEN);
  for (;;)
  {
    socklen_t addrSize = sizeof(clientAddr);
    MSG_NOSIGNAL_ALT_PUSH();
    const ssize_t recBytes = recvfrom(
        socket, (char *)recBuf, MAX_MSG_LEN, MSG_NOSIGNAL,
        (struct sockaddr *)&clientAddr, &addrSize);
    MSG_NOSIGNAL_ALT_PUSH();
    if (recBytes <= 0)
    {
#ifdef _WIN32
      int error = WSAGetLastError();
      if (!(recBytes == 0 || error == WSAEBADF || error == WSAESHUTDOWN ||
            error == WSAEINTR || error == WSAENOTSOCK))
        _print_socket_error("Error getting data", error);
#endif
      break;
    }
    if (recBytes < (int)(sizeof(int) * 2 + thisNameLen))
      continue;
    char *currPtr = recBuf;
    uint32_t swap;
    memcpy(&swap, currPtr, sizeof(swap));
    uint32_t nameLen = ntohl(swap);
    if (nameLen != thisNameLen)
      continue;
    currPtr += sizeof(int);
    char *eventName = currPtr;
    currPtr += nameLen;
    memcpy(&swap, currPtr, sizeof(swap));
    uint32_t bufLen = ntohl(swap);
    currPtr += sizeof(int);
    // check for invalid buffer
    if ((size_t)recBytes != (nameLen + bufLen + 2 * sizeof(int)))
      continue;
    // check to see if this message matches the event name
    if (strncmp(thisEventName, eventName, nameLen))
      continue;
    astadr(arg, (int)bufLen, currPtr);
  }
  FREE_NOW(recBuf);
  return NULL;
}

static int pushEvent(pthread_t thread, SOCKET socket)
{
  EventList *ev = malloc(sizeof(EventList));
  ev->socket = socket;
  ev->thread = thread;
  pthread_mutex_lock(&eventIdMutex);
  static int EVENTID = 0;
  ev->eventid = EVENTID++;
  ev->next = EVENTLIST;
  EVENTLIST = ev;
  pthread_mutex_unlock(&eventIdMutex);
  return ev->eventid;
}

static EventList *popEvent(int eventid)
{
  EventList *ev;
  pthread_mutex_lock(&eventIdMutex);
  EventList **prev = &EVENTLIST;
  for (ev = EVENTLIST; ev; prev = &ev->next, ev = ev->next)
  {
    if (ev->eventid == eventid)
    {
      *prev = ev->next;
      break;
    }
  }
  pthread_mutex_unlock(&eventIdMutex);
  return ev;
}

static void getMulticastAddr(char const *eventName, char *retIp)
{
  char *addr_format;
  size_t i, len = strlen(eventName);
  unsigned char arange[2];
  unsigned int hash = 0, hashnew;
  UdpEventGetAddress(&addr_format, arange);
  for (i = 0; i < len; i++)
    hash += (unsigned int)eventName[i];
  hashnew = (unsigned int)((float)arange[0] +
                           ((float)(hash % 256) / 256.) *
                               ((float)arange[1] - (float)arange[0] + 1.));
  sprintf(retIp, addr_format, hashnew);
  free(addr_format);
}

int MDSUdpEventAstMask(char const *eventName, void (*astadr)(void *, int, char *),
                       void *astprm, int *eventid, __attribute__((unused)) unsigned int cpuMask)
{
  struct sockaddr_in serverAddr;
  int one = 1;
  int udpSocket;
  char ipAddress[64];
  struct ip_mreq ipMreq;
  struct EventInfo *currInfo;
  unsigned short port;
  pthread_t thread;
  memset(&ipMreq, 0, sizeof(ipMreq));
  UdpEventGetPort(&port);
  if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    print_socket_error("Error creating socket");
    return MDSplusERROR;
  }
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Allow multiple connections
  if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)))
  {
    print_socket_error("Cannot set REUSEADDR option");
    return MDSplusERROR;
  }

#ifdef SO_REUSEPORT
  if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one)))
  {
    print_socket_error("Cannot set REUSEPORT option");
  }
#endif
  if (bind(udpSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)))
  {
    perror("Cannot bind socket\n");
    return MDSplusERROR;
  }

  getMulticastAddr(eventName, ipAddress);
  ipMreq.imr_multiaddr.s_addr = inet_addr(ipAddress);
  ipMreq.imr_interface.s_addr = INADDR_ANY;
  if (setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreq,
                 sizeof(ipMreq)) < 0)
  {
    print_socket_error(
        "Error setting socket options IP_ADD_MEMBERSHIP in udpStartReceiver");
    return MDSplusERROR;
  }
  currInfo = (struct EventInfo *)malloc(sizeof(struct EventInfo));
  currInfo->eventName = strdup(eventName);
  currInfo->socket = udpSocket;
  currInfo->arg = astprm;
  currInfo->astadr = astadr;

  {
    int s;
    size_t ssize;
    pthread_attr_t attr;
    s = pthread_attr_init(&attr);
    if (s != 0)
    {
      perror("pthread_attr_init");
      return MDSplusERROR;
    }
    pthread_attr_getstacksize(&attr, &ssize);
    if (ssize < EVENT_THREAD_STACK_SIZE_MIN)
    {
      s = pthread_attr_setstacksize(&attr, EVENT_THREAD_STACK_SIZE_MIN);
      if (s != 0)
      {
        perror("pthread_attr_setstacksize");
        return MDSplusERROR;
      }
    }
    s = pthread_create(&thread, &attr, handleMessage, (void *)currInfo);
    if (s != 0)
    {
      perror("pthread_create");
      return MDSplusERROR;
    }
#ifdef CPU_SET
    if (cpuMask != 0)
    {
      cpu_set_t processorCpuSet;
      unsigned int j;
      CPU_ZERO(&processorCpuSet);
      for (j = 0u; (j < (sizeof(cpuMask) * 8u)) && (j < CPU_SETSIZE); j++)
      {
        if (((cpuMask >> j) & 0x1u) == 0x1u)
        {
          CPU_SET(j, &processorCpuSet);
        }
      }
      pthread_setaffinity_np(thread, sizeof(processorCpuSet), &processorCpuSet);
    }
#endif
  }

  *eventid = pushEvent(thread, udpSocket);
  return MDSplusSUCCESS;
}

int MDSUdpEventAst(char const *eventName, void (*astadr)(void *, int, char *),
                   void *astprm, int *eventid)
{
  return MDSUdpEventAstMask(eventName, astadr, astprm, eventid, 0);
}

int MDSUdpEventCan(int eventid)
{
  EventList *ev = popEvent(eventid);
  if (!ev)
  {
    printf("invalid eventid %d\n", eventid);
    return MDSplusERROR;
  }
#ifdef _WIN32
  /**********************************************
   Windows fails on canceling thread in recvfrom.
   Closing the socket causes recvfrom to error
   with WSAEBADF which terminates the thread.
   This however is a race condition so we cancel
   when we can (ifndef _WIN32)
  **********************************************/
  shutdown(ev->socket, SHUT_RDWR);
  closesocket(ev->socket);
#else
  pthread_cancel(ev->thread);
#endif
  pthread_join(ev->thread, NULL);
  free(ev);
  return MDSplusSUCCESS;
}

static SOCKET send_socket = INVALID_SOCKET;
static unsigned short sendPort = 0;
static pthread_once_t send_socket_once = PTHREAD_ONCE_INIT;
static void send_socket_get()
{
  if ((send_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    print_socket_error("Error creating socket");
  UdpEventGetPort(&sendPort);
}

int MDSUdpEvent(char const *eventName, unsigned int bufLen, char const *buf)
{
  char multiIp[64];
  uint32_t buflen_net_order = (uint32_t)htonl(bufLen);
  SOCKET udpSocket;
  struct sockaddr_in sin;
  char *msg = 0, *currPtr;
  unsigned int msgLen, nameLen = (unsigned int)strlen(eventName), actBufLen;
  uint32_t namelen_net_order = (uint32_t)htonl(nameLen);
  int status;
  char ttl, loop;
  struct in_addr *interface_addr = 0;

  getMulticastAddr(eventName, multiIp);
  pthread_once(&send_socket_once, &send_socket_get);
  udpSocket = send_socket;
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  if (_LibGetHostAddr(multiIp, NULL, (struct sockaddr *)&sin))
    return MDSplusERROR;
  sin.sin_port = htons(sendPort);
  nameLen = (unsigned int)strlen(eventName);
  if (bufLen < MAX_MSG_LEN - (4u + 4u + nameLen))
    actBufLen = bufLen;
  else
    actBufLen = MAX_MSG_LEN - (4u + 4u + nameLen);
  msgLen = 4u + nameLen + 4u + actBufLen;
  msg = malloc(msgLen);
  currPtr = msg;

  memcpy(currPtr, &namelen_net_order, sizeof(namelen_net_order));
  currPtr += sizeof(buflen_net_order);

  memcpy(currPtr, eventName, nameLen);
  currPtr += nameLen;

  memcpy(currPtr, &buflen_net_order, sizeof(buflen_net_order));
  currPtr += sizeof(buflen_net_order);

  if (buf && bufLen > 0)
    memcpy(currPtr, buf, bufLen);

  pthread_mutex_lock(&sendEventMutex);

  if (UdpEventGetTtl(&ttl))
    setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
  if (UdpEventGetLoop(&loop))
    setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
  if (UdpEventGetInterface(&interface_addr))
  {
    status = setsockopt(udpSocket, IPPROTO_IP, IP_MULTICAST_IF,
                        (char *)interface_addr, sizeof(*interface_addr));
    free(interface_addr);
  }

  if (sendto(udpSocket, msg, msgLen, 0, (struct sockaddr *)&sin, sizeof(sin)) ==
      -1)
  {
    print_socket_error("Error sending UDP message");
    status = MDSplusERROR;
  }
  else
    status = MDSplusSUCCESS;
  free(msg);
  pthread_mutex_unlock(&sendEventMutex);
  return status;
}

/**********************
 Message structure:

- event name len (4 byte int)
- event name (event name len bytes)
- buf len (4 byte int)
- buf (buf len chars)

***********************/
