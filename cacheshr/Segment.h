#ifndef SEGMENT_H
#define SEGMENT_H

class Segment
{
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
	int currDataSize;
	long nxt;

public:
	void initialize()
	{
		shape = 0;
		start = 0;
		end = 0;
		dim = 0;
		data = 0;
		startSize = 0;
		endSize = 0;
		dimSize = 0;
		shapeSize = 0;
		dataSize = 0;
		currDataSize = 0;
		nxt = -(long)this;
	}

	void setNext(Segment *segment)
	{
		this->nxt = (long)segment - (long)this;
	}


	Segment *getNext()
	{
		return (Segment *)((long)this + nxt);
	}

	void setData(void *data, int dataSize)
	{
		this->data = (long)data - (long)this;
		this->dataSize = dataSize;
		currDataSize = 0;
	}

	void getData(void **data, int *dataSize)
	{
		*data = (void *)((long)this + this->data);
		*dataSize = this->dataSize;
	}

	int getCurrDataSize() {return currDataSize;}
	void setCurrDataSize(int currDataSize)
	{
		this->currDataSize = currDataSize;
	}

	void setDim(void *dim, int dimSize)
	{
		this->dim = (long)dim - (long)this;
		this->dimSize = dimSize;
	}


	void getDim(void **dim, int *dimSize)
	{
		*dim = (void *)((long)this + this->dim);
		*dimSize = this->dimSize;
	}

	void setShape(void *shape, int shapeSize)
	{
		this->shape = (long)shape - (long)this;
		this->shapeSize = shapeSize;
	}


	void getShape(void **shape, int *shapeSize)
	{
		*shape = (void *)((long)this + this->shape);
		*shapeSize = this->shapeSize;
	}

	void setStart(void *start, int startSize)
	{
		this->start = (long)start - (long)this;
		this->startSize = startSize;
	}

	void getStart(void **start, int *startSize)
	{
		*start = (void *)((long)this + this->start);
		*startSize = this->startSize;
	}

	void setEnd(void *end, int endSize)
	{
		this->end = (long)end - (long)this;
		this->endSize = endSize;
	}

	void getEnd(void **end, int *endSize)
	{
		*end = (void *)((long)this + this->end);
		*endSize = this->endSize;
	}

};


#endif
