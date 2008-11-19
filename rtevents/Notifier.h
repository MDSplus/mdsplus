#ifndef NOTIFIER_H_
#define NOTIFIER_H_
#include <stdio.h>
#include "Runnable.h"
#include "Thread.h"
#include "RelativePointer.h"
#include "Timeout.h"
#include "UnnamedSemaphore.h"

class Notified;
class WatchdogNotified;

class  Notifier
{
	friend class Notified;
	friend class WatchdogNotified;
	Thread *thread;
	Thread watchdogThread;
	UnnamedSemaphore triggerSem;
	UnnamedSemaphore replySem;
	UnnamedSemaphore watchdogSem;
	Notified *notified;
	WatchdogNotified *watchdogNotified;
	RelativePointer nxt, prv; 
	bool synch;

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
void dispose(bool semaphoresOnly = false);
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

#endif /*NOTIFIER_H_*/
