#include "Notifier.h"
#include "SystemException.h"
#include <time.h>


void Notifier::initialize(Thread *thread, Runnable *runnable, void *arg)
{
	this->thread = thread;
	
	triggerSem.initialize(0);
	replySem.initialize(0);
	watchdogSem.initialize(0);

	notified = new Notified(runnable, arg);
	thread->start(notified, this);
	
	watchdogNotified = new WatchdogNotified();
	watchdogThread.start(watchdogNotified, this);
	
	
	synch = false;
	nxt = NULL;
	prv = NULL;
}


//Method trigger can be called by a separate process, provided the Notifier instance is 
//in stored in shared memory. It operated only via shared unnamed semaphore(s).
void Notifier::trigger()
{
	try {
		triggerSem.post();
	}catch(SystemException *exc){}
}


void Notifier::synchTrigger()
{
	synch = true;
	try {
		triggerSem.post();
	}catch(SystemException *exc){}
}

void Notifier::watchdogTrigger()
{
	try {
		synch = true;
		watchdogSem.post();
	}catch(SystemException *exc){}
}



void Notifier::waitTermination()
{
	if(synch)
	{
		try {
			replySem.wait();
		}catch(SystemException *exc){}
	}
	synch = false;
}

//Return true if timeout occurred
bool Notifier::waitTermination(Timeout &timeout)
{
	bool timeoutOccurred = false;
	try {
		if(synch)
		{
			//if(sem_timedwait(&replySem, &waitTimeout))
			if(replySem.timedWait(timeout))
				timeoutOccurred = true;
		}
		synch = false;
		return timeoutOccurred;
	}catch(SystemException *exc){return true;}
}

//Terminates waiting thread and release dynamic memory
//To be called only by the process which called initialize()
void Notifier::dispose(bool semaphoresOnly)
{
	if(!semaphoresOnly)
	{
		notified->dispose();
		watchdogNotified->dispose();
		triggerSem.post();
		watchdogSem.post();
		thread->join(); //wait for actual termination of the underlying thread, once awakened
		watchdogThread.join();
		delete thread;
	}
	triggerSem.dispose();
	replySem.dispose();
	watchdogSem.dispose();
}

//Check for orphan 
bool Notifier::isOrphan()
{
	try {
		return !watchdogSem.isZero();
	}catch(SystemException *exc)
	{
		return true;
	}
}

