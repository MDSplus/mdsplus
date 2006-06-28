#ifndef CACHE_H
#define CACHE_H

#include "SharedDataManager.h"
#include "TreeWriter.h"

struct NidChain
{
	int nid;
	NidChain *nxt;
};



class Cache
{
	SharedDataManager dataManager;
	TreeWriter treeWriter;
	NidChain *chainHead;

public:
	Cache();
	int putRecord(int nid, char *data, int size, int writeThrough);
	int getRecord(int nid, char **data, int *size);
	int flush();

	void * setCallback(int nid, void (* callback)(int));
	void clearCallback(int nid, void *callbackManager);

	int setCurrDim(int nid, char *dim, int dimSize);
	int appendRecord(int nid, char *data, int size);
	int getRecordInfo(int nid, int *numItems, int *isTimed);
	int getRecords(int nid, char **data, int *sizes, char **dims, int *dimSizes);
	void setWarm(int nid, bool warm);

	void startServer();
};

#endif
