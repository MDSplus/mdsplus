#ifndef UNNAMEDSEMAPHORE_H_
#define UNNAMEDSEMAPHORE_H_
#include "SystemSpecific.h"
#ifdef HAVE_WINDOWS_H
#include "WindowsUnnamedSemaphore.h"
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
#include <time.h>
#include "SystemException.h"
#include "Timeout.h"
#include <pthread.h>

struct TimeoutStruct {
    struct timespec waitTimeout;    
    sem_t *sem;
    bool *timeoutOccurred;
    bool waitPending;
};

static void *handleTimeout(void *arg)
{
    struct TimeoutStruct *infoPtr = (struct TimeoutStruct *)arg;
    nanosleep(&infoPtr->waitTimeout, NULL);
    if(infoPtr->waitPending)
    {
	*infoPtr->timeoutOccurred = true;
	sem_post(infoPtr->sem);
    }
    else
    	delete infoPtr;
    return NULL;
}
    
    



class EXPORT UnnamedSemaphore
{
	sem_t semStruct;

	

public:
	void initialize(int initVal)
	{
		int status = sem_init(&semStruct, 1, initVal);
		if(status != 0)
			throw new SystemException("Error initializing Semaphore", errno);
	}
	
	void  wait() 
	{
		int status;
		while(status = sem_wait(&semStruct))
		{
			if(errno != EINTR)
				throw new SystemException("Error waiting semaphore", errno);
		}
	}

	int timedWait(MdsTimeout &timeout)
	{
		struct timespec waitTimeout;
		waitTimeout.tv_sec = timeout.getSecs();
		waitTimeout.tv_nsec = timeout.getNanoSecs();
		bool timeoutOccurred = false;
		struct TimeoutStruct *timout = new TimeoutStruct;
		timout->sem = &semStruct;
		timout->waitTimeout = waitTimeout;
		timout->waitPending = true;
		timout->timeoutOccurred = &timeoutOccurred;
		pthread_t pid;
		pthread_create(&pid, NULL, handleTimeout,  (void *)timout);
		int status = sem_wait(&semStruct);
		if(timeoutOccurred)
		    delete timout;
		else
		    timout->waitPending = false;
		return status;
	}
	void  post() //return 0 if seccessful
	{
		int status = sem_post(&semStruct);
		if(status != 0)
			throw new SystemException("Error waiting Semaphore", errno);
	}
	
	bool isZero()
	{
		int semCount;
		int status = sem_getvalue(&semStruct, &semCount);
		if(status)
			throw new SystemException("Error triggering Notifier", errno);
		return semCount == 0;
	}
	
	void dispose()
	{
		int status = sem_destroy(&semStruct);
		if(status != 0)
			throw new SystemException("Error destroying Semaphore", errno);
	}
};

#endif
#endif /*UNNAMEDSEMAPHORE_H_*/
