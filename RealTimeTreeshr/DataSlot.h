#ifndef DATA_SLOT_H
#define DATA_SLOT_H
#include "FreeSpaceManager.h"
#include "AbsoluteTime.h"

class DataSlot
{
	long data;
	int dataSize;
	long nxt;
	long prv;
	long dim;
	int dimSize;

public:
	void initialize()
	{
		data = 0;
		dataSize = 0;
		nxt = -(long)this;
		prv = -(long)this;
		dim = 0;
		dimSize = 0;
	}

	void setNext(DataSlot *nxt)
	{
		this->nxt = (long)nxt - (long)this;
	}

	void setPrev(DataSlot *prv)
	{
		this->prv = (long)prv - (long)this;
	}

	DataSlot *getNext()
	{
		return (DataSlot *)((long)this + nxt);
	}

	DataSlot *getPrv()
	{
		return (DataSlot *)((long)this + prv);
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

	void setData(void *data, int dataSize)
	{
		this->data = (long)data - (long)this;
		this->dataSize = dataSize;
	}

	void getData(void **data, int *dataSize)
	{
		*data = (void *)((long)this + this->data);
		*dataSize = this->dataSize;
	}

};


#endif