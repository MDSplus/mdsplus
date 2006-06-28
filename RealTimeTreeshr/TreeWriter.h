#ifndef TREE_WRITER_H
#define TREE_WRITER_H

#include "LockManager.h"
#include "SharedDataManager.h"
#include "Thread.h"
#include "Event.h"


struct NidHolder
{
	int nid;
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
	void addNid(int nid);
	void run();
};
#endif