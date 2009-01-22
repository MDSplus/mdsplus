#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_
#include "SystemSpecific.h"
#include "EventHandler.h"
#include "SharedMemManager.h"
#include "Lock.h"


class EventRunnable;
//Class EventManager represents the top manager for events. It is expected that a shared memory
//segment will be dedicated to this instance, which will therefore reside at the beginning of the
//segment. In this segment instances of EventHandler, Notifier and event data buffers will be allocated.
class EXPORT EventManager
{
	friend class EventRunnable;
	Lock lock;
	RelativePointer eventHead;
	EventHandler *getHandler(char *name);
	
public:
	void initialize();
	void *addListener(char *eventName, ThreadAttributes *threadAttr, void (*callback)(char *, char *, int, bool, int, char *), SharedMemManager *memManager, bool copyBuf = false, int retDataSize = 0);
	void removeListener(void *eventAddr,  SharedMemManager *memManager);
	void trigger(char *eventName, char *buf, int size, SharedMemManager *memManager, bool copyBuf = true);
	bool triggerAndWait(char *eventName, char *buf, int size, SharedMemManager *memManager, bool copyBuf = true, Timeout *timeout = 0);
	EventAnswer *triggerAndCollect(char *eventName, char *buf, int size, SharedMemManager *memManager, bool copyBuf = true, EventAnswer *inAnsw = 0, Timeout *timeout = 0);
	void clean(int milliSecs, SharedMemManager *memManager);
	void clean(char *eventName, int milliSecs, SharedMemManager *memManager);
	char *getSharedBuffer(int size);
	static EventManager *getEventManager();
	static SharedMemManager *EventManager::getMemManager();
};

class EventRunnable: public Runnable
{
	void (*callback)(char *, char *, int, bool, int, char*);
	int retSize;
	void *data;
	bool copyBuf;
public:
	EventRunnable(void (*callback)(char *, char *, int, bool, int, char *), bool copyBuf, int retSize, void *data)
	{
		this->callback = callback;
		this->retSize = retSize;
		this->data = data;
		this->copyBuf = copyBuf;
	}
	virtual void run(void *arg)
	{
		EventHandler *eh = (EventHandler *)arg;
		if(copyBuf)
		{
			int dataSize = eh->getDataSize();
			char *evName = new char[strlen(eh->getName()) + 1];
			strcpy(evName, eh->getName());
			char *buf = new char[dataSize];
			memcpy(buf, eh->getDataBuffer(), dataSize);
			char *retBuf = 0;
			if(retSize > 0)
				retBuf = new char[retSize];
			callback(evName, buf, dataSize, eh->isSynch(), (eh->isCollect())?retSize:0, (eh->isCollect())?retBuf:0);
			memcpy(data, retBuf, retSize);
			delete [] retBuf;
			delete [] buf;
			delete [] evName;
		}
		else
			callback(eh->getName(), (char *)eh->getDataBuffer(), eh->getDataSize(), eh->isSynch(), 
			(eh->isCollect())?retSize:0, (eh->isCollect())?(char *)data:0);
	}
};

//User to pack Notifier and EventHandler addresses
class ListenerAddress
{
	EventHandler *handler;
	Notifier *notifier;
	RetEventDataDescriptor *retDataDescr;
public:
	ListenerAddress(EventHandler *handler, Notifier *notifier, RetEventDataDescriptor *retDataDescr)
	{
		this->handler = handler;
		this->notifier = notifier;
		this->retDataDescr = retDataDescr;
	}
	EventHandler *getHandler(){return handler;}
	Notifier *getNotifier(){return notifier;}
	RetEventDataDescriptor *getRetDataDescr(){return retDataDescr;}
};






////////////////Extern C Interface////////////////
#define EVENT_ID 2 //Used in global lock for event structure initialization
#define EVENT_SIZE 10000 //Total size of shared memory used for event management

extern "C" EXPORT void *EventAddListener(char *name,  void (*callback)(char *, char *, int, bool, int retSize, char *retData));
extern "C" EXPORT void *EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, bool, int retSize, char *retData));
extern "C" EXPORT void EventRemoveListener(void *eventHandler);
extern "C" EXPORT int EventTrigger(char *name, char *buf, int size);
extern "C" EXPORT  int EventTriggerAndWait(char *name, char *buf, int size);
extern "C" EXPORT void EventClean();
extern "C" EXPORT void EventReset();
#endif /*EVENTMANAGER_H_*/
