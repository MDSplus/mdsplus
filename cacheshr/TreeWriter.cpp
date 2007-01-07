
extern "C" int putRecordInternal(int nid, char dataType, int numSamples, char *data);
extern "C" int beginSegmentInternal(int nid, int idx, char *start, char *end, char *dim, char *data);
extern "C" int updateSegmentInternal(int nid, int idx, char *start, char *end);
extern "C" int putSegmentInternal(int nid, int idx, char *dim, char *data);

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
	worker.start(this, 0);
}

void TreeWriter::addPutRecord(int nid)
{
	addNid(nid, 0, TREEWRITER_PUT_RECORD);
}

void TreeWriter::addBeginSegment(int nid, int idx)
{
	addNid(nid, idx, TREEWRITER_BEGIN_SEGMENT);
}

void TreeWriter::addUpdateSegment(int nid, int idx)
{
	addNid(nid, idx, TREEWRITER_UPDATE_SEGMENT);
}

void TreeWriter::addPutSegment(int nid, int idx)
{
	addNid(nid, idx, TREEWRITER_PUT_SEGMENT);
}

void TreeWriter::addNid(int nid, int idx, char mode)
{
	NidHolder *newNid;
	lock.lock();

	newNid = new NidHolder();
	newNid->nxt = nidHead;
	newNid->nid = nid;
	newNid->idx = idx;
	newNid->mode = mode;
	nidHead = newNid;
	lock.unlock();
	nidEvent.signal();
}

void TreeWriter::run(void *arg)
{
	int writeNid, writeIdx, writeMode;
	char dataType;
	int numSamples;
	bool timestamped;
	while(1)
	{
		if(!nidHead)
			nidEvent.wait();
		lock.lock();
		if(nidHead)
		{
			writeNid = nidHead->nid;
			writeIdx = nidHead->idx;
			writeMode = nidHead->mode;
			NidHolder *newHead = nidHead->nxt;
			delete nidHead;
			nidHead = newHead;
			lock.unlock();	
			
			char *data, *start, *end, *dim, *shape;
			int dataSize, startSize, endSize, dimSize, shapeSize;
			int status, currDataSize;

			switch(writeMode) {
				case TREEWRITER_PUT_RECORD:
					status = dataManager->getData(writeNid, &dataType, &numSamples, &data, &dataSize);
					if(status &1 )status = putRecordInternal(writeNid, dataType, numSamples, data);
					break;
				case TREEWRITER_BEGIN_SEGMENT:
					status = dataManager->getSegmentLimits(writeNid, writeIdx, &start, &startSize, &end, &endSize, &timestamped);
					if(status & 1) dataManager->getSegmentData(writeNid, writeIdx, &dim, &dimSize, 
						&data, &dataSize, &shape, &shapeSize, &currDataSize, &timestamped);
					if(status & 1) status = beginSegmentInternal(writeNid, writeIdx, start, end, dim, data);
					break;
				case TREEWRITER_UPDATE_SEGMENT:
					status = dataManager->getSegmentLimits(writeNid, writeIdx, &start, &startSize, &end, &endSize, &timestamped);
					if(status & 1) status = updateSegmentInternal(writeNid, writeIdx, start, end);
					break;
				case TREEWRITER_PUT_SEGMENT:
					status = dataManager->getSegmentData(writeNid, writeIdx, &dim, &dimSize, 
						&data, &dataSize, &shape, &shapeSize, &currDataSize, &timestamped);
					if(status & 1) status = putSegmentInternal(writeNid, writeIdx, dim, data);
					break;
			}
		}
	}
}
