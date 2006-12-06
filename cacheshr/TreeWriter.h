#ifndef TREE_WRITER_H
#define TREE_WRITER_H

#include "LockManager.h"
#include "SharedDataManager.h"
#include "Thread.h"
#include "Event.h"

#define TREEWRITER_PUT_RECORD 1
#define TREEWRITER_BEGIN_SEGMENT 2
#define TREEWRITER_PUT_SEGMENT 3
#define TREEWRITER_UPDATE_SEGMENT 4

struct NidHolder
{
	int nid;
	int idx;
	char mode;
	struct NidHolder *nxt;
};


class TreeWriter:Runnable
{
	LockManager lock;
	NidHolder *nidHead;
	Thread worker;
	Event nidEvent;
	SharedDataManager *dataManager;

public:
	TreeWriter();
	void setDataManager(SharedDataManager *dataManager);
	void start();
	void addPutRecord(int nid);
	void addBeginSegment(int nid, int idx);
	void addUpdateSegment(int nid, int idx);
	void addPutSegment(int nid, int idx);
	void addNid(int nid, int idx, char mode);
	void run();
};
#endif
