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
#include <unistd.h>
#include <stdarg.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#define syscall(__NR_gettid) GetCurrentThreadId()
#else
#include <sys/syscall.h>
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include <../UdpEvents.c>
#include "testing.h"


////////////////////////////////////////////////////////////////////////////////
//  utils   ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#define new_unique_event_name(str) \
    _new_unique_event_name("%s_%d_%d",str,__LINE__,getpid())
static char * _new_unique_event_name(const char *prefix, ...) {
    char buffer[300];
    va_list args;
    va_start(args, prefix);
    vsnprintf(buffer,300,prefix,args);
    va_end(args);
    return strdup(buffer);
}

static int astCount = 0;
void eventAst(void *arg, int len __attribute__ ((unused)), char *buf __attribute__ ((unused)) ) {
    printf("received event in thread %ld, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);
    astCount++;
    pthread_exit(0);
}

////////////////////////////////////////////////////////////////////////////////
//  static parts from UdpEvents   //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static void *handleMessage(void *info_in);
static int pushEvent(pthread_t thread, int socket);
static EventList *popEvent(int eventid);
static int getSendSocket();
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
void test_handleMessage() {
    BEGIN_TESTING(UdpEvents handleMessage);

    char * eventName = new_unique_event_name("test_event");
    //    char * eventName = strdup("event");
    struct sockaddr_in serverAddr;
#   ifdef _WIN32
    char flag = 1;
#   else
    int flag = 1;
    int const SOCKET_ERROR = -1;
#   endif
    int udpSocket;
    char ipAddress[64];
    struct ip_mreq ipMreq;
    struct EventInfo *currInfo;
    unsigned short port;
    memset(&ipMreq, 0, sizeof(ipMreq));
    UdpEventGetPort(&port);

    // create socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    TEST1(udpSocket > 0);

    // set address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Allow multiple connections
    TEST1(setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) != SOCKET_ERROR);

    // bind
#   ifdef _WIN32
    TEST0( bind(udpSocket, (SOCKADDR *) & serverAddr, sizeof(serverAddr)) );
#   else
    TEST0( bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) );
#   endif

    getMulticastAddr(eventName, ipAddress);
    ipMreq.imr_multiaddr.s_addr = inet_addr(ipAddress);
    ipMreq.imr_interface.s_addr = INADDR_ANY;
    TEST0(setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq)) < 0);

    currInfo = (struct EventInfo *)malloc(sizeof(struct EventInfo));
    currInfo->eventName = strdup(eventName);
    currInfo->socket = udpSocket;
    currInfo->arg = eventName;
    currInfo->astadr = &eventAst;

    pthread_t thread;
    pthread_create(&thread,NULL,handleMessage,currInfo);
    usleep(200000);
    MDSUdpEvent(eventName,strlen(eventName),eventName);
    pthread_join(thread,NULL);

    free (eventName);
    //    free(currInfo->eventName);
    //    free(currInfo);
    //    *eventid = pushEvent(thread, udpSocket);

    END_TESTING;
}


////////////////////////////////////////////////////////////////////////////////
//  PUSH AND POP  //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static struct _list_el {
    pthread_t thread;
    int id;
} list[10];

static void *_push_handler(void *arg) {
    struct _list_el *li = (struct _list_el *)arg;
    li->id = pushEvent(li->thread,0);
    return NULL;
}

void test_pushEvent() {
    BEGIN_TESTING(UpdEvents pushEvent);
    printf("pushEvent test\n");
    int i;
    for(i=0; i<10; ++i)
        pthread_create(&list[i].thread,NULL,_push_handler,&list[i]);
    for(i=0; i<10; ++i)
        pthread_join(list[i].thread,0);
    END_TESTING
}

static void *_pop_handler(void *arg) {
    struct _list_el *li = (struct _list_el *)arg;
    EventList *ev = popEvent(li->id);
    free(ev);
    return NULL;
}

void test_popEvent() {
    BEGIN_TESTING(UpdEvents popEvent);
    printf("popEvent test\n");
    int i;
    for(i=0; i<10; ++i)
        pthread_create(&list[i].thread,NULL,_pop_handler,&list[i]);
    for(i=0; i<10; ++i)
        pthread_join(list[i].thread,0);
    END_TESTING
}



////////////////////////////////////////////////////////////////////////////////
//  Suppression  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static void * _thread_action(void *arg) {
    (void)arg;
    int status __attribute__ ((unused));
    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
    status = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
    while(1) {
        // do nothing .. //
    }
    return NULL;
}

void test_pthread_cancel_Suppresstion() {
    pthread_t thread[10];
    int i;
    for(i=0; i<10; ++i) {
        pthread_create(&thread[i],NULL,_thread_action,NULL);
        pthread_detach(thread[i]);
    }
    usleep(10000);
    for(i=0; i<10; ++i) {
        while( pthread_cancel(thread[i]) != 0 );
    }
}



////////////////////////////////////////////////////////////////////////////////
//  MAIN   /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    test_handleMessage();
    test_pushEvent();
    test_popEvent();

    // generate a suppression for pthread_cancel valgrind issue //
    //test_pthread_cancel_Suppresstion();


    return 0;
}
