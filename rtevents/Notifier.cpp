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
	triggerSem.post();
}


void Notifier::synchTrigger()
{
	synch = true;
	triggerSem.post();
}

void Notifier::watchdogTrigger()
{
	synch = true;
	watchdogSem.post();
}



void Notifier::waitTermination()
{
	if(synch)
	{
		replySem.wait();
	}
	synch = false;
}

//Return true if timeout occurred
bool Notifier::waitTermination(Timeout &timeout)
{
	bool timeoutOccurred = false;
	
	if(synch)
	{
		//if(sem_timedwait(&replySem, &waitTimeout))
		if(replySem.timedWait(timeout));
			timeoutOccurred = true;
	}
	synch = false;
	return timeoutOccurred;
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
	}
	triggerSem.dispose();
	replySem.dispose();
	watchdogSem.dispose();
	delete thread;
}

//Check for orphan 
bool Notifier::isOrphan()
{
	int semCount = watchdogSem.getValue();
	return semCount != 0;
}

