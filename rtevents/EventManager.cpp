#include "EventManager.h"
#include "Delay.h"

void EventManager::initialize()
{
	eventHead = NULL;
	lock.initialize();
}
	
void *EventManager::addListener(char *eventName, Thread *thread, void (*callback)(char *, char *, int, bool), SharedMemManager *memManager)
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
	Notifier *currNotifier = (Notifier *)currHandler->addListener(thread, new EventRunnable(callback), currHandler, memManager);
	lock.unlock();
	return new ListenerAddress(currHandler, currNotifier);
}

void *EventManager::addCatchAllListener(Thread *thread, void (*callback)(char *, char *, int, bool), SharedMemManager *memManager)
{
	//NOTE: EventHandlers are never deallocated
	
	lock.lock();
	EventHandler *currHandler = getCatchAllHandler();
	if(!currHandler) //New Event
	{
		currHandler = (EventHandler *)memManager->allocate(sizeof(EventHandler));
		currHandler->initialize();
		currHandler->setNext((EventHandler *)eventHead.getAbsAddress());
		eventHead = currHandler;
	}
	Notifier *currNotifier = (Notifier *)currHandler->addListener(thread, new EventRunnable(callback), currHandler, memManager);
	lock.unlock();
	return new ListenerAddress(currHandler, currNotifier);
}


void EventManager::removeListener(void *addr, SharedMemManager *memManager)
{
	ListenerAddress *eventAddr = (ListenerAddress *)addr;
	eventAddr->getHandler()->removeListener((void *)eventAddr->getNotifier(), memManager);
	delete eventAddr;
}

EventHandler *EventManager::getHandler(char *name)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	
	while(currHandler)
	{
		if(!currHandler->isCatchAll() && !strcmp(currHandler->getName(), name))
			break;
		currHandler = currHandler->getNext();
	}
	return currHandler;
}
EventHandler *EventManager::getCatchAllHandler()
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	
	while(currHandler)
	{
		if(currHandler->isCatchAll())
			break;
		currHandler = currHandler->getNext();
	}
	return currHandler;
}

void EventManager::trigger(char *eventName, char *buf, int size, SharedMemManager *memManager)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			currHandler->setData(buf, size, memManager);
			if(currHandler->isCatchAll())
				currHandler->setName(eventName, memManager);
			currHandler->trigger();
		}
		currHandler = currHandler->getNext();
	}
}

void EventManager::triggerAndWait(char *eventName, char *buf, int size, SharedMemManager *memManager)
{
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			currHandler->setData(buf, size, memManager);
			if(currHandler->isCatchAll())
				currHandler->setName(eventName, memManager);
			currHandler->triggerAndWait();
		}
		currHandler = currHandler->getNext();
	}
}
bool EventManager::triggerAndWait(char *eventName, Timeout &timeout, char *buf, int size, SharedMemManager *memManager)
{
	bool isTimeout = false;
	EventHandler *currHandler = (EventHandler *)eventHead.getAbsAddress();
	while(currHandler)
	{
		if(currHandler->corresponds(eventName))
		{
			currHandler->setData(buf, size, memManager);
			if(currHandler->isCatchAll())
				currHandler->setName(eventName, memManager);
			isTimeout |= currHandler->triggerAndWait(timeout);
		}
		currHandler = currHandler->getNext();
	}
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


//////////////////////////External C Interface/////////////////////
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



EXPORT void * EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, bool))
{
	try {
		checkEventManager();
		void *handl = eventManager->addListener(name, new Thread, callback, &memManager);
		int nameLen = strlen(name)+1;
		char *msg = new char[nameLen];
		strcpy(msg, name);

		//Except when registering to supervisor event (i.e. by EvenConnector), signal this registration
		if(strcmp(name, "@@@EVENT_MANAGER@@@"))
			eventManager->trigger("@@@EVENT_MANAGER@@@", msg, nameLen, &memManager);
		delete [] msg;
		return handl;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return NULL;
	}
}
EXPORT void * EventAddListener(char *name,  void (*callback)(char *, char *, int, bool))
{
	try {
		checkEventManager();
		void *handl = eventManager->addListener(name, new Thread, callback, &memManager);
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
		eventManager->triggerAndWait(name, buf, size, &memManager);
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

