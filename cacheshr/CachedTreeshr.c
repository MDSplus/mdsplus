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
#include <dbidef.h>

#define NOT_A_NATIVE_TYPE 2
#define BAD_SHAPE 4
#define BAD_INDEX 6
#define TRUNCATED 8
#define BAD_TYPE 10

#define WRITE_THROUGH 1
#define WRITE_BACK 2
#define WRITE_BUFFER 3
#define WRITE_LAST 4
#define UPDATE_FLUSH 5
#define MAX_OPEN_TREES 1024

#define MAX_BOUND_SIZE 512

char *getCache(int isShared, int size);
int putRecord(char *name, int shot, int nid, char dataType, int numSamples, char *data, int size, int writeThrough, char *cachePtr);
int getRecord(char *name, int shot, int nid, char *dataType, int *numSamples, char **data, int *size, char *cachePtr);
int putRecordInternal(char *name, int shot, int nid, char dataType, int numSamples, char *data, int size);
int deleteRecordInternal(char *name, int shot, int nid);
int beginSegment(char *name, int shot, int nid, int idx, char *start, int startSize, char *end, int endSize, 
						char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, 
						int writeThrough, char *cachePtr);
int beginTimestampedSegment(char *name, int shot, int nid, int idx, int numItems, char *shape, int shapeSize, char *data, int dataSize, 
						_int64 start, _int64 end, char *dim, int dimSize, int writeThrough, char *cachePtr);
int putSegmentInternal(char *name, int shot, int nid, 
						char *start, int startSize, char *end, int endSize, char *dim, int dimSize, char *data, 
						int dataSize, int *shape, int shapeSize, int currDataSize, int isTimestamped, int actSamples, int updateOnly);
int updateSegment(char *name, int shot, int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, 
							 int dimSize, int writeThrough, char *cachePtr);
int getNumSegments(char *name, int shot, int nid, int *numSegments, char *cachePtr);
int getSegmentLimits(char *name, int shot, int nid, int idx, char **start, int *startSize, char **end, int *endSize,char *timestamped, char *cachePtr);
int getSegmentData(char *name, int shot, int nid, int idx, char **dim, int *dimSize, char **data, int *dataSize,char **shape, 
							  int *shapeSize, int *currDataSize, char *timestamped, char *cachePtr);
int getSegmentInfo(char *name, int shot, int nid, int **shape, int *shapeSize, int *currDataSize, char *cachePtr);
int isSegmented(char *name, int shot, int nid, int *segmented, char *cachePtr);
int terminateSegment(char *name, int shot, int nid, char *cachePtr);
int flushTree(char *name, int shot, char *cachePtr);
int flushNode(char *name, int shot, int nid, char *cachePtr);
void *setCallback(char *name, int shot, int nid, void *argument, void (* callback)(int, void *), void *cachePtr);
int clearCallback(char *name, int shot, int nid, char *callbackDescr, void *cachePtr);
int appendSegmentData(char *name, int shot, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, int writeMode, char *cachePtr);
int appendTimestampedSegmentData(char *name, int shot, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, _int64 *timestamp, int numTimestamps, int writeMode, char *cachePtr);
int appendRow(char *name, int shot, int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, _int64 timestamp, int writeMode, char *cachePtr);
int discardOldSegments(char *name, int shot, int nid, _int64 timestamp, char *cachePtr);
int discardData(char *name, int shot, int nid, char *cachePtr);
int setWarm(char *name, int shot, int nid, int warm, char *cachePtr);
void synch(char *cachePtr);
extern int TdiCompile();
extern int TdiData();
extern int TdiEvaluate();
extern int TdiDecompile();


static int cacheIsShared = 1, cacheSize = 200000;



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


static int getNameShot(void *dbi, char *name, int *shot)
{
	int nameLen, shotLen, status;
	struct dbi_itm dbiList[] = {{64, DbiNAME, name, &nameLen},
	{sizeof(int), DbiSHOTID, shot, &shotLen},
	{0, DbiEND_OF_LIST, 0, 0}};

	if(dbi)
		status = _TreeGetDbi(dbi, dbiList);
	else
		status = TreeGetDbi(dbiList);
	if(!(status & 1))
		return status;
	name[nameLen] = 0;
	return status;
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

static int intBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx, 
									  _int64 start, _int64 end, struct descriptor_a *timesD, int writeMode)
{
	_int64 *times;
	int nTimes, timesSize;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	char name[256];
	int shot, status;
	
	DESCRIPTOR_APD(dimsApd, DTYPE_DSC, 0, 0);

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;

	if(!cache) cache = getCache(cacheIsShared, cacheSize);
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
	return beginTimestampedSegment(name, shot, nid, idx, nTimes, (char *)bounds, boundsSize, initialValue->pointer,
						bounds[3], start, end, (char *)times, timesSize, writeMode, cache);
}

EXPORT int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode);
EXPORT int _RTreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode);

EXPORT int _RTreePutSegment(void *dbid, int nid, int segIdx, struct descriptor *dataD, int writeMode)
{
	int bounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;

	if(!cache) cache = getCache(cacheIsShared, cacheSize);

	getDataTypeAndShape(dataD, bounds, &boundsSize);
	status = appendSegmentData(name, shot, nid, bounds, boundsSize, dataD->pointer, bounds[3], -1, segIdx,  writeMode, cache);
	return status;
}
EXPORT int RTreePutSegment(int nid, int segIdx, struct descriptor *dataD, int writeMode)
{
	return _RTreePutSegment(0, nid, segIdx, dataD, writeMode);
}

//Read segmented data into cache memory
typedef ARRAY_BOUNDS(char, 16) Array_bounds_type;
static int copySegmentedIntoCache(void *dbid, int nid, int *copiedSegments)
{
	int status, numSegments, currIdx;
	EMPTYXD(startXd);
	EMPTYXD(endXd);
	EMPTYXD(dataXd);
	EMPTYXD(dimXd);

	_int64 *times;
	int nTimes, i;
	char dtype, dimct;
	int dims[64], segItems, nextRow, oldLen;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	struct descriptor_a *timesD;
	Array_bounds_type *arrD;

	status = (dbid)?_TreeGetNumSegments(dbid, nid, &numSegments):TreeGetNumSegments(nid, &numSegments);
	if(!(status & 1)) return status;
	*copiedSegments = numSegments;
	for(currIdx = 0; currIdx < numSegments; currIdx++)
	{
		status = (dbid)?_TreeGetSegmentLimits(dbid, nid, currIdx, &startXd, &endXd):TreeGetSegmentLimits(nid, currIdx, &startXd, &endXd);
		if(!(status & 1)) return status;
		status = (dbid)?_TreeGetSegment(dbid, nid, currIdx, &dataXd, &dimXd):TreeGetSegment(nid, currIdx, &dataXd, &dimXd);
		if(!(status & 1)) return status;
		//status = TdiData(&dimXd, &dimXd MDS_END_ARG);
		//if(!(status & 1)) return status;

		arrD = (Array_bounds_type *)dataXd.pointer;
		getDataTypeAndShape((struct descriptor *)arrD, bounds, &boundsSize);

		if(startXd.pointer && startXd.pointer->class == CLASS_S && (startXd.pointer->dtype == DTYPE_Q || startXd.pointer->dtype == DTYPE_QU))
		{
			//Assumed to be timestamped
			timesD = (struct descriptor_a *)dimXd.pointer;
			if(timesD->dtype != DTYPE_Q && timesD->dtype != DTYPE_QU)
				return 0;
			times = (_int64*)timesD->pointer;
			nTimes = timesD->arsize/timesD->length;

			status = intBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)dataXd.pointer, -1, times[0], times[nTimes - 1],
				timesD, 0);
			if(!(status & 1)) return status;
		}
		else
		{
			status = _RTreeBeginSegment(dbid, nid, startXd.pointer, endXd.pointer, dimXd.pointer, (struct descriptor_a *)dataXd.pointer, 
				-1, (currIdx < numSegments - 1)?0:UPDATE_FLUSH);
			if(!(status & 1)) return status;
			if(currIdx < numSegments - 1)
			{
				status = _RTreePutSegment(dbid, nid,  -1, dataXd.pointer,0);
				if(!(status & 1)) return status;
			}
			else //Last Segment: keep track of the fact that it may be partially filled
			{
				status = (dbid)?_TreeGetSegmentInfo(dbid, nid, &dtype, &dimct, dims, &segItems, &nextRow):TreeGetSegmentInfo(nid, &dtype, &dimct, dims, &segItems, &nextRow);
				if(status & 1)
				{
					arrD = (Array_bounds_type *)dataXd.pointer;
					oldLen = arrD->arsize;
					if(nextRow < dims[dimct - 1]) //If there is a unfilled section of the segment
					{
				            arrD->arsize = arrD->length;
					    for(i = 0; i < dimct - 1; i++)
					    	arrD->arsize *= dims[i];
					    arrD->arsize *= nextRow;
					    if(arrD->dimct > 1)
					    	arrD->m[dimct - 1] = nextRow;
					}
					status = _RTreePutSegment(dbid, nid,  -1,(struct descriptor *)arrD, 0);
					arrD->arsize = oldLen;
				}
				if(!(status & 1)) return status;
			}
		}
		MdsFree1Dx(&startXd, 0);
		MdsFree1Dx(&endXd, 0);
		MdsFree1Dx(&dataXd, 0);
		MdsFree1Dx(&dimXd, 0);
	}
	return status;
}


EXPORT void test(){printf("TEST\n");}


EXPORT int _RTreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode)
{
	EMPTYXD(startSerXd);	
	EMPTYXD(endSerXd);	
	EMPTYXD(dimSerXd);	
	struct descriptor_a *startSerA, *endSerA, *dimSerA;
	struct descriptor_signal *signalD;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	int status;
	char name[256];
	int shot;
	DESCRIPTOR_APD(dimsApd, DTYPE_DSC, 0, 0);

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	


	if(!cache) cache = getCache(cacheIsShared, cacheSize);
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



	beginSegment(name, shot, nid, idx, (char *)startSerA->pointer, startSerA->arsize, (char *)endSerA->pointer, endSerA->arsize, 
						(char *)dimSerA->pointer, dimSerA->arsize, (char *)bounds, boundsSize, initialValue->pointer,
						bounds[3], writeMode, cache);
	MdsFree1Dx(&startSerXd, 0);
	MdsFree1Dx(&endSerXd, 0);
	MdsFree1Dx(&dimSerXd, 0);
	return 1; 

}

EXPORT int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode)
{
	return _RTreeBeginSegment(0, nid, start, end, dimension, initialValue, idx, writeMode);
}
EXPORT int _RTreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx, int writeMode)
{
	
	return intBeginTimestampedSegment(dbid, nid, initialValue, idx, 0, 0, 0, writeMode);
}
EXPORT int RTreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx, int writeMode)
{
	return _RTreeBeginTimestampedSegment(0, nid, initialValue, idx, writeMode);
}


EXPORT int _RTreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, 
							  struct descriptor *dimension, int idx, int writeMode)
{
	EMPTYXD(startSerXd);	
	EMPTYXD(endSerXd);	
	EMPTYXD(dimSerXd);	
	struct descriptor_a *startSerA, *endSerA, *dimSerA;
	struct descriptor *savedData, *savedRaw;
	int status;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;

	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	if(start) MdsSerializeDscOut(start, &startSerXd);
	if(end) MdsSerializeDscOut(end, &endSerXd);
	if(dimension)
	{
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
	}
	
	
	startSerA = (struct descriptor_a *)startSerXd.pointer;
	endSerA = (struct descriptor_a *)endSerXd.pointer;
	dimSerA = (struct descriptor_a *)dimSerXd.pointer;

	status = updateSegment(name, shot, nid, idx,  (start)?(char *)startSerA->pointer:0, (start)?startSerA->arsize:0, 
						(end)?(char *)endSerA->pointer:0, (end)?endSerA->arsize:0, 
						(dimension)?(char *)dimSerA->pointer:0, (dimension)?dimSerA->arsize:0, writeMode, cache);

	MdsFree1Dx(&startSerXd, 0);
	MdsFree1Dx(&endSerXd, 0);
	MdsFree1Dx(&dimSerXd, 0);
	return status;
}
EXPORT int RTreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, 
							  struct descriptor *dimension, int idx, int writeMode)
{
	return _RTreeUpdateSegment(0, nid, start, end, dimension, idx, writeMode);
}





EXPORT int _RTreePutTimestampedSegment(void *dbid, int nid,  _int64 *timestamps, struct descriptor *dataD,int writeMode)
{
	int bounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);

	getDataTypeAndShape(dataD, bounds, &boundsSize);

	status = appendTimestampedSegmentData(name, shot, nid, bounds, boundsSize, dataD->pointer, 
										 bounds[3], -1, -1, timestamps, bounds[4+bounds[2] - 1],//LastDimension
										 writeMode, cache);
	return status;
}
EXPORT int RTreePutTimestampedSegment(int nid,  _int64 *timestamps, struct descriptor *dataD,int writeMode)
{
	return _RTreePutTimestampedSegment(0, nid, timestamps, dataD, writeMode);
}

//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits

EXPORT int _RTreePutRow(void *dbid, int nid, int bufSize, _int64 *timestamp, struct descriptor_a *rowD, int writeMode)
{
	int bounds[MAX_BOUND_SIZE], extendedBounds[MAX_BOUND_SIZE];
	int boundsSize;
	char *data;
	int dataSize;
	int status;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);


	getDataTypeAndShape((struct descriptor *)rowD, bounds, &boundsSize);
	status = appendRow(name, shot, nid, bounds, boundsSize, rowD->pointer, 
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
		status = beginTimestampedSegment(name, shot, nid, -1, bufSize, (char *)extendedBounds, boundsSize+4, 
			data, dataSize, 0, 0, 0, 0, writeMode, cache);
		if(status & 1)
			status = appendRow(name, shot, nid, bounds, boundsSize, rowD->pointer, 
										 bounds[3], *timestamp, writeMode, cache);
		free(data);
	}
	return status;
}

EXPORT int RTreePutRow(int nid, int bufSize, _int64 *timestamp, struct descriptor_a *rowD, int writeMode)
{
	return _RTreePutRow(0, nid, bufSize, timestamp, rowD, writeMode);
}
	

EXPORT int _RTreeGetNumSegments(void *dbid, int nid, int *numSegments)
{
	int status;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	status =  getNumSegments(name, shot, nid, numSegments, cache);
	if(!(status & 1))
	{
		status = copySegmentedIntoCache(dbid, nid, numSegments);
	}
	return status;
}
EXPORT int RTreeGetNumSegments(int nid, int *numSegments)
{
	return _RTreeGetNumSegments(0, nid, numSegments);
}


EXPORT int _RTreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *retData, struct descriptor_xd *retDim)
{
	char *data, *dim;
	int *shape;
	char timestamped;
	int status, dataSize, shapeSize, dimSize, currDataSize;
	DESCRIPTOR_A_COEFF(arrayD, 0, 0, 0, (unsigned char )256, 0);
	int *boundsPtr;
	int currDim, copiedSegments;	
	DESCRIPTOR_A(dimD, 8, DTYPE_QU, 0, 0);
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;


	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	status = getSegmentData(name, shot, nid, idx, &dim, &dimSize, &data, &dataSize, (char **)&shape, &shapeSize, 
		&currDataSize, &timestamped, cache);
	if(!(status & 1)) 
	{
		status = copySegmentedIntoCache(dbid, nid, &copiedSegments);
		if(status & 1)
			status = getSegmentData(name, shot, nid, idx, &dim, &dimSize, &data, &dataSize, (char **)&shape, &shapeSize, 
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
	if(timestamped)
		arrayD.arsize = currDataSize;
	else
		arrayD.arsize = shape[3];

	if(arrayD.dimct > 1)
	{
		boundsPtr = (int *)((char *)&arrayD + sizeof(struct descriptor_a)  + sizeof(char *));
		for(currDim = 0; currDim < arrayD.dimct; currDim++)
		{
			boundsPtr[currDim] = shape[currDim + 4];
		}
		if(timestamped)
		    boundsPtr[arrayD.dimct - 1] = currDataSize/shape[1];
	}
	else
		arrayD.aflags.bounds = arrayD.aflags.coeff = 0;
	
	status = MdsCopyDxXd((struct descriptor *)&arrayD, retData);

	return status;
}
EXPORT int RTreeGetSegment(int nid, int idx, struct descriptor_xd *retData, struct descriptor_xd *retDim)
{
	return  _RTreeGetSegment(0, nid, idx, retData, retDim);
}
EXPORT int _RTreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd)
{
	char *start, *end;
	int startSize, endSize, status, copiedSegments;
	char timestamped;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;

	if(!cache) cache = getCache(cacheIsShared, cacheSize);

	status = getSegmentLimits(name, shot, nid, idx, &start, &startSize, &end, &endSize, &timestamped, cache);
	if(!(status & 1))
	{
		status = copySegmentedIntoCache(dbid, nid, &copiedSegments);
		if(status & 1)
			status = getSegmentLimits(name, shot, nid, idx, &start, &startSize, &end, &endSize, &timestamped, cache);
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


EXPORT int _RTreeTerminateSegment(void *dbid, int nid)
{
	int status;
	char name[256];
	int shot;
	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return terminateSegment(name, shot, nid, cache);
}

EXPORT int RTreeTerminateSegment(int nid)
{
	return _RTreeTerminateSegment(0, nid);
}



EXPORT int RTreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd)
{
	return _RTreeGetSegmentLimits(0, nid, idx, retStart, retEnd);
}


EXPORT int _RTreeGetSegmentInfo(void *dbid, int nid, char *dtype, char *dimct, int *dims, int *leftItems, int *leftRows)
{
	int *shape, shapeSize, currDataSize, i, nItems, dataSize, status;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	getSegmentInfo(name, shot, nid, &shape, &shapeSize, &currDataSize, cache);
//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits
	*dtype = shape[0];
	*dimct = shape[2];
	nItems = 1;
	for(i = 0; i < shape[2]; i++)
	{
	    nItems *= shape[4+i];
	    dims[i] = shape[4+i];
	}
	dataSize = nItems * shape[1];
	*leftItems = currDataSize/shape[1];
	if(*leftItems == 0)
	    *leftRows = 0;
	else
	    *leftRows = *leftItems/(nItems / shape[4 + shape[2]-1]);
	return 1;
}

EXPORT int RTreeGetSegmentInfo(int nid, char *dtype, char *dimct, int *dims, int *leftItems, int *leftRows)
{
	return _RTreeGetSegmentInfo(0, nid, dtype, dimct, dims, leftItems, leftRows);
}




EXPORT int _RTreeDiscardOldSegments(void *dbid, int nid, _int64 timestamp)
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return discardOldSegments(name, shot, nid, timestamp, cache);
}
EXPORT int RTreeDiscardOldSegments(int nid, _int64 timestamp)
{
	return _RTreeDiscardOldSegments(0, nid, timestamp);
}


EXPORT int _RTreeDiscardData(void *dbid, int nid)
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return discardData(name, shot, nid, cache);
}
EXPORT int RTreeDiscardData(int nid)
{
	return _RTreeDiscardData(0, nid);
}




EXPORT int _RTreePutRecord(void *dbid, int nid, struct descriptor *descriptor_ptr, int writeMode)
{
	EMPTYXD(ser_xd);


	int status;
	struct descriptor_a *arrPtr;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);
	if(!(status & 1))
		return status;
	if(!cache) cache = getCache(cacheIsShared, cacheSize);

	//Manage Empty descriptor
	if(descriptor_ptr->class == CLASS_XD && ((struct descriptor_xd *)descriptor_ptr)->l_length == 0)
		return putRecord(name, shot, nid, 0, 0, 0, 0, writeMode, cache);

	if(descriptor_ptr->class == CLASS_S) //Handle scalars
	{
	    status = putRecord(name, shot, nid, descriptor_ptr->dtype, 1, descriptor_ptr->pointer, 
		descriptor_ptr->length, writeMode, cache);
	    return status;
	}
	if(descriptor_ptr->class == CLASS_A && ((struct descriptor_a*)descriptor_ptr)->dimct <= 1)  //Handle 1D arrays
	{
	    arrPtr = (struct descriptor_a *)descriptor_ptr;
	    status = putRecord(name, shot, nid, arrPtr->dtype, arrPtr->arsize/arrPtr->length, arrPtr->pointer,
		arrPtr->arsize, writeMode, cache);
	    return status;
	}
	
	MdsSerializeDscOut(descriptor_ptr, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;
	status = putRecord(name, shot, nid, 0, 0, arrPtr->pointer, arrPtr->arsize, writeMode, cache);
	MdsFree1Dx(&ser_xd, 0);
	return status;
}
EXPORT int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeMode)
{
	return _RTreePutRecord(0, nid, descriptor_ptr, writeMode);
}





EXPORT int _RTreeFlush(void *dbid)
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return flushTree(name, shot, cache);
}

EXPORT int RTreeFlush()
{
	return _RTreeFlush(0);
}

EXPORT int _RTreeFlushNode(void *dbid, int nid)
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return flushNode(name, shot, nid, cache);
}
EXPORT int RTreeFlushNode(int nid)
{
	return _RTreeFlushNode(0, nid);
}

EXPORT int RTreeFlushNodeShot(char *expName, int shot, int nid)
{
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return flushNode(expName, shot, nid, cache);
}


EXPORT char *_RTreeSetCallback(void *dbid, int nid, void *argument, void (*callback)(int, void *))
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return setCallback(name, shot, nid, argument, callback, cache);
}
EXPORT char *RTreeSetCallback(int nid, void *argument, void (*callback)(int, void *))
{
	return _RTreeSetCallback(0, nid, argument, callback);
}

EXPORT int _RTreeClearCallback(void *dbid, int nid, char *callbackDescr)
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return clearCallback(name, shot, nid, callbackDescr, cache);
}
EXPORT int RTreeClearCallback(int nid, char *callbackDescr)
{
	return _RTreeClearCallback(0, nid, callbackDescr);
}


EXPORT int _RTreeSetWarm(void *dbid, int nid, int warm)
{
	char name[256];
	int shot, status;

	status = getNameShot(dbid, name, &shot);
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	return setWarm(name, shot, nid, warm, cache);
}

EXPORT int RTreeSetWarm(int nid, int warm)
{
	return _RTreeSetWarm(0, nid, warm);
}



EXPORT int _RTreeGetRecord(void *dbid, int nid, struct descriptor_xd *dsc_ptr)
{
	char *data;
	int dataLen;
	int status;
	char dataType;
	int numSamples;
	struct descriptor descr;
	DESCRIPTOR_A(descrA, 0, 0, 0, 0);
	struct descriptor *retDescr;
	char name[256];
	int shot;

	status = getNameShot(dbid, name, &shot);

//printf("RTreeGetRecord\n");
	if(!cache) cache = getCache(cacheIsShared, cacheSize);



	status = getRecord(name, shot, nid, &dataType, &numSamples, &data, &dataLen, cache);
	if(!(status & 1) || dataLen == 0)
	{
		if(dbid)
			status = _TreeGetRecord(dbid,nid, dsc_ptr);
		else
			status = TreeGetRecord(nid, dsc_ptr);
			
		if(status & 1 && dsc_ptr->pointer)
		{
			status = _RTreePutRecord(dbid, nid, dsc_ptr->pointer, 0);
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

EXPORT int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr)
{
	return _RTreeGetRecord(0, nid, dsc_ptr);
}


int putRecordInternal(char *name, int shot, int nid, char dataType, int numSamples, char *data, int size)
{
	int status;
	EMPTYXD(xd);
	struct descriptor descr;
	DESCRIPTOR_A(descrA, 0, 0, 0, 0);
	struct descriptor *retDescr;
	void *dbid = 0;
	void *oldDbid = TreeSwitchDbid(0);
	status = _TreeOpen(&dbid, name, shot, 0);
	if(!(status & 1)) 
	{
		printf("PUT RECORD INTERNAL ERROR !!!, %s %d\n", name, shot);
		TreeSwitchDbid(oldDbid);
		return status;
	}

	if(size == 0)
	{
	    status = _TreePutRecord(dbid, nid, (struct descriptor *)&xd, 0);
	    TreeSwitchDbid(oldDbid);
	    return status;
	}
	if(dataType) //If scalar or array
	{
	    retDescr = rebuildDataDescr(dataType, numSamples, data, size, &descr, (struct descriptor_a *)&descrA);
	    status = _TreePutRecord(dbid, nid, retDescr, 0);
	}
	else
	{
	    status = MdsSerializeDscIn((char *)data, &xd);
	    if(status & 1)
		status = _TreePutRecord(dbid, nid, (struct descriptor *)&xd, 0);
	    MdsFree1Dx(&xd, 0);
	}
	TreeSwitchDbid(oldDbid);
	return status;
}

int deleteRecordInternal(char *name, int shot, int nid)
{
	int status;
	EMPTYXD(xd);
	void *dbid = 0;
	void *oldDbid = TreeSwitchDbid(0);
	status = _TreeOpen(&dbid, name, shot, 0);
	if(!(status & 1))
	{
	    TreeSwitchDbid(oldDbid);
	    return status;
        }
	status = _TreePutRecord(dbid, nid, (struct descriptor *)&xd, 0);
	TreeSwitchDbid(oldDbid);
	return status;
}


int putSegmentInternal(char *name, int shot, int nid, char *start, int startSize, char *end, int endSize, 
					   char *dim, int dimSize, char *data, int dataSize, int *inShape, int shapeSize, int currDataSize,
					    
					   int isTimestamped, int actSamples, int updateOnly)
{
	EMPTYXD(dataXd);
	EMPTYXD(fullDataXd);
	EMPTYXD(dimXd);
	EMPTYXD(startXd);
	EMPTYXD(endXd);
	DESCRIPTOR_A(timesD, 8, DTYPE_QU, 0, 0);
	struct descriptor startD = {8, DTYPE_QU, CLASS_S, 0};
	struct descriptor endD = {8, DTYPE_QU, CLASS_S, 0};

	int status, i, rowItems;
	int shape[512];

	void *dbid = 0;
	void *oldDbid = TreeSwitchDbid(0);
	status = _TreeOpen(&dbid, name, shot, 0);
	if(!(status & 1))
	{
	    TreeSwitchDbid(oldDbid);
	    return status;
	}
	memcpy(shape, inShape, shapeSize);

	//Return Shape and type information. The coding is the following:
//1) data type
//2) item size in bytes
//3) number of dimensions 
//4) total dimension in bytes 
//The remaining elements are the dimension limits

//first build whiole segment description
	status = rebuildFromDataTypeAndShape(data, shape, shapeSize, &fullDataXd);
	if(!(status & 1)) 
	{
	    TreeSwitchDbid(oldDbid);
	    return status;
	}

//then build actual data segment (possibly incomplete)
	if(isTimestamped)
	{
		//Adjust shape description so that actSamples are the actual time samples
		shape[4+shape[2]-1] = actSamples;
	}
	else
	{
	
//Update shape information to keep track of the actual data size stored in the segment
//this is based on currDataSize
		rowItems = 1;
		for(i = 0; i < shape[2]-1; i++)
		{
	    		rowItems *= shape[4+i];
		}	
		 shape[4+shape[2]-1]  = currDataSize/(rowItems * shape[1]);
		 shape[3] = currDataSize;
	}
	
	status = rebuildFromDataTypeAndShape(data, shape, shapeSize, &dataXd);
	if(!(status & 1))
	{
	    TreeSwitchDbid(oldDbid);
	    return status;
	}
	if(isTimestamped)
	{
		timesD.pointer = dim;
		//timesD.arsize = dimSize;
		timesD.arsize = 8*actSamples;
		startD.pointer = start;
		endD.pointer = end;
		status = _TreeBeginSegment(dbid, nid, &startD, &endD, (struct descriptor *)&timesD, (struct descriptor_a *)dataXd.pointer, -1);
		if(!(status & 1))
		{
		    TreeSwitchDbid(oldDbid);
 		    return status;
		}
		MdsFree1Dx(&dataXd, 0);
	}
	else
	{

		status = MdsSerializeDscIn(start, &startXd);
		if(status & 1) status = MdsSerializeDscIn(end, &endXd);
		if(status & 1) status = MdsSerializeDscIn(dim, &dimXd);

		if(status & 1)
		{
			if(!updateOnly)
				status = _TreeBeginSegment(dbid, nid, startXd.pointer, endXd.pointer, dimXd.pointer, (struct descriptor_a *)fullDataXd.pointer, -1);
			if(status & 1) status = _TreePutSegment(dbid, nid, 0, (struct descriptor_a *)dataXd.pointer);
		}
		MdsFree1Dx(&startXd, 0);
		MdsFree1Dx(&endXd, 0);
		MdsFree1Dx(&dimXd, 0);
		MdsFree1Dx(&dataXd, 0);
		MdsFree1Dx(&fullDataXd, 0);
	}
	TreeSwitchDbid(oldDbid);
	return status;
}

EXPORT void RTreeConfigure(int shared, int size)
{
	cacheIsShared = shared;
	cacheSize = size;
}


EXPORT int _RTreeOpen(void **dbid, char *expName, int shot)
{
	return _TreeOpen(dbid, expName, shot, 0);
}

EXPORT int _RTreeClose(void *dbid, char *expName, int shot)
{
	return _TreeClose(dbid, expName, shot);
}
EXPORT int RTreeOpen(char *expName, int shot)
{
	return TreeOpen(expName, shot, 0);
}

EXPORT int RTreeClose(char *expName, int shot)
{
	return TreeClose(expName, shot);
}

EXPORT void RTreeSynch()
{
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
	synch(cache);
}


#ifdef HAVE_WINDOWS_H
EXPORT void RTreeCacheReset()
{
	if(!cache) cache = getCache(cacheIsShared, cacheSize);

}
#else 
#ifdef HAVE_VXWORKS_H
EXPORT void RTreeCacheReset()
{
	if(!cache) cache = getCache(cacheIsShared, cacheSize);

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
	if(!cache) cache = getCache(cacheIsShared, cacheSize);
 }



#endif
#endif






