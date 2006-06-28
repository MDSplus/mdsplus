#ifndef SHARED_MEM_NODE_DATA_H
#define SHARED_MEM_NODE_DATA_H
#define MAX_NODES 128 //Maximum number of nodes containing a copy of this cache

#include <stdio.h>
#include "LockManager.h"
#include "CallbackManager.h"
#include "DataSlot.h"
#include "AbsoluteTime.h"

class SharedMemNodeData
{
	public: //To be defined yet
	    int nid;
		bool timed;
		long currDim;
		int currDimSize;
		int numItems;
		long firstSlot;
		long lastSlot;
//Cache coherency parameters
		bool isOwner; //rue if the owner of the nid, i.e. the last writer
		bool isWarm;  //Warm update: force writer to update this copy of the cache
		int numWarmNodes;
		char warmNodes[MAX_NODES]; //Nodes (indexes) which require warm update when this node is the owner. -1 means no node
		char owner;    //Index of the owner node address
		int ownerTimestamp; //Ownership timestamp

	SharedMemNodeData()
	{
		nid = -1;
		numItems = 0;
		timed = false;
		currDimSize = 0;
		currDim = (long)this;
		firstSlot = lastSlot = -(long)this;
		isOwner = false;
		isWarm = false;
		owner = -1;
		ownerTimestamp = 0;
		resetWarmNodes();
	}
	SharedMemNodeData(int nid)
	{
		this->nid = nid;
		numItems = 0;
		timed = false;
		currDimSize = 0;
		currDim = (long)this;
		firstSlot = lastSlot = (long)this;
		isOwner = false;
		isWarm = false;
		ownerTimestamp = 0;
		owner = -1;
		resetWarmNodes();
	}

	int compare(SharedMemNodeData *n)
	{
		if(nid > n->nid) return 1;
		if(nid < n->nid) return -1;
		return 0;
	}

	int compare(int nid) //Returns 1 if this greater than argument, -1 if smaller, 0 if equals 
	{
		if(this->nid > nid) return 1;
		if(this->nid < nid) return -1;
		return 0;
	}

	void setNid(int nid)
	{
		this->nid = nid;
	}


	int getNid() 
	{
		return nid;
	}

	void  print()
	{
		printf("%d\n", nid);
	}

	void setLastSlot(DataSlot *lastSlot)
	{
		this->lastSlot = (long)lastSlot - (long)this;
	}
	DataSlot *getFirstSlot()
	{
		return (DataSlot *)((long)this + firstSlot);
	}

	DataSlot *getLastSlot()
	{
		return (DataSlot *)((long)this + lastSlot);
	}

	void setFirstSlot(DataSlot *firstSlot)
	{
		this->firstSlot = this->lastSlot = (long)firstSlot - (long)this;
	}

	void setCurrDim(void *dim, int dimSize)
	{
		this->currDim = (long)dim - (long)this;
		this->currDimSize = dimSize;
	}

	void getCurrDim(void **dim, int *dimSize)
	{
		*dim = (void *)((long)this + currDim);
		*dimSize = currDimSize;
	}


	void appendSlot(DataSlot *newSlot)
	{
		void *dim;
		int dimSize;
		if(currDimSize > 0)
		{
			getCurrDim(&dim, &dimSize);
			newSlot->setDim(dim, dimSize);
			currDim = (long)this;
			currDimSize = 0;
		}

		if(numItems == 0)
		{
			setFirstSlot(newSlot);
			setLastSlot(newSlot);
		}
		else
		{
			DataSlot *lastSlot = getLastSlot();
			lastSlot->setNext(newSlot);
			newSlot->setPrev(lastSlot);
			setLastSlot(newSlot);
		}

		numItems++;
	}

	int getNumItems() { return numItems;}
	void resetItems() {numItems = 0;}
	void setTimed(bool timed)
	{
		this->timed = timed;
	}
	bool isTimed() { return timed;}

	void adjustOffsets(SharedMemNodeData *ref)
	{
		setFirstSlot(ref->getFirstSlot());
		setLastSlot(ref->getLastSlot());
		void *dim;
		int dimSize;
		ref->getCurrDim(&dim, &dimSize);
		setCurrDim(dim, dimSize);
	}

	void resetWarmNodes()
	{
		numWarmNodes = 0;
	}

	void addWarm(char warmIdx)
	{
		warmNodes[numWarmNodes++] = warmIdx;
	}

};

#endif