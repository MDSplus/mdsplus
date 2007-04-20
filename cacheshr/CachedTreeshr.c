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

#define MAX_BOUND_SIZE 512

extern char *getCache();
extern int putRecord(int nid, char dataType, int numSamples, char *data, int size, int writeThrough, char *cachePtr);
extern int getRecord(int nid, char *dataType, int *numSamples, char **data, int *size, char *cachePtr);
extern int putRecordInternal(int nid, char dataType, int numSamples, char *data, int size);
extern int flushTree(char *cachePtr);
extern char *setCallback(int nid, void (* callback)(int), char *cachePtr);
extern int clearCallback(int nid, char *callbackDescr, char *cachePtr);
extern int beginSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, 
						char *dim, int dimSize, char *shape, int shapeSize, char *data, int dataSize, char timestamped,  
						int writeThrough, char *cachePtr);
extern int updateSegment(int nid, int idx, char *start, int startSize, char *end, int endSize, char *dim, 
						 int dimSize, int writeThrough, char *cachePtr);
extern int getNumSegments(int nid, int *numSegments, char *cachePtr);
extern int getSegmentLimits(int nid, int idx, char **start, int *startSize, char **end, int *endSize, char *cachePtr, char *timestamped);
extern int getSegmentData(int nid, int idx, char **dim, int *dimSize, char **data, 
						  int *dataSize,char **shape, int *shapeSize, int *currDataSize, char *timestamped, char *cachePtr);
extern int isSegmented(int nid, int *segmented, char *cachePtr);
extern int appendSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, char *cachePtr);
extern int appendTimestampedSegmentData(int nid, int *bounds, int boundsSize, char *data, 
										 int dataSize, int idx, int startIdx, char *timestamp, char *cachePtr);
extern int setWarm(int nid, int warm, char *cachePtr);
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







//Pointer to external RTreeGetTimedRecord
int (*getTimedRecord)(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *minDelta,
							 struct descriptor_xd *outSig);
EXPORT void RTreeSetGetTimedRecord(int (*getTimedRecordIn)(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *minDelta,
							 struct descriptor_xd *outSig))
{
	getTimedRecord = getTimedRecordIn;
}

//Pointer to external TreePutRecord
int (*treePutRecord)(int nid, struct descriptor *data);
EXPORT void RTreeSetPutRecord(int (*treePutRecordIn)(int nid, struct descriptor *data))
{
	treePutRecord = treePutRecordIn;
}

//Pointer to external TreePutRecord
int (*treeGetRecord)(int nid, struct descriptor_xd *outXd);
EXPORT void RTreeSetGetRecord(int (*treeGetRecordIn)(int nid, struct descriptor_xd *outXd))
{
	treeGetRecord = treeGetRecordIn;
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




EXPORT int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeThrough)
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
	beginSegment(nid, idx, (char *)startSerA->pointer, startSerA->arsize, (char *)endSerA->pointer, endSerA->arsize, 
						(char *)dimSerA->pointer, dimSerA->arsize, (char *)bounds, boundsSize, initialValue->pointer,
						bounds[3], 0, writeThrough, cache);
	MdsFree1Dx(&startSerXd, 0);
	MdsFree1Dx(&endSerXd, 0);
	MdsFree1Dx(&dimSerXd, 0);
	return 1; 

}


EXPORT int RTreeBeginTimestampedSegment(int nid, _int64 start,  
							 struct descriptor_a *initialValue, int idx, int writeThrough)
{
	_int64 thisStart;
	_int64 end = 0;
	char *dim;
	int dimSize;
	struct descriptor startD = {8, DTYPE_QU, CLASS_S, (char *)&thisStart};
	EMPTYXD(startSerXd);	
	EMPTYXD(endSerXd);	
	EMPTYXD(dimSerXd);	
	struct descriptor_a *startSerA, *dimSerA;
	int bounds[MAX_BOUND_SIZE], boundsSize;
	
	DESCRIPTOR_APD(dimsApd, DTYPE_DSC, 0, 0);


	if(!cache) cache = getCache();
//Only native arrays can be defined
	if(initialValue->class != CLASS_A)
		return NOT_A_NATIVE_TYPE;

	thisStart = start;
	MdsSerializeDscOut(&startD, &startSerXd);



	getDataTypeAndShape((struct descriptor *)initialValue, bounds, &boundsSize);

//bounds[boundSize/4 - 1]is the last dimension
	dim = calloc(8, bounds[boundsSize/4 - 1]);
	dimSize = 8 * bounds[boundsSize/4 - 1];
	

	
	startSerA = (struct descriptor_a *)startSerXd.pointer;
	dimSerA = (struct descriptor_a *)dimSerXd.pointer;

	


	beginSegment(nid, idx, (char *)startSerA->pointer, startSerA->arsize, (char *)&end,8, 
						dim, dimSize, (char *)bounds, boundsSize, initialValue->pointer,
						bounds[3], 1, writeThrough, cache);
	MdsFree1Dx(&startSerXd, 0);
	free(dim);
	return 1; 

}



EXPORT int RTreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, 
							  struct descriptor *dimension, int idx, int writeThrough)
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

	status = updateSegment(nid, idx,  (char *)startSerA->pointer, startSerA->arsize, (char *)endSerA->pointer, endSerA->arsize, 
						(char *)dimSerA->pointer, dimSerA->arsize, writeThrough, cache);

	MdsFree1Dx(&startSerXd, 0);
	MdsFree1Dx(&endSerXd, 0);
	MdsFree1Dx(&dimSerXd, 0);
	return status;
}





EXPORT int RTreePutSegment(int nid, struct descriptor *dataD, int segIdx)
{
	int bounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;
	_int64 retTimerstamp = 0;

	if(!cache) cache = getCache();

	getDataTypeAndShape(dataD, bounds, &boundsSize);
	status = appendSegmentData(nid, bounds, boundsSize, dataD->pointer, bounds[3], -1, segIdx,  cache);
	return status;
}

EXPORT int RTreePutTimestampedSegment(int nid, struct descriptor *dataD, _int64 timestamp, int segIdx)
{
	int bounds[MAX_BOUND_SIZE];
	int boundsSize;
	int status;
	_int64 retTimerstamp = 0;

	if(!cache) cache = getCache();

	getDataTypeAndShape(dataD, bounds, &boundsSize);
	status = appendTimestampedSegmentData(nid, bounds, boundsSize, dataD->pointer, bounds[3], -1, segIdx,  (char *)&timestamp, cache);
	return status;
}


EXPORT int RTreeGetNumSegments(int nid, int *numSegments)
{
	if(!cache) cache = getCache();
	return getNumSegments(nid, numSegments, cache);
}

EXPORT int RTreeGetSegment(int nid, int idx, struct descriptor_xd *retData, struct descriptor_xd *retDim)
{
	char *data, *dim;
	int *shape;
	char timestamped;
	int status, dataSize, shapeSize, dimSize, currDataSize;
	DESCRIPTOR_A_COEFF(arrayD, 0, 0, 0, (unsigned char )256, 0);
	int *boundsPtr;
	int currDim;	
	DESCRIPTOR_A(dimD, 8, DTYPE_QU, 0, 0);


	if(!cache) cache = getCache();
	status = getSegmentData(nid, idx, &dim, &dimSize, &data, &dataSize, (char **)&shape, &shapeSize, 
		&currDataSize, &timestamped, cache);
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
	int startSize, endSize, status;
	char timestamped;

	if(!cache) cache = getCache();

	status = getSegmentLimits(nid, idx, &start, &startSize, &end, &endSize, &timestamped, cache);
	if(!(status & 1)) return status;
	MdsSerializeDscIn((char *)start, retStart);
	if(timestamped)
	{
		struct descriptor endD = {8, DTYPE_QU, CLASS_S, 0};
		endD.pointer = end;
		MdsCopyDxXd(&endD, retEnd);
	}
	else
		MdsSerializeDscIn((char *)end, retEnd);
	return 1;
}





EXPORT int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeThrough)
{
	EMPTYXD(ser_xd);

	int status;
	struct descriptor_a *arrPtr;
	if(!cache) cache = getCache();


	

	//Manage Empty descriptor
	if(descriptor_ptr->class == CLASS_XD && ((struct descriptor_xd *)descriptor_ptr)->l_length == 0)
		return putRecord(nid, 0, 0, 0, 0, writeThrough, cache);

	if(descriptor_ptr->class == CLASS_S) //Handle scalars
	{
	    status = putRecord(nid, descriptor_ptr->dtype, 1, descriptor_ptr->pointer, 
		descriptor_ptr->length, writeThrough, cache);
	    return status;
	}
	if(descriptor_ptr->class == CLASS_A) //Handle arrays
	{
	    arrPtr = (struct descriptor_a *)descriptor_ptr;
	    status = putRecord(nid, arrPtr->dtype, arrPtr->arsize/arrPtr->length, arrPtr->pointer,
		arrPtr->arsize, writeThrough, cache);
	    return status;
	}
	
	MdsSerializeDscOut(descriptor_ptr, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;
	status = putRecord(nid, 0, 0, arrPtr->pointer, arrPtr->arsize, writeThrough, cache);
	MdsFree1Dx(&ser_xd, 0);
	return status;
}



EXPORT int RTreeFlush()
{
	if(!cache) cache = getCache();
	return flushTree(cache);
}


EXPORT char *RTreeSetCallback(int nid, void (*callback)(int))
{
	if(!cache) cache = getCache();
	return setCallback(nid, callback, cache);
}

EXPORT int RTreeClearCallback(int nid, char *callbackDescr)
{
	if(!cache) cache = getCache();
	return clearCallback(nid, callbackDescr, cache);
}

EXPORT int RTreeSetWarm(int nid, int warm)
{
	if(!cache) cache = getCache();
	return setWarm(nid, warm, cache);
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



	status = getRecord(nid, &dataType, &numSamples, &data, &dataLen, cache);
	if(!(status & 1) || dataLen == 0)
	{
		if(treeGetRecord)
		{
			status = treeGetRecord(nid, dsc_ptr);
		}
		else status = 0;
			
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
	if(!treePutRecord) return 0;
	if(dataType) //If scalar or array
	{
	    retDescr = rebuildDataDescr(dataType, numSamples, data, size, &descr, (struct descriptor_a *)&descrA);
	    status = treePutRecord(nid, retDescr);
	}
	else
	{
	    status = MdsSerializeDscIn((char *)data, &xd);
	    if(status & 1)
		status = treePutRecord(nid, (struct descriptor *)&xd);
	    MdsFree1Dx(&xd, 0);
	}
	return status;
}



int handleSegmentInternal(int nid)
{
	EMPTYXD(xd);
	int status = 1;

	if(getTimedRecord && treePutRecord)
	{
		status = getTimedRecord(nid, 0, 0, 0, &xd);
		if(status & 1)
			status = treePutRecord(nid, xd.pointer);
	}
	if(!(status & 1))
		printf("%s\n", MdsGetMsg(status));
	return status;
}



int beginSegmentInternal(int nid, int idx, char *start, char *end, char *dim, char *data)
{
	//Segments not supported in treeshr yet
	return 1;
}
int updateSegmentInternal(int nid, int idx, char *start, char *end)
{
	//Segments not supported in treeshr yet
	return 1;
}

int putSegmentInternal(int nid, int idx, char *dim, char *data)
{
	//Segments not supported in treeshr yet
	return 1;
}


#ifdef HAVE_WINDOWS_H
EXPORT void cacheReset()
{
	if(!cache) cache = getCache();

}
#else

//For Linux only: remove all persistent semaphores
EXPORT void cacheReset()
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






