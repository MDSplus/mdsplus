#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_
#include "SystemSpecific.h"
#include "SharedMemManager.h"
#include "Notifier.h"
#include "RelativePointer.h"
#include "Timeout.h"
#include "ExitHandler.h"

//Class EventHandler manages the occurrence of a given event. 
//Events can be associated with either an ID integer or a char string.

class RetEventDataDescriptor;
class EventAnswer;

class EventHandler
{
	RelativePointer name;
	RelativePointer notifierHead;
	Lock lock;
	Lock waitLock;
	RelativePointer nxt;
	RelativePointer dataBuffer;
	RelativePointer retDataHead;
	int dataSize;
	int type;
	bool synch;
	bool collect;
	bool intTriggerAndWait(char *buf, int size, int type, SharedMemManager *memManager, bool collect, bool copyBuf, Timeout *timeout = 0);
	
public:
	void setNext(EventHandler *next)
	{
		nxt = next;
	}
	EventHandler *getNext()
	{
		return (EventHandler *)nxt.getAbsAddress();
	}
	bool isSynch() {return synch;}
	bool isCollect() {return collect;}
	void setData(void *buf, int size, int type, bool copyBuf, SharedMemManager *memManager);
	void *getDataBuffer()
	{
		return dataBuffer.getAbsAddress();
	}
	int getDataSize()
	{
		return dataSize;
	}
	char *getName() 
	{
		return (char *)name.getAbsAddress();
	}
	void initialize(const char *name, SharedMemManager *memManager);
	void initialize();
	void *addListener(ThreadAttributes *threadAttr, Runnable *runnable, void *arg, SharedMemManager *memManager);
	//RemoveListener removes the corresponding Notifier from the notifier chain if found
	//The passed address is valid only in the same address space of the process which
	//created the Notifier instance
	void removeListener(void *notifierAddr, SharedMemManager *memManager);
	void resizeRetData(RetEventDataDescriptor *retDataDescr, int newSize, SharedMemManager *memManager);
	void trigger();
	void watchdogTrigger();
	bool triggerAndWait(char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf, Timeout *timeout = 0);
	bool corresponds(const char *name);
	void clean(SharedMemManager *memManager);
	int getRetSize();
	void *getRetBuffer();
	void *addRetBuffer(int size, SharedMemManager *memManager);
	void removeRetDataDescr(RetEventDataDescriptor *retDataDescr,  SharedMemManager *memManager);
	EventAnswer *triggerAndCollect(char *buf, int size, int type, SharedMemManager *memManager, bool copyBuf, EventAnswer *inAnsw = 0, Timeout *timeout = 0);
	int getType(){return type;}
	void lockData(){lock.lock();}
	void unlockData() {lock.unlock();}
	int getNumRegistered();
	int getNumPending();
};

class WaitLockTerminator: public Runnable
{
	Lock *lock;
public:
	WaitLockTerminator(Lock *lock)
	{
		this->lock = lock;
	}
	void run(void *arg)
	{
		printf("PARTE RUN WAIT LOCK TERMINATOR\n");
		lock->unlock();
		printf("FINISCE RUN WAIT LOCK TERMINATOR\n");
	}
};


class RetEventDataDescriptor
{
public:
	RelativePointer nxt, prv;
	RelativePointer data;
	int size;

	void initialize() {data = 0; size = 0; nxt = 0; prv = 0;}
	RetEventDataDescriptor *getNext(){return (RetEventDataDescriptor *)nxt.getAbsAddress();}
	RetEventDataDescriptor *getPrev(){return (RetEventDataDescriptor *)prv.getAbsAddress();}
	void setNext(void *nxt) {this->nxt = nxt;}
	void setPrev(void *prv) {this->prv = prv;}
	void setData(void *data, int size)
	{
		this->data = data;
		this->size = size;
	}
	char *getData()
	{
		if(size == 0)
			return 0; 
		return (char *)data.getAbsAddress();
	}
	int getSize() {return size;}
	void *getData(int &size)
	{
		size = this->size;
		if(size == 0)
			return 0; 
		return data.getAbsAddress();
	}
	void deallocateData(SharedMemManager *memManager)
	{
		if(size == 0) 
			return;
		memManager->deallocate((char *)data.getAbsAddress(), size);
	}
	void resizeData(int newSize, SharedMemManager *memManager)
	{
		if(size != newSize)
		{
			if(size != 0)
				memManager->deallocate((char *)data.getAbsAddress(), size);
			data = memManager->allocate(newSize);
			size = newSize;
		}
	}

};


//Used to pack returned data
class EXPORT EventAnswer 
{
	int numMsg;
	int *retSizes;
	char **retData;
	bool copyBuf;
public:
	EventAnswer(int numMsg, bool copyBuf)
	{
		this->numMsg = numMsg;
		this->copyBuf = copyBuf;
		retSizes = new int[numMsg];
		retData = new char *[numMsg];
	}
	~EventAnswer()
	{
		if(copyBuf)
		{
			for(int i = 0; i < numMsg; i++)
			{
				if(retSizes[i] > 0)
					delete [] retData[i];
			}
			delete[] retSizes;
			delete[] retData;
		}
	}
	void setNumMsg(int numMsg)
	{
		if(numMsg < this->numMsg)
			this->numMsg = numMsg;
	}
	int getNumMsg() {return numMsg;}
	char *getMsgAt(int idx, int &retSize)
	{
		if(idx >= numMsg)
			return 0;
		retSize = retSizes[idx];
		return retData[idx];
	}
	int getMsgSizeAt(int idx)
	{
		if(idx >= numMsg)
			return 0;
		return retSizes[idx];
	}
	void setMsgAt(int idx, char *data, int size)
	{
		if(idx >= numMsg)
			return;
		if(copyBuf)
		{
			retData[idx] = new char[size];
			memcpy(retData[idx], data, size);
		}
		else
			retData[idx] = data;
		retSizes[idx] = size;
	}
};


#endif /*EVENTHANDLER_H_*/
