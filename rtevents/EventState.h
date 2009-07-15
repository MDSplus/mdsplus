#ifndef EVENT_STATE_H_
#define EVENT_STATE_H_
#include <string.h>
#include "State.h"

class EventState:State
{
	class EventDescr {
	public:
		char *name;
		int numRegistered;
		int numPending;
		EventDescr *nxt;
		EventDescr(char *name, int numRegistered, int numPending, EventDescr *nxt)
		{
			this->name = new char[strlen(name) + 1];
			strcpy(this->name, name);
			this->numRegistered = numRegistered;
			this->numPending = numPending;
			this->nxt = nxt;
		}
		~EventDescr()
		{
			delete [] name;
		}
	};

	int numEvents;
	EventDescr *eventHead;
public:
	EventState() 
	{
		numEvents = 0;
		eventHead = 0;
	}
	~EventState()
	{
		EventDescr *currEvent = eventHead, *prevEvent;
		while(currEvent)
		{
			prevEvent = currEvent;
			currEvent = currEvent->nxt;
			delete prevEvent;
		}
	}


	void addEvent(char *name, int numRegistered, int numPending)
	{
		EventDescr *evDescr = new EventDescr(name, numRegistered, numPending, eventHead);
		eventHead = evDescr;
		numEvents++;
	}


    char *serialize(int &retSize)
	{
		EventDescr *currEvent = eventHead;

		retSize = sizeof(int);
		while(currEvent)
		{
			retSize += 3 * sizeof(int) + strlen(currEvent->name);
			currEvent = currEvent->nxt;
		}
		char *retBuf = new char[retSize];
		char *currPtr = retBuf;
		*((int *)currPtr) = swap(numEvents);
		currPtr += sizeof(int);
		currEvent = eventHead;
		while(currEvent)
		{
			int len = strlen(currEvent->name);
			*((int *)currPtr) = swap(len);
			currPtr += sizeof(int);
			memcpy(currPtr, currEvent->name, len);
			currPtr += len;
			*((int *)currPtr) = swap(currEvent->numRegistered);
			currPtr += sizeof(int);
			*((int *)currPtr) = swap(currEvent->numPending);
			currPtr += sizeof(int);
			currEvent = currEvent->nxt;
		}
		return retBuf;
	}
};

#endif