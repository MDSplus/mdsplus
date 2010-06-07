#ifndef SEM_EVENT_H
#define SEM_EVENT_H

#ifdef HAVE_WINDOWS_H

#include <windows.h>
#include <process.h>

class SemEvent
{
	HANDLE eventH;
 
 public: 
	SemEvent(){initialize();}
	void initialize()
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
};

#else
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <semLib.h>
class SemEvent
{
    SEM_ID semaphore;
 
 public: 
	SemEvent(){initialize();}
    void initialize()
    {
	semaphore = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    }
    
    void wait()
    {
    	semTake(semaphore, WAIT_FOREVER);
    }
    void signal()
    {
    	semGive(semaphore);
    }
};


#else
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
class SemEvent
{
    sem_t semaphore;
 
 public: 
	SemEvent(){initialize();}
    void initialize()
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
};


#endif
#endif



#endif
