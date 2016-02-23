#include <unistd.h>
#include <stdarg.h>
#include <syscall.h>
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
    _new_unique_event_name("event_%s_%d_%d",__FILE__,__LINE__,getpid())
static char * _new_unique_event_name(const char *prefix, ...) {
    char buffer[300];
    va_list args;
    va_start(args, prefix);
    vsnprintf(buffer,300,prefix,args);
    va_end(args);
    return strdup(buffer);
}

static int astCount = 0;
void eventAst(void *arg, int len, char *buf) {
    printf("received event in thread %d, name=%s\n",
           syscall(__NR_gettid),
           (char *)arg);    
    astCount++;    
}

void * set_event(void* arg) {
    char *eventName = strdup((const char*)arg);
    sleep(1);    
    MDSUdpEvent(eventName,strlen(eventName),eventName);
    printf("set_event: %s\n",eventName);
    fflush(stdout);
    free(eventName);
}

////////////////////////////////////////////////////////////////////////////////
//  static parts from UdpEvents   //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static void initialize();
static void *handleMessage(void *info_in);
static int pushEvent(pthread_t thread, int socket);
static EventList *popEvent(int eventid);
static int getSendSocket();
static void getMulticastAddr(char const *eventName, char *retIp);




////////////////////////////////////////////////////////////////////////////////
//  test functions  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void test_initialize() {
    BEGIN_TESTING(UdpEvents initialize);
    initialize();
    SKIP_TEST("not implemented yet");
    END_TESTING;
}

void test_handleMessage() {
    BEGIN_TESTING(UdpEvents handleMessage);
        
    
    //    char * eventName = new_unique_event_name("test_event");
    char * eventName = strdup("event");
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
    
    initialize();

    // create socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    TEST1(udpSocket > 0);
    
    // set address
    serverAddr.sin_family = AF_INET;
    UdpEventGetPort(&port);
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
    currInfo->eventName = eventName;
    currInfo->socket = udpSocket;
    currInfo->arg = eventName;
    currInfo->astadr = &eventAst;
    
//    pthread_t thread;
//    pthread_create(&thread,NULL,set_event,"event");    
//    printf("event name: %s\n",eventName);
//    fflush(stdout);
//    handleMessage(currInfo);
//    pthread_join(thread,NULL);
    
    free(currInfo->eventName);
    free(currInfo);
    //    *eventid = pushEvent(thread, udpSocket);
    
    END_TESTING;
}


int main(int argc, char *argv[])
{    
    test_initialize();
    test_handleMessage();
    return 0;
}
