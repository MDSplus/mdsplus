#ifndef SHARED_MEM_NODE_DATA_H
#define SHARED_MEM_NODE_DATA_H
#define MAX_NODES 128 //Maximum number of nodes containing a copy of this cache

#include <stdio.h>
#include "LockManager.h"
#include "CallbackManager.h"
#include "Segment.h"
//#include "DataSlot.h"
//#include "AbsoluteTime.h"

class SharedMemNodeData
{
	public: //To be defined yet
	    int nid;
		bool segmented;
		long data;
		int dataSize;
		int numSegments;
		long firstSegment;
		long lastSegment;

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
		numSegments = 0;
		segmented = false;
		firstSegment = lastSegment = -(long)this;
		data = 0;
		dataSize = 0;
		isOwner = false;
		isWarm = false;
		owner = -1;
		ownerTimestamp = 0;
		resetWarmNodes();
	}
	SharedMemNodeData(int nid)
	{
		this->nid = nid;
		numSegments = 0;
		segmented = false;
		firstSegment = lastSegment = (long)this;
		data = 0;
		dataSize = 0;
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

	void setLastSegment(Segment *lastSegment)
	{
		this->lastSegment = (long)lastSegment - (long)this;
	}
	Segment *getFirstSegment()
	{
		return (Segment *)((long)this + firstSegment);
	}

	Segment *getLastSegment()
	{
		return (Segment *)((long)this + lastSegment);
	}

	
	Segment *getSegmentAt(int idx)
	{
		if(idx >= numSegments)
			return 0;
		if(idx < 0)
			return getLastSegment();

		Segment *currSegment = getFirstSegment();
		for(int i = 0; i < idx; i++)
			currSegment = currSegment->getNext();
		return currSegment;
	}




	void setFirstSegment(Segment *firstSegment)
	{
		this->firstSegment = this->lastSegment = (long)firstSegment - (long)this;
	}

	void appendSegment(Segment *newSegment)
	{
		if(numSegments == 0)
		{
			setFirstSegment(newSegment);
			setLastSegment(newSegment);
		}
		else
		{
			Segment *lastSegment = getLastSegment();
			lastSegment->setNext(newSegment);
			setLastSegment(newSegment);
		}

		numSegments++;
	}


	


	void setNumSegments(int numSegments)
	{
		this->numSegments = numSegments;
	}



	int getNumSegments() { return numSegments;}

	
	void setData(void *data, int dataSize)
	{
		this->data = (long)data - (long)this;
		this->dataSize = dataSize;
	}

	void getData(void **data, int *dataSize)
	{
		*data = (void *)((long)this + this->data);
		*dataSize = this->dataSize;
	}
	
	
	void setSegmented(bool segmented)
	{
		this->segmented = segmented;
	}
	bool isSegmented() { return segmented;}



	void adjustOffsets(SharedMemNodeData *ref)
	{
		if(ref->isSegmented())
		{
			setFirstSegment(ref->getFirstSegment());
			setLastSegment(ref->getLastSegment());
		}
		else
		{
			void *currData;
			int currDataSize;
			ref->getData(&currData, &currDataSize);
			setData(currData, currDataSize);
		}
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
