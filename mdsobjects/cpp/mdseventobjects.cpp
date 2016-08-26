#include "mdsobjects.h"
#include <string>

using namespace MDSplus;
using namespace std;


#define MAX_ARGS 512

extern "C" {
int64_t convertAsciiToTime(const char *ascTime);
int MDSEventAst(const char *eventNameIn, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
int MDSEventCan(int id);
int MDSEvent(const char *eventNameIn, int bufLen, char *buf);
void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int, void *), void *callbackArg);
void  MdsEventRemoveListener(void *eventId);
int MdsEventTrigger(char *name, char *buf, int size);
int MdsEventTriggerAndWait(char *name, char *buf, int size);
}

namespace MDSplus {
void eventAst(void *arg, int len, char *buf)
{   
    Event *ev = (Event *)arg;
    ev->eventBuf.assign(buf, len);
    ev->eventTime = convertAsciiToTime("now");    
    ev->run();
    
    // notify all waiting threads //
    ev->notify();
}
} // MDSplus



void Event::connectToEvents()
{
    if ( !MDSEventAst(this->getName(), MDSplus::eventAst, this, &eventId) )
        throw MdsException("failed to connect to event listener");
}

void Event::disconnectFromEvents()
{    
    if( !MDSEventCan(eventId) )
        throw MdsException("failed to close event listener");
}


Event::Event(const char *name) :
    eventName(name),
    eventId(-1),
    eventTime(convertAsciiToTime("now"))
{}

Event::~Event()
{
    stop();
}

Data * Event::getData() const
{
    if(eventBuf.length() == 0) return NULL;
    return deserialize(eventBuf.c_str());
}

const char *Event::getRaw(size_t *size) const
{
    *size = eventBuf.length();
    return eventBuf.c_str();
}

Uint64 *Event::getTime() const
{    
    return new Uint64(eventTime);
}

const char *Event::getName() const
{
    return eventName.c_str();
}

void Event::start()
{
    connectToEvents();
}

void Event::stop()
{
    if(eventId > -1) {
        disconnectFromEvents();
        eventId = -1;
    }
}

bool Event::isStarted() const
{
    return eventId > -1;
}


void Event::wait(size_t secs)
{
    if( !isStarted() ) start();
    if (secs == 0) condition.wait();
    else if (condition.waitTimeout(secs * 1000) == false)
        throw MdsException("Timeout Occurred");    
}

void Event::notify()
{
    condition.notify(); 
}




void Event::setEvent(const char *evName, Data *evData)
{
    int bufLen;
    char *buf = evData->serialize(&bufLen);
    setEventRaw(evName, bufLen, buf);
    delete[] buf;
}

void Event::setEventRaw(const char *evName, int bufLen, char *buf)
{
    MDSEvent(evName, bufLen, buf);
}

