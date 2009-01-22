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
	
	int timedWait(Timeout &timeout)
	{
		struct timespec waitTimeout;
		time(&waitTimeout.tv_sec);
		waitTimeout.tv_sec += timeout.getSecs();
		waitTimeout.tv_nsec += timeout.getNanoSecs();
		if(waitTimeout.tv_nsec >= 1000000000)
		{
			waitTimeout.tv_nsec %= 1000000000;
			waitTimeout.tv_sec++;
		}
		bool timeoutOccurred = false;
		int status = sem_timedwait(&semStruct, &waitTimeout);
		if(errno != ETIMEDOUT)
			throw new SystemException("Error in UnnamedSemaphore::timedWait", errno);
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
