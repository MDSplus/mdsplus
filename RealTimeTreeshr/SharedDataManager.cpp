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

}


int SharedDataManager::setSharedData(int nid, char *data, int size, bool &isNew)
{
	lock.lock();
	SharedMemNode *node;
	void *newData;

	DataSlot *newSlot = (DataSlot *)(freeSpaceManager.allocateShared(sizeof(DataSlot)));
	newSlot->initialize();
	newData = freeSpaceManager.allocateShared(size);
	memcpy(newData, data, size);
	newSlot->setData(newData, size);
	
	
	node = sharedTree.find(nid);
	if(node)
	{
		isNew = false;
		DataSlot *prevSlot = node->getData()->getFirstSlot(); 
		//Free all the slot chain (in most cases a single data item) 
		while(prevSlot)
		{
			DataSlot *nextSlot = prevSlot->getNext();
			void *currData;
			int currSize;
			prevSlot->getData(&currData, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currData, currSize);
			prevSlot->getDim(&currData, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currData, currSize);

			freeSpaceManager.freeShared((char *)prevSlot, sizeof(DataSlot));
			prevSlot = nextSlot;
		}
		node->getData()->resetItems();
		node->getData()->appendSlot(newSlot);
		node->getData()->setTimed(false);
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
	else
	{
		isNew = true;
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		nodeData.setTimed(false);
		nodeData.appendSlot(newSlot);
		sharedTree.insert(&nodeData);
	}
	lock.unlock();
	return 1;
}


int SharedDataManager::setCurrDim(int nid, char *dim, int dimSize)
{
	lock.lock();
	SharedMemNode *node;

	
	void *currDim = freeSpaceManager.allocateShared(dimSize);
	memcpy(currDim, dim, dimSize);

	node = sharedTree.find(nid);
	if(node)
	{
		SharedMemNodeData *nodeData = node->getData();
		char *prevDim;
		int prevDimSize;
		nodeData->getCurrDim((void **)&prevDim, &prevDimSize);
		if(prevDimSize > 0)
			freeSpaceManager.freeShared(prevDim, prevDimSize);
		nodeData->setCurrDim(currDim, dimSize);
		nodeData->setTimed(true);
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
	else
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		nodeData.setCurrDim(currDim, dimSize);
		nodeData.setTimed(true);
		sharedTree.insert(&nodeData);
	}
	lock.unlock();
	return 1;
}

 int SharedDataManager::resetSharedData(int nid)
 {
	lock.lock();
	SharedMemNode *node = sharedTree.find(nid);
	if(node)
	{
		DataSlot *prevSlot = node->getData()->getFirstSlot(); 
		//Free all the slot chain (in most cases a single data item) 
		while(prevSlot)
		{
			DataSlot *nextSlot = prevSlot->getNext();
			void *currData;
			int currSize;
			prevSlot->getData(&currData, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currData, currSize);
			prevSlot->getDim(&currData, &currSize);
			if(currSize > 0)
				freeSpaceManager.freeShared((char *)currData, currSize);

			freeSpaceManager.freeShared((char *)prevSlot, sizeof(DataSlot));
			prevSlot = nextSlot;
		}
		node->getData()->resetItems();
		node->getData()->setTimed(false);
	}
	return 1;
 }


int SharedDataManager::appendSharedData(int nid, char *data, int size)
{
	lock.lock();
	SharedMemNode *node;
	void *newData;

	
	DataSlot *newSlot = (DataSlot *)(freeSpaceManager.allocateShared(sizeof(DataSlot)));
	newSlot->initialize();
	newData = freeSpaceManager.allocateShared(size);
	memcpy(newData, data, size);
	newSlot->setData(newData, size);
	
	
	node = sharedTree.find(nid);
	if(node)
	{
		node->getData()->appendSlot(newSlot);
		CallbackManager *callback = node->getCallbackManager();
		if(callback)
			callback->callCallback();
	}
	else
	{
		SharedMemNodeData nodeData;
		nodeData.setNid(nid);
		nodeData.appendSlot(newSlot);
		sharedTree.insert(&nodeData);
	}

	lock.unlock();
	return 1;
}


int SharedDataManager::getSharedData(int nid, char **data, int *size)
{
	int status = 0;
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(nid);
	if(retNode)
	{
		status = 1;
		SharedMemNodeData *retData = retNode->getData();
		DataSlot *dataSlot = retData->getFirstSlot();

		dataSlot->getData((void **)data, size); 
	}
	lock.unlock();
	return status;
}




int SharedDataManager::getSharedInfo(int nid, int *numItems, int *isTimed)
{
	int status = 0;
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(nid);
	if(retNode)
	{
		SharedMemNodeData *nodeData = retNode->getData();
		*isTimed = nodeData->isTimed();
		*numItems = nodeData->getNumItems();
		status = 1;
	}
	lock.unlock();
	return status;
}


int SharedDataManager::getMultipleSharedData(int nid, char **data, int *sizes, char **dims, int *dimSizes)
{
	int status = 0;
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(nid);
	if(retNode)
	{
		status = 1;
		SharedMemNodeData *retData = retNode->getData();
		int numItems = retData->getNumItems();
		DataSlot *dataSlot = retData->getFirstSlot();
		for(int i = 0; i < numItems; i++)
		{
			void *currData;
			dataSlot->getData(&currData, &sizes[i]);
			data[i] = (char *)malloc(sizes[i]);
			memcpy(data[i], currData, sizes[i]);
			void *currDim;
			dataSlot->getDim(&currDim, &dimSizes[i]);
			if(dimSizes[i] > 0)
			{
				dims[i] = (char *)malloc(dimSizes[i]);
				memcpy(dims[i], currDim, dimSizes[i]);
			}
			dataSlot = dataSlot->getNext();
		}

	}
	lock.unlock();
	return status;
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

	NotifyServer *notifyServer = NULL;
	int status = 0;
	lock.lock();
	SharedMemNode *retNode = sharedTree.find(nid);
	if(!retNode) 
	//No data has been written in the cache yet, or the not is not cacheable,
	//then ceate a nid node without data
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
		//A new instance of notifyServer is created and associated with the new Callback manager
		notifyServer = new NotifyServer(nid);
		notifyServer->setCallback(callback);
		notifyServer->setCallbackManager(callbackManager);
		NotifyClient *notifyClient = new NotifyClient(nid);
		callbackManager->setNotify(notifyClient);
		lock.unlock();
		notifyServer->startServer();
		delete notifyClient;
	}
	else
		lock.unlock();
	return notifyServer;
}

 void SharedDataManager::clearCallback(int nid, void *callbackDescr)
{
	NotifyServer *notifyServer = (NotifyServer *)callbackDescr;
	CallbackManager *callbackManager = notifyServer->getCallbackManager();
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
		//Set

		freeSpaceManager.freeShared((char *)callbackManager, sizeof(CallbackManager));
	}
	lock.unlock();
	notifyServer->killServer();
	delete notifyServer;
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
		nodeData.setTimed(false);
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
