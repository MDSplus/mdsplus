#ifndef SEGMENT_H
#define SEGMENT_H
#include "FreeSpaceManager.h"
static	void swapBytes(char *buf, int size);

class Segment
{
	bool timestamped;
	long data;
	int dataSize;
	long shape;
	int shapeSize;
	long start;
	int startSize;
	long end;
	int endSize;
	long dim;
	int dimSize;
	int dimOfs;
	int currDataSize;
	long nxt;



public:
	void initialize()
	{
		timestamped = false;
		shape = 0;
		start = 0;
		end = 0;
		dim = 0;
		data = 0;
		startSize = 0;
		endSize = 0;
		dimSize = 0;
		dimOfs = 0;
		shapeSize = 0;
		dataSize = 0;
		currDataSize = 0;
		nxt = -(long)this;
	}

	void free(FreeSpaceManager *fsm)
	{
		char *currPtr;
		int currSize;
		if(startSize > 0)
		{
			getStart(&currPtr, &currSize);
			fsm->freeShared((char *)currPtr, currSize);
		}
		if(endSize > 0)
		{
			getEnd(&currPtr, &currSize);
			fsm->freeShared((char *)currPtr, currSize);
		}
		if(shapeSize > 0)
		{
			getShape(&currPtr, &currSize);
			fsm->freeShared((char *)currPtr, currSize);
		}
		if(dataSize > 0)
		{
			getData(&currPtr, &currSize);
			fsm->freeShared((char *)currPtr, currSize);
		}
		if(dimSize > 0)
		{
			getDim(&currPtr, &currSize);
			fsm->freeShared((char *)currPtr, currSize);
		}
	}


	void setNext(Segment *segment)
	{
		this->nxt = (long)segment - (long)this;
	}


	Segment *getNext()
	{
		return (Segment *)((long)this + nxt);
	}

	void setData(char *data, int dataSize)
	{
		this->data = (long)data - (long)this;
		this->dataSize = dataSize;
		currDataSize = 0;
	}

	void appendTimestamp(char *timestamp)
	{
		memcpy((char *)((long)this + this->dim+dimOfs), timestamp, 8);
		dimOfs += 8;
	}
	void getData(char **data, int *dataSize)
	{
		*data = (char *)((long)this + this->data);
		*dataSize = this->dataSize;
	}

	int getCurrDataSize() {return currDataSize;}
	void setCurrDataSize(int currDataSize)
	{
		this->currDataSize = currDataSize;
	}

	void setDim(char *dim, int dimSize)
	{
		this->dim = (long)dim - (long)this;
		this->dimSize = dimSize;
	}


	void getDim(char **dim, int *dimSize)
	{
		*dim = (char *)((long)this + this->dim);
		*dimSize = this->dimSize;
	}

	void setShape(char *shape, int shapeSize)
	{
		this->shape = (long)shape - (long)this;
		this->shapeSize = shapeSize;
	}


	void getShape(char **shape, int *shapeSize)
	{
		*shape = (char *)((long)this + this->shape);
		*shapeSize = this->shapeSize;
	}

	void setStart(char *start, int startSize)
	{
		this->start = (long)start - (long)this;
		this->startSize = startSize;
	}

	void getStart(char **start, int *startSize)
	{
		*start = (char *)((long)this + this->start);
		*startSize = this->startSize;
	}

	void setEnd(char *end, int endSize)
	{
		this->end = (long)end - (long)this;
		this->endSize = endSize;
	}

	void getEnd(char **end, int *endSize)
	{
		*end = (char *)((long)this + this->end);
		*endSize = this->endSize;
	}
	void setTimestamped(bool timestamped)
	{
	    this->timestamped = timestamped;
	}
	bool isTimestamped()
	{
	    return timestamped;
	}

//Serialization protocol
//	1) One (two bytes) for endianity mismatch detection
//	2) Timestamped (one byte)
//	3) Data size (longword) + data
//	4) Shape size (longword) + shape
//	5) Start size (longword) + start
//	6) End size (longword) + end
//	7) Dim size (longword) + dim

	int getSerializedSize()
	{
		return 2 + 1 + 4 + dataSize + 4 + shapeSize + 4 + startSize + 4 + endSize + 4 + dimSize;
	}

	void getSerialized(char *serialized)
	{
		char *currPtr;
		*(short *)serialized = (short)1;
		serialized[2] = (timestamped)?1:0;
		*(int *)&serialized[3] = dataSize;
		getData(&currPtr, (int *)&serialized[2 + 1]);
		memcpy(&serialized[2 + 1 + 4], currPtr, dataSize);
		getShape(&currPtr, (int *)&serialized[2 + 1 + 4 + dataSize]);
		memcpy(&serialized[2 + 1 + 4 + dataSize + 4], currPtr, shapeSize);
		getStart(&currPtr, (int *)&serialized[2 + 1 + 4 + dataSize + 4 + shapeSize]);
		memcpy(&serialized[2 + 1 + 4 + dataSize + 4 + shapeSize + 4], currPtr, startSize);
		getEnd(&currPtr, (int *)&serialized[2 + 1 + 4 + dataSize + 4 + shapeSize + 4 + startSize]);
		memcpy(&serialized[2 + 1 + 4 + dataSize + 4 + shapeSize + 4 + startSize + 4], currPtr, endSize);
		getDim(&currPtr, (int *)&serialized[2 + 1 + 4 + dataSize + 4 + shapeSize + 4 + startSize + 4 + endSize]);
		memcpy(&serialized[2 + 1 + 4 + dataSize + 4 + shapeSize + 4 + startSize + 4 + endSize + 4], currPtr, dimSize);
	}

	void initialize(char *serialized, FreeSpaceManager *fsm)
	{
		char *currPtr;
		bool forceConversion = ((*(short *)serialized) != 1);
		timestamped = serialized[2]?true:false;
		if(forceConversion)
			swapBytes(&serialized[3], 4);
		dataSize = *(int *)&serialized[3];
		currPtr = fsm->allocateShared(dataSize);
		memcpy(currPtr, &serialized[3+4], dataSize);		
		setData(currPtr, dataSize);

		if(forceConversion)
			swapBytes(&serialized[3 + 4 + dataSize], 4);
		shapeSize = *(int *)&serialized[3 + 4 + dataSize];
		currPtr = fsm->allocateShared(shapeSize);
		memcpy(currPtr, &serialized[3+4 + dataSize + 4], shapeSize);		
		setShape(currPtr, shapeSize);
		for(int i = 0; i < shapeSize/4; i++)
			swapBytes((char *)&((int *)shape)[i], 4);

		if(forceConversion)
			swapBytes(&serialized[3 + 4 + dataSize + 4 + shapeSize], 4);
		startSize = *(int *)&serialized[3 + 4 + dataSize + 4 + shapeSize];
		currPtr = fsm->allocateShared(startSize);
		memcpy(currPtr, &serialized[3+4 + dataSize + 4 + shapeSize + 4], startSize);		
		setStart(currPtr, startSize);

		if(forceConversion)
			swapBytes(&serialized[3 + 4 + dataSize + 4 + shapeSize + 4 + startSize], 4);
		endSize = *(int *)&serialized[3 + 4 + dataSize + 4 + shapeSize + 4 + startSize];
		currPtr = fsm->allocateShared(endSize);
		memcpy(currPtr, &serialized[3+4 + dataSize + 4 + shapeSize + 4 + startSize + 4], startSize);		
//Timestamped segments record direct 8 - byte timestamp for end time
		if(timestamped)
			swapBytes((char *)currPtr, 8);
		setEnd(currPtr, endSize);

		if(forceConversion)
			swapBytes(&serialized[3 + 4 + dataSize + 4 + shapeSize + 4 + startSize + 4 + endSize], 4);
		dimSize = *(int *)&serialized[3 + 4 + dataSize + 4 + shapeSize + 4 + startSize + 4 + endSize];
		currPtr = fsm->allocateShared(dimSize);
		memcpy(currPtr, &serialized[3+4 + dataSize + 4 + shapeSize + 4 + startSize + 4 + endSize + 4], dimSize);		
//Timestamped segments record direct 8 - byte timestamp array for dimension
		if(timestamped)
		{
			for(int i = 0; i < dimSize/8; i++)
				swapBytes((char *)currPtr + 8 * i, 8);
		}
		setDim(currPtr, dimSize);
	}

};


//Common utility routine
static	void swapBytes(char *buf, int size)
	{
		char bufChar;
		short bufShort;
		int bufInt;
		switch(size)  {
			case 2: 
				bufChar = buf[0];
				buf[0] = buf[1];
				buf[1] = bufChar;
				return;
			case 4:
				bufShort = ((short *)buf)[0];
				((short *)buf)[0] = ((short *)buf)[1];
				((short *)buf)[1] = bufShort;
				swapBytes(buf, 2);
				swapBytes(&buf[2], 2);
				return;
			case 8:
				bufInt = ((int *)buf)[0];
				((int *)buf)[0] = ((int *)buf)[1];
				((int *)buf)[1] = bufInt;
				swapBytes(buf, 4);
				swapBytes(&buf[4], 4);
				return;
		}
	}



#endif
