#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

#include "Cache.h"
#define WRITE_THROUGH 1
#define WRITE_BACK 2
#define WRITE_BUFFER 3
#define WRITE_LAST 4



extern "C" char *getCache();
extern "C" int putRecord(int treeIdx, int nid, char dataType, int numSamples, char *data, int size, int writeThrough, char *cachePtr);
extern "C" int getRecord(int treeIdx, int nid, char *dataType, int *numSamples, char **data, int *size, char *cachePtr);
extern "C" int putRecordInternal(int nid, char dataType, int numSamples, char *data, int size);
extern "C" int beginSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
						char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, 
						int writeThrough, char *cachePtr);
extern "C" int beginTimestampedSegment(int treeIdx, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, int dataSize, 
						_int64 start, _int64 end, char *dim, int dimSize, int writeThrough, char *cachePtr);
extern "C" int putSegmentInternal(int nid, 
						char *start, int startSize, char *end, int endSize, char *dim, int dimSize, char *data, 
						int dataSize, int *shape, int shapeSize, int isTimestamped, int actSamples);
extern "C" int updateSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, 
							 int dimSize, int writeThrough, char *cachePtr);
extern "C" int getNumSegments(int treeIdx, int nid, int *numSegments, char *cachePtr);
extern "C" int getSegmentLimits(int treeIdx, int nid, int idx, char **start, int *startSize, char **end, int *endSize,char *timestamped, char *cachePtr);
extern "C" int getSegmentData(int treeIdx, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
							  int *shapeSize, int *currDataSize, char *timestamped, char *cachePtr);
extern "C" int isSegmented(int treeIdx, int nid, int *segmented, char *cachePtr);
extern "C" int flushTree(int treeIdx, char *cachePtr);
extern "C" void *setCallback(int treeIdx, int nid, void (* callback)(int), void *cachePtr);
extern "C" int clearCallback(int treeIdx, int nid, char *callbackDescr, void *cachePtr);
extern "C" int appendSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode, char *cachePtr);
extern "C" int appendTimestampedSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamp, int numTimestamps, int writeMode, char *cachePtr);
extern "C" int appendRow(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int writeMode, char *cachePtr);
extern "C" int discardOldSegments(int treeIdx, int nid, _int64 timestamp, char *cachePtr);
extern "C" int setWarm(int treeIdx, int nid, int warm, char *cachePtr);

static char *cache = 0;

char *getCache()
{
	if(!cache)
	{
		cache = (char *)new Cache();
		((Cache *)cache)->startServer();
	}
	return cache;
}



int putRecord(int treeIdx, int nid, char dataType, int numSamples, char *data, int size, int writeThrough, char *cachePtr)
{
	return ((Cache *)cachePtr)->putRecord(treeIdx, nid, dataType, numSamples, data, size, writeThrough);
}

int getRecord(int treeIdx, int nid, char *dataType, int *numSamples, char **data, int *size, char *cachePtr)
{
	return ((Cache *)cachePtr)->getRecord(treeIdx, nid, dataType, numSamples, data, size);
}

int beginSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
									char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, 
									int writeThrough, char *cachePtr)
{
	return ((Cache *)cachePtr)->beginSegment(treeIdx, nid, idx, start, startSize, end, endSize, dim, dimSize, shape, shapeSize, data, 
			dataSize, writeThrough);
}

int beginTimestampedSegment(int treeIdx, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, int dataSize, 
									_int64 start, _int64 end, char *dim, int dimSize, int writeThrough, char *cachePtr)
{
	return ((Cache *)cachePtr)->beginTimestampedSegment(treeIdx, nid, idx, numItems, shape, shapeSize, data, 
			dataSize, start, end, dim, dimSize, writeThrough);
}

int updateSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, int dimSize, 
				  int writeThrough, char *cachePtr)
{
	return ((Cache *)cachePtr)->updateSegment(treeIdx, nid, idx, start, startSize, end, endSize, dim, dimSize, writeThrough);
}


int getNumSegments(int treeIdx, int nid, int *numSegments, char *cachePtr)
{
	return ((Cache *)cachePtr)->getNumSegments(treeIdx, nid, numSegments);
}

int getSegmentLimits(int treeIdx, int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *timestamped, char *cachePtr)
{
	return ((Cache *)cachePtr)->getSegmentLimits(treeIdx, nid, idx, start, startSize, end, endSize, timestamped);
}

int getSegmentData(int treeIdx, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
				   int *shapeSize, int *currDataSize, char *timestamped, char *cachePtr)
{
	bool boolTimestamped;
	int status;
	int actSamples;
	status = ((Cache *)cachePtr)->getSegmentData(treeIdx, nid, idx, dim, dimSize, data, dataSize, shape, 
		shapeSize, currDataSize, &boolTimestamped, &actSamples);
	*timestamped = (boolTimestamped)?1:0;
	return status;
}

int isSegmented(int treeIdx, int nid, int *segmented, char *cachePtr)

{
	return ((Cache *)cachePtr)->isSegmented(treeIdx, nid, segmented);
}

int flushTree(int treeIdx, char *cachePtr)
{
	return ((Cache *)cachePtr)->flush(treeIdx);
}

void *setCallback(int treeIdx, int nid, void (* callback)(int), void *cachePtr)
{
	return ((Cache *)cachePtr)->setCallback(treeIdx, nid, callback);
}

int  clearCallback(int treeIdx, int nid, char *callbackDescr, void *cachePtr)
{
	return ((Cache *)cachePtr)->clearCallback(treeIdx, nid, callbackDescr);
}

int appendSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode, char *cachePtr)
{
	return ((Cache *)cachePtr)->appendSegmentData(treeIdx, nid, bounds, boundsSize, data, dataSize, idx, startIdx, writeMode);
}

int appendTimestampedSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamps, int numTimestamps, int writeMode, char *cachePtr)

{
	return ((Cache *)cachePtr)->appendTimestampedSegmentData(treeIdx, nid, bounds, boundsSize, data, dataSize, idx, startIdx, 
		timestamps, numTimestamps, writeMode);
}
int appendRow(int treeIdx, int nid, int *bounds, int boundsSize, char *data,
										 int dataSize, _int64 timestamp, int writeMode, char *cachePtr)
{
	return ((Cache *)cachePtr)->appendRow(treeIdx, nid, bounds, boundsSize, data, dataSize, 
		timestamp, writeMode);
}

int discardOldSegments(int treeIdx, int nid, _int64 timestamp, char *cachePtr)
{
	return ((Cache *)cachePtr)->discardOldSegments(treeIdx, nid, timestamp);
}


int setWarm(int treeIdx, int nid, int warm, char *cachePtr)
{
	((Cache *)cachePtr)->setWarm(treeIdx, nid, (bool)warm);
	return 1;
}

////////////////Cache Methods

Cache::Cache()
{
	queueLock.initialize();
	treeWriter.setDataManager(&dataManager);
	treeWriter.start();
	chainHead = chainTail = 0;

}

void Cache::insertInQueue(int treeIdx, int nid, char mode, int idx)
{
	queueLock.lock();
	if(mode == FLUSH_BEGIN_SEGMENT || !inQueue(treeIdx, nid, idx, FLUSH_PUT_RECORD))
	{
		NidChain *newNid = new NidChain;
		newNid->nid = nid;
		newNid->treeIdx = treeIdx;
		if(chainTail)
			chainTail->nxt = newNid;
		chainTail = newNid;
		newNid->nxt = 0;
		newNid->idx = idx;
		newNid->mode = mode;
		if(!chainHead)
			chainHead = newNid;
	}
	queueLock.unlock();
}




int Cache::putRecord(int treeIdx, int nid, char dataType, int numSamples, char *data, int size, int writeMode)
{
	int status = dataManager.setData(treeIdx, nid, dataType, numSamples, data, size);
	if(!(status & 1)) return status;
	if(writeMode == WRITE_THROUGH || writeMode == WRITE_BUFFER)
	{
		treeWriter.addPutRecord(treeIdx, nid);
	}
	else  //WRITE_BACK
	{

		insertInQueue(treeIdx, nid, FLUSH_PUT_RECORD, 0);
	}
	cManager->checkWrite(treeIdx, nid);
	return 1;
}



int Cache::getRecord(int treeIdx, int nid, char *dataType, int *numSamples, char **data, int *size)
{
	cManager->checkRead(treeIdx, nid);
	return dataManager.getData(treeIdx, nid, dataType, numSamples, data, size);
}


int Cache::beginSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
									char *dim, int dimSize, char *shape, int shapeSize, char *data, 
									int dataSize, int writeMode)
{
	int retIdx;
	int status = dataManager.beginSegment(treeIdx, nid, idx, start, startSize, end, endSize, dim, dimSize, shape, shapeSize, 
			data, dataSize, &retIdx);

	
	if(!(status & 1)) return status;

	//if WRITE_THROUGH or WRITE_BUFFER, segments are written in the tree only when they have been filled
	if(writeMode == WRITE_BACK)
	{
		insertInQueue(treeIdx, nid, FLUSH_BEGIN_SEGMENT, retIdx);
	}//If writeMode == 0, this segment is being copied from tree, and therefore requires no attention
	return 1;
}

int Cache::beginTimestampedSegment(int treeIdx, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, 
									int dataSize, _int64 start, _int64 end, char *dim, int dimSize, int writeMode)
{
	int retIdx;
	int status = dataManager.beginTimestampedSegment(treeIdx, nid, idx, numItems, shape, shapeSize, 
			data, dataSize, start, end, dim, dimSize, &retIdx);

	
	if(!(status & 1)) return status;

	//if WRITE_THROUGH or WRITE_BUFFER, segments are written in the tree only when they have been filled
	if(writeMode == WRITE_BACK)
	{
		insertInQueue(treeIdx, nid, FLUSH_BEGIN_SEGMENT, retIdx);
	}
	return 1;
}


int Cache::updateSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
						 char *dim, int dimSize, int writeThrough)
{
	int status = dataManager.updateSegment(treeIdx, nid, idx, start, startSize, end, endSize, dim, dimSize);
	return status;
}


	
int Cache::getNumSegments(int treeIdx, int nid, int *numSegments)
{
	return dataManager.getNumSegments(treeIdx, nid, numSegments);
}

int Cache::appendSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode)
{
	int status, segmentFilled, retIdx;
	status = dataManager.appendSegmentData(treeIdx, nid, bounds, boundsSize, data, dataSize, idx, startIdx, 
		false, 0, 0, &segmentFilled, &retIdx);
	if((status & 1)&&segmentFilled)
	{
		if(writeMode == WRITE_THROUGH)
			treeWriter.addPutSegment(treeIdx, nid, retIdx, 0);
		else if(writeMode == WRITE_BUFFER)
			treeWriter.addPutSegment(treeIdx, nid, retIdx, 1);
	}
	return status;

}

int Cache::appendTimestampedSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamps, int numTimestamps, int writeMode)
{
	int status, segmentFilled, retIdx;

	status = dataManager.appendSegmentData(treeIdx, nid, bounds, boundsSize, data, dataSize, idx, startIdx, 
		true, timestamps, numTimestamps, &segmentFilled, &retIdx);
	if((status & 1)&&segmentFilled)
	{
		if(writeMode == WRITE_THROUGH)
			treeWriter.addPutTimestampedSegment(treeIdx, nid, retIdx, 0);
		else if(writeMode == WRITE_BUFFER)
			treeWriter.addPutTimestampedSegment(treeIdx, nid, 0, 1);
	}
	return status;

}

int Cache::appendRow(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int writeMode)
{
	int status, segmentFilled, retIdx;
	bool newSegmentCreated;
	status = dataManager.appendRow(treeIdx, nid, bounds, boundsSize, data, dataSize, timestamp, &segmentFilled, 
		&retIdx, &newSegmentCreated);
	if((status & 1))
	{
		if(writeMode == WRITE_THROUGH  && segmentFilled)
			treeWriter.addPutTimestampedSegment(treeIdx, nid, retIdx, 0);
		else if((writeMode == WRITE_BUFFER &&segmentFilled)|| writeMode == WRITE_LAST)
			treeWriter.addPutTimestampedSegment(treeIdx, nid, 0, 1);
		else if(writeMode == WRITE_BACK && newSegmentCreated)
			insertInQueue(treeIdx, nid, FLUSH_BEGIN_SEGMENT, retIdx);
	}
	return status;
}


int Cache::getSegmentLimits(int treeIdx, int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *timestamped)
{
	int status;
	bool timestampedBool;
	status =  dataManager.getSegmentLimits(treeIdx, nid, idx, start, startSize, end, endSize, &timestampedBool);
	*timestamped = (timestampedBool)?1:0;
	return 1;
}


int Cache::getSegmentData(int treeIdx, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,
						  char **shape, int *shapeSize, int *currDataSize, bool *timestamped, int *actSamples)
{
	return dataManager.getSegmentData(treeIdx, nid, idx, dim, dimSize, data, dataSize, shape, shapeSize, 
		currDataSize, timestamped, actSamples);
}

int Cache::isSegmented(int treeIdx, int nid, int *segmented)
{
	return dataManager.isSegmented(treeIdx, nid, segmented);
}

int Cache::discardOldSegments(int treeIdx, int nid, _int64 timestamp)
{
	return dataManager.discardOldSegments(treeIdx, nid, timestamp);
}

bool Cache::inQueue(int treeIdx, int nid, int idx, int mode)
{
	NidChain *currChainNid = chainHead;
	while(currChainNid)
	{
		if(currChainNid->mode == mode && currChainNid->idx == idx && currChainNid->treeIdx == treeIdx && currChainNid->nid == nid)
			return true;
		currChainNid = currChainNid->nxt;
	}
	return false;
}


int Cache::flush(int treeIdx) 
{
	char *data, *dim, *shape, *start, *end;
	char dataType;
	int dataSize, dimSize, numSamples, shapeSize, currDataSize, startSize, endSize;
	int status, errStatus = 1, actSamples;
	bool isTimestamped;
	NidChain *currChainNid, *prevChainNid;
	currChainNid = prevChainNid = chainHead;
	queueLock.lock();
	while(currChainNid)	
	{
		if(currChainNid->treeIdx == treeIdx)
		{
			switch(currChainNid->mode) {
				case FLUSH_PUT_RECORD:
					status = dataManager.getData(currChainNid->treeIdx, currChainNid->nid,&dataType, &numSamples,  &data, &dataSize);
					if(status &1 )status = putRecordInternal(currChainNid->nid, dataType, numSamples, data, dataSize);
					if(!(status & 1))
					{
						errStatus = status;
					}
					break;
				case FLUSH_BEGIN_SEGMENT:
					status = dataManager.getSegmentLimits(currChainNid->treeIdx, currChainNid->nid, currChainNid->idx, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager.getSegmentData(currChainNid->treeIdx, currChainNid->nid, currChainNid->idx, &dim, &dimSize, 
						&data, &dataSize, &shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(currChainNid->nid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, (int *)shape, shapeSize, isTimestamped, actSamples);

					
					
					
					if(!(status & 1))
					{
						errStatus = status;
					}
					break;
			}
			if(currChainNid == chainHead)
			{
				chainHead = currChainNid->nxt;
				delete currChainNid;
				currChainNid = prevChainNid = chainHead;
			}
			else
			{
				prevChainNid->nxt = currChainNid->nxt;
				delete currChainNid;
				currChainNid = prevChainNid->nxt;
			}
		}
		else //Skip this descriptor
		{
			prevChainNid = currChainNid;
			currChainNid = currChainNid->nxt;

		}
	}
	queueLock.unlock();
	return errStatus;
 
}

void *Cache::setCallback(int treeIdx, int nid, void (* callback)(int))
{
	return dataManager.setCallback(treeIdx, nid, callback);
}


int Cache::clearCallback(int treeIdx, int nid, char *callbackDescr)
{
	return dataManager.clearCallback(treeIdx, nid, callbackDescr);
}



void Cache::startServer()
{
	cManager = new CoherencyManager(&dataManager);
	cManager->startServer();
}

void Cache::setWarm(int treeIdx, int nid, bool warm)
{
	dataManager.setWarm(treeIdx, nid, warm);
}
