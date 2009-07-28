
extern "C" int deleteRecordInternal(char *name, int shot, int nid);
extern "C" int putRecordInternal(char *name, int shot, int nid, char dataType, int numSamples, char *data);
extern "C" int putSegmentInternal(char *name, int shot, int nid, char *start, int startSize, char *end, int endSize, 
					   char *dim, int dimSize, char *data, int dataSize, int *shape, int shapeSize, int currDataSize,
					   int isTimestamped, int actSamples, int updateOnly);
#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

#include "TreeWriter.h"

TreeWriter::TreeWriter()
{
	lock.initialize();
	nidHead = 0;
	synchWaiting = false;
	workerWaiting = false;
	working = false;
}


void TreeWriter::setDataManager(SharedDataManager *dataManager)
{
	this->dataManager = dataManager;
}


void TreeWriter::start()
{
	worker.start(this, 0);
}

void TreeWriter::addPutRecord(TreeDescriptor treeIdx, int nid)
{
	addNid(treeIdx, nid, 0, TREEWRITER_PUT_RECORD);
}

void TreeWriter::addDelete(TreeDescriptor treeIdx, int nid)
{
	addNid(treeIdx, nid, 0, TREEWRITER_DELETE_RECORD);
}

void TreeWriter::addPutSegment(TreeDescriptor treeIdx, int nid, int idx, int discard)
{
	if(discard)
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_SEGMENT_DISCARD);
	else
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_SEGMENT);
}

void TreeWriter::addPutTimestampedSegment(TreeDescriptor treeIdx, int nid, int idx, int discard)
{
	if(discard)
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_TIMESTAMPED_SEGMENT_DISCARD);
	else
		addNid(treeIdx, nid, idx, TREEWRITER_PUT_TIMESTAMPED_SEGMENT);
}

void TreeWriter::addNid(TreeDescriptor treeIdx, int nid, int idx, char mode)
{
	NidHolder *newNid;
	lock.lock();
//printf("ADD NID %d\n", treeIdx.getShot());
	
	newNid = new NidHolder();
	newNid->treeIdx = treeIdx;
	newNid->nid = nid;
	newNid->idx = idx;
	newNid->mode = mode;
	newNid->nxt = 0;
	if(!nidHead)
		nidHead = newNid;
	else
	{
		NidHolder *currNid = nidHead;
		while(currNid->nxt)
			currNid = currNid->nxt;
		currNid->nxt = newNid;
	}

	if(workerWaiting)
	{
		nidEvent.signal();
		workerWaiting = false;
	}
	lock.unlock();
}

void TreeWriter::synch()
{
	lock.lock();
	if(nidHead || working)
		synchWaiting = true;
	lock.unlock();
	if(synchWaiting)
	{
		synchEvent.wait();
	}
}


void TreeWriter::run(void *arg)
{
	int writeNid, writeMode, writeIdx;
	TreeDescriptor writeTreeIdx;
	char dataType;
	int numSamples;
	int count = 0;
	while(1)
	{
		lock.lock();
		if(!nidHead)
		{
			workerWaiting = true;
			lock.unlock();
			nidEvent.wait();
			lock.lock();
		}
		while(nidHead)
		{
			
			writeNid = nidHead->nid;
			writeTreeIdx = nidHead->treeIdx;
			writeIdx = nidHead->idx;
			writeMode = nidHead->mode;
			NidHolder *newHead = nidHead->nxt;
			delete nidHead;
			nidHead = newHead;
			//printf("TREE WRITER %d\n", count++);
			working = true;
			lock.unlock();	
			
			char *data, *start, *end, *dim;
			int *shape;
			int dataSize, startSize, endSize, dimSize, shapeSize;
			int status, currDataSize, actSamples;
			bool isTimestamped;

			switch(writeMode) {
				case TREEWRITER_DELETE_RECORD:
//printf("DELETE RECORD %d\n", writeTreeIdx.getShot());
					deleteRecordInternal(writeTreeIdx.getName(), writeTreeIdx.getShot(), writeNid);
					break;
				case TREEWRITER_PUT_RECORD:
//printf("PUT RECORD %d\n", writeTreeIdx.getShot());
					status = dataManager->getData(writeTreeIdx, writeNid, &dataType, &numSamples, &data, &dataSize);
					if(status &1 )status = putRecordInternal(writeTreeIdx.getName(), writeTreeIdx.getShot(), writeNid, dataType, numSamples, data);
					break;
				case TREEWRITER_PUT_SEGMENT:
				
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, writeIdx, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, writeIdx, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeTreeIdx.getName(), writeTreeIdx.getShot(), writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 
						currDataSize, 0, 0, 0);
					break;
				case TREEWRITER_PUT_SEGMENT_DISCARD:
//printf("PUT SEGMENT DISCARD\n");

					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, 0, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, 0, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeTreeIdx.getName(), writeTreeIdx.getShot(), writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 
						currDataSize, 0, 0, 0);
					if(status & 1) dataManager->discardFirstSegment(writeTreeIdx, writeNid);
					break;

				case TREEWRITER_PUT_TIMESTAMPED_SEGMENT:
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, writeIdx, &start, &startSize, 
						&end, &endSize, &isTimestamped);
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, writeIdx, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
					if(status &1 )status = putSegmentInternal(writeTreeIdx.getName(), writeTreeIdx.getShot(), writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 
						currDataSize, 1, actSamples, 0);
					break;
				case TREEWRITER_PUT_TIMESTAMPED_SEGMENT_DISCARD:
//printf("PUT TIMESTAMPED SEGMENT DISCARD %d\n", writeTreeIdx.getShot());
				//printf("PUT TIMESTAMP SEGMENT DISCARD\n");
					status = dataManager->getSegmentLimits(writeTreeIdx, writeNid, 0, &start, &startSize, 
						&end, &endSize, &isTimestamped);
				//printf("PUT TIMESTAMP SEGMENT DISCARD 1\n");
					status = dataManager->getSegmentData(writeTreeIdx, writeNid, 0, &dim, &dimSize, 
						&data, &dataSize, (char **)&shape, &shapeSize, &currDataSize, &isTimestamped, &actSamples);
				//printf("PUT TIMESTAMPED SEGMENT DISCARD 2 %f\n", *(float *)data);
					if(status &1 )status = putSegmentInternal(writeTreeIdx.getName(), writeTreeIdx.getShot(), writeNid, 
						start, startSize, end, endSize, dim, dimSize, data, dataSize, shape, shapeSize, 
						currDataSize, 1, actSamples, 0);
				//printf("PUT TIMESTAMP SEGMENT DISCARD 3\n");
					if(status & 1) dataManager->discardFirstSegment(writeTreeIdx, writeNid);
				//printf("PUT TIMESTAMP SEGMENT DISCARD 4\n");
					break;
			}
			working = false;
			lock.lock();
			if(!nidHead && synchWaiting)
			{
				synchEvent.signal();
			}
			lock.unlock();
		}
	}
}
