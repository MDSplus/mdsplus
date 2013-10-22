#ifndef SHAREDMEMSTATE_H_
#define SHAREDMEMSTATE_H_
#include "SystemSpecific.h"
#include "State.h"
#ifndef NULL
#define NULL 0
#endif
class SharedMemState: State
{
	class FreeStateDescr
	{
	public:
		int64_t offset;
		int64_t dataOffset;
		int size;
		FreeStateDescr *nxt;
		FreeStateDescr(int64_t offset, int64_t dataOffset, int size)
		{
			this->offset = offset;
			this->dataOffset = dataOffset;
			this->size = size;
			nxt = NULL;
		}
	};

public:
	int size;
	FreeStateDescr *freeDescrHead;
	FreeStateDescr *unusedDescrHead;
	int numFreeDescr;
	int numUnusedDescr;


	SharedMemState(int size)
	{
		this->size = size;
		freeDescrHead = NULL;
		unusedDescrHead = NULL;
		numFreeDescr = numUnusedDescr = 0;
	}
	~SharedMemState()
	{
		FreeStateDescr *currDescr, *prevDescr;

		currDescr = freeDescrHead;
		while(currDescr)
		{
			prevDescr = currDescr;
			currDescr = currDescr->nxt;
			delete prevDescr;
		}
		currDescr = unusedDescrHead;
		while(currDescr)
		{
			prevDescr = currDescr;
			currDescr = currDescr->nxt;
			delete prevDescr;
		}
	}
	void addFreeDescr(int64_t offset, int64_t dataOffset, int size)
	{
		FreeStateDescr *newDescr = new FreeStateDescr(offset, dataOffset, size);
		newDescr->nxt = freeDescrHead;
		freeDescrHead = newDescr;
		numFreeDescr++;
	}
	void addUnusedDescr(int64_t offset)
	{
		FreeStateDescr *newDescr = new FreeStateDescr(offset, 0, 0);
		newDescr->nxt = unusedDescrHead;
		unusedDescrHead = newDescr;
		numUnusedDescr++;
	}

	char *serialize(int &retSize)
	{
		int i;
		char *retBuf;
		retSize = 3 * sizeof(int) + (numFreeDescr) * (2 * sizeof(int64_t) + sizeof(int)) +  numUnusedDescr * sizeof(int64_t);
		retBuf = new char[retSize];
		char *currPtr = retBuf;
		//Use Big Endian
		*((int *)currPtr) = swap(this->size);
		currPtr += sizeof(int);
		*((int *)currPtr) = swap(numFreeDescr);
		currPtr += sizeof(int);
		*((int *)currPtr) = swap(numUnusedDescr);
		currPtr += sizeof(int);
		FreeStateDescr *currDescr = freeDescrHead;
		for(i = 0; i < numFreeDescr; i++)
		{
			*((int64_t *)currPtr) = swap(currDescr->offset);
			currPtr += sizeof(int64_t);
			*((int64_t *)currPtr) = swap(currDescr->dataOffset);
			currPtr += sizeof(int64_t);
			*((int *)currPtr) = swap(currDescr->size);
			currPtr += sizeof(int);
			currDescr = currDescr->nxt;
		}
		currDescr = unusedDescrHead;
		for(i = 0; i < numUnusedDescr; i++)
		{
			*((int64_t *)currPtr) = swap(currDescr->offset);
			currPtr += sizeof(int64_t);
			currDescr = currDescr->nxt;
		}
		return retBuf;
	}
};

#endif
