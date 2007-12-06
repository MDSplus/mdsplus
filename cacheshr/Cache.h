#ifndef CACHE_H
#define CACHE_H

#include "SharedDataManager.h"
#include "TreeWriter.h"
#include "CoherencyManager.h"

#define FLUSH_PUT_RECORD 1
#define FLUSH_BEGIN_SEGMENT 2


struct NidChain
{
	int treeIdx;
	int nid;
	char mode;
	int idx;
	NidChain *nxt;
};


class Cache
{
	SharedDataManager dataManager;
	TreeWriter treeWriter;
	NidChain *chainHead, *chainTail;
	CoherencyManager *cManager;
	bool inQueue(int treeIdx, int nid, int idx, int mode);
	LockManager queueLock;

	void insertInQueue(int treeIdx, int nid, char mode, int idx);

public:
	Cache();
	int putRecord(int treeIdx, int nid, char dataType, int numSamples, char *data, int size, int writeThrough);
	int getRecord(int treeIdx, int nid, char *dataType, int *numSamples, char **data, int *size);
	int beginSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
		char *dim, int dimSize, char *shape, int shapeSize, char *data, 
		int dataSize, int writeThrough);
	int beginTimestampedSegment(int treeIdx, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, 
		int dataSize, _int64 start, _int64 end, char *dim, int dimSize, int writeThrough);
	int updateSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, int dimSize, int writeThrough);
	int getNumSegments(int treeIdx, int nid, int *numSegments);
	int getSegmentLimits(int treeIdx, int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *timestamped);
	int getSegmentData(int treeIdx, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
		int *shapeSize, int *currDataSize, bool *timestamped, int *actSamples);
	int isSegmented(int treeIdx, int nid, int *segmented);
	int	appendSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode);
	int appendTimestampedSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamp, int numTimestamps, int writeMode);
	int appendRow(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int writeMode);

	int flush(int treeIdx);
	int discardOldSegments(int treeIdx, int nid, _int64 timestamp);

	void * setCallback(int treeIdx, int nid, void (* callback)(int));
	int clearCallback(int treeIdx, int nid, char *callbackManager);	
	void setWarm(int treeIdx, int nid, bool warm);
	void synch();
	void startServer();
};

#endif
