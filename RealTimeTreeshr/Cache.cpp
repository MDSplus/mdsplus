#include "Cache.h"
#include "CoherencyManager.h"

extern "C" void *getCache();
extern "C" int putRecord(int nid, char *data, int size, int writeThrough, void *cachePtr);
extern "C" int getRecord(int nid, char **data, int *size, void *cachePtr);
extern "C" int putRecordInternal(int nid, char *data, int size);
extern "C" int flushTree(void *cachePtr);
extern "C" void *setCallback(int nid, void (* callback)(int), void *cachePtr);
extern "C" void clearCallback(int nid, void *callbackDescr, void *cachePtr);

extern "C" int setCurrDim(int nid,  char *dim, int dimSize, void *cachePtr);
extern "C" int appendRecord(int nid,  char *data, int size, void *cachePtr);
extern "C" int getRecordInfo(int nid, int *numItems, int *isTimed, void *cachePtr);
extern "C" int getRecords(int nid, char **data, int *sizes, char **dims, int *dimSizes, void *cachePtr);


static void *cache = 0;

void *getCache()
{
	LockManager lock(CACHE_LOCK);
	lock.lock();
	if(!cache)
		cache = (void *)new Cache();
	lock.unlock();
	return cache;
}



int putRecord(int nid, char *data, int size, int writeThrough, void *cachePtr)
{
	return ((Cache *)cachePtr)->putRecord(nid, data, size, writeThrough);
}

int getRecord(int nid, char **data, int *size, void *cachePtr)
{
	return ((Cache *)cachePtr)->getRecord(nid, data, size);
}

int flushTree(void *cachePtr)
{
	return ((Cache *)cachePtr)->flush();
}

void *setCallback(int nid, void (* callback)(int), void *cachePtr)
{
	return ((Cache *)cachePtr)->setCallback(nid, callback);
}

void clearCallback(int nid, void *callbackDescr, void *cachePtr)
{
	((Cache *)cachePtr)->clearCallback(nid, callbackDescr);
}

int setCurrDim(int nid,  char *dim, int dimSize, void *cachePtr)
{
	return ((Cache *)cachePtr)->setCurrDim(nid, dim, dimSize);
}

int appendRecord(int nid,  char *data, int size, void *cachePtr)
{
	return ((Cache *)cachePtr)->appendRecord(nid, data, size);
}

int getRecordInfo(int nid, int *numItems, int *isTimed, void *cachePtr)
{
	return ((Cache *)cachePtr)->getRecordInfo(nid, numItems, isTimed);
}

int getRecords(int nid, char **data, int *sizes, char **dims, int *dimSizes, void *cachePtr)
{
	return ((Cache *)cachePtr)->getRecords(nid, data, sizes, dims, dimSizes);
}



////////////////Cache Methods

Cache::Cache()
{
	treeWriter.setDataManager(&dataManager);
	treeWriter.start();
	chainHead = 0;
}


int Cache::putRecord(int nid, char *data, int size, int writeThrough)
{
	bool isNew;
	int status = dataManager.setSharedData(nid, data, size, isNew);
	if(!(status & 1)) return status;
	if(writeThrough)
	{
		treeWriter.addNid(nid);
	}
	else if(isNew)
	{
		NidChain *newNid = new NidChain;
		newNid->nid = nid;
		newNid->nxt = chainHead;
		chainHead = newNid;
	}
	return 1;
}



int Cache::getRecord(int nid, char **data, int *size)
{
	return dataManager.getSharedData(nid, data, size);
}

int Cache::flush() 
{
	char *data;
	int size;
	int status, errStatus = 1;
	NidChain *currChainNid, *prevChainNid;
	currChainNid = chainHead;
	while(currChainNid)	
	{
		dataManager.getSharedData(currChainNid->nid, &data, &size);
		status = putRecordInternal(currChainNid->nid, data, size);
		if(!(status & 1))
		{
			errStatus = status;
		}
		prevChainNid = currChainNid;
		currChainNid = currChainNid->nxt;
		delete prevChainNid;
	}
	chainHead = 0;
	return errStatus;
 
}

void *Cache::setCallback(int nid, void (* callback)(int))
{
	return dataManager.setCallback(nid, callback);
}


void Cache::clearCallback(int nid, void *callbackDescr)
{
	dataManager.clearCallback(nid, callbackDescr);
}


int Cache::setCurrDim(int nid, char *dim, int dimSize)
{
	return dataManager.setCurrDim(nid, dim, dimSize);
}

int Cache::appendRecord(int nid, char *data, int size)
{
	return dataManager.appendSharedData(nid, data, size);
}


int Cache::getRecordInfo(int nid, int *numItems, int *isTimed)
{
	return dataManager.getSharedInfo(nid, numItems, isTimed);
}


int Cache::getRecords(int nid, char **data, int *sizes, char **dims, int *dimSizes)
{
	return dataManager.getMultipleSharedData(nid, data, sizes, dims, dimSizes);
}

void Cache::startServer()
{
	CoherencyManager *cManager = new CoherencyManager(&dataManager);
	cManager->startServer();
}

void Cache::setWarm(int nid, bool warm)
{
	dataManager.setWarm(nid, warm);
}