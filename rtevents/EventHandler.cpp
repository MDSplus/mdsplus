#include "EventHandler.h"
#include "ExitHandler.h"

void EventHandler::initialize(char *inName, SharedMemManager *memManager)
{
	name = memManager->allocate(strlen(inName) + 1);
	char *sharedName = (char *)name.getAbsAddress();
	strcpy(sharedName, inName);
	notifierHead = NULL;
	dataBuffer = NULL;
	setNext(NULL);
	lock.initialize();
	waitLock.initialize();
	catchAll = false;
}

void EventHandler::initialize()
{
	name = NULL;
	notifierHead = NULL;
	dataBuffer = NULL;
	setNext(NULL);
	lock.initialize();
	waitLock.initialize();
	catchAll = true;
}
void EventHandler::setName(char *inName, SharedMemManager *memManager)
{
	if(name.getAbsAddress())
		memManager->deallocate((char *)name.getAbsAddress(), strlen((char *)name.getAbsAddress()));
	name = memManager->allocate(strlen(inName) + 1);
	char *sharedName = (char *)name.getAbsAddress();
	strcpy(sharedName, inName);
}

void *EventHandler::addListener(ThreadAttributes *threadAttr, Runnable *runnable, void *arg, SharedMemManager *memManager)
{
	lock.lock();
	Notifier *newNotifier = (Notifier *)memManager->allocate(sizeof(Notifier));
	newNotifier->initialize(threadAttr, runnable, arg);
	newNotifier->setNext((Notifier *)notifierHead.getAbsAddress());
	if(notifierHead.getAbsAddress())
		((Notifier *)notifierHead.getAbsAddress())->setPrev(newNotifier);
	notifierHead = newNotifier;
	lock.unlock();
	return newNotifier;
}

//RemoveListener removes the corresponding Notifier from the notifier chain if found
//The passed address is valid only in the same address space of the process which
//created the Notifier instance
void EventHandler::removeListener(void *notifierAddr, SharedMemManager *memManager)
{
	lock.lock();
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
	while(currNotifier)
	{
		if(currNotifier == notifierAddr)
		{
			if(currNotifier == notifierHead.getAbsAddress())
			{
				Notifier *nextNotifier = currNotifier->getNext();
				notifierHead = nextNotifier;
				if(nextNotifier)
					nextNotifier->setPrev(NULL);
			}
			else
			{
				currNotifier->getPrev()->setNext(currNotifier->getNext());
			}
			currNotifier->dispose(false, memManager);
			//memManager->deallocate((char *)currNotifier, sizeof(Notifier));
			break;
		}
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
}

void EventHandler::clean(SharedMemManager *memManager)
{
	lock.lock();
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
	while(currNotifier)
	{
		Notifier *nextNotifier = currNotifier->getNext();
		if(currNotifier->isOrphan())
		{
			printf("Removed Orphan for event %s\n", name.getAbsAddress());
			if(currNotifier == notifierHead.getAbsAddress())
			{
				Notifier *nextNotifier = currNotifier->getNext();
				notifierHead = nextNotifier;
				if(nextNotifier)
					nextNotifier->setPrev(NULL);
			}
			else
			{
				currNotifier->getPrev()->setNext(currNotifier->getNext());
			}
			currNotifier->dispose(true, memManager);//Dispose only semaphores
		}
		currNotifier = nextNotifier;
	}
	lock.unlock();
	
}


void EventHandler::triggerAndWait(char *buf, int size, SharedMemManager *memManager)
{
	waitLock.lock();
	setData(buf, size, memManager);
	lock.lock();
	synch = true;
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();

//Count Notifiers
	int numNotifiers = 0;
	while(currNotifier)
	{
		numNotifiers++;
		currNotifier = currNotifier->getNext();
	}
	Notifier **notifiers = new Notifier*[numNotifiers];


	currNotifier = (Notifier *)notifierHead.getAbsAddress();
//First pass: asynchronous triggers
	int i = 0;
	while(currNotifier)
	{
		currNotifier->synchTrigger();
		notifiers[i++] = currNotifier;
		currNotifier = currNotifier->getNext();
	}

	lock.unlock();
	ExitHandler::atExit(new WaitLockTerminator(&waitLock));
	for(i = 0; i < numNotifiers; i++)
		notifiers[i]->waitTermination();
	ExitHandler::dispose();
	waitLock.unlock();
}

bool EventHandler::triggerAndWait(Timeout &timeout)
{
	lock.lock();
	synch = true;
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
//First pass: asynchronous triggers
	while(currNotifier)
	{
		currNotifier->synchTrigger();
		currNotifier = currNotifier->getNext();
	}
	currNotifier = (Notifier *)notifierHead.getAbsAddress();
//Second pass: wait synch termination
	bool timeoutOccurred = false;
	while(currNotifier)
	{
		timeoutOccurred |= currNotifier->waitTermination(timeout);
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
	return timeoutOccurred;
}

void EventHandler::trigger()
{
	lock.lock();
	synch = false;
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
//First pass: asynchronous triggers
	while(currNotifier)
	{
		currNotifier->trigger();
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
}

void EventHandler::watchdogTrigger()
{
	lock.lock();
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
//First pass: asynchronous triggers
	while(currNotifier)
	{
		currNotifier->watchdogTrigger();
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
}

bool EventHandler::corresponds(char *inName)
{
	if(catchAll)
		return true;
	bool corr = false;
	char *sharedName = (char *)name.getAbsAddress();
	int nameLen = strlen(inName);
	if(nameLen > 0 && inName[nameLen-1] == '*')
		corr = !strncmp(sharedName, inName, nameLen - 1);
	else
		corr = !strcmp(sharedName, inName);
	return corr;
}

void EventHandler::setData(void *buf, int size, SharedMemManager *memManager)
{
	if(dataBuffer.getAbsAddress())
		memManager->deallocate((char *)dataBuffer.getAbsAddress(), dataSize);
	dataSize = size;
	if(size == 0)
		dataBuffer = NULL;
	else
		dataBuffer = memManager->allocate(size);
	memcpy(dataBuffer.getAbsAddress(), buf, size);
}
