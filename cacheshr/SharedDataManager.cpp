#include "SharedDataManager.h"



FreeSpaceManager SharedDataManager::freeSpaceManager;
SharedMemManager SharedDataManager::sharedMemManager;
char *SharedDataManager::startAddress;
LockManager SharedDataManager::lock;
SharedMemTree SharedDataManager::sharedTree;

#ifdef HAVE_VXWORKS_H
SEM_ID *LockManager::semaphores;
#endif



 SharedDataManager::SharedDataManager(int size)
{
	void *header, *nullNode;

	bool created = lock.initialize(TREE_LOCK);
	if(created)
	{
		lock.lock();
		startAddress = sharedMemManager.initialize(size);
		freeSpaceManager.initialize((char *)startAddress + 2 * sizeof(long), size - 2 * sizeof(long));
		sharedTree.initialize(&freeSpaceManager, &header, &nullNode);
		//Store address (offset) of tree root in the first longword of the shared memory segment
		*(long *)startAddress = (long)header - (long)startAddress;
		//Store address (offset) of the empty (guard) tree node in the second longword of the shared memory segment
		*(long *)((char *)startAddress +  sizeof(long)) = (long)nullNode - (long)startAddress;

	}
	else
	{
		lock.lock();
		startAddress = sharedMemManager.initialize(size);
		freeSpaceManager.map((char *)startAddress + 2 * sizeof(long));
		header = (char *)(*(long *)startAddress + (long)startAddress);
		nullNode = (char *)(*(long *)((char *)startAddress + sizeof(long)) + (long)startAddress);
		sharedTree.map(&freeSpaceManager, header, nullNode);
	}
	lock.unlock();
}



void SharedDataManager::deleteData(SharedMemNodeData *nodeData)
{
	char *currPtr;
	int currSize;

	if(nodeData->isSegmented())
	{
		Segment *currSegment = nodeData->getFirstSegment();
		int numSegments = nodeData->getNumSegments();
		for(int i = 0; i < numSegments; i++)
		{
			currSegment->getStart(&currPtr, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currPtr, currSize);
			currSegment->getEnd(&currPtr, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currPtr, currSize);
			currSegment->getDim(&currPtr, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currPtr, currSize);
			currSegment->getShape(&currPtr, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currPtr, currSize);
			currSegment->getData(&currPtr, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currPtr, currSize);

			currSegment = currSegment->getNext();
		}
		nodeData->setNumSegments(0);
	}
	else
	{
		nodeData->getData(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
	}
}




int SharedDataManager::deleteData(int nid)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		deleteData(nodeData);
	}
	lock.unlock();
	return 1;
}



int SharedDataManager::setData(int nid, char dataType, int numSamples, char *data, int size) //Write data indexed by nid
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);

	if(!node) 
	//No data has been written in the cache yet
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(nid);
	}
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		deleteData(nodeData);


		char *currData = freeSpaceManager.allocateShared(size);
		memcpy(currData, data, size);
		nodeData->setData((char *)currData, size);
		nodeData->setDataInfo(dataType, numSamples);
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
		lock.unlock();
		
		return 1;
	}
	else
	{
		lock.unlock();
		return 0;
	}
}
	
int SharedDataManager::getData(int nid, char *dataType, int *numSamples, char **data, int *size) //Write data indexed by nid
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		nodeData->getData((char **)data, size);
		nodeData->getDataInfo(dataType, numSamples);
		lock.unlock();
		return 1;
	}
	else
	{
		lock.unlock();
		return 0;
	}
}
	
int SharedDataManager::beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
			char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, bool timestamped)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(!node)
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		nodeData.setSegmented(true);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(nid);
	}

	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			deleteData(nodeData);
			nodeData->setSegmented(true);
		}
		int numSegments = nodeData->getNumSegments();
		if(idx > numSegments)
		{
			lock.unlock();
			return 0;
		}
		if(idx == numSegments || idx < 0)
		{
			segment = (Segment *)freeSpaceManager.allocateShared(sizeof(Segment));
			segment->initialize();
			nodeData->appendSegment(segment);
		}
		else
			segment = nodeData->getSegmentAt(idx);

		char *currPtr;
		int currSize;
		segment->getStart(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(startSize);
		memcpy(currPtr, start, startSize);
		segment->setStart(currPtr, startSize);

		segment->getEnd(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(endSize);
		memcpy(currPtr, end, endSize);
		segment->setEnd(currPtr, endSize);

		segment->getDim(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(dimSize);
		memcpy(currPtr, dim, dimSize);
		segment->setDim(currPtr, dimSize);

		segment->getShape(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(shapeSize);
		memcpy(currPtr, shape, shapeSize);
		segment->setShape(currPtr, shapeSize);

		segment->getData(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(dataSize);
		memcpy(currPtr, data, dataSize);
		segment->setData(currPtr, dataSize);

		segment->setTimestamped(timestamped);

		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}





int SharedDataManager::isSegmented(int nid, int *segmented)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		*segmented = nodeData->isSegmented()?1:0;
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::getNumSegments(int nid, int *numSegments)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		*numSegments = nodeData->getNumSegments();
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}

//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits








int SharedDataManager::appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, bool timestamped, char *timestamp)
{
	int numSegments;
	int *shape;
	int shapeSize;
	char *segmentData;
	int segmentSize;

	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		numSegments = nodeData->getNumSegments();
		if(idx >= numSegments || numSegments == 0)
		{
			lock.unlock();
			return BAD_INDEX;
		}
		Segment *segment = nodeData->getSegmentAt(idx);
		segment->getShape((char **)&shape, &shapeSize);
		//Check Shape
		if(bounds[0] != shape[0])
		{
			lock.unlock();
			return BAD_TYPE;
		}
		if(bounds[2] < shape[2] - 1 || bounds[2] > shape[2])
		{
			lock.unlock();
			return BAD_SHAPE;
		}
		if(timestamped)
		{
			if(bounds[2] != shape[2] - 1) //If timestamped a single data row can be inserted
			{
				lock.unlock();
				return BAD_SHAPE;
			}
		}

		for(int i = 0; i < shape[2] - 1; i++)
		{
			if(bounds[4 + i] != shape[4 + i])
			{
				lock.unlock();
				return BAD_SHAPE;
			}
		}
		int currSegmentSize = segment->getCurrDataSize();
		segment->getData((char **)&segmentData, &segmentSize);
		if(startIdx < 0)
		{
			int leftSize = segmentSize - currSegmentSize;
			if(dataSize > leftSize)
			{
				memcpy(&segmentData[currSegmentSize], data, leftSize);
				segment->setCurrDataSize(currSegmentSize + leftSize);
				lock.unlock();
				return TRUNCATED;
			}
			memcpy(&segmentData[currSegmentSize], data, dataSize);
			segment->setCurrDataSize(currSegmentSize + dataSize);
			if(timestamped)
			{	
				char *endPtr;
				int endSize;
				segment->appendTimestamp(timestamp);
				segment->getEnd(&endPtr, &endSize);
				memcpy(endPtr, timestamp, 8);

			}
			CallbackManager *callback = node->getCallbackManager();
			if(callback)
			callback->callCallback();
			lock.unlock();
			return 1;
		}
		else
		{
			int leftSize = segmentSize - startIdx;
			if(dataSize > leftSize)
			{
				memcpy(&segmentData[startIdx], data, leftSize);
				segment->setCurrDataSize(startIdx + leftSize);
				lock.unlock();
				return TRUNCATED;
			}
			memcpy(&segmentData[startIdx], data, dataSize);
			segment->setCurrDataSize(startIdx + dataSize);
			CallbackManager *callback = node->getCallbackManager();
			if(callback)
			callback->callCallback();
			lock.unlock();
			return 1;
		}
	}
	lock.unlock();
	return 0;
}




int SharedDataManager::updateSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
								  char *dim, int dimSize)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx > numSegments || idx < 0)
		{
			lock.unlock();
			return 0;
		}
		segment = nodeData->getSegmentAt(idx);

		char *currPtr;
		int currSize;
		segment->getStart(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(startSize);
		memcpy(currPtr, start, startSize);
		segment->setStart(currPtr, startSize);

		segment->getEnd(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(endSize);
		memcpy(currPtr, end, endSize);
		segment->setEnd(currPtr, endSize);

		segment->getDim(&currPtr, &currSize);
		if(currSize > 0)
			freeSpaceManager.freeShared((char *)currPtr, currSize);
		currPtr = freeSpaceManager.allocateShared(dimSize);
		memcpy(currPtr, dim, dimSize);
		segment->setDim(currPtr, dimSize);

		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}

int SharedDataManager::getSegmentLimits(int nid, int idx, char **start, int *startSize, 
		char **end, int *endSize, bool *timestamped)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx > numSegments || idx < 0)
		{
			lock.unlock();
			return 0;
		}
		segment = nodeData->getSegmentAt(idx);

		segment->getStart((char **)start, startSize);

		segment->getEnd((char **)end, endSize);
		*timestamped = segment->isTimestamped();

		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}

int SharedDataManager::getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,
									  char **shape, int *shapeSize, int *currDataSize, bool *timestamped)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx > numSegments || idx < 0)
		{
			lock.unlock();
			return 0;
		}
		segment = nodeData->getSegmentAt(idx);

		segment->getDim((char **)dim, dimSize);
		segment->getData((char **)data, dataSize);
		segment->getShape((char **)shape, shapeSize);
		*currDataSize = segment->getCurrDataSize();
		*timestamped = segment->isTimestamped();

		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}







void SharedDataManager::callCallback(int nid)
{
	SharedMemNode *node;

	node = sharedTree.find(nid);
	if(node)
	{
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
}




 void *SharedDataManager::setCallback(int nid, void (*callback)(int))
{

	char *retPtr = NULL;
	int status = 0;
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(nid);
	if(!retNode) 
	//No data has been written in the cache yet, or the not is not cacheable,
	//then create a nid node without data
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		sharedTree.insert(&nodeData);
		retNode = sharedTree.find(nid);
	}


	if(retNode)
	{
		//Create a new Callback manager and concatenate it to the linked list of Callback managers
		//associated with this  nid node
		CallbackManager *callbackManager = (CallbackManager *)freeSpaceManager.allocateShared(sizeof(CallbackManager));
		CallbackManager *prevCallbackManager = retNode->getCallbackManager();
		if(prevCallbackManager)
			prevCallbackManager->setPrev((char *)callbackManager);
		callbackManager->setNext((char *)retNode->getCallbackManager());
		callbackManager->setPrev(NULL);
		retNode->setCallbackManager(callbackManager);
		callbackManager->initialize(nid, callback);
		SharedMemNodeData *nodeData = retNode->getData();
		nodeData->setWarm(true);
		lock.unlock();
		return callbackManager;
	}
	else
	{
		lock.unlock();
		return NULL;
	}
}

 int SharedDataManager::clearCallback(int nid, char *callbackDescr)
{
	int status = 0;
	CallbackManager *callbackManager = (CallbackManager *)callbackDescr;
	callbackManager->dispose();
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(nid);
	//Remove the CallbackManager instance from the queue associated with the nid node
	if(retNode)
	{
		CallbackManager *callbackManagerHead = retNode->getCallbackManager();
		if(callbackManagerHead == callbackManager)
			retNode->setCallbackManager(callbackManager->getNext());
		else
		{
			CallbackManager *next = callbackManager->getNext();
			CallbackManager *prev = callbackManager->getPrev();
			if(prev)
				prev->setNext((char *)next);
			if(next)
				next->setPrev((char *)prev);
		}
		freeSpaceManager.freeShared((char *)callbackManager, sizeof(CallbackManager));
		status = 1;
	}
	lock.unlock();
	return status;
}

SharedMemNodeData *SharedDataManager::getNodeData(int nid, bool create)
{
	SharedMemNode *node;
	node = sharedTree.find(nid);
	if(node)
		return node->getData();

	if(create)
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(nid);
		return node->getData();
	}

	return 0;
}


//Cache coerency methods
void SharedDataManager::getCoherencyInfo(int nid, bool &isOwner, int &ownerIdx, bool &isWarm, bool &isDirty, int &timestamp)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->getCoherencyInfo(isOwner, ownerIdx, isWarm, isDirty, timestamp);

}

	
void SharedDataManager::getCoherencyInfo(int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm, char *&readerList, int &numReader)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->getCoherencyInfo(isOwner, ownerIdx, isWarm, timestamp, warmList, numWarm, readerList, numReader);

}

	
void SharedDataManager::addReader(int nid, int readerIdx)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->addReader(readerIdx);
	lock.unlock();
}

void  SharedDataManager::addWarm(int nid, int warmIdx)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->addWarm(warmIdx);
	lock.unlock();
}

void SharedDataManager::setOwner(int nid, int ownerIdx, int timestamp)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->setOwner(ownerIdx, timestamp);
	lock.unlock();
}

void SharedDataManager::setCoherencyInfo(int nid, bool isOwner, int ownerIdx, bool isWarm, int timestamp,
		char *warmNodes, int numWarmNodes, char *readerNodes, int numReaderNodes)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->setCoherencyInfo(isOwner, ownerIdx, isWarm, timestamp,warmNodes, numWarmNodes, 
			readerNodes, numReaderNodes);
	lock.unlock();
}

void SharedDataManager::setWarm(int nid, bool warm)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->setWarm(warm);
	lock.unlock();
}

bool SharedDataManager::isWarm(int nid)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		return nodeData->isWarm();
	return false;
}

void SharedDataManager::setDirty(int nid, bool isDirty)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->setDirty(isDirty);
	lock.unlock();
}


Event *SharedDataManager::getDataEvent(int nid)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		return nodeData->getDataEvent();
	return NULL;
}

int SharedDataManager::getSerializedSize(int nid)
{
	int size = 0;
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		size =  nodeData->getSerializedSize();
	lock.unlock();
	return size;
}

void SharedDataManager::getSerialized(int nid, char *serialized)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
		nodeData->getSerialized(serialized);
	lock.unlock();
}

void SharedDataManager::setSerializedData(int nid, char *serializedData, int dataLen)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);

	if(!node) 
	//No data has been written in the cache yet
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(nid);
	}
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		nodeData->free(&freeSpaceManager);
		nodeData->initialize(serializedData, &freeSpaceManager);
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
	lock.unlock();
}



