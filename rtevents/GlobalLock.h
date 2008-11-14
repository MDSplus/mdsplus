#ifndef GLOBALLOCK_H_
#define GLOBALLOCK_H_

#include "NamedSemaphore.h"

//Requiored  to avoid name clash with Windows definitions
#ifdef HAVE_WINDOWS_H
#define GlobalLock NonWindowsGlobalLock
#endif


//Class Lock handles a critical section using a unnamed semaphore
class GlobalLock
{
	NamedSemaphore sem;
public:
	//Open the semaphore. 
	void initialize(int id)
	{
		sem.initialize(id, 1);
	}
	
	void dispose()
	{
		sem.dispose();
	}
	
	void lock()
	{
		sem.wait();
	}

	void unlock()
	{
		sem.post();
	}
};

#endif /*GLOBALLOCK_H_*/
