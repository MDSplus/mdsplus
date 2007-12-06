#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#include <semaphore.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <mdstypes.h>

#define NOT_A_NATIVE_TYPE 2
#define BAD_SHAPE 4
#define BAD_INDEX 6
#define TRUNCATED 8
#define BAD_TYPE 10

#define WRITE_THROUGH 1
#define WRITE_BACK 2
#define WRITE_BUFFER 3
#define WRITE_LAST 4
#define MAX_OPEN_TREES 1024

#define MAX_BOUND_SIZE 512

extern char *getCache();
extern int putRecord(int treeIdx, int nid, char dataType, int numSamples, char *data, int size, int writeMode, char *cachePtr);
extern int getRecord(int treeIdx, int nid, char *dataType, int *numSamples, char **data, int *size, char *cachePtr);
extern int putRecordInternal(int nid, char dataType, int numSamples, char *data, int size);
extern int flushTree(int treeIdx, char *cachePtr);
extern char *setCallback(int treeIdx, int nid, void (* callback)(int), char *cachePtr);
extern int clearCallback(int treeIdx, int nid, char *callbackDescr, char *cachePtr);
extern int beginSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, 
						char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, 
						int writeMode, char *cachePtr);
extern int beginTimestampedSegment(int treeIdx, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, int dataSize, 
						_int64 start, _int64 end, char *dim, int dimSize, int writeThrough, char *cachePtr);
extern int updateSegment(int treeIdx, int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, 
						 int dimSize, int writeMode, char *cachePtr);
extern int getNumSegments(int treeIdx, int nid, int *numSegments, char *cachePtr);
extern int getSegmentLimits(int treeIdx, int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *cachePtr, char *timestamped);
extern int getSegmentData(int treeIdx, int nid, int idx, char **dim, int *dimSize, char **data, 
						  int *dataSize,char **shape, int *shapeSize, int *currDataSize, char *timestamped, char *cachePtr);
extern int isSegmented(int treeIdx, int nid, int *segmented, char *cachePtr);
extern int appendSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode, char *cachePtr);
extern int appendTimestampedSegmentData(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamp, int numTimestamps, int writeMode, char *cachePtr);
extern int discardOldSegments(int treeIdx, int nid, _int64 timestamp, char *cachePtr);
extern int appendRow(int treeIdx, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int writeMode, char *cachePtr);
extern int setWarm(int treeIdx, int nid, int warm, char *cachePtr);
extern int synch(char *cachePtr);
extern int TdiCompile();
extern int TdiData();
extern int TdiEvaluate();
extern int TdiDecompile();


static struct descriptor *rebuildDataDescr(char dataType, int numSamples, char *data, int dataLen, struct descriptor *descr, 
	struct descriptor_a *descrA)
{
    if(numSamples == 1) //Scalar
    {
	descr->dtype = dataType;
	descr->length = dataLen;
	descr->class = CLASS_S;
	descr->pointer = data;
	return descr;
    }
    descrA->class = CLASS_A;
    descrA->dtype = dataType;
    descrA->length = dataLen/numSamples;
    descrA->arsize = dataLen;
    descrA->pointer = data;
    return (struct  descriptor *)descrA;
}


//Debug functions
static void printDecompiled(struct descriptor *inD)
{
	int status;
	EMPTYXD(out_xd);
	char *buf;

	status = TdiDecompile(inD, &out_xd MDS_END_ARG);
	if(!(status & 1))
	{
		printf("%s\n", MdsGetMsg(status));
		return;
	}
	buf = malloc(out_xd.pointer->length + 1);
	memcpy(buf, out_xd.pointer->pointer, out_xd.pointer->length);
	buf[out_xd.pointer->length] = 0;
	out_xd.pointer->pointer[out_xd.pointer->length - 1] = 0;
	printf("%s\n", buf);
	free(buf);
	MdsFree1Dx(&out_xd, 0);
}

static void printVarContent(char *var)	
{
	struct descriptor varD = {strlen(var),DTYPE_T, CLASS_S, var};
	EMPTYXD(varXd);
	int status;
	status = TdiCompile(&varD, &varXd MDS_END_ARG);
	if(status & 1)status = TdiData(&varXd, &varXd MDS_END_ARG);
	if(status & 1)
		printDecompiled(varXd.pointer);
	else 
		printf("%s\n", MdsGetMsg(status));
	MdsFree1Dx(&varXd, 0);
}




static char *cache;

static int shotNumbers[MAX_OPEN_TREES];
static int shotNumIdx = 0;
static int currShotNum;

static int rebuildFromDataTypeAndShape(char *inData, int *shape, int shapeSize, struct descriptor_xd *outXd)
{
//	#define DESCRIPTOR_A_BOUNDS(name, len, type, ptr, dimct, arsize) 

	int i;
	DESCRIPTOR_A_BOUNDS(arrayD, 0, 0, 0, 16, 0);
	arrayD.class = CLASS_A;
	arrayD.dtype = shape[0];
	arrayD.length = shape[1];
	arrayD.dimct = shape[2];
	arrayD.arsize = shape[3];
	for(i = 0; i < arrayD.dimct; i++)
	{
		arrayD.m[i] = shape[4+i];
	}
	arrayD.pointer = inData;
	return MdsCopyDxXd((struct descriptor *)&arrayD, outXd);
}








//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits
static void getDataTypeAndShape(struct descriptor *inData, int *outShape, int *outSize)
{
	int currDim;
	int *boundsPtr;
	struct descriptor_a *data;
	if(inData->class == CLASS_S)
	{
		outShape[0] = inData->dtype;
		outShape[1] = inData->length;
		outShape[2] = 0;
		outShape[3] = inData->length;
		*outSize = 4 * sizeof(int);
		return;
	}
	data = (struct descriptor_a *)inData;
	outShape[0] = data->dtype;
	outShape[1] = data->length;
	outShape[2] = data->dimct;
	outShape[3] = data->arsize;
	if(data->dimct == 1)
	{
		outShape[4] = data->arsize/data->length;
		*outSize = 5 * sizeof(int);
		return;
	}
	//Here, more than one dimension is defined
	boundsPtr = (int *)((char *)data + sizeof(struct descriptor_a) + sizeof(char *));
	for(currDim = 0; currDim < data->dimct; currDim++)
	{
		outShape[currDim + 4] = boundsPtr[currDim];
	}
	*outSize = (data->dimct + 4)*sizeof(int);
}


//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits

static int intBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx, 
									  _int64 start, _int64 end, struct descriptor_a *timesD, int writeMode)
{
	_int64 *times;
	int nTimes, timesSize;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	
	DESCRIPTOR_APD(dimsApd, DTYPE_DSC, 0, 0);


	if(!cache) cache = getCache();
//Only native arrays can be defined
	if(initialValue->class != CLASS_A)
		return NOT_A_NATIVE_TYPE;


	getDataTypeAndShape((struct descriptor *)initialValue, bounds, &boundsSize);
	if(timesD)
	{
		times = (_int64 *)timesD->pointer;
		nTimes = timesD->arsize/timesD->length;
		timesSize = nTimes * 8;
	}
	else
	{
		times = 0;
		timesSize = 0;
		nTimes = bounds[4+bounds[2] - 1]; //Last dimesnsion, i.e. number of time samples
	}

	return beginTimestampedSegment(currShotNum, nid, idx, nTimes, (char *)bounds, boundsSize, initialValue->pointer,
						bounds[3], start, end, (char *)times, timesSize, writeMode, cache);

}

EXPORT int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode);

//Read segmented data into cache memory
static int copySegmentedIntoCache(int nid, int *copiedSegments)
{
	int status, numSegments, currIdx;
	EMPTYXD(startXd);
	EMPTYXD(endXd);
	EMPTYXD(dataXd);
	EMPTYXD(dimXd);

	_int64 *times;
	int nTimes;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	struct descriptor_a *arrD, *timesD;

	status = TreeGetNumSegments(nid, &numSegments);
	if(!(status & 1)) return status;
	*copiedSegments = numSegments;
	for(currIdx = 0; currIdx < numSegments; currIdx++)
	{
		status = TreeGetSegmentLimits(nid, currIdx, &startXd, &endXd);
		if(!(status & 1)) return status;
		status = TreeGetSegment(nid, currIdx, &dataXd, &dimXd);
		if(!(status & 1)) return status;
		status = TdiData(&dimXd, &dimXd MDS_END_ARG);
		if(!(status & 1)) return status;

		arrD = (struct descriptor_a *)dataXd.pointer;
		getDataTypeAndShape((struct descriptor *)arrD, bounds, &boundsSize);

		if(startXd.pointer && startXd.pointer->class == CLASS_S && (startXd.pointer->dtype == DTYPE_Q || startXd.pointer->dtype == DTYPE_QU))
		{
			//Assumed to be timestamped
			timesD = (struct descriptor_a *)dimXd.pointer;
			if(timesD->dtype != DTYPE_Q && timesD->dtype != DTYPE_QU)
				return 0;
			times = (_int64*)timesD->pointer;
			nTimes = timesD->arsize/timesD->length;

			status = intBeginTimestampedSegment(nid, (struct descriptor_a *)dataXd.pointer, -1, times[0], times[nTimes - 1],
				timesD, 0);
			if(!(status & 1)) return status;
		}
		else
		{
			status = RTreeBeginSegment(nid, startXd.pointer, endXd.pointer, dimXd.pointer, (struct descriptor_a *)dataXd.pointer, 
				-1, 0);
		}
	}
	return status;
}





EXPORT int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode)
{
	EMPTYXD(startSerXd);	
	EMPTYXD(endSerXd);	
	EMPTYXD(dimSerXd);	
	struct descriptor_a *startSerA, *endSerA, *dimSerA;
	struct descriptor_signal *signalD;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	
	DESCRIPTOR_APD(dimsApd, DTYPE_DSC, 0, 0);


	if(!cache) cache = getCache();
//Only native arrays can be defined
	if(initialValue->class != CLASS_A)
		return NOT_A_NATIVE_TYPE;


	MdsSerializeDscOut(start, &startSerXd);
	MdsSerializeDscOut(end, &endSerXd);
	if(dimension->dtype == DTYPE_SIGNAL)//If a signal has been passed as dimension (to hold more than one dimension)
	{
		signalD = (struct descriptor_signal *)dimension;
		dimsApd.arsize = (signalD->ndesc-2) * sizeof(struct descriptor *);
		dimsApd.pointer = &signalD->dimensions[0];

		MdsSerializeDscOut((struct descriptor *)&dimsApd, &dimSerXd);
	}
	else //Any other expression
		MdsSerializeDscOut(dimension, &dimSerXd);

	getDataTypeAndShape((struct descriptor *)initialValue, bounds, &boundsSize);
	
	startSerA = (struct descriptor_a *)startSerXd.pointer;
	endSerA = (struct descriptor_a *)endSerXd.pointer;
	dimSerA = (struct descriptor_a *)dimSerXd.pointer;



	beginSegment(currShotNum, nid, idx, (char *)startSerA->pointer, startSerA->arsize, (char *)endSerA->pointer, endSerA->arsize, 
						(char *)dimSerA->pointer, dimSerA->arsize, (char *)bounds, boundsSize, initialValue->pointer,
						bounds[3], writeMode, cache);
	MdsFree1Dx(&startSerXd, 0);
	MdsFree1Dx(&endSerXd, 0);
	MdsFree1Dx(&dimSerXd, 0);
	return 1; 

}

EXPORT int RTreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx, int writeMode)
{
	
	return intBeginTimestampedSegment(nid, initialValue, idx, 0, 0, 0, writeMode);
}


EXPORT int RTreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, 
							  struct descriptor *dimension, int idx, int writeMode)
{
	EMPTYXD(startSerXd);	
	EMPTYXD(endSerXd);	
	EMPTYXD(dimSerXd);	
	struct descriptor_a *startSerA, *endSerA, *dimSerA;
	struct descriptor *savedData, *savedRaw;
	int status;

	if(!cache) cache = getCache();
	MdsSerializeDscOut(start, &startSerXd);
	MdsSerializeDscOut(end, &endSerXd);
	if(dimension->dtype = DTYPE_SIGNAL)//If a signal has been passed as dimension
	{
		savedData = ((struct descriptor_signal *)dimension)->data;
		savedRaw = ((struct descriptor_signal *)dimension)->raw;
		((struct descriptor_signal *)dimension)->data = 0;
		((struct descriptor_signal *)dimension)->raw = 0;
		MdsSerializeDscOut(dimension, &dimSerXd);
		((struct descriptor_signal *)dimension)->data = savedData;
		((struct descriptor_signal *)dimension)->raw = savedRaw;
	}
	else //Any other expression
		MdsSerializeDscOut(start, &dimSerXd);

	startSerA = (struct descriptor_a *)startSerXd.pointer;
	endSerA = (struct descriptor_a *)endSerXd.pointer;
	dimSerA = (struct descriptor_a *)dimSerXd.pointer;

	status = updateSegment(currShotNum, nid, idx,  (char *)startSerA->pointer, startSerA->arsize, (char *)endSerA->pointer, endSerA->arsize, 
						(char *)dimSerA->pointer, dimSerA->arsize, writeMode, cache);

	MdsFree1Dx(&startSerXd, 0);
	MdsFree1Dx(&endSerXd, 0);
	MdsFree1Dx(&dimSerXd, 0);
	return status;
}





EXPORT int RTreePutSegment(int nid, struct descriptor *dataD, int segIdx, int writeMode)
{
	int bounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;

	if(!cache) cache = getCache();

	getDataTypeAndShape(dataD, bounds, &boundsSize);
	status = appendSegmentData(currShotNum, nid, bounds, boundsSize, dataD->pointer, bounds[3], -1, segIdx,  writeMode, cache);
	return status;
}

EXPORT int RTreePutTimestampedSegment(int nid, struct descriptor *dataD, _int64 *timestamps, int writeMode)
{
	int bounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;
	if(!cache) cache = getCache();

	getDataTypeAndShape(dataD, bounds, &boundsSize);

	status = appendTimestampedSegmentData(currShotNum, nid, bounds, boundsSize, dataD->pointer, 
										 bounds[3], -1, -1, timestamps, bounds[4+bounds[2] - 1],//LastDimension
										 writeMode, cache);
	return status;
}

//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits

EXPORT int RTreePutRow(int nid, int bufSize, _int64 *timestamp, struct descriptor_a *rowD, int writeMode)
{
	int bounds[MAX_BOUND_SIZE], extendedBounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;
	char *data;
	int dataSize;
	if(!cache) cache = getCache();

	getDataTypeAndShape((struct descriptor *)rowD, bounds, &boundsSize);
	status = appendRow(currShotNum, nid, bounds, boundsSize, rowD->pointer, 
										 bounds[3], *timestamp, writeMode, cache);
	if(!(status & 1)) //This happens the first time a row is written
	{
		memcpy(extendedBounds, bounds, boundsSize);
		dataSize = bounds[3] * bufSize;
//Change bounds to describe an array of bufSize data items
		extendedBounds[2]++; //One more dimension
		extendedBounds[3] *= bufSize; //Total dimension in bytes
		extendedBounds[4+extendedBounds[2]-1] = bufSize; //Last dimension keeps the time samples
		data = malloc(dataSize);
		memset(data, 0, dataSize);
		status = beginTimestampedSegment(currShotNum, nid, -1, bufSize, (char *)extendedBounds, boundsSize+4, 
			data, dataSize, 0, 0, 0, 0, writeMode, cache);
		if(status & 1)
			status = appendRow(currShotNum, nid, bounds, boundsSize, rowD->pointer, 
										 bounds[3], *timestamp, writeMode, cache);
		free(data);
	}
	return status;
}


EXPORT int RTreeGetNumSegments(int nid, int *numSegments)
{
	int status;
	if(!cache) cache = getCache();
	status =  getNumSegments(currShotNum, nid, numSegments, cache);
	if(!(status & 1))
	{
		status = copySegmentedIntoCache(nid, numSegments);
	}
	return status;
}

EXPORT int RTreeGetSegment(int nid, int idx, struct descriptor_xd *retData, struct descriptor_xd *retDim)
{
	char *data, *dim;
	int *shape;
	char timestamped;
	int status, dataSize, shapeSize, dimSize, currDataSize;
	DESCRIPTOR_A_COEFF(arrayD, 0, 0, 0, (unsigned char )256, 0);
	int *boundsPtr;
	int currDim, copiedSegments;	
	DESCRIPTOR_A(dimD, 8, DTYPE_QU, 0, 0);


	if(!cache) cache = getCache();
	status = getSegmentData(currShotNum, nid, idx, &dim, &dimSize, &data, &dataSize, (char **)&shape, &shapeSize, 
		&currDataSize, &timestamped, cache);
	if(!(status & 1)) 
	{
		status = copySegmentedIntoCache(nid, &copiedSegments);
		if(status & 1)
			status = getSegmentData(currShotNum, nid, idx, &dim, &dimSize, &data, &dataSize, (char **)&shape, &shapeSize, 
				&currDataSize, &timestamped, cache);
	}
	if(!(status & 1)) return status;
	
	if(!timestamped)
		MdsSerializeDscIn((char *)dim, retDim);
	else
	{	
		dimD.arsize = dimSize;
		dimD.pointer = dim;
		MdsCopyDxXd((struct descriptor *)&dimD, retDim);
	}
	
	arrayD.dtype = shape[0];
	arrayD.length = shape[1];
	arrayD.pointer = data;
	arrayD.dimct = shape[2];
	arrayD.arsize = shape[3];
	if(arrayD.dimct > 1)
	{
		boundsPtr = (int *)((char *)&arrayD + sizeof(struct descriptor_a)  + sizeof(char *));
		for(currDim = 0; currDim < arrayD.dimct; currDim++)
		{
			boundsPtr[currDim] = shape[currDim + 4];
		}
	}
	else
		arrayD.aflags.bounds = arrayD.aflags.coeff = 0;
	
	status = MdsCopyDxXd((struct descriptor *)&arrayD, retData);

	return status;
}

EXPORT int RTreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd)
{
	char *start, *end;
	int startSize, endSize, status, copiedSegments;
	char timestamped;

	if(!cache) cache = getCache();

	status = getSegmentLimits(currShotNum, nid, idx, &start, &startSize, &end, &endSize, &timestamped, cache);
	if(!(status & 1))
	{
		status = copySegmentedIntoCache(nid, &copiedSegments);
		if(status & 1)
			status = getSegmentLimits(currShotNum, nid, idx, &start, &startSize, &end, &endSize, &timestamped, cache);
	}
	if(!(status & 1)) return status;

	if(timestamped)
	{
		struct descriptor endD = {8, DTYPE_QU, CLASS_S, 0};
		struct descriptor startD = {8, DTYPE_QU, CLASS_S, 0};
		endD.pointer = end;
		startD.pointer = start;
		MdsCopyDxXd(&startD, retStart);
		MdsCopyDxXd(&endD, retEnd);
	}
	else
	{
		MdsSerializeDscIn((char *)start, retStart);
		MdsSerializeDscIn((char *)end, retEnd);
	}
	return 1;
}

EXPORT int RTreeDiscardOldSegments(int nid, _int64 timestamp)
{
	if(!cache) cache = getCache();
	return discardOldSegments(currShotNum, nid, timestamp, cache);
}



EXPORT int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeMode)
{
	EMPTYXD(ser_xd);

	int status;
	struct descriptor_a *arrPtr;
	if(!cache) cache = getCache();


	

	//Manage Empty descriptor
	if(descriptor_ptr->class == CLASS_XD && ((struct descriptor_xd *)descriptor_ptr)->l_length == 0)
		return putRecord(currShotNum, nid, 0, 0, 0, 0, writeMode, cache);

	if(descriptor_ptr->class == CLASS_S) //Handle scalars
	{
	    status = putRecord(currShotNum, nid, descriptor_ptr->dtype, 1, descriptor_ptr->pointer, 
		descriptor_ptr->length, writeMode, cache);
	    return status;
	}
	if(descriptor_ptr->class == CLASS_A) //Handle arrays
	{
	    arrPtr = (struct descriptor_a *)descriptor_ptr;
	    status = putRecord(currShotNum, nid, arrPtr->dtype, arrPtr->arsize/arrPtr->length, arrPtr->pointer,
		arrPtr->arsize, writeMode, cache);
	    return status;
	}
	
	MdsSerializeDscOut(descriptor_ptr, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;
	status = putRecord(currShotNum, nid, 0, 0, arrPtr->pointer, arrPtr->arsize, writeMode, cache);
	MdsFree1Dx(&ser_xd, 0);
	return status;
}



EXPORT int RTreeFlush()
{
	if(!cache) cache = getCache();
	return flushTree(currShotNum, cache);
}


EXPORT char *RTreeSetCallback(int nid, void (*callback)(int))
{
	if(!cache) cache = getCache();
	return setCallback(currShotNum, nid, callback, cache);
}

EXPORT int RTreeClearCallback(int nid, char *callbackDescr)
{
	if(!cache) cache = getCache();
	return clearCallback(currShotNum, nid, callbackDescr, cache);
}

EXPORT int RTreeSetWarm(int nid, int warm)
{
	if(!cache) cache = getCache();
	return setWarm(currShotNum, nid, warm, cache);
}



EXPORT int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr)
{
	char *data;
	int dataLen;
	int status;
	char dataType;
	int numSamples;
	struct descriptor descr;
	DESCRIPTOR_A(descrA, 0, 0, 0, 0);
	struct descriptor *retDescr;

//printf("RTreeGetRecord\n");
	if(!cache) cache = getCache();



	status = getRecord(currShotNum, nid, &dataType, &numSamples, &data, &dataLen, cache);
	if(!(status & 1) || dataLen == 0)
	{
		status = TreeGetRecord(nid, dsc_ptr);
			
		if(status & 1 && dsc_ptr->pointer)
		{
			status = RTreePutRecord(nid, dsc_ptr->pointer, 0);
		}
//printf("STATUS: %d\n", status);
	}
	else
	{
		if(dataType)
		{
	    		retDescr = rebuildDataDescr(dataType, numSamples, data, dataLen, &descr, (struct descriptor_a *)&descrA);
			MdsCopyDxXd(retDescr, dsc_ptr);
		}
		else
			MdsSerializeDscIn((char *)data, dsc_ptr);
	}
	return status;
}



int putRecordInternal(int nid, char dataType, int numSamples, char *data, int size)
{
	int status;
	EMPTYXD(xd);
	struct descriptor descr;
	DESCRIPTOR_A(descrA, 0, 0, 0, 0);
	struct descriptor *retDescr;
	if(dataType) //If scalar or array
	{
	    retDescr = rebuildDataDescr(dataType, numSamples, data, size, &descr, (struct descriptor_a *)&descrA);
	    status = TreePutRecord(nid, retDescr, 0);
	}
	else
	{
	    status = MdsSerializeDscIn((char *)data, &xd);
	    if(status & 1)
		status = TreePutRecord(nid, (struct descriptor *)&xd, 0);
	    MdsFree1Dx(&xd, 0);
	}
	return status;
}


int putSegmentInternal(int nid, char *start, int startSize, char *end, int endSize, 
					   char *dim, int dimSize, char *data, int dataSize, int *shape, int shapeSize, 
					   int isTimestamped, int actSamples)
{
	EMPTYXD(dataXd);
	EMPTYXD(dimXd);
	EMPTYXD(startXd);
	EMPTYXD(endXd);
	DESCRIPTOR_A(timesD, 8, DTYPE_QU, 0, 0);
	struct descriptor startD = {8, DTYPE_QU, CLASS_S, 0};
	struct descriptor endD = {8, DTYPE_QU, CLASS_S, 0};

	int status;

//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits
	if(isTimestamped)
	{
		//Adjust shape description so that actSamples are the actual time samples
		shape[4+shape[2]-1] = actSamples;
	}
	status = rebuildFromDataTypeAndShape(data, shape, shapeSize, &dataXd);
	if(!(status & 1)) return status;
	if(isTimestamped)
	{
		timesD.pointer = dim;
		//timesD.arsize = dimSize;
		timesD.arsize = 8*actSamples;
		startD.pointer = start;
		endD.pointer = end;
		status = TreeBeginSegment(nid, &startD, &endD, &timesD, dataXd.pointer, -1);
		if(!(status & 1)) return status;
		MdsFree1Dx(&dataXd, 0);
	}
	else
	{

		status = MdsSerializeDscIn(start, &startXd);
		if(status & 1) status = MdsSerializeDscIn(end, &endXd);
		if(status & 1) status = MdsSerializeDscIn(dim, &dimXd);
		if(status & 1) status = TreeBeginSegment(nid, startXd.pointer, endXd.pointer, dimXd.pointer, dataXd.pointer, -1);
		MdsFree1Dx(&startXd, 0);
		MdsFree1Dx(&endXd, 0);
		MdsFree1Dx(&dimXd, 0);
		MdsFree1Dx(&dataXd, 0);
	}
	return status;
}

EXPORT int RTreeOpen(char *expName, int shot)
{
	int status;
	status = TreeOpen(expName, shot, 0);
	if(status & 1)
	{
		currShotNum = shot;
		if(shotNumIdx < MAX_OPEN_TREES - 1)
			shotNumbers[shotNumIdx++] = currShotNum;
	}
	return status;
}

EXPORT int RTreeClose(char *expName, int shot)
{
	int status;
	status = TreeClose(expName, shot);
	if(shotNumIdx > 1)
		shotNumIdx--;
	currShotNum = shotNumbers[shotNumIdx-1];
	return status;
}

EXPORT void RTreeSynch()
{
	if(!cache) cache = getCache();
	synch(cache);
}


#ifdef HAVE_WINDOWS_H
EXPORT void RTreeCacheReset()
{
	if(!cache) cache = getCache();

}
#else 
#ifdef HAVE_VXWORKS_H
EXPORT void RTreeCacheReset()
{
	if(!cache) cache = getCache();

}
#else

//For Linux only: remove all persistent semaphores
EXPORT void RTreeCacheReset()
{
    char buf[256];
    int i;
   for(i = 0; i < 10; i++)
    {
		sprintf(buf, "/mdscachex%d", i);
		sem_unlink(buf);
    }
	if(!cache) cache = getCache();
 }



#endif
#endif






