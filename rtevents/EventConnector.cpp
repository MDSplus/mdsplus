#include "SharedMemManager.h"

#include "MessageManager.h"
#include "TCPMessageManager.h"
#include "Lock.h"
#include "UnnamedSemaphore.h"
#include "EventManager.h"
#include "Runnable.h"
#include "Thread.h"
#include <stdio.h>

static void eventCallback(char *name, char *buf, int bufLen, bool isSynch);

//Class InternalPending describes an event received from outside (and therefore for which an 
//internal listener exosts) for which another
//node registered. In this case, EventConnector has registered as listener for this event
//but its associated callback has not to send the event to the other node (it will receive it directly
//from the event originator).
class InternalPending
{
public:
	char *buf;
	int size;
	InternalPending *nxt, *prv;
	InternalPending(){nxt = prv = 0;}
	bool corresponds(char *inBuf, int inSize)
	{
		if(size != inSize) return false;
		for(int i = 0; i < size; i++)
			if(buf[i] != inBuf[i])
				return false;
		return true;
	}
	~InternalPending()
	{
		if(size > 0)
			delete [] buf;
	}
};


//Class External pending describes the propagation to another node of  a synchronous event. 
//When the message acknowledge will be received (indicating that the remote client processed the event)
//the receiver thread will post the associate semaphore (using the unique id), thus letting the event
//callback terminate.

class ExternalPending
{
public:
	unsigned int id;
	UnnamedSemaphore sem;
	NetworkAddress *addr;
	ExternalPending *nxt, *prv;
	ExternalPending(int id, NetworkAddress *addr)
	{
		this->id = id;
		this->addr = addr;
		nxt = prv = 0;
		sem.initialize(0);
	}
	~ExternalPending()
	{
		sem.dispose();
	}
};

//Class ExternalListener describes the remote listener for an event. 
class ExternalListener
{
public:
	NetworkAddress *addr;
	ExternalListener *nxt, *prv;
	ExternalListener(NetworkAddress *addr)
	{
		this->addr = addr;
		nxt = prv = 0;
	}
};


#define IS_SYNCH_EVENT 1
#define IS_ASYNCH_EVENT 2
#define IS_EVENT_REGISTRATION 3
#define IS_EVENT_ACK 4
//Class Event Message described the event message sent over the network
class EventMessage
{
public:
	char mode;
	char *name;
	unsigned int waitId;
	char *buf;
	int bufLen;
	
	EventMessage(char *name, char *buf, int bufLen, bool synch, unsigned int waitId)
	{
		if(synch)
			mode = IS_SYNCH_EVENT;
		else
			mode = IS_ASYNCH_EVENT;
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
		this->buf = buf; 
		this->bufLen = bufLen;
		this->waitId = waitId;
	}
	
	EventMessage(char *name, int waitId)
	{
		mode = IS_EVENT_ACK;
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
		this->waitId = waitId;
	}
	
	EventMessage(char *name)
	{
		mode = IS_EVENT_REGISTRATION;
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
	}
	
	EventMessage(char *inBuf, int inBufSize, MessageManager *msgManager)
	{
		char *ptr = inBuf;
		mode = *ptr;
		ptr++;
		int nameLen = msgManager->toNative(*(unsigned int *)ptr);
		name = new char[nameLen+1];
		ptr += 4;
		memcpy(name, ptr, nameLen);
		name[nameLen] = 0;
		ptr += nameLen;
		switch(mode) {
			case IS_ASYNCH_EVENT:
				bufLen = msgManager->toNative(*(unsigned int *)ptr);
				ptr += 4;
				buf = ptr; //Note that buffer is not copied
				ptr += bufLen;
				waitId = 0;
				break;
			case IS_SYNCH_EVENT:
				bufLen = msgManager->toNative(*(unsigned int *)ptr);
				ptr += 4;
				buf = ptr; //Note that buffer is not copied
				ptr += bufLen;
				waitId = msgManager->toNative(*(unsigned int *)ptr);
				ptr += 4;
				break;
			case IS_EVENT_ACK:
				bufLen = 0;
				buf = 0;
				waitId = msgManager->toNative(*(unsigned int *)ptr);
				ptr += 4;
				break;
			case IS_EVENT_REGISTRATION:
				bufLen = 0;
				buf = 0;
				waitId = 0;
				break;
		}
		if(ptr - inBuf != inBufSize)
			printf("INTERNAL ERROR: WRONG MESSAGE SIZE IN DISTRIBUTED EVENTS");
	}
	
	char *serialize(int &retSize, MessageManager *msgManager)
	{
		int size;
		switch(mode) 
		{
			case IS_ASYNCH_EVENT:
				size = 1 /* mode*/ + strlen(name) + 4/*name and name length*/+ bufLen + 4 /*buf and length */;
				break;
			case IS_SYNCH_EVENT:
				size = 1 /* mode*/ + strlen(name) + 4/*name and name length*/+ 
					bufLen + 4 /*buf and length */ + 4 /*waitId*/;
				break;
			case IS_EVENT_ACK:
				size = 1 /* mode*/ + strlen(name) + 4/*name and name length*/+ 4 /*waitId*/;
				break;
			case IS_EVENT_REGISTRATION:
				size = 1 /* mode*/ + strlen(name) + 4/*name and name length*/;
				break;
		}

		char *retBuf = new char[size];
		char *ptr = retBuf;
		*ptr = mode;
		ptr++;
		int nameLen = strlen(name);
		*((unsigned int *)ptr) = msgManager->fromNative(nameLen);
		ptr += 4;
		memcpy(ptr, name, nameLen);
		ptr += nameLen;
		switch (mode)
		{
			case IS_ASYNCH_EVENT:
				*((unsigned int *)ptr) = msgManager->fromNative(bufLen);
				ptr += 4;
				memcpy(ptr, buf, bufLen);
				break;
			case IS_SYNCH_EVENT:
				*((unsigned int *)ptr) = msgManager->fromNative(bufLen);
				ptr += 4;
				memcpy(ptr, buf, bufLen);
				ptr += bufLen;
				*((unsigned int *)ptr) = msgManager->fromNative(waitId);
				break;
			case IS_EVENT_ACK:
				*((unsigned int *)ptr) = msgManager->fromNative(waitId);
				break;
			case IS_EVENT_REGISTRATION:
				break;
		}
		retSize = size;
		return retBuf;
	}
	
	~EventMessage()
	{
		delete [] name;
		//Buffer is not allocated
	}
};


static int extPendingId = 0;

//Class External event is the root of all the data structures required to describe an event
//for which some external node registered
class ExternalEvent
{
public:
	char *eventName;
	InternalPending *intPendingHead;
	ExternalPending *extPendingHead;
	ExternalListener *extListenerHead;
	ExternalEvent *nxt, *prv;
	
	ExternalEvent(char *name)
	{
		eventName = new char[strlen(name) + 1];
		strcpy(eventName, name);
		intPendingHead = NULL;
		extPendingHead = NULL;
		extListenerHead = NULL;
		nxt = prv = NULL;
	}
	//Check if the callback has been triggered by the reception of an external event.
	//in this case it is only required to discard the corresponding InternalPending instance
	bool isExternalEvent(char *buf, int size)
	{
		InternalPending *currPend = intPendingHead;
		while(currPend)
		{
			if(currPend->corresponds(buf, size))
			{
				if(currPend == intPendingHead)
					intPendingHead = currPend->nxt;
				else
					currPend->prv->nxt = currPend->nxt;
				if(currPend->nxt)
					currPend->nxt->prv = currPend->prv;
			}
			delete currPend;
			return true;
		}
		return false;
	}

//Handle the receipt of a network message indicating the termination of a synchronous trigger
//the message will brinh the unique long id of ExternalPending instance
	void signalExternalTermination(unsigned int id)
	{
		ExternalPending *currPend = extPendingHead;
		while(currPend)
		{
			if(currPend->id == id)
			{
				currPend->sem.post();
				break;
			}
		}
	}

	
//SendEvent sends a message describing the event. 
	void sendSynchEvent(char *buf, int bufLen, MessageManager *msgManager, 
			UnnamedSemaphore ** &retSems, unsigned int * &waitIds, int &numSems)
	{
		//count listeners
		int numListeners = 0;
		ExternalListener *currListener = extListenerHead;
		while(currListener)
		{
			numListeners++;
			currListener = currListener->nxt;
		}
		retSems = new UnnamedSemaphore *[numListeners];
		waitIds = new unsigned int[numListeners];
		currListener = extListenerHead;
		int semIdx = 0;
		while(currListener)
		{
			addExternalPending(currListener->addr, retSems[semIdx], waitIds[semIdx]);
			EventMessage msg(eventName, buf, bufLen, true, waitIds[semIdx]);
			int msgLen;
			char *msgBuf = msg.serialize(msgLen, msgManager);
			msgManager->sendMessage(currListener->addr, msgBuf, msgLen);
			delete [] msgBuf;
			currListener = currListener->nxt;
			semIdx++;
		}
		numSems = semIdx;
	}
	
	//SendEvent sends a message describing the event. 
	void sendAsynchEvent(char *buf, int bufLen, MessageManager *msgManager)
	{
		ExternalListener *currListener = extListenerHead;
		EventMessage msg(eventName, buf, bufLen, false, 0);
		int msgLen;
		char *msgBuf = msg.serialize(msgLen, msgManager);
		while(currListener)
		{
			msgManager->sendMessage(currListener->addr, msgBuf, msgLen);
			currListener = currListener->nxt;
		}
		delete [] msgBuf;
	}
	
	void addExternalPending(NetworkAddress *addr, UnnamedSemaphore * &retSem, unsigned int &retId)
	{
		retId = extPendingId++;
		ExternalPending *currPending = new ExternalPending(retId, addr);
		currPending->nxt = extPendingHead;
		extPendingHead = currPending;
		retSem = &currPending->sem;
	}
	void removeExternalPending(unsigned int id)
	{
		ExternalPending *currPend = extPendingHead;
		while(currPend)
		{
			if(currPend->id == id)
			{
				if(currPend == extPendingHead)
					extPendingHead = currPend->nxt;
				else
					currPend->prv->nxt = currPend->nxt;
				if(currPend->nxt)
					currPend->nxt->prv = currPend->prv;
				delete currPend;
				return;
			}
		}
	}
	
	void unblockExternalPending(NetworkAddress *addr)
	{
		ExternalPending *currPend = extPendingHead;
		while(currPend)
		{
			if(currPend->addr->equals(addr))
				currPend->sem.post();
			currPend = currPend->nxt;
		}
	}
	void addExternalListener(NetworkAddress *addr)
	{
		bool exists = false;
		ExternalListener *currListener = extListenerHead;
		while(!exists && currListener)
		{
			exists = currListener->addr->equals(addr);
			currListener = currListener->nxt;
		}
		if(exists)
			return;
		ExternalListener *newListener = new ExternalListener(addr);
		newListener->prv = extListenerHead;
		extListenerHead = newListener;
	}
	void removeExternalListeners(NetworkAddress *addr)
	{
		ExternalListener *currListener = extListenerHead;
		while(currListener)
		{
			if(currListener->addr->equals(addr))
			{
				if(currListener == extListenerHead)
					extListenerHead = currListener->nxt;
				else
					currListener->prv->nxt = currListener->nxt;
				if(currListener->nxt)
					currListener->nxt->prv = currListener->prv;
				delete currListener;
				return;
			}
		}
	}
};

//Top structure class
class ExternalEventManager
{
public:
	Lock lock;
	ExternalEvent *extEventHead;
	
	ExternalEventManager()
	{
		lock.initialize();
		extEventHead = 0;
	}
	~ExternalEventManager()
	{
		lock.dispose();
	}
	
	ExternalEvent *findEvent(char *name)
	{
		ExternalEvent *currEvent = extEventHead;
		while(currEvent)
		{
			if(!strcmp(name, currEvent->eventName))
				break;
			currEvent = currEvent->nxt;
		}
		return currEvent;
	}
	
	ExternalEvent *newEvent(char *name)
	{
		ExternalEvent *newEv = new ExternalEvent(name);
		newEv->nxt = extEventHead;
		extEventHead = newEv;
		return newEv;
	}
	
	bool isExternalEvent(char *name, char *buf, int size)
	{
		bool isExt = false;
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			printf("INTERNALE ERROR: received event with no event structure!!");
		}
		else
			isExt = extEvent->isExternalEvent(buf, size);
		lock.unlock();
		return  isExt;
	}

	void signalExternalTermination(char *name, unsigned int id)
	{
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			printf("INTERNALE ERROR: received event message with no event structure!!");
		}
		else
			extEvent->signalExternalTermination(id);
		lock.unlock();
	}
	void sendAsynchEvent(char *name, char *buf, int bufLen, MessageManager *msgManager)
	{
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			printf("INTERNALE ERROR: received event message with no event structure!!");
		}
		else
			extEvent->sendAsynchEvent(buf, bufLen, msgManager);
		lock.unlock();
	}
	void sendSynchEvent(char *name, char *buf, int bufLen, MessageManager *msgManager, 
			UnnamedSemaphore ** &retSems, unsigned int * &waitIds, int &numSems)
	{
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			printf("INTERNALE ERROR: received event message with no event structure!!");
		}
		else
			extEvent->sendSynchEvent(buf, bufLen, msgManager, retSems, waitIds, numSems);
		lock.unlock();
	}
/*	void addExternalPending(char *name, UnnamedSemaphore * &retSem, unsigned int &retId)
	{
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			printf("INTERNALE ERROR: received event message with no event structure!!");
		}
		else
			extEvent->addExternalPending(retSem, retId);
		lock.unlock();
	}
*/	void removeExternalPending(char *name, unsigned int id)
	{
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			printf("INTERNALE ERROR: received event message with no event structure!!");
		}
		else
			extEvent->removeExternalPending(id);
		lock.unlock();
		
	}
	void addExternalListener(char *name, NetworkAddress *addr)
	{
		bool isNewEvent = false;
		lock.lock();
		ExternalEvent *extEvent = findEvent(name);
		if(!extEvent)
		{
			extEvent = newEvent(name);
			isNewEvent = true;
		}
		extEvent->addExternalListener(addr);
		if(isNewEvent)
			EventAddListener(name, eventCallback);

		lock.unlock();
	}
	//Called upon connection termination
	void removeExternalListeners(NetworkAddress *addr)
	{
		lock.lock();
		ExternalEvent *extEvent = extEventHead;
		while(extEvent)
		{
			extEvent->removeExternalListeners(addr);
		}
		lock.unlock();
	}
	void unblockExternalPending(NetworkAddress *addr)
	{
		lock.lock();
		ExternalEvent *extEvent = extEventHead;
		while(extEvent)
		{
			extEvent->unblockExternalPending(addr);
		}
		lock.unlock();
	}
};

//Class TrigWaitRunnable provides trigger and wait for a given event on a separate thread
class TrigWaitRunnable: public Runnable
{
public:
	MessageManager *msgManager;
	NetworkAddress *addr;
	char *name;
	char *buf;
	int bufLen;
	unsigned int waitId;
	
	TrigWaitRunnable(char *name,char *buf, int bufLen,  unsigned int waitId, MessageManager *msgManager, NetworkAddress *addr)
	{
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
		this->buf = new char[bufLen];
		memcpy(this->buf, buf, bufLen);
		this->bufLen = bufLen;
		this->msgManager = msgManager;
		this->addr = addr;
		this->waitId = waitId;
	}
	~TrigWaitRunnable()
	{
		delete []name;
		delete[] buf;
	}
	virtual void run(void *arg)
	{
		EventTriggerAndWait(name, buf, bufLen);
		EventMessage msg(name, waitId);
		int msgLen;
		char *msgBuf = msg.serialize(msgLen, msgManager);
		msgManager->sendMessage(addr, msgBuf, msgLen);
		delete []msgBuf;
	}
};


//Class EventMessageReceiver handles the reception of event messages over the network
class EventMessageReceiver:public MessageReceiver
{
public:
	MessageManager *msgManager;
	ExternalEventManager *extEventManager;
	EventMessageReceiver(ExternalEventManager *extEventManager, MessageManager *msgManager)
	{
		this->extEventManager = extEventManager;
		this->msgManager = msgManager;
	}
	
	virtual void messageReceived(NetworkAddress *addr, char *buf, int size)
	{
		
		Thread thread;
		EventMessage evMsg(buf, size, msgManager);
		switch(evMsg.mode)
		{
			case IS_ASYNCH_EVENT:
				printf("RICEVUTO IS_ASYNCH_EVENT\n");
				EventTrigger(evMsg.name, evMsg.buf, evMsg.bufLen);
				break;
			case IS_SYNCH_EVENT:
				printf("RICEVUTO IS_SYNCH_EVENT\n");
				thread.start((Runnable *)new TrigWaitRunnable(evMsg.name, evMsg.buf, evMsg.bufLen, evMsg.waitId, msgManager, addr));
				break;
			case IS_EVENT_ACK:
				printf("RICEVUTO IS_EVENT_ACK\n");
				extEventManager->signalExternalTermination(evMsg.name, evMsg.waitId);
				break;
			case IS_EVENT_REGISTRATION:
				printf("RICEVUTO IS_EVENT_REGISTRATION\n");
				extEventManager->addExternalListener(evMsg.name, addr);
				break;
		}
	}
	virtual void connectionTerminated(NetworkAddress *addr)
	{
		extEventManager->unblockExternalPending(addr);
		extEventManager->removeExternalListeners(addr);
	}

};


static ExternalEventManager *extEventManager;
static NetworkAddress *extAddresses[512];
static MessageManager *msgManager;
static int numExtAddresses;
#define TCP_PORT 4000 


static void eventCallback(char *name, char *buf, int bufLen, bool isSynch)
{
printf("EVENT CALLBACK\n");

	if(isSynch)
	{
		int numSems;
		UnnamedSemaphore **sems;
		unsigned int *waitIds;
		extEventManager->sendSynchEvent(name, buf, bufLen, msgManager, sems, waitIds, numSems);
		for(int i = 0; i < numSems; i++)
		{
			sems[i]->wait();
			extEventManager->removeExternalPending(name, waitIds[i]);
		}
		delete []sems;
		delete [] waitIds;
	}
	else
		extEventManager->sendAsynchEvent(name, buf, bufLen, msgManager);
}


static void registerEventCallback(char *name, char *buf, int bufLen, bool isSynch)
{
printf("REGISTER EVENT CALLBACK\n");
	
	EventMessage *evMessage = new EventMessage(buf);
	int msgLen;
	char *msg = evMessage->serialize(msgLen, msgManager); 
	for(int i = 0; i < numExtAddresses; i++)
	{
		try {
			msgManager->sendMessage(extAddresses[i], msg, msgLen);
		}catch(SystemException *exc)
		{
			printf("Error Sending registration message: %s\n", exc->what());
		}
	}
}

static void trim(char *line)
{
	int i;
	for(i = strlen(line) - 1; i >= 0 && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n'); i--);
	line[i+1] = 0;
}
	
static void readExtAddresses(char *fileName)
{
	FILE *confFile = fopen(fileName, "r");
	if(!confFile)
	{
		printf("Cannot open configuration file\n");
		exit(0);
	}
	char line[512];
	fgets(line, 512, confFile);
	trim(line);
	if(!strcmp(line, "TCP") || !strcmp(line, "tcp"))
	{

		int addrIdx = 0;
		while(fgets(line, 512, confFile))
		{
			trim(line);
			if(strlen(line) == 0)
				continue;
			printf("%s\n", line);
			extAddresses[addrIdx++] = new IPAddress(line, TCP_PORT);

		}
		numExtAddresses = addrIdx;
		msgManager = new TCPMessageManager();
	}
	//other Networks not yet supported
}


int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Usage: EventConnector <configuration file>\n");
		exit(0);
	}
	readExtAddresses(argv[1]);
	EventReset();
	extEventManager = new ExternalEventManager();
	EventMessageReceiver messageReceiver(extEventManager, msgManager);

	msgManager->connectReceiver(new IPAddress(TCP_PORT), &messageReceiver);
	EventAddListener("@@@EVENT_MANAGER@@@",  registerEventCallback);
	UnnamedSemaphore sem;
	sem.initialize(0);
	sem.wait(); //Suspend forever
	return 0;
}
