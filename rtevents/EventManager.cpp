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
	
void *EventManager::addListener(const char *eventName, ThreadAttributes *threadAttr, void (*callback)(char *, char *, int, void *, bool, int, char*, int), SharedMemManager *memManager, void *callbackArg, bool copyBuf, int retDataSize)
{
	//NOTE: EventHandlers are never deallocated
	bool isNewHandler = false;

	lock.lock();
	EventHandler *currHandler = getHandler(eventName);
	if(!currHandler) //New Event
	{
		currHandler = (EventHandler *)memManager->allocate(sizeof(EventHandler));
		currHandler->initialize(eventName, memManager);
		currHandler->setNext((EventHandler *)eventHead.getAbsAddress());
		isNewHandler = true;
	}
	RetEventDataDescriptor *retDataDescr;
	retDataDescr = (RetEventDataDescriptor *)currHandler->addRetBuffer(retDataSize, memManager);
	Notifier *currNotifier = (Notifier *)currHandler->addListener(threadAttr, new EventRunnable(callback, copyBuf, retDataDescr, callbackArg), currHandler, memManager);

	ListenerAddress *retAddr = new ListenerAddress(currHandler, currNotifier, retDataDescr);
	addIntListener(retAddr);
	if(isNewHandler)
		eventHead = currHandler;
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
	//delete eventAddr;
	lock.unlock();
}




EventHandler *EventManager::getHandler(const char *name)
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

void EventManager::trigger(const char *eventName, char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			currHandler->setData(buf, size, type, copyBuf, memManager);
			currHandler->trigger();
		}
		currHandler = currHandler->getNext();
	}
}

EventAnswer *EventManager::triggerAndCollect(const char *eventName, char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf, EventAnswer *inAnsw, MdsTimeout *timeout)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			//currHandler->setData(buf, size, copyBuf, memManager);
			return currHandler->triggerAndCollect(buf, size, type, memManager, copyBuf, inAnsw, timeout);
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
bool EventManager::triggerAndWait(const char *eventName, char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf, MdsTimeout *timeout)
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
			isTimeout |= currHandler->triggerAndWait(dataBuf, size, type, memManager, copyBuf, timeout);
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
void EventManager::clean(const char *eventName, int milliSecs, SharedMemManager *memManager)
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


EventState *EventManager::getState()
{
	EventState *evState = new EventState;
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		evState->addEvent(currHandler->getName(), currHandler->getNumRegistered(), currHandler->getNumPending());
		currHandler = currHandler->getNext();
	}
	return evState;
}


static EventManager *eventManager = 0;
static SharedMemManager memManager;



static void checkEventManager()
{
	if(!eventManager) //If event struct not yet set-up for this process
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

class WatchdogRunnable:public Runnable
{
	SharedMemManager *memManager;
public:
	WatchdogRunnable(SharedMemManager *memManager)
	{
		this->memManager = memManager;
	}
	void run(void *arg)
	{
		EventManager *evManager = (EventManager *)arg;
		Delay *delay = new Delay(1000); //1 sec clean period
		while(true)
		{
			delay->wait();
			//printf("CLEAN\n");
			evManager->clean(100, memManager);
		}
	}
};

static bool watchdogStarted = false;
void EventManager::startWatchdog(SharedMemManager *memManager)
{
	if(watchdogStarted)
		return;
	watchdogStarted = true;
	Thread *thread = new Thread;
	thread->start(new WatchdogRunnable(memManager), this);
}


