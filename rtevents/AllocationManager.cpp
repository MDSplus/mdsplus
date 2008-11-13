//#include "AllocationManager.h"
#include "SharedMemManager.h"

AllocationManager::FreeDescriptor *AllocationManager::getDescriptor() //Get a free descriptor from the list. if none found, signal an error
	{
		FreeDescriptor *retDsc = reinterpret_cast<FreeDescriptor *>(unusedListHead.getAbsAddress());
		if(retDsc == NULL)
		{
			printf("PANIC: no free desciptor left\n");
			exit(0);
			return 0;
		}
		unusedListHead = retDsc->getNext();
		return retDsc;
	}

void AllocationManager::releaseDescriptor(FreeDescriptor *dsc)
	{
		FreeDescriptor *unusedDscHead = reinterpret_cast<FreeDescriptor *>(unusedListHead.getAbsAddress()); 
		dsc->setNext(unusedDscHead);
		unusedListHead = dsc;
	}


void AllocationManager::initialize(int size)
	{
		
		//Initial condition: a single segment of all the available memory
		//and a single spare unused FreeDescriptor
		FreeDescriptor *freeDsc = reinterpret_cast<FreeDescriptor *>((char *)this + sizeof(*this));
		FreeDescriptor *unusedDsc = reinterpret_cast<FreeDescriptor *>((char *)this + sizeof(*this) + sizeof(FreeDescriptor));
		freeDsc->startAddr = (char *)this + sizeof(*this) + 2*sizeof(FreeDescriptor);
		freeDsc->size = size - sizeof(*this) - 2*sizeof(FreeDescriptor);
		freeDsc->setNext(NULL);
		unusedDsc->setNext(NULL);
		freeListHead = freeDsc;
		unusedListHead = unusedDsc;
		
//Initially available size
		this->freeSize = freeDsc->size;
	}
	
	int AllocationManager::getFreeSize()
	{
		return freeSize;
	}

	char *AllocationManager::allocateShared(int inSize)
	{
		int remainder = inSize % 4;
		int size = (remainder == 0)?inSize:inSize + 4 - remainder;
		char *retAddr;
		FreeDescriptor *currDsc, *prevDsc;

		FreeDescriptor *freeDscHead = reinterpret_cast<FreeDescriptor *>(freeListHead.getAbsAddress());
		FreeDescriptor *unusedDscHead = reinterpret_cast<FreeDescriptor *>(unusedListHead.getAbsAddress());
		
		while(true)
		{
			int freeUnusableSize = 0;
			//Search the first free segment with enough space
			for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->size < size; currDsc = currDsc->getNext())
			{
				freeUnusableSize += currDsc->size;
				prevDsc = currDsc;
			}

			if(!currDsc)
			{
				printf("Shared Memory Allocation Failed: Waiting for free space. FreeUnusable size = %d %d\n", 
					freeUnusableSize, getFreeSize());
				return 0;
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
			freeSize -= size;
			return retAddr;
		}
		else //The contiguous space is just that required, 
		{
			//Move current free space descriptor from freeDscHead list...
			if(currDsc == freeDscHead)
				freeListHead = currDsc->getNext();
			else
				prevDsc->setNext(currDsc->getNext());
			//...into unusedDscHead list;
			releaseDescriptor(currDsc);
		//printf("ALLOCATE DONE 2\n");
			retAddr = currDsc->getStartAddr();
			freeSize -= size;
			return retAddr;
		}
	}
	

	void AllocationManager::deallocateShared(char *addr, int inSize)
	{
		int remainder = inSize % 4;
		int size = (remainder == 0)?inSize:inSize + 4 - remainder;
		FreeDescriptor *currDsc, *prevDsc;
		FreeDescriptor *freeDscHead = reinterpret_cast<FreeDescriptor *>(freeListHead.getAbsAddress());
		char *currPtr; 

		//Find corresponding FreeDescriptor
		for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->getStartAddr() < addr; currDsc = currDsc->getNext())
			prevDsc = currDsc;


		if( prevDsc->getStartAddr() > addr) //Freed memory lower than any other free memory segment
		{
			if((addr + size) < prevDsc->getStartAddr()) //If freed space not contiguous to first free segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				newDsc->setNext(freeListHead);
				freeListHead = newDsc;
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
				prevDsc->setNext(currDsc->getNext());
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
		freeSize += size;
		//Deallocation finished. Make sure at least one unused FreeDescriptor is available
		FreeDescriptor *unusedDscHead = reinterpret_cast<FreeDescriptor *>(unusedListHead.getAbsAddress());
		if(!unusedDscHead)
		{
			FreeDescriptor *newDsc = reinterpret_cast<FreeDescriptor *>(allocateShared(sizeof(FreeDescriptor)));
			releaseDescriptor(newDsc);
		}
		//printf("FREE DONE\n");
	}

	char *AllocationManager::getFirstBuffer()
	{
		return (char *)this + sizeof(*this) + 2*sizeof(FreeDescriptor);
	}
	
	void AllocationManager::print()
	{
		FreeDescriptor *currDsc;
		FreeDescriptor *freeDscHead = reinterpret_cast<FreeDescriptor *>(freeListHead.getAbsAddress());

		currDsc = freeDscHead;
		while(currDsc)
		{
			printf("FreeDescriptor: StartAddress: %d(%x)\t size = %d\n", currDsc->getStartAddr(), currDsc->getStartAddr(), currDsc->size);
			currDsc = currDsc->getNext();
		}
		printf("\n\n");
	}
