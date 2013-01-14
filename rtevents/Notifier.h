#ifndef NOTIFIER_H_
#define NOTIFIER_H_
#include <stdio.h>
#include "SystemSpecific.h"
#include "Runnable.h"
#include "Thread.h"
#include "RelativePointer.h"
#include "Timeout.h"
#include "UnnamedSemaphore.h"
#include "SharedMemManager.h"
#include "Delay.h"

class Notified;
class WatchdogNotified;

class  EXPORT Notifier
{
	friend class Notified;
	friend class WatchdogNotified;
	friend class NotifierTerminator;
	Thread *thread;
	Thread *watchdogThread;
	UnnamedSemaphore triggerSem;
	UnnamedSemaphore replySem;
	UnnamedSemaphore watchdogSem;
	Notified *notified;
	WatchdogNotified *watchdogNotified;
	RelativePointer nxt, prv; 
	bool synch;
	bool finished;

public:
bool isSynch() { return synch;}
Notifier *getNext() {return (Notifier *)nxt.getAbsAddress();}
void setNext(Notifier *ntf) {nxt = ntf;}
Notifier *getPrev() {return (Notifier *)prv.getAbsAddress();}
void setPrev(Notifier *ntf) {prv = ntf;}
void initialize(ThreadAttributes *threadAttr, Runnable *runnable, void *arg);
void trigger();
void synchTrigger();
void watchdogTrigger();
bool waitTermination(Timeout &);
void waitTermination();
bool isPending();
void dispose(bool semaphoresOnly, SharedMemManager *memManager);
void dispose();
bool isOrphan();
};


class Notified: public Runnable
{
	Runnable *rtn;
	void *arg;
	bool disposed;
public:
	Notified(Runnable *inRtn, void *inArg)
	{
		rtn = inRtn;
		arg = inArg;
		disposed = false;
	}
	
	void dispose() 
	{
		disposed = true;
	}
	virtual void run(void *inNtf)
	{
		Notifier *ntf = (Notifier *)inNtf;
		
		while(true)
		{
			//if(sem_wait(&ntf->triggerSem))
			ntf->triggerSem.wait();
			if(disposed)
			{
				printf("Notifier exited\n");
				delete rtn;
				ntf->finished = true;
				return;
			}
			rtn->run(arg);
			if(ntf->synch)
				ntf->replySem.post();
				//sem_post(&ntf->replySem);
		}
	}
};

class WatchdogNotified: public Runnable
{
	bool disposed;
public:
	
	void dispose() 
	{
		disposed = true;
	}
	virtual void run(void *inNtf)
	{
		Notifier *ntf = (Notifier *)inNtf;
		disposed = false;
		while(true)
		{
			//if(sem_wait(&ntf->watchdogSem))
			ntf->watchdogSem.wait();
			if(disposed)
			{
				printf("Watchdog Notified exited\n");
				return;
			}
		}
	}
};

class NotifierTerminator: public Runnable
{
	Notifier *ntf;
	SharedMemManager *memManager;
public:
	NotifierTerminator(Notifier *ntf, SharedMemManager *memManager)
	{
		this->ntf = ntf;
		this->memManager = memManager;
	}

	virtual void run(void *threadPtr)
	{
		Delay delay(100);
		for(int i = 0; i < 10; i++) //1 Sec timeout
		{
			delay.wait();
			if(ntf->finished)
			{
				ntf->replySem.dispose();
				memManager->deallocate((char *)ntf, sizeof(Notifier));
				delete((Thread *)threadPtr);
				return;
			}
		}
//After timeout: remove Notifier anyway
		ntf->replySem.dispose();
		memManager->deallocate((char *)ntf, sizeof(Notifier));
		delete((Thread *)threadPtr);
	}
};



#endif /*NOTIFIER_H_*/
