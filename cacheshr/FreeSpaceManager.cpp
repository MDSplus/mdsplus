#include "FreeSpaceManager.h"


char *FreeSpaceManager::startAddress;
FreeSpaceManager::FreeDescriptor *FreeSpaceManager::getDescriptor() //Get a free descriptor from the list. if none fount, signal an error
	{

		//FreeDescriptor *unusedDscHead = *(FreeDescriptor **)(startAddress + sizeof(FreeDescriptor *));
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(long *)(startAddress + sizeof(long)));

		FreeDescriptor *retDsc;
		if(unusedDscHead == NULL)
		{
			printf("FATAL ERROR: no free desciptor left\n");
			exit(0);
			return retDsc;
		}
		retDsc = unusedDscHead;
		//unusedDscHead = unusedDscHead->nxt;
		unusedDscHead = unusedDscHead->getNext();
		//*(FreeDescriptor **)(startAddress + sizeof(FreeDescriptor *)) = unusedDscHead;
		*(long *)(startAddress + sizeof(long)) = (char *)unusedDscHead - (char *)startAddress;
		return retDsc;
	}

	void FreeSpaceManager::releaseDescriptor(FreeDescriptor *dsc)
	{
		//FreeDescriptor *unusedDscHead = *(FreeDescriptor **)(startAddress + sizeof(FreeDescriptor *));
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(long *)(startAddress + sizeof(long)));
		//dsc->nxt = unusedDscHead;
		dsc->setNext(unusedDscHead);
		unusedDscHead = dsc;
	//	*(FreeDescriptor **)(startAddress + sizeof(FreeDescriptor *)) = unusedDscHead;
		*(long *)(startAddress + sizeof(long)) = (char *)unusedDscHead - (char *)startAddress;
	}


	void FreeSpaceManager::initialize(char *startAddr, int size)
	{

		this->startAddress = startAddr;
		//Creates a single free memory descriptor
		//FreeDescriptor *freeDsc = (FreeDescriptor *)(startAddr + 2 * sizeof(FreeDescriptor *) + sizeof(int));
		FreeDescriptor *freeDsc = (FreeDescriptor *)(startAddress + 2 * sizeof(long) + sizeof(int));

		//freeDsc->startAddr = (char *)(startAddr + 2 * sizeof(FreeDescriptor *) + sizeof(int) + 2 * sizeof(FreeDescriptor));
		freeDsc->setStartAddr((char *)(startAddress + 2 * sizeof(long) + sizeof(int) + 2 * sizeof(FreeDescriptor)));

		//freeDsc->size = size - 2 * sizeof(FreeDescriptor) - 2 * sizeof(FreeDescriptor *) - sizeof(int);
		freeDsc->size = size - 2 * sizeof(FreeDescriptor) - 2 * sizeof(long) - sizeof(int);

		//freeDsc->nxt = NULL;
		freeDsc->setNext(NULL);

		//*((FreeDescriptor **)(startAddr)) = freeDsc;
		*((long *)(startAddr)) = (char *)freeDsc - (char *)startAddr;

		//FreeDescriptor *unusedDsc = (FreeDescriptor *)(startAddr + 2 * sizeof(FreeDescriptor *) + sizeof(int) + sizeof(FreeDescriptor)); //1 Free desciptor initially
		FreeDescriptor *unusedDsc = (FreeDescriptor *)(startAddress + 2 * sizeof(long) + sizeof(int) + sizeof(FreeDescriptor)); //1 Free desciptor initially

		
		//unusedDsc->nxt = NULL;
		unusedDsc->setNext(NULL);


		//*(FreeDescriptor **)(startAddr + sizeof(FreeDescriptor *)) = unusedDsc;
		*(long *)(startAddr + sizeof(long)) = (char *)unusedDsc - (char *)startAddr;


		//*(int *)(startAddr + 2 * sizeof(FreeDescriptor *)) = size;
		*(int *)(startAddr + 2 * sizeof(long)) = size;
	}

	void FreeSpaceManager::map(char *startAddress)
	{
		this->startAddress = startAddress;
	}


	void *FreeSpaceManager::allocateShared(int size) //Find first suitable free space. Return NULL if not found
	{
		char *retAddr;
		FreeDescriptor *currDsc, *prevDsc;
	//	FreeDescriptor *freeDscHead = *(FreeDescriptor **)(startAddress);
		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(long *)startAddress);
	//	FreeDescriptor *unusedDscHead = *(FreeDescriptor **)(startAddress + sizeof(FreeDescriptor *));
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(long *)(startAddress + sizeof(long)));
		
		//for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->size < size; currDsc = currDsc->nxt)
		for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->size < size; currDsc = currDsc->getNext())
			prevDsc = currDsc;

		if(!currDsc)
		{
			printf("SHARED MEMORY ALLOCATION FAILED\n");
			print();
			exit(0);
			
			
			return NULL; //Not enough space available
		}
		if(currDsc->size > size)	//If more free contiguous space is found
		{							//Adjust current free space descriptor
		//	retAddr = currDsc->startAddr;
			retAddr = currDsc->getStartAddr();
			//currDsc->startAddr += size;
			currDsc->setStartAddr(retAddr + size);
			currDsc->size -= size;

			return retAddr;
		}
		else //The contiguous space is just that required, 
		{
			//Move current free space descriptor from freeDscHead list...
			if(currDsc == freeDscHead)
			{
 				//freeDscHead = currDsc->nxt;
				freeDscHead = currDsc->getNext();
				//*((FreeDescriptor **)(startAddress)) = freeDscHead;
				*((long*)(startAddress)) = (char *)freeDscHead - (char *)startAddress;
			}

			else
				prevDsc->next = currDsc->next;
			//...into unusedDscHead list;
			releaseDescriptor(currDsc);
			//return currDsc->startAddr;

			return currDsc->getStartAddr();
		}
	}



	void FreeSpaceManager::freeShared(char *addr, int size)
	{
		FreeDescriptor *currDsc, *prevDsc;
		//FreeDescriptor *freeDscHead = *(FreeDescriptor **)(startAddress);
		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(long *)(startAddress));

		//for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->startAddr < addr; currDsc = currDsc->nxt)
		for(currDsc = prevDsc = freeDscHead; currDsc && currDsc->getStartAddr() < addr; currDsc = currDsc->getNext())
			prevDsc = currDsc;

		//Freed memory lies between zones described by prevDsc and currDsc
		//if(prevDsc->startAddr > addr) //Freed memory lower than any other free memory segment
		if(prevDsc->getStartAddr() > addr) //Freed memory lower than any other free memory segment
		{
			//if((addr + size) < prevDsc->startAddr) //If freed space not contiguous to first free segment
			if((addr + size) < prevDsc->getStartAddr()) //If freed space not contiguous to first free segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				//newDsc->startAddr = addr;
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				//newDsc->nxt = freeDscHead;
				newDsc->setNext(freeDscHead);
				freeDscHead = newDsc;
				//*(FreeDescriptor **)(startAddress) = freeDscHead;
				*(long *)(startAddress) = (char *)freeDscHead - (char *)startAddress;
			}
			//else if((addr + size) > prevDsc->startAddr) //Check for overlapped memory segments 
			else if((addr + size) > prevDsc->getStartAddr()) //Check for overlapped memory segments 
			{
				printf("FATAL ERROR: overlapped memory segments in freeShared\n");
				exit(0);
			}
			else //Freed space is contiguous to first free segment
			{
				//freeDscHead->startAddr = addr;
				freeDscHead->setStartAddr(addr);
				freeDscHead->size += size;
			}
		}
		else if(!currDsc) //Freed memory higher than any other free memory segment
		{
			//if((prevDsc->startAddr + prevDsc->size) < addr) //If freed space not contiguous to last free segment
			if((prevDsc->getStartAddr() + prevDsc->size) < addr) //If freed space not contiguous to last free segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				//newDsc->startAddr = addr;
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				//newDsc->nxt = NULL;
				newDsc->setNext(NULL);
				//prevDsc->nxt = newDsc;
				prevDsc->setNext(newDsc);
			}
			//else if((prevDsc->startAddr + prevDsc->size) > addr)  //Check for overlapped memory segments 
			else if((prevDsc->getStartAddr() + prevDsc->size) > addr)  //Check for overlapped memory segments 
			{
				printf("FATAL ERROR: overlapped memory segments in freeShared\n");
				exit(0);
			}
			else //Freed space is contiguous to first free segment
			{
				prevDsc->size += size;
			}
		}
		else //Freed memory between two memory segments
		{
			//if(((prevDsc->startAddr + prevDsc->size) > addr) || ((addr + size) > currDsc->startAddr))  //Check for overlapped memory segments 
			if(((prevDsc->getStartAddr() + prevDsc->size) > addr) || ((addr + size) > currDsc->getStartAddr()))  //Check for overlapped memory segments 
			{
				printf("FATAL ERROR: overlapped memory segments in freeShared\n");
				exit(0);
			}
			//else if(((prevDsc->startAddr + prevDsc->size) == addr) && ((addr + size) == currDsc->startAddr))
			else if(((prevDsc->getStartAddr() + prevDsc->size) == addr) && ((addr + size) == currDsc->getStartAddr()))
				//If freed segment contiguous to both adjacent segments
			{
				prevDsc->size += (size + currDsc->size);
				prevDsc->next = currDsc->next;
				releaseDescriptor(currDsc);
			}
			//else if ((prevDsc->startAddr + prevDsc->size) == addr) //If freed segment contiguous to previous segments
			else if ((prevDsc->getStartAddr() + prevDsc->size) == addr) //If freed segment contiguous to previous segments
			{
				prevDsc->size += size;
			}
			//else if ((addr + size) == currDsc->startAddr) //If freed segment contiguous to next segments
			else if ((addr + size) == currDsc->getStartAddr()) //If freed segment contiguous to next segments
			{
				currDsc->size += size;
				//currDsc->startAddr = addr;
				currDsc->setStartAddr(addr);
			}
			else //If freed segment not contiguous to any adjacent segment
			{
				FreeDescriptor *newDsc = getDescriptor();
				//newDsc->startAddr = addr;
				newDsc->setStartAddr(addr);
				newDsc->size = size;
				//newDsc->nxt = currDsc;
				newDsc->setNext(currDsc);
				//prevDsc->nxt = newDsc;
				prevDsc->setNext(newDsc);
			}
		}
		//Deallocation finished. Make sure at least one unused FreeDescriptor is available
		//FreeDescriptor *unusedDscHead = *(FreeDescriptor **)(startAddress + sizeof(FreeDescriptor *));
		FreeDescriptor *unusedDscHead = (FreeDescriptor *)(startAddress + *(long *)(startAddress + sizeof(long)));
		if(!unusedDscHead)
		{
			FreeDescriptor *newDsc = (FreeDescriptor *)allocateShared(sizeof(FreeDescriptor));
			releaseDescriptor(newDsc);
		}

	}

	void FreeSpaceManager::print()
	{
		FreeDescriptor *currDsc;
	//	FreeDescriptor *freeDscHead = *(FreeDescriptor **)(startAddress);
		FreeDescriptor *freeDscHead = (FreeDescriptor *)(startAddress + *(long *)(startAddress));

		currDsc = freeDscHead;
		while(currDsc)
		{
			//printf("FreeDescriptor: StartAddress: %d(%x)\t size = %d\n", currDsc->startAddr, currDsc->startAddr, currDsc->size);
			printf("FreeDescriptor: StartAddress: %d(%x)\t size = %d\n", currDsc->getStartAddr(), currDsc->getStartAddr(), currDsc->size);
			//currDsc = currDsc->nxt;
			currDsc = currDsc->getNext();
		}
		printf("\n\n");
	}
