#ifndef EVENT_H
#define EVENT_H
#include "EventManager.h"
extern "C" void EventReset();
//Facade class for Event Managent
class Event
{
	EventManager *evManager;
	SharedMemManager *memManager;

public:
	Event()
	{
		evManager = EventManager::getEventManager();
		memManager = evManager->getMemManager();
	}

	void *addListener(char *eventName, void (*callback)(char *, char *, int, bool, int, char *, int), bool copyBuf = true, int retDataSize = 0)
	{
		return evManager->addListener(eventName, 0, callback, memManager, copyBuf, retDataSize);
	}

	void *addListener(char *eventName, void (*callback)(char *, char *, int, bool, int, char *, int), ThreadAttributes *tAttr, bool copyBuf = true, int retDataSize = 0)
	{
		return evManager->addListener(eventName, tAttr, callback, memManager, copyBuf, retDataSize);
	}

	void *addListenerGlobal(char *eventName, void (*callback)(char *, char *, int, bool, int, char *, int), int retDataSize = 0)
	{
		void *addr = evManager->addListener(eventName, 0, callback, memManager, true, retDataSize);
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

	bool triggerAndWait(char *eventName, char *buf, int size, int type, bool copyBuf = true, Timeout *timeout = 0)
	{
		return evManager->triggerAndWait(eventName, buf, size, type, memManager, copyBuf, timeout);
	}

	bool triggerAndWait(char *eventName, char *buf, int size, bool copyBuf = true, Timeout *timeout = 0)
	{
		return evManager->triggerAndWait(eventName, buf, size, 0, memManager, copyBuf, timeout);
	}

	EventAnswer *triggerAndCollect(char *eventName, char *buf, int size, int type, bool copyBuf = true, EventAnswer *inAnsw = 0, Timeout *timeout = 0)
	{
		return evManager->triggerAndCollect(eventName, buf, size, type, memManager, copyBuf, inAnsw, timeout);
	}
	EventAnswer *triggerAndCollect(char *eventName, char *buf, int size, bool copyBuf = true, EventAnswer *inAnsw = 0, Timeout *timeout = 0)
	{
		return evManager->triggerAndCollect(eventName, buf, size, 0, memManager, copyBuf, inAnsw, timeout);
	}
	void clean(int milliSecs = 100)
	{
		evManager->clean(milliSecs, memManager);
	}

	void clean(char *eventName, int milliSecs = 100)
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

	void reset()
	{
		EventReset();
	}
};

#endif
