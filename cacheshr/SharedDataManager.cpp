#include "SharedDataManager.h"



FreeSpaceManager SharedDataManager::freeSpaceManager;
SharedMemManager SharedDataManager::sharedMemManager;
void *SharedDataManager::startAddress;
LockManager SharedDataManager::lock;
SharedMemTree SharedDataManager::sharedTree;


 SharedDataManager::SharedDataManager(int size)
{
	void *header, *nullNode;

	bool created = lock.initialize(TREE_LOCK);
	if(created)
	{
		lock.lock();
		startAddress = sharedMemManager.initialize(size);
//		freeSpaceManager.initialize((char *)startAddress + 4 * sizeof(long), size - 4 * sizeof(long));
		freeSpaceManager.initialize((char *)startAddress + 2 * sizeof(long), size - 2 * sizeof(long));
		sharedTree.initialize(&freeSpaceManager, &header, &nullNode);
		//Store address (offset) of tree root in the first longword of the shared memory segment
		*(long *)startAddress = (long)header - (long)startAddress;
		//Store address (offset) of the empty (guard) tree node in the second longword of the shared memory segment
		*(long *)((char *)startAddress +  sizeof(long)) = (long)nullNode - (long)startAddress;

		//Allocate first slot of free nids and srtore its  address (offset) in the third longword of the shared memory segment
		//freeNids = (LinkedSlot *)freeSpaceManager.allocateShared(sizeof(LinkedSlot));
		//freeNids->initialize();
		//*(long *)((char *)startAddress +  2 * sizeof(long)) = (long)freeNids - (long)startAddress;
		//Highest allocated nid set to zero
		//*(long *)((char *)startAddress +  3 * sizeof(long)) = 0; 
		

	}
	else
	{
		lock.lock();
		startAddress = sharedMemManager.initialize(size);
		freeSpaceManager.map((char *)startAddress + 2 * sizeof(long));
//		freeSpaceManager.map((char *)startAddress + 4 * sizeof(long));
		header = (void *)(*(long *)startAddress + (long)startAddress);
		nullNode = (void *)(*(long *)((char *)startAddress + sizeof(long)) + (long)startAddress);
		sharedTree.map(&freeSpaceManager, header, nullNode);
		//freeNids = (LinkedSlot *)((char *)startAddress +  2 * sizeof(long));
	}
	lock.unlock();
	freeSpaceManager.print();

}



void SharedDataManager::deleteData(SharedMemNodeData *nodeData)
{
	void *currPtr;
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



int SharedDataManager::setData(int nid, char *data, int size) //Write data indexed by nid
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


		void *currData = freeSpaceManager.allocateShared(size);
		memcpy(currData, data, size);
		nodeData->setData((void *)currData, size);
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
		lock.unlock();
		
		
		freeSpaceManager.print();
		
		return 1;
	}
	else
	{
		lock.unlock();
		return 0;
	}
}
	
int SharedDataManager::getData(int nid, char **data, int *size) //Write data indexed by nid
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
		nodeData->getData((void **)data, size);
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
									char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize)
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

		void *currPtr;
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
										 int dataSize, int idx, int startIdx)
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
		if(idx >= numSegments)
		{
			lock.unlock();
			return BAD_INDEX;
		}
		Segment *segment = nodeData->getSegmentAt(idx);
		segment->getShape((void **)&shape, &shapeSize);
		//Check Shape
		if(bounds[0] != shape[0])
			return BAD_TYPE;
		if(bounds[2] < shape[2] - 1 || bounds[2] > shape[2])
			return BAD_SHAPE;
		for(int i = 0; i < shape[2] - 1; i++)
		{
			if(bounds[4 + i] != shape[4 + i])
			return BAD_SHAPE;
		}
		int currSegmentSize = segment->getCurrDataSize();
		segment->getData((void **)&segmentData, &segmentSize);
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

		void *currPtr;
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

int SharedDataManager::getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize)
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

		segment->getStart((void **)start, startSize);

		segment->getEnd((void **)end, endSize);

		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}

int SharedDataManager::getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,
									  char **shape, int *shapeSize, int *currDataSize)
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

		segment->getDim((void **)dim, dimSize);
		segment->getData((void **)data, dataSize);
		segment->getShape((void **)shape, shapeSize);
		*currDataSize = segment->getCurrDataSize();

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

	void *retPtr = NULL;
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
			prevCallbackManager->setPrev(callbackManager);
		callbackManager->setNext(retNode->getCallbackManager());
		callbackManager->setPrev(NULL);
		retNode->setCallbackManager(callbackManager);
		callbackManager->initialize(nid, callback);
		lock.unlock();
		return callbackManager;
	}
	else
	{
		lock.unlock();
		return NULL;
	}
}

 int SharedDataManager::clearCallback(int nid, void *callbackDescr)
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
				prev->setNext(next);
			if(next)
				next->setPrev(prev);
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
void SharedDataManager::getCoherencyInfo(int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	isOwner = nodeData->isOwner;
	ownerIdx = nodeData->owner;
	isWarm = nodeData->isWarm;
	timestamp = nodeData->ownerTimestamp;
}


void SharedDataManager::getCoherencyInfo(int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	isOwner = nodeData->isOwner;
	ownerIdx = nodeData->owner;
	isWarm = nodeData->isWarm;
	timestamp = nodeData->ownerTimestamp;
	numWarm = nodeData->numWarmNodes;
	if(numWarm > 0)
	{
		warmList = new char[numWarm];
		memcpy(warmList, nodeData->warmNodes, numWarm);
	}
}

void SharedDataManager::setOwner(int nid, int ownerIdx, int timestamp)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
	{
		nodeData->owner = ownerIdx;
		nodeData->isOwner = false;
		nodeData->ownerTimestamp = timestamp;
	}
	lock.unlock();
}


void SharedDataManager::setCoherencyInfo(int nid, bool isOwner, int ownerIdx, bool isWarm, 
										 int timestamp, char *warmList, int numWarm)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
	{
		nodeData->owner = ownerIdx;
		nodeData->isOwner = isOwner;
		nodeData->isWarm = isWarm;
		nodeData->ownerTimestamp = timestamp;
		nodeData->numWarmNodes = numWarm;
		memcpy(nodeData->warmNodes, warmList, numWarm);
	}
	lock.unlock();
}

void SharedDataManager::setWarm(int nid, bool warm)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	if(nodeData)
	{
		nodeData->isWarm = warm;
	}
	lock.unlock();
}

bool SharedDataManager::isWarm(int nid)
{
	SharedMemNodeData *nodeData = getNodeData(nid, true);
	return nodeData->isWarm;
}
