#ifndef FREE_SPACE_MANAGER_H
#define FREE_SPACE_MANAGER_H
#include <stdio.h>
#include <stdlib.h>
#include "Event.h"
#include "LockManager.h"
//Class FreeSpaceManager supervises memory allocation within a shared segment. 
//The first two elements of this segment will hold a pointer (_int64 offset to segment start) to the list of FreeDescriptor elements describing free space
//within the segment, and a pointer (_int64 offset to segment start) to the list of currently unused FreedDescriptor elements. FreeDescriptor elements
//are allocated from the segment as any other memory element
//The third element keeps the size of the segment
//The fourth element is the counter (int) of entities which are waiting for free space
//The last element is a Event instance, used to signal when memory is released when the above counter is non zero

#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

class FreeSpaceManager
{

	Event *fullEvent;
	int getPendingCount();
	void incrementPendingCount();
	void decrementPendingCount();

public:
	
	static char *startAddress;
	
	class FreeDescriptor {
	public: 
		_int64 startAddr;
		int size;
		_int64 next;
		//struct FreeDescriptor *nxt;

		void setStartAddr(char *startAddr)
		{
			this->startAddr = reinterpret_cast<_int64>(startAddr) - reinterpret_cast<_int64>(startAddress);
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
			    next = reinterpret_cast<_int64>(d) - reinterpret_cast<_int64>(startAddress);
		}

		FreeDescriptor *getNext()
		{
			if(next == 0) return NULL;
			return (FreeDescriptor *)(startAddress + next);
		}
	}; //End Inner class FreeDescriptor



	int getFreeSize();
	FreeDescriptor *getDescriptor();
	void releaseDescriptor(FreeDescriptor *dsc);
	void initialize(char *startAddr, int size);
	void map(char *startAddress);
	char *allocateShared(int size, LockManager *lock); 
	void freeShared(char *addr, int size, LockManager *lock);
	void print();
};
#endif
