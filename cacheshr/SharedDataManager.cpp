#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

#include "SharedDataManager.h"
#define CACHESHR_MID 1


SimpleAllocationManager SharedDataManager::allocationManager;
char *SharedDataManager::startAddress;
Lock SharedDataManager::lock;
SharedMemTree SharedDataManager::sharedTree;

#ifdef HAVE_VXWORKS_H
SEM_ID *LockManager::semaphores;
#endif

//SharedDataManager reserves the first _int64 of the shared memory to hold the address(offset)
//of the node tree root.

 SharedDataManager::SharedDataManager(int size)
{
	void *header;
	static bool memInitialized = false;
	lock.initialize(); //Will always return true since the lock is local
	lock.lock();
	startAddress = (char *)malloc(size);
	//allocationManager.initialize((char *)startAddress + sizeof(_int64), size - sizeof(_int64));
	sharedTree.initialize(&allocationManager, &header);
	//Store address (offset) of tree root in the first _int64word of the shared memory segment
	*(_int64 *)startAddress = reinterpret_cast<_int64>(header) -
	reinterpret_cast<_int64>(startAddress);
	lock.unlock();
}





int SharedDataManager::discardData(TreeDescriptor treeId, int nid)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		node->getData()->free(&allocationManager);
	}
	lock.unlock();
	return 1;
}




int SharedDataManager::setData(TreeDescriptor treeId, int nid, char dataType, int numSamples, char *data, int size) //Write data indexed by nid
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);

	if(!node) 
	//No data has been written in the cache yet
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(treeId, nid);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(treeId, nid);
	}
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData() ;
		nodeData->free(&allocationManager);

		char *currData = allocationManager.allocateShared(size);
		memcpy(currData, data, size);
		nodeData->setData((char *)currData, size);
		nodeData->setDataInfo(dataType, numSamples);
		CallbackManager *callback = node->getData()->getCallbackManager();
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
	
int SharedDataManager::getData(TreeDescriptor treeId, int nid, char *dataType, int *numSamples, char **data, int *size) //Write data indexed by nid
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
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
	
int SharedDataManager::beginSegment(TreeDescriptor treeId, int nid, int idx, char *start, int startSize, char *end, int endSize, 
			char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, int *retIdx)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(!node)
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(treeId, nid);
		nodeData.setSegmented(true);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(treeId, nid);
	}

	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			nodeData->free(&allocationManager);
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
			segment = (Segment *)allocationManager.allocateShared(sizeof(Segment));
			segment->initialize();
			nodeData->appendSegment(segment);
			*retIdx = numSegments;
		}
		else
		{
			segment = nodeData->getSegmentAt(idx);
			*retIdx = idx;
		}

		char *currPtr;
		segment->free(&allocationManager);

		currPtr = allocationManager.allocateShared(startSize);
		memcpy(currPtr, start, startSize);
		segment->setStart(currPtr, startSize);

		currPtr = allocationManager.allocateShared(endSize);
		memcpy(currPtr, end, endSize);
		segment->setEnd(currPtr, endSize);

		currPtr = allocationManager.allocateShared(dimSize);
		memcpy(currPtr, dim, dimSize);
		segment->setDim(currPtr, dimSize);

		currPtr = allocationManager.allocateShared(shapeSize);
		memcpy(currPtr, shape, shapeSize);
		segment->setShape(currPtr, shapeSize);

		currPtr = allocationManager.allocateShared(dataSize);
		memcpy(currPtr, data, dataSize);
		segment->setData(currPtr, dataSize);

		segment->setTimestamped(false);

		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::beginTimestampedSegment(TreeDescriptor treeId, int nid, int idx, int numItems, char *shape, 
											   int shapeSize, char *data, int dataSize, _int64 start, 
											   _int64 end, char *dim, int dimSize, int *retIdx)
{
	Segment *segment;
//printf("BEGIN TIMESTAMPED SEGMENT SHARE: %d\n", *(int *)shape);
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(!node)
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(treeId, nid);
		nodeData.setSegmented(true);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(treeId, nid);
	}

	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			nodeData->free(&allocationManager);
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
			segment = (Segment *)allocationManager.allocateShared(sizeof(Segment));
			
//			printf("ALLOCATED SEGMENT: %x\n", segment);
			
			segment->initialize();
			nodeData->appendSegment(segment);
			*retIdx = numSegments;
		}
		else
		{
			segment = nodeData->getSegmentAt(idx);
			*retIdx = idx;
		}

		char *currPtr;
		segment->free(&allocationManager);

		if(dimSize == 0)
		{
			currPtr = allocationManager.allocateShared(8 * numItems);
//			printf("ALLOCATED SEGMENT DIM: %x\n", currPtr);
			segment->setDim(currPtr, 8 * numItems);
		}
		currPtr = allocationManager.allocateShared(shapeSize);
//			printf("ALLOCATED SEGMENT SHAPE: %x  %d\n", currPtr, *(int *)shape);
		memcpy(currPtr, shape, shapeSize);
		segment->setShape(currPtr, shapeSize);

		currPtr = allocationManager.allocateShared(dataSize);
//			printf("ALLOCATED SEGMENTDATA: %x\n", currPtr);
		memcpy(currPtr, data, dataSize);
		segment->setData(currPtr, dataSize);
	
		//The following parameters are meaningful only when copying a segment from the tree into cache
		segment->setStartTimestamp(start);
		segment->setEndTimestamp(end);
		if(dimSize > 0)
		{
			currPtr = allocationManager.allocateShared(dimSize);
//			printf("ALLOCATED SEGMENT DIM : %x\n", currPtr);
			memcpy(currPtr, dim, dimSize);
			segment->setDim(currPtr, dimSize);
			segment->setStartTimestamp(*(_int64 *)dim);
			if(end == 0) segment->setEndTimestamp(*(_int64 *)dim);
		}
		segment->setTimestamped(true);

		lock.unlock();
//printf("END BEGIN TIMESTAMPED SEGMENT LAST SEGMENT: %x\n", nodeData->getLastSegment());
		return 1;
	}
	lock.unlock();
	return 0;
}





int SharedDataManager::isSegmented(TreeDescriptor treeId, int nid, int *segmented)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
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


int SharedDataManager::getNumSegments(TreeDescriptor treeId, int nid, int *numSegments)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
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
/*
//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits

*/
int SharedDataManager::appendSegmentData(TreeDescriptor treeId, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, 
										 bool isTimestamped, _int64 *timestamps, int numTimestamps, int *segmentFilled, 
										 int *retIdx)
{
	int numSegments;
	int *shape;
	int shapeSize;
	char *segmentData;
	int segmentSize;

	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		numSegments = nodeData->getNumSegments();
		if(numSegments == 0) //May happen the first time putRow is called
		{
			lock.unlock();
			
		//	printf("APPENDSEGMENT DATA: NO SEGMENTS");
			
			return TRUNCATED;
		}

		if(idx >= numSegments || numSegments == 0)
		{
			lock.unlock();
//			printf("APPENDSEGMENT DATA: BAD INDEX");
			return BAD_INDEX;
		}
		Segment *segment = nodeData->getSegmentAt(idx);
/*		if(!segment->isTimestamped())
		{
		    lock.unlock();
		    return 0;
		}
*/		
		segment->getShape((char **)&shape, &shapeSize);
		//Check Shape. Meaning of bound array:
//		1) data type
//		2) item size in bytes
//		3) number of dimensions 
//		4) total dimension in bytes 
//		The remaining elements are the dimension limits
		if(bounds[0] != shape[0])
		{
			lock.unlock();
//			printf("APPENDSEGMENT DATA: BAD TYPE %d   %d\n", bounds[0], shape[0]);
			return BAD_TYPE;
		}
		if(bounds[2] < shape[2] - 1 || bounds[2] > shape[2])
		{
			lock.unlock();
//			printf("APPENDSEGMENT DATA: BAD TYPE 1");
			return BAD_SHAPE;
		}

		for(int i = 0; i < shape[2] - 1; i++)
		{
			if(bounds[4 + i] != shape[4 + i])
			{
				lock.unlock();
//			printf("APPENDSEGMENT DATA: BAD SHAPE");
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
				lock.unlock();
	//		printf("APPENDSEGMENT DATA: DATASIZE > LEFT SIZE");
				return TRUNCATED;
			}
			if(leftSize == dataSize)
				*segmentFilled = 1;
			else
				*segmentFilled = 0;

			memcpy(&segmentData[currSegmentSize], data, dataSize);
			segment->setCurrDataSize(currSegmentSize + dataSize);
			if(isTimestamped)
				segment->appendTimestamps(timestamps, numTimestamps);
			*retIdx = numSegments - 1;
			CallbackManager *callback = node->getData()->getCallbackManager();

			if(callback)
				callback->callCallback();
			lock.unlock();
			return 1;
		}
		else
		{
			int itemSize = bounds[3]; //Total size of a data item (array)
			int startOfs = startIdx * itemSize;
			int leftSize = segmentSize - startOfs;
			if(dataSize > leftSize)
			{
				lock.unlock();
//			printf("APPENDSEGMENT DATA: DATASIZE > LEFT SIZE");
				return TRUNCATED;
			}
			if(leftSize == dataSize)
				*segmentFilled = 1;
			else
				*segmentFilled = 0;
			memcpy(&segmentData[startOfs], data, dataSize);
			*retIdx = numSegments - 1;
			//Segment size does not change
			CallbackManager *callback = node->getData()->getCallbackManager();
			if(callback)
				callback->callCallback();
			lock.unlock();
			return 1;
		}
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::findSegment(TreeDescriptor treeId, int nid, int *retIdx)
{
	int numSegments;

	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		numSegments = nodeData->getNumSegments();
		if(numSegments == 0) 
		{
			lock.unlock();
			return 0; //No segments
		}
		*retIdx = numSegments - 1;
		lock.unlock();
		return 1;
	}
	return 0;
}






int SharedDataManager::appendRow(TreeDescriptor treeId, int nid, int *bounds, int boundsSize, char *data, int dataSize, 
								 _int64 timestamp, int *segmentFilled, int *retIdx, bool *newSegmentCreated)
{
	
	int status = appendSegmentData(treeId, nid, bounds, boundsSize, data, dataSize, -1, -1, true, 
		&timestamp, 1, segmentFilled, retIdx);

	if((status & 1) && *segmentFilled)
	{

//printf("SEGMENT FILLED FOR %f\n", *(float *)data);

		int *prevBounds, prevBoundsSize;
		char *prevData;
		int prevDataSize;
		*newSegmentCreated = true;
//		status = getSegmentData(treeId, nid, 0, &dim, &dimSize, &prevData, &prevDataSize,
//									  (char **)&prevBounds, &prevBoundsSize, &currDataSize, &timestamped, &actSamples);
		status = getSegmentDataAndShapeCopy(treeId, nid, 0, &prevData, &prevDataSize,
									  (char **)&prevBounds, &prevBoundsSize);

//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits
		int newDataSize = prevBounds[3];
		status = beginTimestampedSegment(treeId, nid, -1, newDataSize/dataSize, (char *)prevBounds, prevBoundsSize, 
			prevData, newDataSize, 0, 0, 0, 0, retIdx);
		delete [] prevData;
		delete [] prevBounds;
	}
	else
		*newSegmentCreated = false;
	return status;
}



int SharedDataManager::updateSegment(TreeDescriptor treeId, int nid, int idx, char *start, int startSize, char *end, int endSize, 
								  char *dim, int dimSize)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
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
		if(startSize > 0)
		{
		    if(currSize > 0)
			allocationManager.deallocateShared((char *)currPtr, currSize);
		    currPtr = allocationManager.allocateShared(startSize);
		    memcpy(currPtr, start, startSize);
		    segment->setStart(currPtr, startSize);
		}
		if(endSize > 0)
		{
		    segment->getEnd(&currPtr, &currSize);
		    if(currSize > 0)
			allocationManager.deallocateShared((char *)currPtr, currSize);
		    currPtr = allocationManager.allocateShared(endSize);
		    memcpy(currPtr, end, endSize);
		    segment->setEnd(currPtr, endSize);
		}
		
		if(dimSize > 0)
		{
		    segment->getDim(&currPtr, &currSize);
		    if(currSize > 0)
			allocationManager.deallocateShared((char *)currPtr, currSize);
		    currPtr = allocationManager.allocateShared(dimSize);
		    memcpy(currPtr, dim, dimSize);
		    segment->setDim(currPtr, dimSize);
  		}
		CallbackManager *callback = node->getData()->getCallbackManager();
		if(callback)
			callback->callCallback();
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}

int SharedDataManager::getSegmentLimits(TreeDescriptor treeId, int nid, int idx, char **start, int *startSize, 
		char **end, int *endSize, bool *timestamped)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx >= numSegments || idx < 0)
		{
			lock.unlock();
			return 0;
		}
		segment = nodeData->getSegmentAt(idx);

		if(segment->isTimestamped())
		{
			*timestamped = true;
			segment->getStartTimestamp(start, startSize);
			segment->getEndTimestamp(end, endSize);
		}
		else
		{
			segment->getStart((char **)start, startSize);
			segment->getEnd((char **)end, endSize);
			*timestamped = false;
		}
		
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::getSegmentInfo(TreeDescriptor treeIdx, int nid, int **shape, int *shapeSize, int *currDataSize)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeIdx, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		segment = nodeData->getSegmentAt(numSegments - 1);
		segment->getShape(reinterpret_cast<char **>(shape), shapeSize);
		*currDataSize = segment->getCurrDataSize();
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::getSegmentInfo(TreeDescriptor treeIdx, int nid, int idx, int **shape, int *shapeSize, int *currDataSize)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeIdx, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx >= numSegments)
			idx = numSegments - 1;
		segment = nodeData->getSegmentAt(idx);
		segment->getShape(reinterpret_cast<char **>(shape), shapeSize);
		*currDataSize = segment->getCurrDataSize();
		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}




int SharedDataManager::getSegmentData(TreeDescriptor treeId, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,
									  char **shape, int *shapeSize, int *currDataSize, bool *timestamped, int *actSamples)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx >= numSegments || idx < 0)
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
		*actSamples = segment->getActSamples();

		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}
int SharedDataManager::getSegmentDataAndShapeCopy(TreeDescriptor treeId, int nid, int idx, char **data, int *dataSize,
									  char **shape, int *shapeSize)
{
	Segment *segment;
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		int numSegments = nodeData->getNumSegments();
		if(idx >= numSegments || idx < 0)
		{
			lock.unlock();
			return 0;
		}
		segment = nodeData->getSegmentAt(idx);

		char *currData;
		segment->getData((char **)&currData, dataSize);
		*data = new char[*dataSize];
		memcpy(*data, currData,*dataSize);
		char *currShape;
		segment->getShape((char **)&currShape, shapeSize);
		*shape = new char[*shapeSize];
		memcpy(*shape, currShape, *shapeSize);

		lock.unlock();
		return 1;
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::discardOldSegments(TreeDescriptor treeId, int nid, _int64 timestamp)
{
	//printf("START DISCARD\n");
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		nodeData->discardOldSegments(timestamp, &allocationManager);		
		lock.unlock();
	//printf("END DISCARD\n");
		return 1;
	}
	lock.unlock();
	return 0;
}


int SharedDataManager::discardFirstSegment(TreeDescriptor treeId, int nid)
{
	//printf("START DISCARD\n");
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		if(!nodeData->isSegmented())
		{
			lock.unlock();
			return 0;
		}
		nodeData->discardFirstSegment(&allocationManager);		
		lock.unlock();
	//printf("END DISCARD\n");
		return 1;
	}
	lock.unlock();
	return 0;
}


/*
void SharedDataManager::callCallback(TreeDescriptor treeId, int nid)
{
	SharedMemNode *node;

	node = sharedTree.find(treeId, nid);
	if(node)
	{
		CallbackManager *callback = node->getData()->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
}

*/


 void *SharedDataManager::setCallback(TreeDescriptor treeId, int nid, void *argument, void (*callback)(int, void *))
{

	char *retPtr = NULL;
	int status = 0;
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(treeId, nid);
	if(!retNode) 
	//No data has been written in the cache yet, or the not is not cacheable,
	//then create a nid node without data
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(treeId, nid);
		sharedTree.insert(&nodeData);
		retNode = sharedTree.find(treeId, nid);
	}


	if(retNode)
	{
		//Create a new Callback manager and concatenate it to the linked list of Callback managers
		//associated with this  nid node
		CallbackManager *callbackManager = (CallbackManager *)allocationManager.allocateShared(sizeof(CallbackManager));
		CallbackManager *prevCallbackManager = retNode->getData()->getCallbackManager();
		if(prevCallbackManager)
			prevCallbackManager->setPrev((char *)callbackManager);
		callbackManager->setNext((char *)retNode->getData()->getCallbackManager());
		callbackManager->setPrev(NULL);
		retNode->getData()->setCallbackManager(callbackManager);
		callbackManager->initialize(nid, argument, callback);
		SharedMemNodeData *nodeData = retNode->getData();
		lock.unlock();
		return callbackManager;
	}
	else
	{
		lock.unlock();
		return NULL;
	}
}

 int SharedDataManager::clearCallback(TreeDescriptor treeId, int nid, char *callbackDescr)
{
	int status = 0;
	CallbackManager *callbackManager = (CallbackManager *)callbackDescr;
	callbackManager->dispose();
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(treeId, nid);
	//Remove the CallbackManager instance from the queue associated with the nid node
	if(retNode)
	{
		CallbackManager *callbackManagerHead = retNode->getData()->getCallbackManager();
		if(callbackManagerHead == callbackManager)
			retNode->getData()->setCallbackManager(callbackManager->getNext());
		else
		{
			CallbackManager *next = callbackManager->getNext();
			CallbackManager *prev = callbackManager->getPrev();
			if(prev)
				prev->setNext((char *)next);
			if(next)
				next->setPrev((char *)prev);
		}
		allocationManager.deallocateShared((char *)callbackManager, sizeof(CallbackManager));
		status = 1;
	}
	lock.unlock();
	return status;
}

SharedMemNodeData *SharedDataManager::getNodeData(TreeDescriptor treeId, int nid, bool create)
{
	SharedMemNode *node;
	node = sharedTree.find(treeId, nid);
	if(node)
		return node->getData();

	if(create)
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(treeId, nid);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(treeId, nid);
		return node->getData();
	}

	return 0;
}

////////////////////////////////////////////////
//Cache coerency methods
////////////////////////////////////////////////
void SharedDataManager::getCoherencyInfo(TreeDescriptor treeId, int nid, bool &isOwner, int &ownerIdx, bool &isWarm, bool &isDirty, int &timestamp)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->getCoherencyInfo(isOwner, ownerIdx, isWarm, isDirty, timestamp);
	lock.unlock();
}

	
void SharedDataManager::getCoherencyInfo(TreeDescriptor treeId, int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm, char *&readerList, int &numReader)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->getCoherencyInfo(isOwner, ownerIdx, isWarm, timestamp, warmList, numWarm, readerList, numReader);
	lock.unlock();

}

	
void SharedDataManager::addReader(TreeDescriptor treeId, int nid, int readerIdx)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->addReader(readerIdx);
	lock.unlock();
}

void  SharedDataManager::addWarm(TreeDescriptor treeId, int nid, int warmIdx)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->addWarm(warmIdx);
	lock.unlock();
}

void SharedDataManager::setOwner(TreeDescriptor treeId, int nid, int ownerIdx, int timestamp)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->setOwner(ownerIdx, timestamp);
	lock.unlock();
}

void SharedDataManager::setCoherencyInfo(TreeDescriptor treeId, int nid, bool isOwner, int ownerIdx, bool isWarm, int timestamp,
		char *warmNodes, int numWarmNodes, char *readerNodes, int numReaderNodes)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->setCoherencyInfo(isOwner, ownerIdx, isWarm, timestamp,warmNodes, numWarmNodes, 
			readerNodes, numReaderNodes);
	lock.unlock();
}

void SharedDataManager::setWarm(TreeDescriptor treeId, int nid, bool warm)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->setWarm(warm);
	lock.unlock();
}

bool SharedDataManager::isWarm(TreeDescriptor treeId, int nid)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	bool warm = false;
	if(nodeData)
		warm = nodeData->isWarm();
	lock.unlock();
	return warm;
}

void SharedDataManager::setDirty(TreeDescriptor treeId, int nid, bool isDirty)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->setDirty(isDirty);
	lock.unlock();
}


SemEvent *SharedDataManager::getDataEvent(TreeDescriptor treeId, int nid)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	SemEvent *retEv = NULL;
	if(nodeData)
		retEv = nodeData->getDataEvent();
	lock.unlock();
	return retEv;
}

int SharedDataManager::getSerializedSize(TreeDescriptor treeId, int nid)
{
	int size = 0;
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		size =  nodeData->getSerializedSize();
	lock.unlock();
	return size;
}

void SharedDataManager::getSerialized(TreeDescriptor treeId, int nid, char *serialized)
{
	lock.lock();
	SharedMemNodeData *nodeData = getNodeData(treeId, nid, true);
	if(nodeData)
		nodeData->getSerialized(serialized);
	lock.unlock();
}

void SharedDataManager::setSerializedData(TreeDescriptor treeId, int nid, char *serializedData, int dataLen)
{
	lock.lock();
	SharedMemNode *node = sharedTree.find(treeId, nid);

	if(!node) 
	//No data has been written in the cache yet
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(treeId, nid);
		sharedTree.insert(&nodeData);
		node = sharedTree.find(treeId, nid);
	}
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		nodeData->free(&allocationManager);
		nodeData->initialize(serializedData, &allocationManager);
		CallbackManager *callback = node->getData()->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
	lock.unlock();
}



