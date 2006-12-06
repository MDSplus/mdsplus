#ifndef FREE_SPACE_MANAGER_H
#define FREE_SPACE_MANAGER_H
#include <stdio.h>
#include <stdlib.h>
//Class FreeSpaceManager supervises memory allocation within a shared segment. 
//The first two elements of this segment will hold a pointer (long offset to segment start) to the list of FreeDescriptor elements describing free space
//within the segment, and a pointer (long offset to segment start) to the list of currently unused FreedDescriptor elements. FreeDescriptor elements
//are allocated from the segment as any other memory element
//The third element keeps the size of the segment



class FreeSpaceManager
{

public:
	
	static char *startAddress;
	
	class FreeDescriptor {
	public: 
		long startAddr;
		int size;
		long next;
		//struct FreeDescriptor *nxt;

		void setStartAddr(char *startAddr)
		{
			this->startAddr = (long)startAddr - (long)startAddress;
		}
		char *getStartAddr()
		{
			return startAddress + startAddr;
		}

		void setNext(FreeDescriptor *d)
		{
			if(d == NULL)
			    next = 0;
			else
			    next = (long)d - (long)startAddress;
		}

		FreeDescriptor *getNext()
		{
			if(next == 0) return NULL;
			return (FreeDescriptor *)(startAddress + next);
		}
	}; //End Inner class FreeDescriptor




	FreeDescriptor *getDescriptor();
	void releaseDescriptor(FreeDescriptor *dsc);
	void initialize(char *startAddr, int size);
	void map(char *startAddress);
	void *allocateShared(int size); 
	void freeShared(char *addr, int size);
	void print();
};
#endif
