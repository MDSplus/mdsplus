#ifndef ALLOCATIONMANAGER_H_
#define ALLOCATIONMANAGER_H_
#include <stdlib.h>
#include <stdio.h>
#include "SystemSpecific.h"
#include "RelativePointer.h"
#include "SharedMemState.h"
//Class AllocationManager supervises memory allocation within a shared segment. 
//The first two elements of this segment will hold a pointer (int64_t offset to segment start) to the list of FreeDescriptor elements describing free space
//within the segment, and a pointer (int64_t offset to segment start) to the list of currently unused FreedDescriptor elements. FreeDescriptor elements
//are allocated from the segment as any other memory element
//The third element keeps the size of the segment
//The fourth element is the counter (int) of entities which are waiting for free space
//The last element is a Event instance, used to signal when memory is released when the above counter is non zero

class EXPORT AllocationManager
{

	class FreeDescriptor {
	public:	
		RelativePointer startAddr;
		int size;
		RelativePointer next;
		int64_t getStartOffset() {return startAddr.getOffset();}
		int64_t getNextOffset() {return next.getOffset();}
		int getSize() {return size;}
		char *getStartAddr()
		{
			return reinterpret_cast<char *>(startAddr.getAbsAddress());
		}
		void setStartAddr(char *ptr)
		{
			startAddr = ptr;
		}
		FreeDescriptor *getNext()
		{
			return reinterpret_cast<FreeDescriptor *>(next.getAbsAddress());
		}
		void setNext(FreeDescriptor *dsc)
		{
			next = dsc;
		}
		void setNext(RelativePointer &rp)
		{
			next = rp;
		}
	};
	//Pointer to the list of FreeDescriptor describing free space
	RelativePointer freeListHead;
	
	//Pointer to the list of unuised FreeDescriptors
	RelativePointer unusedListHead;
	
	//Currently available free size
	int freeSize;
	
	//Allocate a new FreeDescriptor in the shared memory
	FreeDescriptor *getDescriptor();
	
	//Release an unused FreeDescriptor, i.e. append it to the list of unused FreeDescriptors
	void releaseDescriptor(FreeDescriptor *);
	
public:
		//Initialize structures. To be called only once by the first process creating the shared memory segment
	void initialize(int size);

	//Get Free size 
	int getFreeSize();

	//Allocate a new buffer in shared memory
	char *allocateShared(int size);
	//Free buffer
	void deallocateShared(char *addr, int size);
	
	//Return the address of the first allocated buffer. This is useful when the shared memory
	//segment will hold data structures shared by several processes, starting from a common
	//header
	char *getFirstBuffer();

	//Dump the content of the managed memory
	void print();
	SharedMemState *getState(int size);
};

#endif /*ALLOCATIONMANAGER_H_*/
