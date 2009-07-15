#ifndef SHAREDMEMMANAGER_H_
#define SHAREDMEMMANAGER_H_
#include "SystemSpecific.h"
#include "Lock.h"
#include "SharedMemory.h"
#include "AllocationManager.h"
#include "GlobalLock.h"

//Global Lock id used in SharedMemManager Initialization
#define MEM_MANAGER_LOCK 1

class EXPORT SharedMemManager
{
	//globalLock is used to guarantree mutual exclusion in memory initialization
	//or mapping.
	
	GlobalLock globalLock;
	//Mutual exclusion in buffer allocation-deallocation is instead ensured by 
	//a shared instance of Lock, stored in the initial section of the shared memory.
	Lock *memLock;
	SharedMemory sharedMemory;
	char * startAddr;
	AllocationManager *allocationManager;
	bool isLocked;
	
	
	int memId;
	int size;
public:
	bool initialize(int memId, int size)
	{
		bool isFirst;
		isLocked = false;
		this->memId = memId;
		this->size = size;
		globalLock.initialize(MEM_MANAGER_LOCK);
		globalLock.lock();
		try {
			isFirst = sharedMemory.initialize(memId, size);
			startAddr = sharedMemory.getStartAddr();
			if(isFirst)
			{
				memLock = (Lock *)startAddr;
				memLock->initialize();
				//Mutual exclusion allocation-deallocation lock is stored at the beginning 
				//of the shared memory segment;
				allocationManager = (AllocationManager *)(startAddr + sizeof(Lock));
				allocationManager->initialize(size - sizeof(Lock));
			}
			else //Data structures already created, no need to access them
			{
				memLock = (Lock *)startAddr;
				allocationManager = (AllocationManager *)(startAddr + sizeof(Lock));
			}
		}catch(SystemException *exc)
		{
			globalLock.unlock();
			throw exc;
		}
		globalLock.unlock();
		return isFirst;
	}
	
	void reset()
	{
		memLock = (Lock *)startAddr;
		memLock->initialize();
		//Mutual exclusion allocation-deallocation lock is stored at the beginning 
		//of the shared memory segment;
		allocationManager = (AllocationManager *)(startAddr + sizeof(Lock));
		allocationManager->initialize(size - sizeof(Lock));
	}
	
	
	char *allocate(int size)
	{
	//	if(!isLocked)
			memLock->lock();
		char *retBuf = allocationManager->allocateShared(size);
	//	if(!isLocked)
			memLock->unlock();


//printf("ALLOCATED %d bytes %x\n", size, retBuf);

		return retBuf;
	}
	
	void deallocate(char *buf, int size)
	{
//		if(!isLocked)
			memLock->lock();
		allocationManager->deallocateShared(buf, size);
//		if(!isLocked)
			memLock->unlock();

//printf("DEALLOCATED %d bytes at %x\n", size, buf);
	}
	
	//Global lock: to be used when building linked structures in shared memory 
	void lock()
	{
		memLock->lock();
		isLocked = true;
	}
	
	void unlock()
	{
		isLocked = false;
		memLock->unlock();
	}
	
	char *getFirstBuffer()
	{
		return allocationManager->getFirstBuffer();
	}

	SharedMemState *getState()
	{
		memLock->lock();
		SharedMemState *retState = allocationManager->getState(size);
		memLock->unlock();
		return retState;
	}
};

#endif /*SHAREDMEMMANAGER_H_*/
