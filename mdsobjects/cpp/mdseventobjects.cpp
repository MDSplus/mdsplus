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
	int MDSEventAst(char *eventNameIn, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
	int MDSEventCan(int id);
	int MDSEvent(char *eventNameIn, int bufLen, char *buf);
}


extern "C" void eventAst(void *arg, int len, char *buf)
{
	Event *ev = (Event *)arg;
	ev->eventData = deserialize(buf, len);
	ev->eventTime = convertAsciiToTime("now");
	ev->run();
}
	


Event::Event(char *evName)
{
	eventData = 0;
	eventName = new char[strlen(evName) + 1];
	strcpy(eventName, evName);
	eventId = -1;
	MDSEventAst(evName, eventAst, this, &eventId);
}

Event::~Event()
{
	if(eventData)
		deleteData(eventData);
	delete [] eventName;
	if(eventId != -1)
		MDSEventCan(eventId);
}

void Event::setevent(char *evName, Data *evData)
{
	int bufLen;
	char *buf = evData->serialize(&bufLen);
	MDSEvent(evName, bufLen, buf);
}
