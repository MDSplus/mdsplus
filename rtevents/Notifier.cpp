#include "Notifier.h"
#include "SystemException.h"
#include <time.h>


void Notifier::initialize(ThreadAttributes *threadAttr, Runnable *runnable, void *arg)
{
	thread = new Thread(threadAttr);
	
	
	triggerSem.initialize(0);
	replySem.initialize(0);
	watchdogSem.initialize(0);

	notified = new Notified(runnable, arg);
	thread->start(notified, this);
	
	watchdogNotified = new WatchdogNotified();
	watchdogThread = new Thread(0);
	watchdogThread->start(watchdogNotified, this);
	
	
	synch = false;
	finished = true;
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
		//synch = true;
		watchdogSem.post();
	}catch(SystemException *exc){}
}

bool Notifier::isPending()
{
	return synch;
}



void Notifier::waitTermination()
{
	if(synch)
	{
		finished = false;
		try {
			replySem.wait();
		}catch(SystemException *exc){}
		finished = true;
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
			finished = false;
			//if(sem_timedwait(&replySem, &waitTimeout))
			if(replySem.timedWait(timeout))
				timeoutOccurred = true;
			finished = true;
		}
		synch = false;
		return timeoutOccurred;
	}catch(SystemException *exc){return true;}
}

//Terminates waiting thread and release dynamic memory
//To be called only by the process which called initialize()
void Notifier::dispose(bool semaphoresOnly, SharedMemManager *memManager)
{
	if(!semaphoresOnly) //Notifier instance is being disposed by the same listener process which created it
	{
		notified->dispose();
		watchdogNotified->dispose();
		triggerSem.post();
		watchdogSem.post();
		thread->join(); //wait for actual termination of the underlying thread, once awakened
		watchdogThread->join();
		delete thread;
		delete watchdogThread;
		//delete notified;
		//delete watchdogNotified;
		triggerSem.dispose();
		replySem.dispose();
		watchdogSem.dispose();
	}
	else 
//Notifier instance is being disposed possibly by another process in a clean operation
//it cannot be deallocated immediately, but only after either
//when a sender waiting for termination terminates or a timeout expires (in the case also the sender was gone)
 
	{
		triggerSem.dispose();
		watchdogSem.dispose();
		//Trigger possible waiting sender
		replySem.post();
		Thread *terminatorThread = new Thread();
		terminatorThread->start(new NotifierTerminator(this, memManager), (void *)terminatorThread);
	}
}
void Notifier::dispose() //Only local
{
	notified->dispose();
	watchdogNotified->dispose();
	triggerSem.post();
	watchdogSem.post();
	thread->join(); //wait for actual termination of the underlying thread, once awakened
	watchdogThread->join();
	delete thread;
	delete watchdogThread;
	//delete notified;
	//delete watchdogNotified;
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

