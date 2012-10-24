#ifndef EVENT_H
#define EVENT_H
#include "EventManager.h"
////////////////Extern C Interface////////////////
extern "C" EXPORT void *EventAddListener(char *name,  void (*callback)(char *, char *, int, void *, bool, int retSize, char *retData, int type), void *callbackArg);
extern "C" EXPORT void *EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, void *, bool, int retSize, char *retData, int type), void *callbackArg);
extern "C" EXPORT void EventRemoveListener(void *eventHandler);
extern "C" EXPORT int EventTrigger(char *name, char *buf, int size);
extern "C" EXPORT  int EventTriggerAndWait(char *name, char *buf, int size);
extern "C" EXPORT void EventClean();
extern "C" EXPORT void EventReset();
//Facade class for Event Managent
class EXPORT Event
{
	EventManager *evManager;
	SharedMemManager *memManager;

public:
	Event()
	{
		memManager = evManager->getMemManager();
		evManager = EventManager::getEventManager();
	}

	void *addListener(const char *eventName, void (*callback)(char *, char *, int, void *, bool, int, char *, int), void *callbackArg = 0, bool copyBuf = true, int retDataSize = 0)
	{
		void *id = evManager->addListener(eventName, 0, callback, memManager, callbackArg, copyBuf, retDataSize);
		evManager->startWatchdog(memManager);
		return id;
	}

	void *addListener(const char *eventName, void (*callback)(char *, char *, int, void *, bool, int, char *, int), ThreadAttributes *tAttr, void *callbackArg = 0, bool copyBuf = true, int retDataSize = 0)
	{
		return evManager->addListener(eventName, tAttr, callback, memManager, callbackArg, copyBuf, retDataSize);
	}

	void *addListenerGlobal(const char *eventName, void (*callback)(char *, char *, int, void *, bool, int, char *, int), void *callbackArg = 0, int retDataSize = 0)
	{
		void *addr = evManager->addListener(eventName, 0, callback, memManager, callbackArg, true, retDataSize);
		//Except when registering to supervisor event (i.e. by EvenConnector), signal this registration
		int msgSize = strlen(eventName) + 16;
		char *msg = new char[msgSize];
		sprintf(msg, "%s %d", eventName, retDataSize);

		printf("AddEventListener: %s\n", msg);
		
		if(strcmp(eventName, "@@@EVENT_MANAGER@@@"))
			triggerAndWait("@@@EVENT_MANAGER@@@", msg, strlen(msg) + 1);
		delete [] msg;
		return addr;
	}



	void removeListener(void *eventAddr)
	{
		evManager->removeListener(eventAddr, memManager);
	}
	void resizeListener(void *eventAddr, int newSize)
	{
		evManager->resizeListener(eventAddr, newSize, memManager);
	}
	void trigger(char *eventName, char *buf, int size, int type = 0, bool copyBuf = true)
	{
		evManager->trigger(eventName, buf, size, type, memManager, copyBuf);
	}

	bool triggerAndWait(const char *eventName, char *buf, int size, int type, bool copyBuf = true, MdsTimeout *timeout = 0)
	{
		return evManager->triggerAndWait(eventName, buf, size, type, memManager, copyBuf, timeout);
	}

	bool triggerAndWait(const char *eventName, char *buf, int size, bool copyBuf = true, MdsTimeout *timeout = 0)
	{
		return evManager->triggerAndWait(eventName, buf, size, 0, memManager, copyBuf, timeout);
	}

	EventAnswer *triggerAndCollect(const char *eventName, char *buf, int size, int type, bool copyBuf = true, EventAnswer *inAnsw = 0, MdsTimeout *timeout = 0)
	{
		return evManager->triggerAndCollect(eventName, buf, size, type, memManager, copyBuf, inAnsw, timeout);
	}
	EventAnswer *triggerAndCollect(const char *eventName, char *buf, int size, bool copyBuf = true, EventAnswer *inAnsw = 0, MdsTimeout *timeout = 0)
	{
		return evManager->triggerAndCollect(eventName, buf, size, 0, memManager, copyBuf, inAnsw, timeout);
	}
	void clean(int milliSecs = 100)
	{
		evManager->clean(milliSecs, memManager);
	}

	void clean(const char *eventName, int milliSecs = 100)
	{
		evManager->clean(eventName, milliSecs, memManager);
	}

	char *getSharedBuffer(int size)
	{
		return memManager->allocate(size);
	}

	void freeSharedBuffer(char *buf, int size)
	{
		memManager->deallocate(buf, size);
	}

	EventState *getState() {return evManager->getState();}
	SharedMemState *getMemState() {return memManager->getState();}

	void reset();
};
#endif /*EVENTMANAGER_H_*/

