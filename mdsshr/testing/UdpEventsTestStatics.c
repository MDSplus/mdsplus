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
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../UdpEvents.c"
#include <mdsmsg.h>
#include "testing.h"

////////////////////////////////////////////////////////////////////////////////
//  utils   ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define new_unique_event_name(str) \
  _new_unique_event_name("%s_%d_%d", str, __LINE__, getpid())
static char *_new_unique_event_name(const char *prefix, ...)
{
  char buffer[300];
  va_list args;
  va_start(args, prefix);
  vsnprintf(buffer, 300, prefix, args);
  va_end(args);
  return strdup(buffer);
}

pthread_mutex_t astCount_mutex = PTHREAD_MUTEX_INITIALIZER;
static int astCount = 0;
void eventAst(void *arg, int len, char *buf)
{
  printf("received event in thread %ld, name=%s, len=%d\n", CURRENT_THREAD_ID(), (char *)arg, len);
  char access = 0;
  int i;
  for (i = 0; i < len; i++)
  { // this will trigger asan if len is invalid
    access ^= buf[i];
  }
  (void)access;  // silence set but unused warning
  pthread_mutex_lock(&astCount_mutex);
  astCount++;
  pthread_mutex_unlock(&astCount_mutex);
  pthread_exit(0);
}

////////////////////////////////////////////////////////////////////////////////
//  static parts from UdpEvents   //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static void *handleMessage(void *info_in);
static int pushEvent(pthread_t thread, SOCKET socket);
static EventList *popEvent(int eventid);
static void getMulticastAddr(char const *eventName, char *retIp);

////////////////////////////////////////////////////////////////////////////////
//  test functions  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief test_handleMessage tests static void *handleMessage(void *info_in);
/// handleMessage is the start function of the waiting thread .. it loops over
/// the receive action from socket and eventually triggers the Ast function if
/// the event matches the proper name. This tests mimics the UdpEventAst
/// fucntion but it doesn't detach the waiting thread, this seems to prevent
/// memory allocation errors in valgrind.
///
void test_handleMessage()
{
  BEGIN_TESTING(UdpEvents handleMessage);

  char *eventName = new_unique_event_name("test_event");
  struct sockaddr_in serverAddr;
  int one = 1;
  SOCKET udpSocket;
  char ipAddress[64];
  struct ip_mreq ipMreq;
  struct EventInfo *currInfo;
  unsigned short port;
  memset(&ipMreq, 0, sizeof(ipMreq));
  UdpEventGetPort(&port);

  // create socket
  udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
  TEST1(udpSocket != INVALID_SOCKET);

  // set address
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Allow multiple connections
  TEST0(setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)));

  // bind
  TEST0(bind(udpSocket, (void *)&serverAddr, sizeof(serverAddr)));

  getMulticastAddr(eventName, ipAddress);
  ipMreq.imr_multiaddr.s_addr = inet_addr(ipAddress);
  ipMreq.imr_interface.s_addr = INADDR_ANY;
  TEST0(setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq)));

  currInfo = (struct EventInfo *)malloc(sizeof(struct EventInfo));
  currInfo->eventName = strdup(eventName);
  currInfo->socket = udpSocket;
  currInfo->arg = eventName;
  currInfo->astadr = &eventAst;

  pthread_t thread;
  pthread_create(&thread, NULL, handleMessage, currInfo);
  MDSUdpEvent(eventName, strlen(eventName), eventName);
  pthread_join(thread, NULL);
  free(eventName);
  END_TESTING;
}

////////////////////////////////////////////////////////////////////////////////
//  PUSH AND POP  //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static struct _list_el
{
  pthread_t thread;
  int id;
} list[10];

static void *_push_handler(void *arg)
{
  struct _list_el *li = (struct _list_el *)arg;
  li->id = pushEvent(li->thread, 0);
  return NULL;
}

void test_pushEvent()
{
  BEGIN_TESTING(UpdEvents pushEvent);
  printf("pushEvent test\n");
  int i;
  for (i = 0; i < 10; ++i)
    pthread_create(&list[i].thread, NULL, _push_handler, &list[i]);
  for (i = 0; i < 10; ++i)
    pthread_join(list[i].thread, 0);
  END_TESTING
}

static void *_pop_handler(void *arg)
{
  struct _list_el *li = (struct _list_el *)arg;
  EventList *ev = popEvent(li->id);
  free(ev);
  return NULL;
}

void test_popEvent()
{
  BEGIN_TESTING(UpdEvents popEvent);
  printf("popEvent test\n");
  int i;
  for (i = 0; i < 10; ++i)
    pthread_create(&list[i].thread, NULL, _pop_handler, &list[i]);
  for (i = 0; i < 10; ++i)
    pthread_join(list[i].thread, 0);
  END_TESTING
}

////////////////////////////////////////////////////////////////////////////////
//  MAIN   /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
  // Override the default port in eventsConfig.xml so tests can run in parallel
  int test_index = 0;
  char * test_index_env = getenv("TEST_INDEX");
  if (test_index_env) {
    test_index = atoi(test_index_env);
  }

  int port = 8000 + test_index;
  char port_str[12];
  snprintf(port_str, sizeof(port_str), "%d", port);

  setenv("mdsevent_port", port_str, 1); 

  test_handleMessage();
  test_pushEvent();
  test_popEvent();
  return 0;
}
