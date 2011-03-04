#include <config.h>
#ifdef HAVE_WINDOWS_H
#include <winsock.h>
#include <stdio.h>
#else

#include <stdlib.h>
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostlib.h>
#include <stdio.h>
#include <semLib.h>
#else
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <errno.h>
#endif

#endif
#include <mdsshr.h>
#include "libroutines.h"
#include "mdsshrthreadsafe.h"

static int releaseEventInfo(void *ptr);


//int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid) {}
//int MDSEventCan(void *eventid) {}
//int MDSEvent(char *evname){}

#define MULTICAST_EVENT_PORT 4000
#define MAX_MSG_LEN 4096
#define MAX_EVENTS 1000000 /*Maximum number of events handled by a single process*/






#ifdef HAVE_WINDOWS_H
static unsigned long *eventIdMutex;
static int eventIdMutex_initialized = 0;
static unsigned long *sendEventMutex;
static int sendEventMutex_initialized = 0;
static unsigned long *getSocketMutex;
static int getSocketMutex_initialized = 0;
static unsigned long *getPortMutex;
static int getPortMutex_initialized = 0;
static int eventTopIdx = 0; 
static void *eventInfos[MAX_EVENTS];
static int sendSocket = 0;
static int udpPort = -1;
#else
static pthread_mutex_t eventIdMutex;
static int eventIdMutex_initialized = 0;
static pthread_mutex_t sendEventMutex;
static int sendEventMutex_initialized = 0;
static pthread_mutex_t getSocketMutex;
static int getSocketMutex_initialized = 0;
static pthread_mutex_t getPortMutex;
static int getPortMutex_initialized = 0;
static int eventTopIdx = 0; 
static void *eventInfos[MAX_EVENTS];
static int sendSocket = 0;
static int udpPort = -1;
#endif

struct EventInfo {
	int socket;
	pthread_t thread;
	char *eventName;
	void *arg;
	void (*astadr)(void *,int,char *);
};

/**********************
 Message structure:

- event name len (4 byte int)
- event name (event name len bytes)
- buf len (4 byte int)
- buf (buf len chars)

***********************/

#ifdef HAVE_WINDOWS_H
extern int pthread_create(pthread_t  *thread, void *dummy, void (*rtn)(void *), void *rtn_param);
#endif

#ifdef HAVE_WINDOWS_H
static void handleMessage(void *arg)
#else
static void *handleMessage(void *arg)
#endif
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
	thisNameLen=strlen(eventInfo->eventName);
	while(1)
	{
#ifdef HAVE_WINDOWS_H
		if((recBytes = recvfrom(eventInfo->socket, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, &addrSize)) < 0) 
				if (WSAGetLastError() == WSAESHUTDOWN){
					return;
				} else
#else
#ifdef HAVE_VXWORKS_H
		if((recBytes = recvfrom(eventInfo->socket, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, &addrSize)) < 0)
#else
		if((recBytes = recvfrom(eventInfo->socket, (char *)recBuf, MAX_MSG_LEN, 0, 
			(struct sockaddr *)&clientAddr, (socklen_t *)&addrSize)) < 0)
#endif
#endif
    	{
			perror("Error receiving UDP messages\n");
			continue;
        }
    	
		if (recBytes < (int)(sizeof(int)*2+thisNameLen))
		  continue;
		currPtr = recBuf;
		nameLen = ntohl(*((unsigned int *)currPtr));
		if (nameLen != thisNameLen)
		  continue;
		currPtr += sizeof(int);
                eventName=currPtr;
		currPtr += nameLen;
		bufLen = ntohl(*((unsigned int *)currPtr));
		currPtr += sizeof(int);
                if (recBytes != (nameLen+bufLen+8)) /*** check for invalid buffer ***/
		  continue;
                if (strncmp(eventInfo->eventName,eventName,nameLen)) /*** check to see if this message matches the event name ***/
		  continue;
		eventInfo->astadr(eventInfo->arg, bufLen, currPtr);
	}
}

static void initialize()
{
	static int initialized = 0;
	#ifdef HAVE_WINDOWS_H

		WSADATA wsaData;
		WORD wVersionRequested;
		wVersionRequested = MAKEWORD(1,1);

		if(!initialized)
		{
			initialized = 1;
			WSAStartup(wVersionRequested,&wsaData);
		}

#endif 
}

static int releaseEventInfo(void *ptr)
{
	int i,status=0;
	LockMdsShrMutex(&eventIdMutex,&eventIdMutex_initialized);

	for(i = 0; i < eventTopIdx; i++)
	{
		if(eventInfos[i] == ptr)
		{
		  free(((struct EventInfo *)ptr)->eventName);
		  free(ptr);
		  eventInfos[i] = 0;
		  status=1;
		}
	}
	UnlockMdsShrMutex(&eventIdMutex);
	return status;
}

static int getEventId(void *ptr)
{
	int i,ans;
	LockMdsShrMutex(&eventIdMutex,&eventIdMutex_initialized);

	if(eventTopIdx >= MAX_EVENTS - 1) //If top reached, find some hole
	{
		for(i = 0; i < MAX_EVENTS; i++)
		{
			if(!eventInfos[i])
			{
				eventInfos[i] = ptr;
				ans = i+1;
			}
		}
                if (i == MAX_EVENTS)
		{
			printf("Too Many events!!");
			ans = 0;
		}
	}
        else
	{
		eventInfos[eventTopIdx] = ptr;
		eventTopIdx++;
		ans= eventTopIdx;
	}
	UnlockMdsShrMutex(&eventIdMutex);
	return ans;
}


static struct EventInfo * getEventInfo(int id)
{
	return (id > MAX_EVENTS) ? 0 : (struct EventInfo *)eventInfos[id-1];
}


static int getSocket()
{
  LockMdsShrMutex(&getSocketMutex,&getSocketMutex_initialized);
  if(!sendSocket)
    {
      if((sendSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
	  perror("Error creating socket\n");
	  sendSocket = -1;
	}
    }
    UnlockMdsShrMutex(&getSocketMutex);
    return sendSocket;
}


static int getPort()
{
	char *portStr;
	struct servent *serverInfo;
	LockMdsShrMutex(&getPortMutex,&getPortMutex_initialized);
	if(udpPort == -1)
	{
		portStr = getenv("mdsevent_port");
		if(portStr) 
		{
			sscanf(portStr, "%d", &udpPort);
		}
		else
		{
			serverInfo = getservbyname("mdsplus_event", "udp");
			if(serverInfo)
				udpPort = serverInfo->s_port;
			else
				udpPort = MULTICAST_EVENT_PORT;
		}
	}
    UnlockMdsShrMutex(&getPortMutex);
	return udpPort;
}


static void getMulticastAddr(char *eventName, char *retIp)
{
	int i;
	int len = strlen(eventName);
	unsigned int hash = 0;
	for(i = 0; i < len; i++)
		hash += eventName[i];
	sprintf(retIp, "225.0.0.%d", hash%256);
}


int MDSUdpEventAst(char *eventName, void (*astadr)(void *,int,char *), void *astprm, int *eventid)
{

    struct sockaddr_in serverAddr;
	char flag = 1;
	int intFlag = 1;
	int udpSocket;
	char ipAddress[64]; 
    struct ip_mreq ipMreq;
	struct EventInfo *currInfo;
	int error;

	initialize();

	if((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Error creating socket\n");
		return 0;
	}
	
	//    serverAddr.sin_len = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
#ifdef HAVE_WXWORKS_H
    serverAddr.sin_len = (u_char)sizeof(struct sockaddr_in);
#endif
    serverAddr.sin_port = htons(getPort());
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

//Allow multiple connections
#ifdef HAVE_WINDOWS_H
   if(setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(char)) == SOCKET_ERROR)
#else
   if(setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &intFlag, sizeof(int)) < 0)
#endif
	{   
		printf("Cannot set REUSEADDR option\n");
#ifdef HAVE_WINDOWS_H
		error = WSAGetLastError();
		switch(error)
		{
			case WSANOTINITIALISED: printf("WSAENETDOWN\n"); break;
			case WSAENETDOWN: printf("WSAENETDOWN\n"); break; 
			case WSAEADDRINUSE: printf("WSAEADDRINUSE\n"); break;
			case WSAEINTR : printf("WSAEINTR\n"); break;
			case WSAEINPROGRESS: printf("WSAEINPROGRESS\n"); break;
			case WSAEALREADY: printf("WSAEALREADY\n"); break;
			case WSAEADDRNOTAVAIL: printf("WSAEADDRNOTAVAIL\n"); break;
			case WSAEAFNOSUPPORT: printf("WSAEAFNOSUPPORT\n"); break;
			case WSAECONNREFUSED : printf("WSAECONNREFUSED\n"); break;
			case WSAEFAULT : printf("WSAEFAULT\n"); break;
			default: printf("BOH\n");
		}
#else
	perror("\n");
#endif
		return 0;
	}

#ifdef HAVE_WINDOWS_H
	if(bind(udpSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) != 0)
#else
	if(bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) != 0)
#endif
	{   
		perror("Cannot bind socket\n");
		return 0;
	}

	getMulticastAddr(eventName, ipAddress);
	ipMreq.imr_multiaddr.s_addr = inet_addr(ipAddress);
    ipMreq.imr_interface.s_addr = INADDR_ANY;
	if(setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq)) < 0)
    {	
  	   	perror("Error setting socket options IP_ADD_MEMBERSHIPin udpStartReceiver\n");
#ifdef HAVE_WINDOWS_H
		error = WSAGetLastError();
		switch(error)
		{
			case WSANOTINITIALISED: printf("WSAENETDOWN\n"); break;
			case WSAENETDOWN: printf("WSAENETDOWN\n"); break; 
			case WSAEADDRINUSE: printf("WSAEADDRINUSE\n"); break;
			case WSAEINTR : printf("WSAEINTR\n"); break;
			case WSAEINPROGRESS: printf("WSAEINPROGRESS\n"); break;
			case WSAEALREADY: printf("WSAEALREADY\n"); break;
			case WSAEADDRNOTAVAIL: printf("WSAEADDRNOTAVAIL\n"); break;
			case WSAEAFNOSUPPORT: printf("WSAEAFNOSUPPORT\n"); break;
			case WSAECONNREFUSED : printf("WSAECONNREFUSED\n"); break;
			case WSAEFAULT : printf("WSAEFAULT\n"); break;
			default: printf("BOH\n");
		}
#endif
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
#ifndef HAVE_WINDOWS_H
	  pthread_cancel(currInfo->thread);
	  close(currInfo->socket);
#else
	  shutdown(currInfo->socket,2);
	  closesocket(currInfo->socket);
#endif
	  releaseEventInfo(currInfo);
	  return 1;
	} else
	  return 0;
}


int MDSUdpEvent(char *evName, int bufLen, char *buf)
{
	char multiIp[64];
	int udpSocket;
	struct sockaddr_in sin;
	char *msg, *currPtr;
	int msgLen, nameLen, actBufLen;
	int status, error;
	struct hostent *hp =(struct hostent *) NULL;

	initialize();
	getMulticastAddr(evName, multiIp);
	udpSocket = getSocket();


#ifdef HAVE_VXWORKS_H
	bzero((char *)&sin, sizeof(struct sockaddr_in));
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons( getPort() );
	sin.sin_len = (u_char)(sizeof(struct sockaddr_in));

      	if(((sin.sin_addr.s_addr = inet_addr(multiIp)) == ERROR) &&
	     	((sin.sin_addr.s_addr = hostGetByName(multiIp)) == ERROR))  

	    	perror("Unknown recipient name in IP address  initialization\n");
#else
	memset((char *)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	hp = gethostbyname(multiIp);
	if (hp == NULL)
	{
		unsigned int addr = inet_addr(multiIp);
		if (addr != 0xffffffff)
    		hp = gethostbyaddr((const char *) &addr, (int) sizeof(addr), AF_INET);
	}
	if(hp != NULL)
		memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
	sin.sin_port = htons( getPort() );
#endif
	nameLen = strlen(evName);
	if(bufLen < MAX_MSG_LEN - (4 + 4 + nameLen))
		actBufLen = bufLen;
	else
		actBufLen = MAX_MSG_LEN - (4 + 4 + nameLen);
	msgLen = 4 + nameLen + 4 + actBufLen;
	msg = malloc(msgLen);
	currPtr = msg;
	*((unsigned int *)currPtr) = htonl(nameLen);
	currPtr += 4;
	memcpy(currPtr, evName, nameLen);
	currPtr += nameLen;


	*((unsigned int *)currPtr) = htonl(bufLen);
	currPtr += 4;
	memcpy(currPtr, buf, bufLen);

	LockMdsShrMutex(&sendEventMutex,&sendEventMutex_initialized);
    if(sendto(udpSocket, msg, msgLen, 0, 
		(struct sockaddr *)&sin, sizeof(sin))==-1)
    {
		perror("Error sending UDP message!\n");
#ifdef HAVE_WINDOWS_H
		error = WSAGetLastError();
		switch(error)
		{
			case WSANOTINITIALISED: printf("WSAENETDOWN\n"); break;
			case WSAENETDOWN: printf("WSAENETDOWN\n"); break; 
			case WSAEADDRINUSE: printf("WSAEADDRINUSE\n"); break;
			case WSAEINTR : printf("WSAEINTR\n"); break;
			case WSAEINPROGRESS: printf("WSAEINPROGRESS\n"); break;
			case WSAEALREADY: printf("WSAEALREADY\n"); break;
			case WSAEADDRNOTAVAIL: printf("WSAEADDRNOTAVAIL\n"); break;
			case WSAEAFNOSUPPORT: printf("WSAEAFNOSUPPORT\n"); break;
			case WSAECONNREFUSED : printf("WSAECONNREFUSED\n"); break;
			case WSAEFAULT : printf("WSAEFAULT\n"); break;
			default: printf("BOH\n");
		}
#endif
		status =  0;
    }
	else
		status = 1;
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


/*

unsigned int UDPNetworkManager::fromNative(unsigned int n)
{
	return htonl(n);
}

unsigned int UDPNetworkManager::toNative(unsigned int n)
{
	return ntohl(n);
}


*/
