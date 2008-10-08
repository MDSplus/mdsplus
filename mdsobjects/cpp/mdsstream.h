#ifndef MDSSTREAMS_H
#define MDSSTREAMS_H
#include "mdsdata.h"

using namespace MDSobjects;
class DataStreamConsumer
{
public:
	virtual void acceptSegment(ArrayData *data, Data *start, Data *end, Data *times) = 0;
	virtual void acceptRow(Data *data, _int64 time, bool isLast = false) = 0;
};

class DataStreamProducer
{
public:
	virtual void addDataStreamConsumer(DataStreamConsumer *consumer) = 0;
	virtual void removeDataStreamConsumer(DataStreamConsumer *consumer) = 0;
};

#endif