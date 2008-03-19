#ifndef SHARED_MEM_NODE_DATA_H
#define SHARED_MEM_NODE_DATA_H
#define MAX_NODES 128 //Maximum number of nodes containing a copy of this cache

#include <stdio.h>
#include "LockManager.h"
#include "CallbackManager.h"
#include "Segment.h"
#include "Event.h"

class SharedMemNodeData
{
	public: //To be defined yet
	    int nid;
		int treeId;
		bool segmented;
		char dataType;
		int numSamples;
		_int64 data;
		int dataSize;
		int numSegments;
		_int64 firstSegment;
		_int64 lastSegment;

		//Callback list
		_int64 callbackManager;

		//Cache coherency parameters
		bool owner; //true if the owner of the nid, i.e. the last writer
		bool warm;  //Warm update: force writer to update this copy of the cache
		bool dirty; //Indicates whether data needs to be read from outside
		int numWarmNodes;
		char warmNodes[MAX_NODES]; //Nodes (indexes) which require warm update when this node is the owner. -1 means no node
		int numReaderNodes;
		char readerNodes[MAX_NODES]; //Nodes (indexes) which require warm update when this node is the owner. -1 means no node
		char ownerIdx;    //Index of the owner node address
		int ownerTimestamp; //Ownership timestamp
		bool eventActive;
		Event dataEvent;

	SharedMemNodeData()
	{
		nid = -1;
		treeId = -1;
		dataType = 0;
		numSamples = 0;
		numSegments = 0;
		segmented = false;
		firstSegment = lastSegment = - reinterpret_cast<_int64>(this);
		data = 0;
		dataSize = 0;
		owner = false;
		warm = false;
		dirty = true;
		ownerIdx = (char)-1;
		ownerTimestamp = 0;
		numWarmNodes = 0;
		numReaderNodes = 0;
		eventActive = false;
		callbackManager = 0;
	}
	SharedMemNodeData(int nid)
	{
		this->nid = nid;
		treeId = -1;
		dataType = 0;
		numSamples = 0;
		numSegments = 0;
		segmented = false;
		firstSegment = lastSegment = reinterpret_cast<_int64>(this);
		data = 0;
		dataSize = 0;
		owner = false;
		warm = false;
		dirty = true;
		ownerIdx = (char)-1;
		ownerTimestamp = 0;
		numWarmNodes = 0;
		numReaderNodes = 0;
		eventActive = false;
		callbackManager = 0;
	}
	SharedMemNodeData(int nid, int treeId)
	{
		this->nid = nid;
		this->treeId = treeId;
		dataType = 0;
		numSamples = 0;
		numSegments = 0;
		segmented = false;
		firstSegment = lastSegment = reinterpret_cast<_int64>(this);
		data = 0;
		dataSize = 0;
		owner = false;
		warm = false;
		dirty = true;
		ownerIdx = (char)-1;
		ownerTimestamp = 0;
		numWarmNodes = 0;
		numReaderNodes = 0;
		eventActive = false;
		callbackManager = 0;
	}

	int compare(SharedMemNodeData *n)
	{
		if(nid > n->nid) return 1;
		if(nid < n->nid) return -1;
		if(treeId > n->treeId) return 1;
		if(treeId < n->treeId) return -1;
		return 0;
	}

	int compare(int treeId, int nid) //Returns 1 if this greater than argument, -1 if smaller, 0 if equals 
	{
		if(this->nid > nid) return 1;
		if(this->nid < nid) return -1;
		if(this->treeId > treeId) return 1;
		if(this->treeId < treeId) return -1;
		return 0;
	}

	void setNid(int treeId, int nid)
	{
		this->nid = nid;
		this->treeId = treeId;
	}


	int getNid() 
	{
		return nid;
	}

	void setTreeId(int treeId)
	{
		this->treeId = treeId;
	}


	int getTreeId() 
	{
		return treeId;
	}

	void  print()
	{
		printf("%d %d\n", treeId, nid);
	}

	void setLastSegment(Segment *lastSegment)
	{
		this->lastSegment = reinterpret_cast<_int64>(lastSegment) - reinterpret_cast<_int64>(this);
	}
	Segment *getFirstSegment()
	{
		return reinterpret_cast<Segment *>(reinterpret_cast<_int64>(this) + firstSegment);
	}

	Segment *getLastSegment()
	{
		return reinterpret_cast<Segment *>(reinterpret_cast<_int64>(this) + lastSegment);
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

	void discardOldSegments(_int64 timestamp, FreeSpaceManager *fsm, LockManager *lock)
	{
		if(numSegments == 0) 
			return;
		Segment *currSegment = getFirstSegment();
		if(!currSegment->isTimestamped()) 
			return;

		while(numSegments > 0)
		{
			_int64 *endTimePtr;
			int endSize;
			currSegment->getEndTimestamp((char **)&endTimePtr, &endSize);

			if(*endTimePtr < timestamp) 
			{
				setFirstSegment(currSegment->getNext());
				currSegment->free(fsm, lock);
				fsm->freeShared((char *)currSegment, sizeof(Segment), lock);
				currSegment = getFirstSegment();
				numSegments--;
			}
			else
				break; //No need to proccede
		}
		if(numSegments == 0)
			firstSegment = lastSegment = -reinterpret_cast<_int64>(this);
	}

	void discardFirstSegment(FreeSpaceManager *fsm, LockManager *lock)
	{
		if(numSegments == 0) 
			return;
		Segment *currSegment = getFirstSegment();

		char *currData;
		int dataLen;
		currSegment->getData(&currData, &dataLen);
		printf("DISCARD FIRST SEGMENT: %f\n", *(float *)currData);

		setFirstSegment(currSegment->getNext());
		getFirstSegment()->getData(&currData, &dataLen);
		printf("DISCARD FIRST SEGMENT 1: %f\n", *(float *)currData);

		numSegments--;
		currSegment->free(fsm, lock);
		fsm->freeShared((char *)currSegment, sizeof(Segment), lock);
		getFirstSegment()->getData(&currData, &dataLen);
		printf("DISCARD FIRST SEGMENT: %f\n", *(float *)currData);

	}



	void setFirstSegment(Segment *firstSegment)
	{
		this->firstSegment = reinterpret_cast<_int64>(firstSegment) - reinterpret_cast<_int64>(this);
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

	
	void setData(char *data, int dataSize)
	{
		this->data = reinterpret_cast<_int64>(data) - reinterpret_cast<_int64>(this);
		this->dataSize = dataSize;
	}

	void getData(char **data, int *dataSize)
	{
		*data = reinterpret_cast<char *>(reinterpret_cast<_int64>(this) + this->data);
		*dataSize = this->dataSize;
	}
	

	void getDataInfo(char *dataType, int *numSamples) 
	{
	    *dataType = this->dataType;
	    *numSamples = this->numSamples;
	}
	void setDataInfo(char dataType, int numSamples)
	{
	    this->dataType = dataType;
	    this->numSamples = numSamples;
	}
	void setSegmented(bool segmented)
	{
		this->segmented = segmented;
	}
	bool isSegmented() { return segmented;}



	//Adjust segment offsets. 
	//Used when copying node data from one instance to another 
	//(recall that adresses are stored as self-relative offsets)
	void adjustOffsets(SharedMemNodeData *ref)
	{
		if(ref->isSegmented())
		{
			setFirstSegment(ref->getFirstSegment());
			setLastSegment(ref->getLastSegment());
		}
		else
		{
			char *currData;
			int currDataSize;
			ref->getData(&currData, &currDataSize);
			setData(currData, currDataSize);
		}
	}

	void getCoherencyInfo(bool &isOwner, int &ownerIdx, bool &isWarm, bool &isDirty, int &timestamp)
	{
		isOwner = this->owner;
		ownerIdx = this->ownerIdx;
		isWarm = this->warm;
		isDirty = this->dirty;
		timestamp = this->ownerTimestamp;
	}

	void getCoherencyInfo(bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm, char *&readerList, int &numReaders)
	{
		isOwner = this->owner;
		ownerIdx = this->ownerIdx;
		isWarm = this->warm;
		timestamp = this->ownerTimestamp;
		warmList = this->warmNodes;
		readerList = this->readerNodes;
		numWarm = this->numWarmNodes;
		numReaders = this->numReaderNodes;
	}

	void addReader(int readerIdx)
	{
		readerNodes[numReaderNodes++] = readerIdx;
	}


	void addWarm(int warmIdx)
	{
		warmNodes[numWarmNodes++] = warmIdx;
	}

	void setOwner(int ownerIdx, int timestamp)
	{
		this->ownerIdx = ownerIdx;
		this->ownerTimestamp = timestamp;
		this->owner = false;
	}

	void setCoherencyInfo(bool isOwner, int ownerIdx, bool isWarm, int timestamp,
		char *warmNodes, int numWarmNodes, char *readerNodes, int numReaderNodes)
	{
		int i;
		this->owner = isOwner;
		this->ownerIdx = ownerIdx;
		this->warm = isWarm;
		this->ownerTimestamp = timestamp;
		this->numWarmNodes = numWarmNodes;
		for(i = 0; i < numWarmNodes; i++)
			this->warmNodes[i] = warmNodes[i];
		this->numReaderNodes = numReaderNodes;
		for(i = 0; i < numReaderNodes; i++)
			this->readerNodes[i] = readerNodes[i];
	}

	void setWarm(bool warm)
	{
		this->warm = warm;
	}
	bool isWarm()
	{
		return warm;
	}

	void setDirty(bool isDirty)
	{
		this->dirty = isDirty;
	}

	Event *getDataEvent()
	{
		if(!eventActive)
		{
			dataEvent.initialize();
			eventActive = true;
		}
		return &dataEvent;
	}



//Serialization protocol for data:
//2 bytes: one (to detect endianity mismatch)
//1 byte: segmented

//	either(not segmented)
//1 byte: data Type
//4 bytes numSamples
//4 bytes: dataSize 
//dataSize bytes: data

//or (segmented)
//4 bytes: num Segment
//for each segment:
//4 bytes: total segment len
//segment
	

	void initialize(char *serialized, FreeSpaceManager *fsm, LockManager *lock)
	{
		bool forceConversion = *(short *)serialized != 1;
		segmented = (serialized[2])?true:false;
		if(segmented)
		{
			int currOfs = 3;
			numSegments = *(int *)&serialized[currOfs];
			if(forceConversion)
				swapBytes((char *)&numSegments, 4);
			currOfs += 4;
			for(int i = 0; i < numSegments; i++)
			{
				int currSize = *(int *)&serialized[currOfs];
				if(forceConversion)
					swapBytes((char *)&currSize, 4);
				currOfs += 4;
				Segment * currSegment = (Segment*)fsm->allocateShared(currSize, lock);
				currSegment->initialize(&serialized[currOfs], fsm, lock);
				appendSegment(currSegment);
				currOfs += currSize;
			}
		}
		else
		{
			dataType = serialized[3];
			numSamples = *(int *)&serialized[4];
			if(forceConversion)
				swapBytes((char *)&numSamples, 4);
			dataSize = *(int *)&serialized[8];
			if(forceConversion)
				swapBytes((char *)&dataSize, 4);
			if(dataSize > 0)
			{
				char *currPtr = fsm->allocateShared(dataSize, lock);
				memcpy(currPtr, &serialized[12], dataSize);
//If dataType != 0, stored data are not serialized, so they need endianity conversion
				if(dataType != 0 && forceConversion)
				{
					int sampleSize = dataSize / numSamples;
					if(sampleSize > 1)
					{
						for(int i = 0; i < numSamples; i++)
						{
							swapBytes(&currPtr[i * sampleSize], sampleSize);
						}
					}
				}
				setData(currPtr, dataSize);
			}
		}
	}



	void free(FreeSpaceManager *fsm, LockManager *lock)
	{
		Segment **segments = new Segment *[numSegments];
		if(segmented)
		{
			Segment *currSegment = getFirstSegment();
			int i;
			for(i = 0; i < numSegments; i++)
			{
				segments[i] = currSegment;
				currSegment->free(fsm, lock);
				currSegment = currSegment->getNext();
			}
			for(i = 0; i < numSegments; i++)
				fsm->freeShared((char *)segments[i], sizeof(Segment), lock);
			numSegments = 0;
			firstSegment = lastSegment = -reinterpret_cast<_int64>(this);
		}
		else
		{
			char *currPtr;
			int currSize;
			if(dataSize > 0)
			{
				getData(&currPtr, &currSize);
				fsm->freeShared((char *)currPtr, currSize, lock);
			}
			dataSize = 0;
		}
	}

	void getSerialized(char *serialized)
	{
		*(short *)serialized = 1;
		serialized[2] = segmented?1:0;
		if(segmented)
		{
			int currOfs = 3;
			for(int i = 0; i > numSegments; i++)
			{
				Segment *currSegment = getSegmentAt(i);
				int currLen = currSegment->getSerializedSize();
				*(int *)&serialized[currOfs] = currLen;
				currOfs += 4;
				currSegment->getSerialized(&serialized[currOfs]);
				currOfs += currLen;
			}
		}
		else //Not segmented
		{
			serialized[3] = dataType;
			*(int *)&serialized[4] = numSamples;
			char *currPtr;
			int dataSize;
			getData(&currPtr, &dataSize);
			*(int *)&serialized[8] = dataSize;
			memcpy(&serialized[12], currPtr, dataSize);
		}
	}

	int getSerializedSize()
	{
		if(!segmented)
			return 12 + dataSize;
		else
		{
			int totSize = 2 + 1 + 4  + 4 * numSegments;
			for(int i = 0; i < numSegments; i++)
			{
				Segment *currSegment = getSegmentAt(i);
				totSize += currSegment->getSerializedSize();
			}
			return totSize;
		}
	}

	void setCallbackManager(CallbackManager *callbackManager)
	{
		if(callbackManager == 0)
			this->callbackManager = 0;
		else
			this->callbackManager = reinterpret_cast<_int64>(callbackManager) - reinterpret_cast<_int64>(this);
	}

	CallbackManager *getCallbackManager()
	{
		if(callbackManager == 0)
			return 0;
		return (CallbackManager *)((char *)this + callbackManager);
	}


};

#endif

