#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_
#include "EventHandler.h"
#include "Lock.h"

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif


class EventRunnable;
//Class EventManager represents the top manager for events. It is expected that a shared memory
//segment will be dedicated to this instance, which will therefore reside at the beginning of the
//segment. In this segment instances of EventHandler, Notifier and event data buffers will be allocated.
class EventManager
{
	friend class EventRunnable;
	Lock lock;
	RelativePointer eventHead;
	EventHandler *getHandler(char *name);
	EventHandler *getCatchAllHandler();
	
public:
	void initialize();
	void *addCatchAllListener(ThreadAttributes *threadAttr, void (*callback)(char *, char *, int, bool), SharedMemManager *memManager);
	void *addListener(char *eventName, ThreadAttributes *threadAttr, void (*callback)(char *, char *, int, bool), SharedMemManager *memManager);
	void removeListener(void *eventAddr,  SharedMemManager *memManager);
	void trigger(char *eventName, char *buf, int size, SharedMemManager *memManager);
	void triggerAndWait(char *eventName, char *buf, int size, SharedMemManager *memManager);
	bool triggerAndWait(char *eventName, Timeout &timeout, char *buf, int size, SharedMemManager *memManager);
	void clean(int milliSecs, SharedMemManager *memManager);
	void clean(char *eventName, int milliSecs, SharedMemManager *memManager);
};

class EventRunnable: public Runnable
{
	void (*callback)(char *, char *, int, bool);
public:
	EventRunnable(void (*callback)(char *, char *, int, bool))
	{
		this->callback = callback;
	}
	virtual void run(void *arg)
	{
		EventHandler *eh = (EventHandler *)arg;
		char *evName = new char[strlen(eh->getName()) + 1];
		strcpy(evName, eh->getName());
		int size = eh->getDataSize();
		char *buf = new char[size];
		memcpy(buf, eh->getDataBuffer(), size);
		callback(evName, buf, size, eh->isSynch());
		delete [] buf;
		delete [] evName;
	}
};

//User to pack Notifier and EventHandler addresses
class ListenerAddress
{
	EventHandler *handler;
	Notifier *notifier;
public:
	ListenerAddress(EventHandler *handler, Notifier *notifier)
	{
		this->handler = handler;
		this->notifier = notifier;
	}
	EventHandler *getHandler(){return handler;}
	Notifier *getNotifier(){return notifier;}
};

////////////////Extern C Interface////////////////
#define EVENT_ID 2 //Used in global lock for event structure initialization
#define EVENT_SIZE 10000 //Total size of shared memory used for event management

extern "C" EXPORT void *EventAddListener(char *name,  void (*callback)(char *, char *, int, bool));
extern "C" EXPORT void *EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, bool));
extern "C" EXPORT void EventRemoveListener(void *eventHandler);
extern "C" EXPORT int EventTrigger(char *name, char *buf, int size);
extern "C" EXPORT  int EventTriggerAndWait(char *name, char *buf, int size);
extern "C" EXPORT void EventClean();
extern "C" EXPORT void EventReset();
#endif /*EVENTMANAGER_H_*/
