/*struct NidHolder
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

public:
	TreeWriter();
	void start();
	void addNid(int nid, char *data, int size);
};
*/

extern "C" int putRecordInternal(int nid, char *data, int size);

#include "TreeWriter.h"

TreeWriter::TreeWriter()
{
	lock.initialize(TREE_WRITER_LOCK);
	nidHead = 0;
}


void TreeWriter::setDataManager(SharedDataManager *dataManager)
{
	this->dataManager = dataManager;
}


void TreeWriter::start()
{
	worker.start(this);
}

void TreeWriter::addNid(int nid)
{
	NidHolder *currNid, *prevNid;
	lock.lock();
	for(currNid = prevNid = nidHead; currNid && currNid->nid != nid; currNid = currNid->nxt)
		prevNid = currNid;

	if(!prevNid) //no elements in the queue
	{
		prevNid = new NidHolder();
		prevNid->nxt = 0;
		prevNid->nid = nid;
		nidHead = prevNid;
		lock.unlock();
		nidEvent.signal();
	}
	else if(!currNid) //No element with that nid in the queue
	{
		prevNid->nxt = currNid = new NidHolder();
		currNid->nid = nid;
		currNid->nxt = 0;
		lock.unlock();
		nidEvent.signal();
	}
	else
		lock.unlock();

	//Nothing to do if the nid is already in the queue
}

void TreeWriter::run()
{
	int writeNid;
	while(1)
	{
		if(!nidHead)
			nidEvent.wait();
		lock.lock();
		if(nidHead)
		{
			writeNid = nidHead->nid;
			NidHolder *newHead = nidHead->nxt;
			delete nidHead;
			nidHead = newHead;
		
			char *data;
			int size;
			int status = dataManager->getSharedData(writeNid, &data, &size);
			if(!(status & 1))
				printf("Error getting shared data!\n");
			else
			{
				printf("Scrivo %d\n", writeNid);
				status = putRecordInternal(writeNid, data, size);
				if(!(status & 1))
					printf("Error writing data in TreeWriter\n");
			}
		}
		lock.unlock();
	}
}