#include "mdsobjects.h"
using namespace MDSplus;

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_ARGS 512


extern "C" {
	_int64 convertAsciiToTime(char *ascTime);
	int MDSUdpEventAst(char *eventNameIn, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
	int MDSUdpEventCan(int id);
	int MDSUdpEvent(char *eventNameIn, int bufLen, char *buf);
	void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int, void *), void *callbackArg);
	void MdsEventRemoveListener(void *eventId);
	int MdsEventTrigger(char *name, char *buf, int size);
	int MdsEventTriggerAndWait(char *name, char *buf, int size);
}


extern "C" void eventAst(void *arg, int len, char *buf)
{
	Event *ev = (Event *)arg;
	ev->eventBufSize = len;
	ev->eventBuf = new char[len];
	memcpy(ev->eventBuf, buf, len);
	ev->eventTime = convertAsciiToTime("now");
	ev->run();
}
extern "C" void reventAst(char *evname, char *buf, int len, void *arg)
{
	eventAst(arg, len, buf);
}
	
void Event::connectToEvents()
{
	MDSUdpEventAst(eventName, eventAst, this, &eventId);
}
void Event::disconnectFromEvents()
{
	MDSUdpEventCan(eventId);
}

Data *Event::getData()
{
	if(eventBufSize == 0)
		return NULL;
    return deserialize(eventBuf);

}


void REvent::connectToEvents()
{
	reventId = MdsEventAddListener(eventName,  reventAst, this);
}
void REvent::disconnectFromEvents()
{
	MdsEventRemoveListener(reventId);
}

Event::Event(char *evName)
{
	eventBufSize = 0;
	eventBuf = 0;
	eventName = new char[strlen(evName) + 1];
	strcpy(eventName, evName);
	eventId = -1;
	connectToEvents();
}
REvent::REvent(char *evName)
{
	eventBufSize = 0;
	eventBuf = 0;
	eventName = new char[strlen(evName) + 1];
	strcpy(eventName, evName);
	eventId = -1;
	connectToEvents();
}


Event::~Event()
{
	delete [] eventName;
	if(eventId != -1)
		disconnectFromEvents();
		
}


void Event::setEvent(char *evName, Data *evData)
{
	int bufLen;
	char *buf = evData->serialize(&bufLen);
	MDSUdpEvent(evName, bufLen, buf);
}
void Event::setEventRaw(char *evName, int bufLen, char *buf)
{
	MDSUdpEvent(evName, bufLen, buf);
}

void REvent::setEvent(char *evName, Data *evData)
{
	int bufLen;
	char *buf = evData->serialize(&bufLen);
	MdsEventTrigger(evName, buf, bufLen);
}
void REvent::setEventAndWait(char *evName, Data *evData)
{
	int bufLen;
	char *buf = evData->serialize(&bufLen);
	MdsEventTriggerAndWait(evName, buf, bufLen);
}
void REvent::setEventRawAndWait(char *evName, int bufLen, char *buf)
{
	MdsEventTriggerAndWait(evName, buf, bufLen);
}
