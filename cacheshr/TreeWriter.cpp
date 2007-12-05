
extern "C" int putRecordInternal(int nid, char dataType, int numSamples, char *data);
extern "C" int beginSegmentInternal(int nid, int idx, char *start, char *end, char *dim, char *data);
extern "C" int updateSegmentInternal(int nid, int idx, char *start, char *end);
extern "C" int putSegmentInternal(int nid, char *start, int startSize, char *end, int endSize, 
					   char *dim, int dimSize, char *data, int dataSize, int *shape, int shapeSize, int isTimestamped, int actSamples);
#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

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

void TreeWriter::addPutRecord(int treeIdx, int nid)
{
	addNid(treeIdx, nid, 0, TREEWRITER_PUT_RECORD);
}

void TreeWriter::addPutSegment(int treeIdx, int nid, int idx, int discard)
{
	if(discard)
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_SEGMENT_DISCARD);
	else
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_SEGMENT);
}

void TreeWriter::addPutTimestampedSegment(int treeIdx, int nid, int idx, int discard)
{
	if(discard)
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_TIMESTAMPED_SEGMENT_DISCARD);
	else
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_TIMESTAMPED_SEGMENT);
}

void TreeWriter::addNid(int treeIdx, int nid, int idx, char mode)
{
	NidHolder *newNid;
	lock.lock();

	newNid = new NidHolder();
	newNid->treeIdx = treeIdx;
	newNid->nid = nid;
	newNid->idx = idx;
	newNid->mode = mode;
	newNid->nxt = nidHead;
	nidHead = newNid;
	lock.unlock();
	nidEvent.signal();
}

void TreeWriter::run(void *arg)
{
	int writeNid, writeIdx, writeMode, writeTreeIdx;
	char dataType;
	int numSamples;
	while(1)
	{
		if(!nidHead)
			nidEvent.wait();
		lock.lock();
		while(nidHead)
		{
			writeNid = nidHead->nid;
			writeTreeIdx = nidHead->treeIdx;
			writeIdx = nidHead->idx;
			writeMode = nidHead->mode;
			NidHolder *newHead = nidHead->nxt;
			delete nidHead;
			nidHead = newHead;
			lock.unlock();	
			
			char *data, *start, *end, *dim;
			int *shape;
			int dataSize, startSize, endSize, dimSize, shapeSize;
			int status, currDataSize, actSamples;
			bool isTimestamped;

			switch(writeMode) {
				case TREEWRITER_PUT_RECORD:
					status = dataManager->getData(writeTreeIdx, writeNid, &dataType, &numSamples, &data, &dataSize);
					if(status &1 )status = putRecordInternal(writeNid, dataType, numSamples, data);
					break;
				case TREEWRITER_PUT_SEGMENT:
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, writeIdx, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, writeIdx, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 0, 0);
					break;
				case TREEWRITER_PUT_SEGMENT_DISCARD:
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, 0, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, 0, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 0, 0);
					if(status & 1) dataManager->discardFirstSegment(writeTreeIdx, writeNid);
					break;

				case TREEWRITER_PUT_TIMESTAMPED_SEGMENT:
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, writeIdx, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, writeIdx, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 1, actSamples);
					break;
				case TREEWRITER_PUT_TIMESTAMPED_SEGMENT_DISCARD:
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, 0, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, 0, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 1, actSamples);
					if(status & 1) dataManager->discardFirstSegment(writeTreeIdx, writeNid);
					break;
			}
		}
	}
}
