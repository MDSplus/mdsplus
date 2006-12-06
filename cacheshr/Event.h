#ifndef EVENT_H
#define EVENT_H

#ifdef HAVE_WINDOWS_H

#include <windows.h>
#include <process.h>

class Event
{
	HANDLE eventH;

public:
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
#else
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
class Event
{
    sem_t semaphore;
 
 public:   
    Event()
    {
    	int pshared = 1;
	unsigned int value = 0;
	int sts;
	
	sts = sem_init(&semaphore, pshared, value);
	if(sts)
	    perror("sem_init() Failed");
    }
    
    void wait()
    {
    	sem_wait(&semaphore);
    }
    void signal()
    {
    	sem_post(&semaphore);
    }

#endif
};




#endif
