#ifndef CACHE_H
#define CACHE_H

#include "SharedDataManager.h"
#include "TreeWriter.h"
#include "CoherencyManager.h"

#define FLUSH_PUT_RECORD 1
#define FLUSH_BEGIN_SEGMENT 2
#define FLUSH_UPDATE_SEGMENT 3


struct NidChain
{
	TreeDescriptor treeIdx;
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
	bool inQueue(TreeDescriptor treeIdx, int nid, int idx, int mode);
	LockManager queueLock;

	void insertInQueue(TreeDescriptor treeIdx, int nid, char mode, int idx);

public:
	Cache();
	Cache(bool isShared, int size);
	int putRecord(TreeDescriptor treeIdx, int nid, char dataType, int numSamples, char *data, int size, int writeThrough);
	int getRecord(TreeDescriptor treeIdx, int nid, char *dataType, int *numSamples, char **data, int *size);
	int beginSegment(TreeDescriptor treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
		char *dim, int dimSize, char *shape, int shapeSize, char *data, 
		int dataSize, int writeThrough);
	int beginTimestampedSegment(TreeDescriptor treeIdx, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, 
		int dataSize, _int64 start, _int64 end, char *dim, int dimSize, int writeThrough);
	int updateSegment(TreeDescriptor treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, int dimSize, int writeThrough);
	int getNumSegments(TreeDescriptor treeIdx, int nid, int *numSegments);
	int getSegmentLimits(TreeDescriptor treeIdx, int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *timestamped);
	int getSegmentData(TreeDescriptor treeIdx, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
		int *shapeSize, int *currDataSize, bool *timestamped, int *actSamples);
	int getSegmentInfo(TreeDescriptor treeIdx, int nid, int idx, int **shape, int *shapeSize, int *currDataSize);
	int getSegmentInfo(TreeDescriptor treeIdx, int nid, int **shape, int *shapeSize, int *currDataSize);
	int isSegmented(TreeDescriptor treeIdx, int nid, int *segmented);
	int	appendSegmentData(TreeDescriptor treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode);
	int appendTimestampedSegmentData(TreeDescriptor treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamp, int numTimestamps, int writeMode);
	int appendRow(TreeDescriptor treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int writeMode);

	int flush(TreeDescriptor treeIdx);
	int flush(TreeDescriptor treeIdx, int nid);
	int terminateSegment(TreeDescriptor treeIdx, int nid);
	int discardOldSegments(TreeDescriptor treeIdx, int nid, _int64 timestamp);
	int discardData(TreeDescriptor treeIdx, int nid);

	void * setCallback(TreeDescriptor treeIdx, int nid, void *argument, void (* callback)(int, void *));
	int clearCallback(TreeDescriptor treeIdx, int nid, char *callbackManager);	
	void setWarm(TreeDescriptor treeIdx, int nid, bool warm);
	void synch();
	void startServer();
};

#endif
