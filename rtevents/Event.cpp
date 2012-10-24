#include "SharedMemManager.h"
#include "Event.h"

extern "C" EXPORT void EventWait(char *name, char *buf, int size, int *retSize);
extern "C" EXPORT void EventReset();
extern "C" EXPORT void * EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, void *, bool, int, char *, int), void *callbackArg);
extern "C" EXPORT void * EventAddListener(char *name,  void (*callback)(char *, char *, int, void *, bool, int, char*, int), void *callbackArg);
extern "C" EXPORT void EventRemoveListener(void *eventHandler);
extern "C" EXPORT  int EventTrigger(char *name, char *buf, int size);
extern "C" EXPORT int EventTriggerAndWait(char *name, char *buf, int size);
extern "C" EXPORT int EventTriggerAndTimedWait(char *name, char *buf, int size, int millisecs);
extern "C" EXPORT void EventClean();

extern "C" EXPORT char *EventGetSerializedState(int *size);
extern "C" EXPORT char *EventGetSerializedMemState(int *size);


//////////////////////////External C Interface/////////////////////

void Event::reset()
{
	GlobalLock eventLock;
	eventLock.initialize(EVENT_ID);
	eventLock.lock();
	bool firstCreated = memManager->initialize(EVENT_ID, EVENT_SIZE);
	memManager->reset();
	evManager = (EventManager *)memManager->allocate(sizeof(EventManager));
	evManager->initialize();
//	evManager = EventManager::getEventManager();
	eventLock.unlock();
	eventLock.dispose();
}

EXPORT void EventReset()
{
	Event ev;
	ev.reset();
}

//Callback Management
extern "C" {
    struct CallbackInfo{
      void (*callback)(char *, char *, int, void *, bool, int, char *, int);
	  void *callbackArg;
    };
	void intCallbackC(char *c1, char *c2, int i1, void *ciPtr, bool b, int i2, char *c3, int i3)
	{
		CallbackInfo *ci = (CallbackInfo *)ciPtr;
		ci->callback(c1, c2, i1, ci->callbackArg, b, i2, c3, i3);
	}
}
//Indirect call to make SUN compiler happy
static void intCallbackCPP(char *c1, char *c2, int i1, void *ciPtr, bool b, int i2, char *c3, int i3)
{
	intCallbackC(c1,c2,i1,ciPtr, b,i2,c3, i3);
}
extern "C" EXPORT void * EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, void *, bool, int, char *, int), void *callbackArg)
{
	Event ev;
	try {
		CallbackInfo *ci = new CallbackInfo;
		ci->callback = callback;
		ci->callbackArg = callbackArg;
		void *handl = ev.addListenerGlobal(name, intCallbackCPP, (void *)ci);
		return handl;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return NULL;
	}
}
extern "C" EXPORT void * EventAddListener(char *name,  void (*callback)(char *, char *, int, void *, bool, int, char*, int), void *callbackArg)
{
	Event ev;
	try {
		CallbackInfo *ci = new CallbackInfo;
		ci->callback = callback;
		ci->callbackArg = callbackArg;
		void *handl = ev.addListener(name, intCallbackCPP, (void *)ci);
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
	Event ev;
	try {
		ev.removeListener(eventHandler);
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
	}
	
}

EXPORT  int EventTrigger(char *name, char *buf, int size)
{
	Event ev;
	try {
		ev.trigger(name, buf, size);
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
	Event ev;
	try {
		ev.triggerAndWait(name, buf, size);
		return 0;
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
		return -1;
	}
}
//	bool triggerAndWait(const char *eventName, char *buf, int size, bool copyBuf = true, MdsTimeout *timeout = 0)

EXPORT int EventTriggerAndTimedWait(char *name, char *buf, int size, int millisecs)
{
	Event ev;
	try {
		MdsTimeout *timout = new MdsTimeout(millisecs);
		ev.triggerAndWait(name,  buf, size, true, timout);
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
	Event ev;
	try {
		ev.clean(100);
	}
	catch(SystemException *exc)
	{
		printf("%s\n", exc->what());
	}
}

struct WaitDescr
{
	UnnamedSemaphore *sem;
	char *buf;
	int size;
	int *retSize;
};

static void waitCallback(char *name, char *buf, int size, void *arg, bool isTimed, int retSize, char *retBuf, int type)
{
	WaitDescr *waitDsc = (WaitDescr *)arg;
	int actSize = (size < waitDsc->size)?size:waitDsc->size;
	memcpy(waitDsc->buf, buf, actSize);
	*(waitDsc->retSize) = actSize;
	waitDsc->sem->post();
}




EXPORT void EventWait(char *name, char *buf, int size, int *retSize)
{

	UnnamedSemaphore sem;
	sem.initialize(0);
	Event ev;
	WaitDescr waitDsc;
	waitDsc.sem = &sem;
	waitDsc.size = size;
	waitDsc.retSize = retSize;
	waitDsc.buf = buf;

	void *evHandler = ev.addListener(name, waitCallback, (void *)&waitDsc);
	sem.wait();
	ev.removeListener(evHandler);
	sem.dispose();
}


char *EventGetSerializedState(int *size)
{
	Event ev;
	EventState *evState = ev.getState();
	int serSize;
	char *ser = evState->serialize(serSize);
	char *outSer = (char *)malloc(serSize);
	memcpy(outSer, ser, serSize);
	delete evState;
	delete[]ser;
	*size = serSize;
	return outSer;
}


char *EventGetSerializedMemState(int *size)
{
	Event ev;
	SharedMemState *evState = ev.getMemState();
	int serSize;
	char *ser = evState->serialize(serSize);
	char *outSer = (char *)malloc(serSize);
	memcpy(outSer, ser, serSize);
	delete evState;
	delete[]ser;
	*size = serSize;
	return outSer;
}
