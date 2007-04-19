#ifndef CACHE_H
#define CACHE_H

#include "SharedDataManager.h"
#include "TreeWriter.h"
#include "Coherencymanager.h"

#define FLUSH_PUT_RECORD 1
#define FLUSH_BEGIN_SEGMENT 2


struct NidChain
{
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
	bool inQueue(int nid, int idx, int mode);

public:
	Cache();
	int putRecord(int nid, char dataType, int numSamples, char *data, int size, int writeThrough);
	int getRecord(int nid, char *dataType, int *numSamples, char **data, int *size);
	int beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
		char *dim, int dimSize, char *shape, int shapeSize, char *data, 
		int dataSize, char timestamped, int writeThrough);
	int updateSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, int dimSize, int writeThrough);
	int getNumSegments(int nid, int *numSegments);
	int getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *timestamped);
	int getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
		int *shapeSize, int *currDataSize, bool *timestamped);
	int isSegmented(int nid, int *segmented);
	int appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx);
	int appendTimestampedSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, char *timestamp, int startIdx);


	int flush();

	void * setCallback(int nid, void (* callback)(int));
	int clearCallback(int nid, char *callbackManager);	
	void setWarm(int nid, bool warm);

	void startServer();
};

#endif
