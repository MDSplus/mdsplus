#ifndef LINKED_LIST_SLOT
#define LINKED_LIST_SLOT 
//Class linked slot is used to handle linked lists of integers. A set of integer is stored in a single slot.
#include "FreeSpaceManager.h"

#define NUM_SLOTS 10

class LinkedSlot
{
	long slots[NUM_SLOTS]; //Number of integer elements in the slot
	long next;


public:
	void initialize()
	{
		for(int i = 0; i < NUM_SLOTS; i++)
			slots[i] = -1;
		next = 0;
	}


	int getNid(FreeSpaceManager *freeSpaceManager)
	{
		return getNid(0, freeSpaceManager);
	}

	int getNid(LinkedSlot *prevSlot, FreeSpaceManager *freeSpaceManager)
	{
		for(int i = 0; i < NUM_SLOTS; i++)
		{
			if(slots[i] != -1)
			{
				int retNid = slots[i];
				slots[i] = -1;
				//Free this slot if empty
				if(i == NUM_SLOTS - 1 && prevSlot)
				{
					if(next == 0)
						prevSlot->next = 0;
					else
						prevSlot->next = (char *)this - (char *)prevSlot + next;
					freeSpaceManager->freeShared((char *)this, sizeof(LinkedSlot));
				}
				return retNid;
			}
		}
		if(next == 0) //Last slot
			return -1;

		LinkedSlot *nextSlot = (LinkedSlot *)((char *)this + next);
		return nextSlot->getNid(this, freeSpaceManager);
	}	
	
	void putNid(int nid, FreeSpaceManager *freeSpaceManager)
	{
		for(int i = 0; i < NUM_SLOTS; i++)
		{
			if(slots[i] == -1)
			{
				slots[i] = nid;
				return;
			}
		}
		if(next == 0) //Last slot, need to allocate a new one
		{
			LinkedSlot *newSlot = (LinkedSlot *)freeSpaceManager->allocateShared(sizeof(LinkedSlot));
			newSlot->initialize();
			next = (char *)newSlot - (char *)this;
			newSlot->slots[0] = nid;
			return;
		}
		LinkedSlot *nextSlot = (LinkedSlot *)((char *)this + next);
		nextSlot->putNid(nid, freeSpaceManager);
	}

//Methods used by MdsTreeNode
//Add a child offset: same semanthics than putNid
	void addChild(long child, FreeSpaceManager *freeSpaceManager)
	{
		putNid(child, freeSpaceManager);
	}

	int getChildAt(int idx)
	{
		LinkedSlot *currSlot = this;
		for(int i = 0; i < idx / NUM_SLOTS; i++)
		{
			currSlot = (LinkedSlot *)((char *)currSlot + currSlot->next);
		}
		return currSlot->slots[idx % NUM_SLOTS];
	}

};

#endif