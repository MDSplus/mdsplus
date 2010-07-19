#include "EventHandler.h"
#include "ExitHandler.h"

void EventHandler::initialize(const char *inName, SharedMemManager *memManager)
{
	name = memManager->allocate(strlen(inName) + 1);
	char *sharedName = (char *)name.getAbsAddress();
	strcpy(sharedName, inName);
	notifierHead = NULL;
	dataBuffer = NULL;
	retDataHead = NULL;
	setNext(NULL);
	lock.initialize();
	waitLock.initialize();
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


void *EventHandler::addRetBuffer(int size, SharedMemManager *memManager)
{
	lock.lock();
	RetEventDataDescriptor *dataDescr = (RetEventDataDescriptor *)memManager->allocate(sizeof(RetEventDataDescriptor));
	dataDescr->initialize();
	void *data;
	if(size > 0)
		data = memManager->allocate(size);
	else
		data = 0;
	dataDescr->setData(data, size);
	dataDescr->setNext(retDataHead.getAbsAddress());
	if(retDataHead.getAbsAddress())
	{
		RetEventDataDescriptor *currHead = (RetEventDataDescriptor *)retDataHead.getAbsAddress();
		currHead->setPrev(dataDescr);
	}
	retDataHead = dataDescr;
	lock.unlock();
	return dataDescr;
}

void EventHandler::removeRetDataDescr(RetEventDataDescriptor *retDataDescr,  SharedMemManager *memManager)
{
	printf("EventHandler::removeRetDataDescr\n");
	lock.lock();
	RetEventDataDescriptor *currDataDescr = (RetEventDataDescriptor *)retDataHead.getAbsAddress();
	while(currDataDescr)
	{
		if(currDataDescr == retDataDescr)
		{
			if(currDataDescr->getPrev())
				currDataDescr->getPrev()->setNext(currDataDescr->getNext());
			if(currDataDescr->getNext())
				currDataDescr->getNext()->setPrev(currDataDescr->getPrev());
			if(retDataHead.getAbsAddress() == currDataDescr)
				retDataHead = currDataDescr->getNext();
			retDataDescr->deallocateData(memManager);

			memManager->deallocate((char *)retDataDescr, sizeof(RetEventDataDescriptor));
			lock.unlock();
			return;
		}
		currDataDescr = currDataDescr->getNext();
	}
	lock.unlock();
}

void EventHandler::resizeRetData(RetEventDataDescriptor *retDataDescr, int newSize, SharedMemManager *memManager)
{
	lock.lock();
	retDataDescr->resizeData(newSize, memManager);
	lock.unlock();
}
		


//RemoveListener removes the corresponding Notifier from the notifier chain if found
//The passed address is valid only in the same address space of the process which
//created the Notifier instance
void EventHandler::removeListener(void *notifierAddr, SharedMemManager *memManager)
{
	waitLock.lock();
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
			memManager->deallocate((char *)currNotifier, sizeof(Notifier));
			break;
		}
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
	waitLock.unlock();
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
		  printf("Removed Orphan for event %s\n",(char *) name.getAbsAddress());
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

EventAnswer *EventHandler::triggerAndCollect(char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf, EventAnswer *inAnsw, Timeout *timeout)
{
	waitLock.lock();
	//Count 
	RetEventDataDescriptor *currDataDescr = (RetEventDataDescriptor *)retDataHead.getAbsAddress();
	int numRet = 0;
	while(currDataDescr)
	{
		numRet++;
		currDataDescr = currDataDescr->getNext();
	}
	EventAnswer *ans; 
	if(inAnsw)
	{
		ans = inAnsw;
		ans->setNumMsg(numRet);
	}
	else
		ans = new EventAnswer(numRet, copyBuf);

	bool timeoutOccurred = intTriggerAndWait(buf, size, type, memManager, true, copyBuf, timeout);
	if(timeout && timeoutOccurred) 
		return ans;


	int idx = 0;
	for(currDataDescr = (RetEventDataDescriptor *)retDataHead.getAbsAddress(); currDataDescr; currDataDescr = currDataDescr->getNext())
	{
		int currSize;
		void *currMsg = currDataDescr->getData(currSize);
		ans->setMsgAt(idx++, (char *)currMsg, currSize);
	}
	
	waitLock.unlock();
	return ans;
}





bool EventHandler::triggerAndWait(char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf, Timeout *timeout)
{
	waitLock.lock();
	bool ans = intTriggerAndWait(buf, size, type, memManager, false, copyBuf, timeout);
	waitLock.unlock();
	return ans;
}

#define MAX_NOTIFIERS 100
bool EventHandler::intTriggerAndWait(char *buf, int size, int type, SharedMemManager *memManager, bool isCollect, bool copyBuf, Timeout *timeout)
{
	Notifier *autoNotifiers[MAX_NOTIFIERS];

	lock.lock();
	setData(buf, size, type, copyBuf, memManager);
	collect = isCollect;
	synch = true;
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
	bool timeoutOccurred = false;
//Count Notifiers
	int numNotifiers = 0;
	while(currNotifier)
	{
		numNotifiers++;
		currNotifier = currNotifier->getNext();
	}

	Notifier **notifiers;
	if(numNotifiers > MAX_NOTIFIERS)
		notifiers = new Notifier*[numNotifiers];
	else
		notifiers = autoNotifiers;

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
	{
		if(timeout)
			timeoutOccurred |= notifiers[i]->waitTermination(*timeout);
		else
			notifiers[i]->waitTermination();
	}
	ExitHandler::dispose();
	if(numNotifiers > MAX_NOTIFIERS)
		delete [] notifiers;

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

int EventHandler::getNumRegistered()
{
	int numNotifiers = 0;
	lock.lock();
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
//First pass: asynchronous triggers
	while(currNotifier)
	{
		numNotifiers++;
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
	return numNotifiers;
}
int EventHandler::getNumPending()
{
	int numPending = 0;
	lock.lock();
	Notifier *currNotifier = (Notifier *)notifierHead.getAbsAddress();
//First pass: asynchronous triggers
	while(currNotifier)
	{
		if(currNotifier->isPending())
			numPending++;
		currNotifier = currNotifier->getNext();
	}
	lock.unlock();
	return numPending;
}


bool EventHandler::corresponds(const char *inName)
{
	bool corr = false;
	char *sharedName = (char *)name.getAbsAddress();
	int nameLen = strlen(inName);
	if(nameLen > 0 && inName[nameLen-1] == '*')
		corr = !strncmp(sharedName, inName, nameLen - 1);
	else
		corr = !strcmp(sharedName, inName);
	return corr;
}

void EventHandler::setData(void *buf, int size, int type, bool copyBuf, SharedMemManager *memManager)
{
	this->type = type;
	if(copyBuf)
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
	else
	{
		dataBuffer = buf;
		dataSize = size;
	}
}


