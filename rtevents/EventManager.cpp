#include "EventManager.h"
#include "Delay.h"
#include "ExitHandler.h"

static bool exiting = false;
static void removePendingListeners();
class HandlePendingListeners: public Runnable
{
public:
	void run(void *arg)
	{
		exiting = true;
		removePendingListeners();
	}
};

struct ListenerDescr {
	void *listenerAddr;
	ListenerDescr *nxt, *prv;
};
static bool exitHandlerInstalled = 0;

static ListenerDescr *pendingListeners = 0;
static void addIntListener(void *addr)  //NOTE to be called in locked section
{
	ListenerDescr *newDescr = new ListenerDescr;
	newDescr->listenerAddr = addr;
	if(!pendingListeners)
	{
		pendingListeners = newDescr;
		newDescr->nxt = newDescr->prv = 0;
	}
	else
	{
		newDescr->prv = 0;
		newDescr->nxt = pendingListeners;
		pendingListeners->prv = newDescr;
		pendingListeners = newDescr;
	}
	if(!exitHandlerInstalled)
	{
		exitHandlerInstalled = true;
		ExitHandler::atExit(new HandlePendingListeners);
	}
}


static void removeIntListener(void *addr)  //NOTE to be called in locked section
{
	ListenerDescr *currDescr;
	for(currDescr = pendingListeners; currDescr; currDescr = currDescr->nxt)
	{
		if(currDescr->listenerAddr == addr)
		{
			if(currDescr->prv)
			{
				currDescr->prv->nxt = currDescr->nxt;
				if(currDescr->nxt)
					currDescr->nxt->prv = currDescr->prv;
			}
			else
			{
				pendingListeners = currDescr->nxt;
				if(pendingListeners)
					pendingListeners->prv = 0;
			}
			delete currDescr;
			break;
		}
	}
}

static void removePendingListeners()
{
	ListenerDescr *currDescr;
	for(currDescr = pendingListeners; currDescr; currDescr = currDescr->nxt)
	{
		EventRemoveListener(currDescr->listenerAddr);
	}
}



void EventManager::initialize()
{
	eventHead = NULL;
	lock.initialize();
}
	
void *EventManager::addListener(char *eventName, ThreadAttributes *threadAttr, void (*callback)(char *, char *, int, bool, int, char*), SharedMemManager *memManager, bool copyBuf, int retDataSize)
{
	//NOTE: EventHandlers are never deallocated
	
	lock.lock();
	EventHandler *currHandler = getHandler(eventName);
	if(!currHandler) //New Event
	{
		currHandler = (EventHandler *)memManager->allocate(sizeof(EventHandler));
		currHandler->initialize(eventName, memManager);
		currHandler->setNext((EventHandler *)eventHead.getAbsAddress());
		eventHead = currHandler;
	}
	RetEventDataDescriptor *retDataDescr = (RetEventDataDescriptor *)currHandler->addRetBuffer(retDataSize, memManager);
	int size;
	void *data = retDataDescr->getData(size);
	Notifier *currNotifier = (Notifier *)currHandler->addListener(threadAttr, new EventRunnable(callback, copyBuf, size, data), currHandler, memManager);

	ListenerAddress *retAddr = new ListenerAddress(currHandler, currNotifier, retDataDescr);
	addIntListener(retAddr);
	lock.unlock();
	return retAddr;
}



void EventManager::removeListener(void *addr, SharedMemManager *memManager)
{
	lock.lock();
	ListenerAddress *eventAddr = (ListenerAddress *)addr;
	eventAddr->getHandler()->removeListener((void *)eventAddr->getNotifier(), memManager);
	eventAddr->getHandler()->removeRetDataDescr(eventAddr->getRetDataDescr(), memManager);
	delete eventAddr;
	if(!exiting) removeIntListener(addr);
	lock.unlock();
}


void EventManager::resizeListener(void *addr, int newSize, SharedMemManager *memManager)
{
	lock.lock();
	ListenerAddress *eventAddr = (ListenerAddress *)addr;
	eventAddr->getHandler()->resizeRetData(eventAddr->getRetDataDescr(), newSize, memManager);
	delete eventAddr;
	if(!exiting) removeIntListener(addr);
	lock.unlock();
}




EventHandler *EventManager::getHandler(char *name)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	
	while(currHandler)
	{
		if(!strcmp(currHandler->getName(), name))
			break;
		currHandler = currHandler->getNext();
	}
	return currHandler;
}

void EventManager::trigger(char *eventName, char *buf, int size, SharedMemManager *memManager, bool copyBuf)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			currHandler->setData(buf, size, copyBuf, memManager);
			currHandler->trigger();
		}
		currHandler = currHandler->getNext();
	}
}

EventAnswer *EventManager::triggerAndCollect(char *eventName, char *buf, int size, SharedMemManager *memManager, bool copyBuf, EventAnswer *inAnsw, Timeout *timeout)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			currHandler->setData(buf, size, copyBuf, memManager);
			return currHandler->triggerAndCollect(buf, size, memManager, copyBuf, inAnsw, timeout);
		}
		currHandler = currHandler->getNext();
	}
	if(inAnsw)
	{
		inAnsw->setNumMsg(0);
		return inAnsw;
	}
	else
		return new EventAnswer(0, copyBuf);
}
bool EventManager::triggerAndWait(char *eventName, char *buf, int size, SharedMemManager *memManager, bool copyBuf, Timeout *timeout)
{
	bool isTimeout = false;
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	char *dataBuf;
	if(copyBuf && size > 0)
	{
		dataBuf = new char[size];
		memcpy(dataBuf, buf, size);
	}
	else
		dataBuf = buf;


	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			isTimeout |= currHandler->triggerAndWait(dataBuf, size, memManager, copyBuf, timeout);
		}
		currHandler = currHandler->getNext();
	}
	if(copyBuf && size > 0)
		delete[]dataBuf;

	return isTimeout;
}
void EventManager::clean(int milliSecs, SharedMemManager *memManager)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		currHandler->watchdogTrigger();
		currHandler = currHandler->getNext();
	}
	Delay delay(milliSecs);
	delay.wait();
	currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		currHandler->clean(memManager);
		currHandler = currHandler->getNext();
	}
}
void EventManager::clean(char *eventName, int milliSecs, SharedMemManager *memManager)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
			currHandler->watchdogTrigger();
		currHandler = currHandler->getNext();
	}
	Delay delay(milliSecs);
	delay.wait();
	currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
			currHandler->clean(memManager);
		currHandler = currHandler->getNext();
	}
}

static EventManager *eventManager = 0;
static SharedMemManager memManager;



static void checkEventManager()
{
	if(!eventManager) //If event struct not yet set-up fior this process
	{
		GlobalLock eventLock;
		eventLock.initialize(EVENT_ID);
		eventLock.lock();
		bool firstCreated = memManager.initialize(EVENT_ID, EVENT_SIZE);
		if(firstCreated)
		{
			eventManager = (EventManager *)memManager.allocate(sizeof(EventManager));
			eventManager->initialize();
		}
		else
			eventManager = (EventManager *)memManager.getFirstBuffer();
		eventLock.unlock();
		eventLock.dispose();
	}
}

SharedMemManager *EventManager::getMemManager()
{
	checkEventManager();
	return &memManager;
}

EventManager *EventManager::getEventManager() 
{
	checkEventManager();
	return eventManager;
}

//////////////////////////External C Interface/////////////////////

EXPORT void EventReset()
{
	GlobalLock eventLock;
	eventLock.initialize(EVENT_ID);
	eventLock.lock();
	bool firstCreated = memManager.initialize(EVENT_ID, EVENT_SIZE);
	memManager.reset();
	eventManager = (EventManager *)memManager.allocate(sizeof(EventManager));
	eventManager->initialize();
	eventLock.unlock();
	eventLock.dispose();
}



EXPORT void * EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, bool, int, char *))
{
	try {
		checkEventManager();
		void *handl = eventManager->addListener(name, NULL, callback, &memManager);
		int nameLen = strlen(name)+1;
		char *msg = new char[nameLen];
		strcpy(msg, name);

		//Except when registering to supervisor event (i.e. by EvenConnector), signal this registration
		if(strcmp(name, "@@@EVENT_MANAGER@@@"))
			eventManager->triggerAndWait("@@@EVENT_MANAGER@@@", msg, nameLen, &memManager, false);
		delete [] msg;
		return handl;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return NULL;
	}
}
EXPORT void * EventAddListener(char *name,  void (*callback)(char *, char *, int, bool, int, char*))
{
	try {
		checkEventManager();
		void *handl = eventManager->addListener(name, NULL, callback, &memManager);
		return handl;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return NULL;
	}
}
EXPORT void EventRemoveListener(void *eventHandler)
{
	if(!eventManager)
		return;
	try {
		eventManager->removeListener(eventHandler, &memManager);
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
	}
	
}

EXPORT  int EventTrigger(char *name, char *buf, int size)
{
	try {
		checkEventManager();
		eventManager->trigger(name, buf, size, &memManager);
		return 0;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return -1;
	}
	
}
EXPORT int EventTriggerAndWait(char *name, char *buf, int size)
{
	try {
		checkEventManager();
		eventManager->triggerAndWait(name, buf, size, &memManager, true);
		return 0;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return -1;
	}
}

EXPORT int EventTriggerAndTimedWait(char *name, char *buf, int size, int millisecs)
{
	try {
		checkEventManager();
		Timeout *timout = new Timeout(millisecs);
		eventManager->triggerAndWait(name,  buf, size, &memManager, true, timout);
		delete timout;
		return 0;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return -1;
	}
}

EXPORT void EventClean()
{
	try {
		checkEventManager();
		eventManager->clean(100, &memManager);
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
	}
}

