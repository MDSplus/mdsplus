#ifndef NAMEDSEMAPHORE_H_
#define NAMEDSEMAPHORE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>
#include "SystemException.h"
#include "Timeout.h"

class NamedSemaphore
{
	sem_t *semPtr;
public:
	void initialize(char *name, int initVal) //Named semaphore
	{
		semPtr = sem_open(name, O_CREAT, 0x0777, 1);
        //It appears that Linux does not report the correct privileges in sem_open, so do it with chmod
        chmod(name, 0777);

		if(semPtr == SEM_FAILED)
			throw new SystemException("Error initializing Named Semaphore", errno);
	}
	void initialize(int id, int initVal) //Named semaphore
	{
		char name[64], name1[64];
		sprintf(name, "REALTIME_%d", id);
		sprintf(name1, "/dev/shm/sem.REALTIME_%d", id);
		semPtr = sem_open(name, O_CREAT, 0x0777, 1);
        //It appears that Linux does not report the correct privileges in sem_open, so do it with chmod
        chmod(name1, 0777);

		if(semPtr == SEM_FAILED)
			throw new SystemException("Error initializing Named Semaphore", errno);
	}
	
	int wait() //Return 0 if successful
	{
		sem_wait(semPtr);
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
		int status = sem_timedwait(semPtr, &waitTimeout);
		if(errno != ETIMEDOUT)
			throw new SystemException("Error in UnnamedSemaphore::timedWait", errno);
		return status;
	}
	int post() //Return 0 if successful
	{
		sem_post(semPtr);
	}
	
	int getValue()
	{
		int semCount;
		int status = sem_getvalue(semPtr, &semCount);
		if(status)
			throw new SystemException("Error triggering Notifier", errno);
		return semCount;
	}
	
	void dispose()
	{
		int status = sem_close(semPtr);
		if(status != 0)
			throw new SystemException("Error destroying Named Semaphore", errno);
	}
};



#endif /*NAMEDSEMAPHORE_H_*/
