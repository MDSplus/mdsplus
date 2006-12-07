#include "Cache.h"
//#include "CoherencyManager.h"

extern "C" void *getCache();
extern "C" int putRecord(int nid, char *data, int size, int writeThrough, void *cachePtr);
extern "C" int getRecord(int nid, char **data, int *size, void *cachePtr);
extern "C" int putRecordInternal(int nid, char *data, int size);
extern "C" int beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
						char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, 
						int writeThrough, void *cachePtr);
extern "C" int handleSegmentInternal(int nid);
extern "C" int beginSegmentInternal(int nid, int idx, char *start, char *end, char *dim, char *data);
extern "C" int updateSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, 
							 int dimSize, int writeThrough, void *cachePtr);
extern "C" int getNumSegments(int nid, int *numSegments, void *cachePtr);
extern "C" int getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize, void *cachePtr);
extern "C" int getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
							  int *shapeSize, int *currDataSize, void *cachePtr);
extern "C" int isSegmented(int nid, int *segmented, void *cachePtr);
extern "C" int flushTree(void *cachePtr);
extern "C" void *setCallback(int nid, void (* callback)(int), void *cachePtr);
extern "C" int clearCallback(int nid, void *callbackDescr, void *cachePtr);
extern "C" int appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, void *cachePtr);

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

int beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
									char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, 
									int writeThrough, void *cachePtr)
{
	return ((Cache *)cachePtr)->beginSegment(nid, idx, start, startSize, end, endSize, dim, dimSize, shape, shapeSize, data, 
			dataSize, writeThrough);
}

int updateSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, int dimSize, 
				  int writeThrough, void *cachePtr)
{
	return ((Cache *)cachePtr)->updateSegment(nid, idx, start, startSize, end, endSize, dim, dimSize, writeThrough);
}


int getNumSegments(int nid, int *numSegments, void *cachePtr)
{
	return ((Cache *)cachePtr)->getNumSegments(nid, numSegments);
}

int getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize, void *cachePtr)
{
	return ((Cache *)cachePtr)->getSegmentLimits(nid, idx, start, startSize, end, endSize);
}

int getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
				   int *shapeSize, int *currDataSize, void *cachePtr)
{
	return ((Cache *)cachePtr)->getSegmentData(nid, idx, dim, dimSize, data, dataSize, shape, shapeSize, currDataSize);
}

int isSegmented(int nid, int *segmented, void *cachePtr)

{
	return ((Cache *)cachePtr)->isSegmented(nid, segmented);
}

int flushTree(void *cachePtr)
{
	return ((Cache *)cachePtr)->flush();
}

void *setCallback(int nid, void (* callback)(int), void *cachePtr)
{
	return ((Cache *)cachePtr)->setCallback(nid, callback);
}

int  clearCallback(int nid, void *callbackDescr, void *cachePtr)
{
	return ((Cache *)cachePtr)->clearCallback(nid, callbackDescr);
}

int appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, void *cachePtr)
{
	return ((Cache *)cachePtr)->appendSegmentData(nid, bounds, boundsSize, data, dataSize, idx, startIdx);
}



////////////////Cache Methods

Cache::Cache()
{
	treeWriter.setDataManager(&dataManager);
	treeWriter.start();
	chainHead = chainTail = 0;
}


int Cache::putRecord(int nid, char *data, int size, int writeThrough)
{
	int status = dataManager.setData(nid, data, size);
	if(!(status & 1)) return status;
	if(writeThrough)
	{
		treeWriter.addPutRecord(nid);
	}
	else
	{
		if(!inQueue(nid, 0, FLUSH_PUT_RECORD))
		{
			NidChain *newNid = new NidChain;
			newNid->nid = nid;
			if(chainTail)
				chainTail->nxt = newNid;
			else
				chainTail = newNid;
			newNid->nxt = 0;
			newNid->idx = 0;
			newNid->mode = FLUSH_PUT_RECORD;
			if(!chainHead)
				chainHead = newNid;
		}
	}
	return 1;
}



int Cache::getRecord(int nid, char **data, int *size)
{
	return dataManager.getData(nid, data, size);
}


int Cache::beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
									char *dim, int dimSize, char *shape, int shapeSize, char *data, 
									int dataSize, int writeThrough)
{
	int status = dataManager.beginSegment(nid, idx, start, startSize, end, endSize, dim, dimSize, shape, shapeSize, 
			data, dataSize);

	
	if(!(status & 1)) return status;
	if(writeThrough)
	{
		treeWriter.addBeginSegment(nid, idx);
	}
	else
	{
		if(!inQueue(nid, idx, FLUSH_BEGIN_SEGMENT))
		{
			NidChain *newNid = new NidChain;
			newNid->nid = nid;
			if(chainTail)
				chainTail->nxt = newNid;
			else
				chainTail = newNid;
			newNid->nxt = 0;
			newNid->idx = idx;
			newNid->mode = FLUSH_BEGIN_SEGMENT;
			if(!chainHead)
				chainHead = newNid;
		}
	}
	return 1;
}


int Cache::updateSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
						 char *dim, int dimSize, int writeThrough)
{
	int status = dataManager.updateSegment(nid, idx, start, startSize, end, endSize, dim, dimSize);
	if(!(status & 1)) return status;
	if(writeThrough)
	{
		treeWriter.addUpdateSegment(nid, idx);
	}
	return 1;

}


	
int Cache::getNumSegments(int nid, int *numSegments)
{
	return dataManager.getNumSegments(nid, numSegments);
}


int Cache::appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx)
{
	return dataManager.appendSegmentData(nid, bounds, boundsSize, data, dataSize, idx, startIdx);
}


int Cache::getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize)
{
	return dataManager.getSegmentLimits(nid, idx, start, startSize, end, endSize);
}


int Cache::getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,
						  char **shape, int *shapeSize, int *currDataSize)
{
	return dataManager.getSegmentData(nid, idx, dim, dimSize, data, dataSize, shape, shapeSize, currDataSize);
}

int Cache::isSegmented(int nid, int *segmented)
{
	return dataManager.isSegmented(nid, segmented);
}


bool Cache::inQueue(int nid, int idx, int mode)
{
	NidChain *currChainNid = chainHead;
	while(currChainNid)
	{
		if(currChainNid->mode == mode && currChainNid->idx == idx && currChainNid->nid == nid)
			return true;
		currChainNid = currChainNid->nxt;
	}
	return false;
}


int Cache::flush() 
{
	char *data;
	int dataSize;
	int status, errStatus = 1;
	NidChain *currChainNid, *prevChainNid;
	currChainNid = chainHead;
	while(currChainNid)	
	{
		switch(currChainNid->mode) {
			case FLUSH_PUT_RECORD:
				status = dataManager.getData(currChainNid->nid, &data, &dataSize);
				if(status &1 )status = putRecordInternal(currChainNid->nid, data, dataSize);
				if(!(status & 1))
				{
					errStatus = status;
				}
				break;
			case FLUSH_BEGIN_SEGMENT:
				status = handleSegmentInternal(currChainNid->nid);
				if(!(status & 1))
				{
					errStatus = status;
				}
				break;
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


int Cache::clearCallback(int nid, void *callbackDescr)
{
	return dataManager.clearCallback(nid, callbackDescr);
}



void Cache::startServer()
{
	//CoherencyManager *cManager = new CoherencyManager(&dataManager);
	//cManager->startServer();
}

void Cache::setWarm(int nid, bool warm)
{
	dataManager.setWarm(nid, warm);
}
