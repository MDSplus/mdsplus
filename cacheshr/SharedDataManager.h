
#ifndef SHARED_DATA_MANAGER_H
#define SHARED_DATA_MANAGER_H
#include "SharedMemNode.h"
#include "FreeSpaceManager.h"
#include "LockManager.h"
#include "SharedMemManager.h"
#include "SharedMemTree.h"
#include "CallbackManager.h"
#include "Event.h"
#include "Thread.h"

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
	SharedMemNodeData *getNodeData(int treeId, int nid, bool create);

public:
    static char *startAddress; //Initial address of the associetd Memory Zone
	static int size;		//The size in bytes of the associated Shared Memory Zone
	static FreeSpaceManager freeSpaceManager; //The manager of free space
	static SharedMemManager sharedMemManager; //The manager of shared memory allocation
	static LockManager lock;   //Global lock, used when managing nodes
	static SharedMemTree sharedTree; //ID tree
	static bool initialized;

public:
	
	SharedDataManager(int size = DEFAULT_SIZE);
	FreeSpaceManager *getFreeSpaceManager() { return &freeSpaceManager;}
	

	int setData(int treeId, int nid, char dataType, int numSamples, char *data, int size); //Write data indexed by nid
	int getData(int treeId, int nid, char *dataType, int *numSamples, char **data, int *size); //Read data indexed by nid
	int beginSegment(int treeId, int nid, int idx, char *start, int startSize, char *end, int endSize, 
		char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, int *retIdx);
	int beginTimestampedSegment(int treeId, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, int dataSize, 
		_int64 start, _int64 end, char *dim, int dimSize, int *retIdx);

	int isSegmented(int treeId, int nid, int *segmented);
	int getNumSegments(int treeId, int nid, int *numSegments);
	int updateSegment(int treeId, int nid, int idx, char *start, int startLen, char *end, int endLen, char *dim, int dimLen);
	int getSegmentLimits(int treeId, int nid, int idx, char **start, int *startSize, char **end, int *endSize, bool *timestamped);
	int getSegmentInfo(int treeIdx, int nid, int **shape, int *shapeSize, int *currDataSize);
	int getSegmentData(int treeId, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize, char **shape, 
		int *shapeSize, int *currDataSize, bool *timestamped, int *actSamples);
	int getSegmentDataAndShapeCopy(int treeId, int nid, int idx, char **data, int *dataSize, char **shape, 
		int *shapeSize);
	int appendSegmentData(int treeId, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, bool isTimestamped, 
										 _int64 *timestamps, int numTimestamps, int *segmentFilled, int *retIdx);
	int appendRow(int treeId, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int *segmentFilled, int *retIdx, bool *segmentCreated);
	int appendRow(int treeId, int nid, int *bounds, int boundsSize, char *data, int dataSize, char *timestamp, int blockSize);
	int discardOldSegments(int treeId, int nid, _int64 timestamp);
	int discardFirstSegment(int treeId, int nid);
	int discardData(int treeId, int nid);

	void *setCallback(int treeId, int nid, void (*callback)(int));   //Associate a callback with the nid
	int clearCallback(int treeId, int nid, char *callbackDescr);	 //Remove the callback from the nid
//	void callCallback(int treeId, int nid);							 //Call all callbacks associated with the nid
//	void *allocateShared(int size) {return freeSpaceManager.allocateShared(size); }
//	void freeShared(char *addr, int size) {freeSpaceManager.freeShared(addr, size); }

//Cache coerency methods
	void getCoherencyInfo(int treeId, int nid, bool &isOwner, int &ownerIdx, bool &isWarm, bool &isDirty, int &timestamp);
	void getCoherencyInfo(int treeId, int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm, char *&readerList, int &numReaders);
	void addReader(int treeId, int nid, int readerIdx);
	void addWarm(int treeId, int nid, int readerIdx);
	void setOwner(int treeId, int nid, int ownerIdx, int timestamp);	
	void setCoherencyInfo(int treeId, int nid, bool isOwner, int ownerIdx, bool isWarm, int timestamp,
		char *warmNodes, int numWarmNodes, char *readerNodes, int numReaderNodes);
	void setWarm(int treeId, int nid, bool warm);
	bool isWarm(int treeId, int nid);
	void setDirty(int treeId, int nid, bool isDirty);
	Event *getDataEvent(int treeId, int nid);

	int getSerializedSize(int treeId, int nid);
	void getSerialized(int treeId, int nid, char *serialized);
	void setSerializedData(int treeId, int nid, char *serializedData, int dataLen);
};

#endif	
