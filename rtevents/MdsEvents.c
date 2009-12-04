#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ipdesc.h>
#include <mdsdescrip.h>
#include <strroutines.h>
#include <string.h>
#include "SystemSpecific.h"
#include "../mdsshr/mdsshrthreadsafe.h"

EXPORT void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int, void *), void *callbackArg);
EXPORT int MdsEventTriggerAndWait(char *name, char *buf, int size);





extern void EventWait(char *name, char *buf, int size, int *retSize);
extern void EventReset();
extern void * EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, void *, char, int, char *, int), void *callbackArg);
extern void * EventAddListener(char *name,  void (*callback)(char *, char *, int, void *, char, int, char*, int), void *callbackArg);
extern void EventRemoveListener(void *eventHandler);
extern int EventTrigger(char *name, char *buf, int size);
extern int EventTriggerAndWait(char *name, char *buf, int size);
extern int EventTriggerAndTimedWait(char *name, char *buf, int size, int millisecs);
extern void EventClean();

extern char *EventGetSerializedState(int *size);
extern char *EventGetSerializedMemState(int *size);



#define MAX_IP 256
//NOTE in the following Client means the entity which registers for an event while Server refers to entities 
//which receive the mdsip registration message. When an event is issued, the server will receive it on the 
//local machine and will send a Event mdsip message to the Client.

//Struct ClientIpDescriptor describes the mdsip connections which are used when registering to any event
//in order to broadcast the registration message
//It is created only once, at the first event registration and its content is taken from env variable REVENT_MDSIP_LIST

//Struct ServerIpDescriptor describes the open mdsip connections at the server side. Note that the server may not define
//env variable REVENT_MDSIP_LIST, i.e. it may not register for events.
//It is used to avoid opening a mdsip connection for every received event registration message

struct IpDescriptor
{
	char *ipAddr;
	SOCKET id;
};

static struct IpDescriptor clientIpDescriptors[MAX_IP];
static int numClientIpDescriptors;

static struct IpDescriptor serverIpDescriptors[MAX_IP];
static int numServerIpDescriptors;



//Struct ClientEventDescriptor describes the single registered event. It is used to keep a reference count
//which is incremented every time someone registers for the event and decremented upon event deregistration
//When the reference count goes to zoreo a unregistration message is broadcast
struct ClientEventDescriptor
{
	char *eventName;
	int refCount;
	struct ClientEventDescriptor *nxt;
};
static struct ClientEventDescriptor *clientEventDescrHead = 0;


//Struct ServerEventDescriptor describes an event for which soemone on another computer registered
//it is updated every time a registration message is received, and is used every time such an event 
//is issued locally in order to send Event message to inetrested listeners 
//(i.e. machines where someone registered for the event)
struct ServerEventDescriptor 
{
	char *eventName;
	char busy;
	int numIp;
	char *ipAddrs[MAX_IP];
	SOCKET ids[MAX_IP];
	void *eventId;
	struct ServerEventDescriptor *nxt;
};


static struct ServerEventDescriptor *serverEventDescrHead = 0;

static unsigned long *eventMutex;
static int eventMutex_initialized = 0;
static void lock()
{
	LockMdsShrMutex(&eventMutex,&eventMutex_initialized);

}
static void unlock()
{
    UnlockMdsShrMutex(&eventMutex);
}

static void configure()
{
	char *ipList, *ipList1, *currIp;
	static char configured = 0;
	if(configured)
		return;
	lock();
	
	numClientIpDescriptors = 0;
	ipList = getenv("REVENT_MDSIP_LIST");
	if(!ipList)
	{
		unlock();
		return;
	}
	ipList1 = malloc(strlen(ipList)+1);
	strcpy(ipList1, ipList);


	currIp = strtok(ipList1, ";" );
	while( currIp != NULL && numClientIpDescriptors < MAX_IP) 
	{
		clientIpDescriptors[numClientIpDescriptors].id = ConnectToMds(currIp);
		if(clientIpDescriptors[numClientIpDescriptors].id > 0)
		{
			clientIpDescriptors[numClientIpDescriptors].ipAddr = malloc(strlen(currIp) + 1);
			strcpy(clientIpDescriptors[numClientIpDescriptors].ipAddr, currIp);
			numClientIpDescriptors++;
		}
		currIp = strtok(NULL, ";" );
	}
	configured = 1;
	unlock();
	///CODE FOR FILLING numIps, thisIp and ipDescrs
}

static void eventHandler(char *evName, char *buf, int size, void *arg);



static void EventRegisterRemote(char *eventName)
{
	int i, status;
	struct descrip evNameDsc, thisIpDsc, resDsc;

	struct ClientEventDescriptor *currEventDescr, *prevEventDescr;
	char thisIp[512];
	strncpy(thisIp, getenv("REVENT_THIS_MDSIP"), 511);
	configure();
	if(!thisIp || !*thisIp || numClientIpDescriptors == 0)
		return;  //Missing information for managing rempte events

	MakeDescrip(&thisIpDsc, DTYPE_CSTRING, 0, 0, thisIp);
	MakeDescrip(&evNameDsc, DTYPE_CSTRING, 0, 0, eventName);
	lock();
	prevEventDescr = currEventDescr = clientEventDescrHead;
	while(currEventDescr && strcmp(currEventDescr->eventName, eventName))
	{
		prevEventDescr = currEventDescr;
		currEventDescr = currEventDescr->nxt;
	}
	if(!currEventDescr)
	{
		currEventDescr = (struct ClientEventDescriptor *)malloc(sizeof(struct ClientEventDescriptor));
		currEventDescr->eventName = malloc(strlen(eventName) + 1);
		strcpy(currEventDescr->eventName, eventName);
		currEventDescr->refCount = 1;
		for(i = 0; i < numClientIpDescriptors; i++)
		{
			status = MdsValue(clientIpDescriptors[i].id, "RtEventsShr->EventRegisterExecute($1,$2)", &evNameDsc, &thisIpDsc, &resDsc, NULL);
			if(!(status & 1))
				printf("Error sending event: %s\n", MdsGetMsg(status));
		}
		if(prevEventDescr)
			prevEventDescr->nxt = currEventDescr;
		else
			clientEventDescrHead = currEventDescr;
	}
	else //already found: someone already registered fior this event on this machine
		currEventDescr->refCount++;
	unlock();
}



EXPORT int EventRegisterExecute(char *eventName, char *mdsipAddr)
{
	int idx, i;
	struct ServerEventDescriptor *currDescr, *prevDescr; 
printf("EVENT REGISTER EXECUTE %s %s\n", eventName, mdsipAddr);

	lock();
	//Look for an already open connection
	currDescr = prevDescr = serverEventDescrHead;
	while(currDescr && strcmp(currDescr->eventName, eventName))
	{
		prevDescr = currDescr;
		currDescr = currDescr->nxt;
	}
	if(!currDescr)
	{
		currDescr = (struct ServerEventDescriptor *)malloc(sizeof(struct ServerEventDescriptor));
		currDescr->eventName = malloc(strlen(eventName) + 1);
		strcpy(currDescr->eventName, eventName);
		currDescr->numIp = 0;
		currDescr->nxt = 0;
		currDescr->busy = 0;
		if(prevDescr)
			prevDescr->nxt = currDescr;
		else
			serverEventDescrHead = currDescr;
		currDescr->eventId = EventAddListener(eventName,  (void (*)(char *, char *, int, void *, char, int , char *, int))eventHandler, (void *)currDescr);
	}
	for(idx = 0; idx < currDescr->numIp && strcmp(currDescr->ipAddrs[idx], mdsipAddr); idx++);
	if(idx == currDescr->numIp && idx < MAX_IP) //Connection not yet open
	{	
		for(i = 0; i < numServerIpDescriptors; i++)
		{
			if(!strcmp(mdsipAddr, serverIpDescriptors[i].ipAddr))
				break;
		}
		if(i == numServerIpDescriptors && i < MAX_IP)
		{
			serverIpDescriptors[i].ipAddr = malloc(strlen(mdsipAddr)+1);
			strcpy(serverIpDescriptors[i].ipAddr, mdsipAddr);
			serverIpDescriptors[i].id = ConnectToMds(mdsipAddr);
			if(serverIpDescriptors[i].id > 0)
				numServerIpDescriptors++;
		}
		currDescr->ipAddrs[idx] = malloc(strlen(mdsipAddr) + 1);
		strcpy(currDescr->ipAddrs[idx], mdsipAddr);
		currDescr->ids[idx] = serverIpDescriptors[i].id;
		currDescr->numIp++;
	}
	unlock();
	return 1;
}



static void eventHandler(char *evName, char *buf, int size, void *arg)
{
	int i, status;
	struct descrip evNameDsc, bufDsc, resDsc;
	struct descrip bufSizeDsc;

	struct ServerEventDescriptor *currEventDescr;

printf("event handler %s\n", evName);

	lock();
	currEventDescr= (struct ServerEventDescriptor *)arg;
	if(currEventDescr->busy)
		return; //Do not propagate events received from outside
	unlock();

	MakeDescrip(&evNameDsc,DTYPE_CSTRING,0,0,evName);
	MakeDescrip(&bufDsc, DTYPE_UCHAR, 1, &size, buf);
	MakeDescrip(&bufSizeDsc, DTYPE_LONG, 0,0, (char *)&size);
	for(i = 0; i < currEventDescr->numIp; i++)
		status = MdsValue(currEventDescr->ids[i], "RtEventsShr->EventTriggerExecute($1,$2,$3)", &evNameDsc, &bufDsc, &bufSizeDsc, &resDsc, NULL);
}


EXPORT int EventTriggerExecute(char *eventName, char *buf, int *bufSize)
{
	//Make sure this event will not be propagated outside this computer
	struct ServerEventDescriptor *currDescr;
printf("EVENT TRIGGER EXECUTE %s\n", eventName);

	lock();
	currDescr = serverEventDescrHead;
	while(currDescr && strcmp(currDescr->eventName, eventName))
		currDescr = currDescr->nxt;
	if(currDescr)
		currDescr->busy = 1;
	unlock();
	MdsEventTriggerAndWait(eventName, buf, *bufSize);
	currDescr->busy = 0;
	return 1;
}


EXPORT void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int, void *), void *callbackArg)
{
	void *id = EventAddListener(name,  (void (*)(char *, char *, int, void *, char, int retSize, char *retData, int type))callback, callbackArg);
	EventRegisterRemote(name);
	return id;
}

EXPORT int MdsEventTrigger(char *name, char *buf, int size)
{
	return (EventTrigger(name, buf, size)==0)?1:0;
}

EXPORT int MdsEventTriggerAndWait(char *name, char *buf, int size)
{
	return (EventTriggerAndWait(name, buf, size)==0)?1:0;
}

EXPORT extern void MdsEventWait(char *name, char *buf, int size, int *retSize)
{
	EventWait(name, buf, size, retSize);
}


EXPORT extern void MdsEventClean()
{
	EventClean();
}


EXPORT struct descriptor_xd *MdsEventGetState()
{
	static EMPTYXD(xd);
	DESCRIPTOR_A(stateD, 1, DTYPE_B, 0, 0); 
	char *state;
	int size;

	state = EventGetSerializedState(&size);
	stateD.pointer = state;
	stateD.arsize = size;

	MdsCopyDxXd((struct descriptor *)&stateD, &xd);
	free(state);
	return &xd;
}

EXPORT struct descriptor_xd *MdsEventGetMemState()
{
	static EMPTYXD(xd);
	DESCRIPTOR_A(stateD, 1, DTYPE_B, 0, 0); 
	char *state;
	int size;

	state = EventGetSerializedMemState(&size);
	stateD.pointer = state;
	stateD.arsize = size;

	MdsCopyDxXd((struct descriptor *)&stateD, &xd);
	free(state);
	return &xd;
}

