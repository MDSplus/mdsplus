
#ifndef SHARED_DATA_MANAGER_H
#define SHARED_DATA_MANAGER_H
#include "SimpleAllocationManager.h"
#include <Thread.h>
#include "SharedMemNode.h"
#include "SharedMemTree.h"
#include "CallbackManager.h"
#include "SemEvent.h"


#include <string.h>
#include <stdlib.h>

#define NOT_A_NATIVE_TYPE 2
#define BAD_SHAPE 4
#define BAD_INDEX 6
#define TRUNCATED 8
#define BAD_TYPE 10


#define DEFAULT_SIZE 20000000

class SharedDataManager
{
private:
	SharedMemNodeData *getNodeData(TreeDescriptor treeId, int nid, bool create);

public:
    static char *startAddress; //Initial address of the associetd Memory Zone
	static int size;		//The size in bytes of the associated Shared Memory Zone
	static SimpleAllocationManager allocationManager; //The manager of shared memory allocation
	static Lock lock;   //Global lock, used when managing nodes
	static SharedMemTree sharedTree; //ID tree
	static bool initialized;

public:
	
	SharedDataManager(int size = DEFAULT_SIZE);
	

	int setData(TreeDescriptor treeId, int nid, char dataType, int numSamples, char *data, int size); //Write data indexed by nid
	int getData(TreeDescriptor treeId, int nid, char *dataType, int *numSamples, char **data, int *size); //Read data indexed by nid
	int beginSegment(TreeDescriptor treeId, int nid, int idx, char *start, int startSize, char *end, int endSize, 
		char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, int *retIdx);
	int beginTimestampedSegment(TreeDescriptor treeId, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, int dataSize, 
		int64_t start, int64_t end, char *dim, int dimSize, int *retIdx);

	int findSegment(TreeDescriptor treeId, int nid, int *retIdx);
	int isSegmented(TreeDescriptor treeId, int nid, int *segmented);
	int getNumSegments(TreeDescriptor treeId, int nid, int *numSegments);
	int updateSegment(TreeDescriptor treeId, int nid, int idx, char *start, int startLen, char *end, int endLen, char *dim, int dimLen);
	int getSegmentLimits(TreeDescriptor treeId, int nid, int idx, char **start, int *startSize, char **end, int *endSize, bool *timestamped);
	int getSegmentInfo(TreeDescriptor treeIdx, int nid, int **shape, int *shapeSize, int *currDataSize);
	int getSegmentInfo(TreeDescriptor treeIdx, int nid, int idx, int **shape, int *shapeSize, int *currDataSize);
	int getSegmentData(TreeDescriptor treeId, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize, char **shape, 
		int *shapeSize, int *currDataSize, bool *timestamped, int *actSamples);
	int getSegmentDataAndShapeCopy(TreeDescriptor treeId, int nid, int idx, char **data, int *dataSize, char **shape, 
		int *shapeSize);
	int appendSegmentData(TreeDescriptor treeId, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, bool isTimestamped, 
										 int64_t *timestamps, int numTimestamps, int *segmentFilled, int *retIdx);
	int appendRow(TreeDescriptor treeId, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int64_t timestamp, int *segmentFilled, int *retIdx, bool *segmentCreated);
	int appendRow(TreeDescriptor treeId, int nid, int *bounds, int boundsSize, char *data, int dataSize, char *timestamp, int blockSize);
	int discardOldSegments(TreeDescriptor treeId, int nid, int64_t timestamp);
	int discardFirstSegment(TreeDescriptor treeId, int nid);
	int discardData(TreeDescriptor treeId, int nid);

	void *setCallback(TreeDescriptor treeId, int nid, void *argument, void (*callback)(int, void *));   //Associate a callback with the nid
	int clearCallback(TreeDescriptor treeId, int nid, char *callbackDescr);	 //Remove the callback from the nid
//	void callCallback(TreeDescriptor treeId, int nid);							 //Call all callbacks associated with the nid
//	void *allocateShared(int size) {return freeSpaceManager.allocateShared(size); }
//	void freeShared(char *addr, int size) {freeSpaceManager.freeShared(addr, size); }

//Cache coerency methods
	void getCoherencyInfo(TreeDescriptor treeId, int nid, bool &isOwner, int &ownerIdx, bool &isWarm, bool &isDirty, int &timestamp);
	void getCoherencyInfo(TreeDescriptor treeId, int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm, char *&readerList, int &numReaders);
	void addReader(TreeDescriptor treeId, int nid, int readerIdx);
	void addWarm(TreeDescriptor treeId, int nid, int readerIdx);
	void setOwner(TreeDescriptor treeId, int nid, int ownerIdx, int timestamp);	
	void setCoherencyInfo(TreeDescriptor treeId, int nid, bool isOwner, int ownerIdx, bool isWarm, int timestamp,
		char *warmNodes, int numWarmNodes, char *readerNodes, int numReaderNodes);
	void setWarm(TreeDescriptor treeId, int nid, bool warm);
	bool isWarm(TreeDescriptor treeId, int nid);
	void setDirty(TreeDescriptor treeId, int nid, bool isDirty);
	SemEvent *getDataEvent(TreeDescriptor treeId, int nid);

	int getSerializedSize(TreeDescriptor treeId, int nid);
	void getSerialized(TreeDescriptor treeId, int nid, char *serialized);
	void setSerializedData(TreeDescriptor treeId, int nid, char *serializedData, int dataLen);
};

#endif	
