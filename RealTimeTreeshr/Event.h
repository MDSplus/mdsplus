#ifndef EVENT_H
#define EVENT_H

#ifdef HAVE_WINDOWS_H

#include <windows.h>
#include <process.h>

class Event
{
	HANDLE eventH;

public:
	Event(char *name)
	{
		eventH = CreateEvent(NULL, FALSE, FALSE, name);
	}
	Event()
	{
		eventH = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	void wait()
	{
		WaitForSingleObject(eventH, INFINITE);
	}
	void signal()
	{
		SetEvent(eventH);
	}

#endif
};




#endif