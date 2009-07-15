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
		_int64 offset;
		_int64 dataOffset;
		int size;
		FreeStateDescr *nxt;
		FreeStateDescr(_int64 offset, _int64 dataOffset, int size)
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
	void addFreeDescr(_int64 offset, _int64 dataOffset, int size)
	{
		FreeStateDescr *newDescr = new FreeStateDescr(offset, dataOffset, size);
		newDescr->nxt = freeDescrHead;
		freeDescrHead = newDescr;
		numFreeDescr++;
	}
	void addUnusedDescr(_int64 offset)
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
		retSize = 3 * sizeof(int) + (numFreeDescr) * (2 * sizeof(_int64) + sizeof(int)) +  numUnusedDescr * sizeof(_int64);
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
			*((_int64 *)currPtr) = swap(currDescr->offset);
			currPtr += sizeof(_int64);
			*((_int64 *)currPtr) = swap(currDescr->dataOffset);
			currPtr += sizeof(_int64);
			*((int *)currPtr) = swap(currDescr->size);
			currPtr += sizeof(int);
			currDescr = currDescr->nxt;
		}
		currDescr = unusedDescrHead;
		for(i = 0; i < numUnusedDescr; i++)
		{
			*((_int64 *)currPtr) = swap(currDescr->offset);
			currPtr += sizeof(_int64);
			currDescr = currDescr->nxt;
		}
		return retBuf;
	}
};

#endif
