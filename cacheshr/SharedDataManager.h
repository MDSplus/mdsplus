//Class SharedMemInfo keeps shared zone - related information. It is  stored in shared memory at the beginning
//of the corresponding shared memory zone. An instance of SharedMemInfo keeps the root of a tree of SharedMemNode instances,
//each containing:
// - the address in the shared zone of the corresponding element (i.e. the address of a SharedMemObject instance)
// - the address of the first notify element (an instance of class NotifyUpdate), still allocated in the initial part of tthe shared zone
// - the unique identifier of the corresponding SaredMemObject
// - the color of the node (red/black)
// - the left and right child references. SharedNemNode instances are organized as a Red/Black search tree.
// - the ID of the lock for that data item
// - the head of the list of callback descriptors (instances of CallbackInfo) 

//The class handles also the holes in the allocated part. Whenever a new dataitem has to be allocated, first an hole of the same dimension
//or slightly larger is searched. If not found, the data item is taken from the free part of the Shared memory Zone.
//Each hole is described by an instance of HoleInfo. HoleInfo instances are organized as a red/black tree ordered
//by hole size.

#ifndef SHARED_DATA_MANAGER_H
#define SHARED_DATA_MANAGER_H
#include "SharedMemNode.h"
#include "FreeSpaceManager.h"
#include "LockManager.h"
#include "SharedMemManager.h"
#include "SharedMemTree.h"
#include "CallbackManager.h"
//#include "DataSlot.h"
#include <string.h>
#include <stdlib.h>

#define NOT_A_NATIVE_TYPE 2
#define BAD_SHAPE 4
#define BAD_INDEX 6
#define TRUNCATED 8
#define BAD_TYPE 10





#define DEFAULT_SIZE 500000

class SharedDataManager
{
private:
	SharedMemNodeData *getNodeData(int nid, bool create);

public:
    static void *startAddress; //Initial address of the associetd Memory Zone
	static int size;		//The size in bytes of the associated Shared Memory Zone
	static FreeSpaceManager freeSpaceManager; //The manager of free space
	static SharedMemManager sharedMemManager; //The manager of shared memory allocation
	static LockManager lock;   //Global lock, used wehen managing nodes
	static SharedMemTree sharedTree; //ID tree
	static bool initialized;

public:
	
	SharedDataManager(int size = DEFAULT_SIZE);
	FreeSpaceManager *getFreeSpaceManager() { return &freeSpaceManager;}
	

	int deleteData(int nid);
	void deleteData(SharedMemNodeData *nodeData);
	int setData(int nid, char *data, int size); //Write data indexed by nid
	int getData(int nid, char **data, int *size); //Read data indexed by nid
	int beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
									char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize);

	int isSegmented(int nid, int *segmented);
	int getNumSegments(int nid, int *numSegments);
	int updateSegment(int nid, int idx, char *start, int startLen, char *end, int endLen, char *dim, int dimLen);
	int getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize);
	int getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
		int *shapeSize, int *currDataSize);
	int appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx);

	void *setCallback(int nid, void (*callback)(int));   //Associate a callback with the nid
	int clearCallback(int nid, void *callbackDescr);	 //Remove the callback from the nid
	void callCallback(int nid);							 //Call all callbacks associated with the nid
	void append(int nid, char *data, int size);			 //Append a new data item
	void *allocateShared(int size) {return freeSpaceManager.allocateShared(size); }
	void freeShared(char *addr, int size) {freeSpaceManager.freeShared(addr, size); }

//Cache coerency methods
	void getCoherencyInfo(int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp);
	void getCoherencyInfo(int nid, bool &isOwner, int &ownerIdx, bool &isWarm, int &timestamp, 
		char * &warmList, int &numWarm);
	void setOwner(int nid, int ownerIdx, int timestamp);	
	void setCoherencyInfo(int nid, bool isOwner, int ownerIdx, bool isWarm, int timestamp, char *warmList, int numWarm);
	void setWarm(int nid, bool warm);
	bool isWarm(int nid);

};

#endif	
