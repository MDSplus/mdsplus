#ifndef TREE_WRITER_H
#define TREE_WRITER_H

#include "LockManager.h"
#include "SharedDataManager.h"
#include "Thread.h"
#include "Event.h"

#define TREEWRITER_PUT_RECORD 1
#define TREEWRITER_PUT_SEGMENT 2
#define TREEWRITER_PUT_TIMESTAMPED_SEGMENT 3
#define TREEWRITER_PUT_SEGMENT_DISCARD 4
#define TREEWRITER_PUT_TIMESTAMPED_SEGMENT_DISCARD 5

struct NidHolder
{
	int treeIdx;
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
	void addPutRecord(int treeIdx, int nid);
	void addPutSegment(int treeIdx, int nid, int idx, int discard);
	void addPutTimestampedSegment(int treeIdx, int nid, int idx, int discard);
	void addNid(int treeIdx, int nid, int idx, char mode);
	void run(void *arg);
};

#endif
