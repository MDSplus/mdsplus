#include "FreeSpaceManager.h"
#define FREE_THRESHOLD 50000
//Class FreeSpaceManager supervises memory allocation within a shared segment. 
//The first two elements of this segment will hold a pointer (_int64 offset to segment start) to the list of FreeDescriptor elements describing free space
//within the segment, and a pointer (_int64 offset to segment start) to the list of currently unused FreedDescriptor elements. FreeDescriptor elements
//are allocated from the segment as any other memory element
//The third element keeps the size of the segment
//The fourth element is the counter (int) of entities which are waiting for free space
//The last element is a Event instance, used to signal when memory is released when the above counter is non zero

char *FreeSpaceManager::startAddress;
FreeSpaceManager::FreeDescriptor *FreeSpaceManager::getDescriptor() //Get a free descriptor from the list. if none found, signal an error
	{
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress + sizeof(_int64)));

		FreeDescriptor *retDsc;
		if(unusedDscHead == NULL)
		{
			printf("PANIC: no free desciptor left\n");
			exit(0);
			return retDsc;
		}
		retDsc = unusedDscHead;
		unusedDscHead = unusedDscHead->getNext();
		*(_int64 *)(startAddress + sizeof(_int64)) = (char *)unusedDscHead - (char *)startAddress;
		return retDsc;
	}

	void FreeSpaceManager::releaseDescriptor(FreeDescriptor *dsc)
	{
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress + sizeof(_int64)));
		dsc->setNext(unusedDscHead);
		unusedDscHead = dsc;
		*(_int64 *)(startAddress + sizeof(_int64)) = (char *)unusedDscHead - (char *)startAddress;
	}


	void FreeSpaceManager::initialize(char *startAddr, int size)
	{
		//fullEvent.initialize();
printf("FreeSpaceManager::initialize %d\n", size);
		this->startAddress = startAddr;
		//Creates a single free memory descriptor describing the whole segment and a single unused free descriptor 

		//Initialize unique free descriptor do descrribe the whole segment (except header)
		FreeDescriptor *freeDsc = (FreeDescriptor *)(startAddress + 2 * sizeof(_int64) + sizeof(int) + sizeof(int) + sizeof(Event));

		freeDsc->setStartAddr((char *)(startAddress + 2 * sizeof(_int64) + sizeof(int) + sizeof(int) + sizeof(Event) + 2 * sizeof(FreeDescriptor)));
		freeDsc->size = size - 2 * sizeof(FreeDescriptor) - 2 * sizeof(_int64) - sizeof(int) - sizeof(int) - sizeof(Event);
		freeDsc->setNext(NULL);

		//The first _int64 of the shared segment is the address (offset) of the first used free descriptor
		*((_int64 *)(startAddr)) = (char *)freeDsc - (char *)startAddr;

		//Initialize a single spare FreeDescriptor
		FreeDescriptor *unusedDsc = (FreeDescriptor *)(startAddress + 2 * sizeof(_int64) + sizeof(int) + 
			+ sizeof(int) + sizeof(Event) + sizeof(FreeDescriptor)); //1 Free desciptor initially

		unusedDsc->setNext(NULL);
		*(_int64 *)(startAddr + sizeof(_int64)) = (char *)unusedDsc - (char *)startAddr;

		//Initialize usable size
		*(int *)(startAddr + 2 * sizeof(_int64)) = size - 2* sizeof(_int64) - sizeof(int) - sizeof(int) - sizeof(Event);

		//Initialize pending counter to 0
		*(int *)(startAddr + 2 * sizeof(_int64)+ sizeof(int)) = 0;

		//Initialize full event
		fullEvent = (Event *)(startAddr + 2 * sizeof(_int64)+ sizeof(int) + sizeof(int));
		fullEvent->initialize();
	}

	void FreeSpaceManager::map(char *startAddress)
	{
		this->startAddress = startAddress;
	}

	int FreeSpaceManager::getPendingCount()
	{
		return *(int *)(startAddress + 2 * sizeof(_int64)+ sizeof(int));
	}
	
	void FreeSpaceManager::incrementPendingCount()
	{
		int *pendingCount = (int *)(startAddress + 2 * sizeof(_int64)+ sizeof(int));
		(*pendingCount)++;
	}

	void FreeSpaceManager::decrementPendingCount()
	{
		int *pendingCount = (int *)(startAddress + 2 * sizeof(_int64)+ sizeof(int));
		if(*pendingCount <= 0)
		{
			printf("PANIC: FreeSpaceManager Pending count decremented below zero\n");
			exit(0);
		}
		(*pendingCount)--;
	}


	int FreeSpaceManager::getFreeSize()
	{
		FreeDescriptor *currDsc;
		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)startAddress);
		
		int freeSize = 0;
		for(currDsc = freeDscHead; currDsc; currDsc = currDsc->getNext())
		{
			freeSize += currDsc->size;
		}
		return freeSize;
	}


	char *FreeSpaceManager::allocateShared(int size, LockManager *lock) //Find first suitable free space. Return NULL if not found
	{

		//printf("ALLOCATE SHARED %d\n", size);
		
		char *retAddr;
		FreeDescriptor *currDsc, *prevDsc;

		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)startAddress);
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress + sizeof(_int64)));
		
		while(true)
		{
			int freeUnusableSize = 0;
			for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->size < size; currDsc = currDsc->getNext())
			{
				freeUnusableSize += currDsc->size;
				prevDsc = currDsc;
			}

			if(!currDsc)
			{
				printf("Shared Memory Allocation Failed: Waiting for free space. FreeUnusable size = %d %d\n", 
					freeUnusableSize, getFreeSize());
				//Not enough space available: wait for more space
				incrementPendingCount();
				lock->unlock();
				fullEvent->wait();
				printf("Memory freed, retry allocation\n");
				lock->lock();
				freeDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)startAddress);
				unusedDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress + sizeof(_int64)));
				decrementPendingCount();
			}
			else
				break;
		}
		if(currDsc->size > size)	//If more free contiguous space is found
		{							//Adjust current free space descriptor
			retAddr = currDsc->getStartAddr();
			currDsc->setStartAddr(retAddr + size);
			currDsc->size -= size;
		//printf("ALLOCATE DONE 1\n");
			return retAddr;
		}
		else //The contiguous space is just that required, 
		{
			//Move current free space descriptor from freeDscHead list...
			if(currDsc == freeDscHead)
			{
				freeDscHead = currDsc->getNext();
				*((_int64*)(startAddress)) = (char *)freeDscHead - (char *)startAddress;
			}

			else
				prevDsc->next = currDsc->next;

			//...into unusedDscHead list;
			releaseDescriptor(currDsc);
		//printf("ALLOCATE DONE 2\n");
			return currDsc->getStartAddr();
		}
	}



	void FreeSpaceManager::freeShared(char *addr, int size, LockManager *lock)
	{

		//printf("FREE SHARED %d\n", size);
		
		
		FreeDescriptor *currDsc, *prevDsc;
		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress));

		//Find corresponding FereDescriptor
		for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->getStartAddr() < addr; currDsc = currDsc->getNext())
			prevDsc = currDsc;

		//Freed memory lies between zones described by prevDsc and currDsc

		if(prevDsc->getStartAddr() > addr) //Freed memory lower than any other free memory segment
		{
			if((addr + size) < prevDsc->getStartAddr()) //If freed space not contiguous to first free segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				newDsc->setNext(freeDscHead);
				freeDscHead = newDsc;
				*(_int64 *)(startAddress) = (char *)freeDscHead - (char *)startAddress;
			}
			else if((addr + size) > prevDsc->getStartAddr()) //Check for overlapped memory segments 
			{
				printf("PANIC: overlapped memory segments in freeShared\n");
				exit(0);
			}
			else //Freed space is contiguous to first free segment
			{
				freeDscHead->setStartAddr(addr);
				freeDscHead->size += size;
			}
		}
		else if(!currDsc) //Freed memory higher than any other free memory segment
		{
			if((prevDsc->getStartAddr() + prevDsc->size) < addr) //If freed space not contiguous to last free segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				newDsc->setNext(NULL);
				prevDsc->setNext(newDsc);
			}
			else if((prevDsc->getStartAddr() + prevDsc->size) > addr)  //Check for overlapped memory segments 
			{
				printf("PANIC: overlapped memory segments in freeShared\n");
				exit(0);
			}
			else //Freed space is contiguous to last free segment
			{
				prevDsc->size += size;
			}
		}
		else //Freed memory between two memory segments
		{
			//Check for overlapped memory segments 
			if(((prevDsc->getStartAddr() + prevDsc->size) > addr) || ((addr + size) > currDsc->getStartAddr()))  
			{
				printf("PANIC: overlapped memory segments in freeShared\n");
				exit(0);
			}
			//If freed segment contiguous to both adjacent segments
			else if(((prevDsc->getStartAddr() + prevDsc->size) == addr) && ((addr + size) == currDsc->getStartAddr()))
			{
				prevDsc->size += (size + currDsc->size);
				prevDsc->next = currDsc->next;
				releaseDescriptor(currDsc);
			}
			//If freed segment contiguous to previous segments
			else if ((prevDsc->getStartAddr() + prevDsc->size) == addr) 
			{
				prevDsc->size += size;
			}
			//If freed segment contiguous to next segments
			else if ((addr + size) == currDsc->getStartAddr()) 
			{
				currDsc->size += size;
				currDsc->setStartAddr(addr);
			}
			else //If freed segment not contiguous to any adjacent segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				newDsc->setNext(currDsc);
				prevDsc->setNext(newDsc);
			}
		}
		//Deallocation finished. Make sure at least one unused FreeDescriptor is available
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress + sizeof(_int64)));
		if(!unusedDscHead)
		{
			FreeDescriptor *newDsc = (FreeDescriptor *)allocateShared(sizeof(FreeDescriptor), lock);
			releaseDescriptor(newDsc);
		}
		int pend;
		if((pend = getPendingCount()) > 0)
		{
		    
		    int freeSize = getFreeSize();
		    if(freeSize > FREE_THRESHOLD)
		    {
			printf("%d pending threads detected in free memory\n", pend);
			fullEvent->signal();
		    }
		}

		//printf("FREE DONE\n");
	}

	void FreeSpaceManager::print()
	{
		FreeDescriptor *currDsc;
		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(_int64 *)(startAddress));

		currDsc = freeDscHead;
		while(currDsc)
		{
			printf("FreeDescriptor: StartAddress: %d(%x)\t size = %d\n", currDsc->getStartAddr(), currDsc->getStartAddr(), currDsc->size);
			currDsc = currDsc->getNext();
		}
		printf("\n\n");
	}
