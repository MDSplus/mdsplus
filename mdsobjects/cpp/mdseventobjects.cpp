#include "mdsobjects.h"

#include <string>

using namespace MDSplus;
using namespace std;

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_ARGS 512

extern "C" {
	int64_t convertAsciiToTime(const char *ascTime);
	int MDSUdpEventAst(char *eventNameIn, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
	int MDSUdpEventCan(int id);
	int MDSUdpEvent(char *eventNameIn, int bufLen, char *buf);
	int MDSEventAst(char *eventNameIn, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
	int MDSEventCan(int id);
	int MDSEvent(char *eventNameIn, int bufLen, char *buf);
	void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int, void *), void *callbackArg);
	void MdsEventRemoveListener(void *eventId);
	int MdsEventTrigger(char *name, char *buf, int size);
	int MdsEventTriggerAndWait(char *name, char *buf, int size);
}

extern "C" void eventAst(void *arg, int len, char *buf)
{
	Event *ev = (Event *)arg;
	ev->eventBuf.assign(buf, len);
	ev->eventTime = convertAsciiToTime("now");
	ev->run(); 
    ev->notify();
}

extern "C" void reventAst(char *evname, char *buf, int len, void *arg)
{
	eventAst(arg, len, buf);
}
	
void Event::connectToEvents()
{
	//MDSUdpEventAst(eventName, eventAst, this, &eventId);
	MDSEventAst(eventName, eventAst, this, &eventId);
}
void Event::disconnectFromEvents()
{
//	MDSUdpEventCan(eventId);
	MDSEventCan(eventId);
}

Data *Event::getData()
{
	if(eventBuf.length() == 0)
		return NULL;
    return deserialize(eventBuf.c_str());
}


Event::Event(char *evName)
{
	std::size_t size = std::string(evName).size();
	eventName = new char[size + 1];
	std::copy(&evName[0], &evName[size], eventName);
	eventName[size] = 0;
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
	setEventRaw(evName, bufLen, buf);
    delete[] buf;
}

void Event::setEventRaw(char *evName, int bufLen, char *buf)
{
//	MDSUdpEvent(evName, bufLen, buf);
	MDSEvent(evName, bufLen, buf);
}

