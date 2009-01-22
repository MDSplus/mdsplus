#ifndef EVENT_H
#define EVENT_H
#include "EventManager.h"

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

	void *addListener(char *eventName, void (*callback)(char *, char *, int, bool, int, char *), bool copyBuf = false, int retDataSize = 0)
	{
		return evManager->addListener(eventName, 0, callback, memManager, copyBuf, retDataSize);
	}
	void removeListener(void *eventAddr)
	{
		evManager->removeListener(eventAddr, memManager);
	}
	void trigger(char *eventName, char *buf, int size, bool copyBuf = true)
	{
		evManager->trigger(eventName, buf, size, memManager, copyBuf);
	}

	bool triggerAndWait(char *eventName, char *buf, int size, bool copyBuf = true, Timeout *timeout = 0)
	{
		return evManager->triggerAndWait(eventName, buf, size, memManager, copyBuf, timeout);
	}

	EventAnswer *triggerAndCollect(char *eventName, char *buf, int size, bool copyBuf = true, EventAnswer *inAnsw = 0, Timeout *timeout = 0)
	{
		return evManager->triggerAndCollect(eventName, buf, size, memManager, copyBuf, inAnsw, timeout);
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
};

#endif