#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <opcopcodes.h>




extern void *getCache();
extern int putRecord(int nid, char *data, int size, int writeThrough, void *cachePtr);
extern int getRecord(int nid, char **data, int *size, void *cachePtr);
extern int putRecordInternal(int nid, char *data, int size);
extern int flushTree(void *cachePtr);
extern void *setCallback(int nid, void (* callback)(int), void *cachePtr);
extern void clearCallback(int nid, void *callbackDescr, void *cachePtr);

extern int setCurrDim(int nid,  char *dim, int dimSize, void *cachePtr);
extern int appendRecord(int nid,  char *data, int size, void *cachePtr);
extern int getRecordInfo(int nid, int *numItems, int *isTimed, void *cachePtr);
extern int getRecords(int nid, char **data, int *sizes, char **dims, int *dimSizes, void *cachePtr);

extern int TdiCompile();
extern int TdiData();
extern int TdiEvaluate();
extern int TdiDecompile();

static int mergeSignals1D(struct descriptor_a *signalsApd, struct descriptor *startD, 
						  struct descriptor *endD, struct descriptor_xd *outXd);

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




static void *cache;


//Get the shape 
static int *getShape(struct descriptor *data, int *outDims)
{
	int status;
	char *shapeExpr = "SHAPE(DATA($1))";
	struct descriptor shapeD = {strlen(shapeExpr), DTYPE_T, CLASS_S, shapeExpr};
	EMPTYXD(shapeXd);
	struct descriptor_a *shapePtr;
	int *shape;
	int numDims;

	status = TdiCompile(&shapeD, data, &shapeXd MDS_END_ARG);
	if(!(status & 1)) return 0;

	status = TdiData(&shapeXd, &shapeXd MDS_END_ARG);
	if(!(status & 1)) return 0;

	shapePtr = (struct descriptor_a *)shapeXd.pointer;
	if(!shapePtr || shapePtr->class != CLASS_A || shapePtr->dtype != DTYPE_L)
	{
		printf("Internal error in getShape: returned shape not an integer array\n");
		MdsFree1Dx(&shapeXd, 0);
		*outDims = 0;
		return 0;
	}
	if(shapePtr->arsize == 0) //It is a scalar
	{
		MdsFree1Dx(&shapeXd, 0);
		*outDims = 0;
		return 0;
	}
	numDims = shapePtr->arsize/shapePtr->length;
	shape = (int *)malloc(numDims * sizeof(int));
	memcpy(shape, shapePtr->pointer, numDims * sizeof(int));
	MdsFree1Dx(&shapeXd,0);
	*outDims = numDims;
	return shape;
}



//Check two arrays for equality	
static int areCompatible(struct descriptor *dscPtr1, struct descriptor *dscPtr2)
{
	int i;
	int dim1, dim2;
	int *dims1, *dims2;
	int compatible;

	if(dscPtr1->class != dscPtr2->class) return 0;
	if(dscPtr1->class != CLASS_S && dscPtr1->class != CLASS_A) return 0; //Only scalars and arrays are allowed
	
	if(dscPtr1->dtype != dscPtr2->dtype) return 0; //Must be of the same dtype

	dims1 = getShape(dscPtr1, &dim1);
	dims2 = getShape(dscPtr2, &dim2);
	
	compatible = 1;
	if(dim1 == dim2) 
	{
		for(i = 0; i < dim1; i++)
		{
			if(dims1[i] != dims2[i]) 
				compatible = 0;
		}
	}
	else
		compatible = 0;
	if(dims1) free((char *)dims1);
	if(dims2) free((char *)dims2);
	return compatible;
}





//Get one or more data chunks and return the MDS descriptor. If more than one data chumk, make an array one dimension greater
//The dimension of every data chunk must be THE SAME
static int packArray(char **data, int *sizes, int numDataItems, struct descriptor_xd *outXd)
{
	static EMPTYXD(templateXd);
	int i;
	struct descriptor_xd *xds;
	int status = 1, totSize;
	DESCRIPTOR_A(arrayD, 0, 0, 0, 0);
	char *outData;
	int currOffset;
	int *dims;
	int numDims;
	char setRangeExpr[1024];
	struct descriptor setRangeExprD;

	if(numDataItems == 0)
	{
		MdsCopyDxXd((struct descriptor *)&templateXd, outXd); //Empty 
		return 1;
	}

	xds = malloc(sizeof(struct descriptor_xd) * numDataItems);

	//Unpack data into descriptors	
	for(i = 0; i < numDataItems; i++)
	{
		memcpy(&xds[i], &templateXd, sizeof(struct descriptor_xd));
		status = MdsSerializeDscIn((char *)data[i], &xds[i]);
	}

	//Check that they all have the same dimensions
	
	for(i = 1; i < numDataItems; i++)
	{
		if(!areCompatible(xds[i].pointer, xds[0].pointer))
		{
			printf("Incompatible dimension in data chuncks\n");
			status = 0;
		}
	}

	//Get total data size
	totSize = 0;
	for(i = 0; i < numDataItems; i++)
	{
		if(xds[i].pointer->class == CLASS_S) 
			totSize += xds[i].pointer->length;
		else
			totSize += ((struct descriptor_a *)xds[i].pointer)->arsize;
	}
	outData = malloc(totSize);
	
	
	if(status & 1)
	{
	
		if(xds[0].pointer->class == CLASS_S) //Handle Scalars)
		{
			arrayD.dtype = xds[0].pointer->dtype;
			arrayD.length = xds[0].pointer->length;
			arrayD.arsize = totSize;
			currOffset = 0;
			for(i = 0; i < numDataItems; i++)
			{
				memcpy(&outData[currOffset], xds[i].pointer->pointer, xds[i].pointer->length);
				currOffset += xds[i].pointer->length;
			}
			arrayD.pointer = outData;
			status = MdsCopyDxXd((struct descriptor *)&arrayD, outXd);
		}
		else //Handle arrays
		{
			dims = getShape(xds[0].pointer, &numDims);

			//Build SET_RANGE expression
			//sprintf(setRangeExpr, "SET_RANGE(%d,", numDataItems);
			sprintf(setRangeExpr, "SET_RANGE(");
			for(i = 0; i < numDims; i++)
			{
				sprintf(&setRangeExpr[strlen(setRangeExpr)], "%d,", dims[i]); 
			}


			sprintf(&setRangeExpr[strlen(setRangeExpr)], "%d,$1)", numDataItems);
			//sprintf(&setRangeExpr[strlen(setRangeExpr)], "$1)", numDataItems);

			//Build linear array
			arrayD.dtype = xds[0].pointer->dtype;
			arrayD.length = xds[0].pointer->length;
			arrayD.arsize = totSize;
			currOffset = 0;
			for(i = 0; i < numDataItems; i++)
			{
				memcpy(&outData[currOffset], ((struct descriptor_a *)xds[i].pointer)->pointer, 
					((struct descriptor_a *)xds[i].pointer)->arsize);
				currOffset += ((struct descriptor_a *)xds[i].pointer)->arsize;
			}
			arrayD.pointer = outData;

			setRangeExprD.dtype = DTYPE_T;
			setRangeExprD.class = CLASS_S;
			setRangeExprD.length = strlen(setRangeExpr);
			setRangeExprD.pointer = setRangeExpr;

			status = TdiCompile(&setRangeExprD, &arrayD, outXd MDS_END_ARG);
			if(status & 1)
				status = TdiData(outXd, outXd MDS_END_ARG);
		}
	}
	free(outData);
	for(i = 0; i < numDataItems; i++)
		MdsFree1Dx(&xds[i], 0);
	free(xds);

	return status;
}

//Get (portion of) 1D signal included betweel start and end 
static int getBoundedSignal(struct descriptor_signal *signalD, struct descriptor *startD, struct descriptor *endD, 
							struct descriptor_xd *boundedSigXd)
{
	char boundExpr[64];
	struct descriptor boundExprD = {0, DTYPE_T, CLASS_S, boundExpr};
	char sizeExpr[64];
	struct descriptor sizeExprD = {0, DTYPE_T, CLASS_S, sizeExpr};
	char dataExpr[64];
	struct descriptor dataExprD = {0, DTYPE_T, CLASS_S, dataExpr};
	char dimExpr[64];
	struct descriptor dimExprD = {0, DTYPE_T, CLASS_S, dimExpr};
	int status;
	EMPTYXD(sizeXd);
	int size = 0;

	printf("PRIMA DI BOUNDED: ");
	printDecompiled(signalD);

	if(startD && endD)
	{
		sprintf(boundExpr, "$1[($2):($3)]");
		boundExprD.length = strlen(boundExpr);
		status = TdiCompile(&boundExprD, signalD, startD, endD, boundedSigXd MDS_END_ARG);
		if(status & 1) status = TdiEvaluate(boundedSigXd, boundedSigXd MDS_END_ARG);
	}
	else if(startD)
	{
		sprintf(boundExpr, "$1[($2):*]");
		boundExprD.length = strlen(boundExpr);
		status = TdiCompile(&boundExprD, signalD, startD, boundedSigXd MDS_END_ARG);
		if(status & 1) status = TdiEvaluate(boundedSigXd, boundedSigXd MDS_END_ARG);
	}
	else if(endD)
	{
		sprintf(boundExpr, "$1[*:($2)]");
		boundExprD.length = strlen(boundExpr);
		status = TdiCompile(&boundExprD, signalD, endD, boundedSigXd MDS_END_ARG);
		if(status & 1) status = TdiEvaluate(boundedSigXd, boundedSigXd MDS_END_ARG);
	}
	else
		status = MdsCopyDxXd((struct descriptor *)signalD, boundedSigXd);

	sprintf(sizeExpr, "SIZE($1)");
	sizeExprD.length = strlen(sizeExpr);
	status = TdiCompile(&sizeExprD, boundedSigXd, &sizeXd MDS_END_ARG);
	if(status & 1) status = TdiData(&sizeXd, &sizeXd MDS_END_ARG);
	if(sizeXd.pointer->dtype == DTYPE_L && sizeXd.pointer->class == CLASS_S)
		size = *(int *)sizeXd.pointer->pointer;
	
	MdsFree1Dx(&sizeXd, 0);

	printf("BOUNDED: ");
	printDecompiled(boundedSigXd);

	return size;
}



//Get the merged 1D signal, between start and end. Handle null pointers for start and end
EXPORT struct descriptor_xd *RTreeMergeSignals1D(struct descriptor_xd *signalsXd, float *start, float *end)
{
	static EMPTYXD(outXd);
	int status;
	struct descriptor startD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)start};
	struct descriptor endD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)end};


	status = mergeSignals1D((struct descriptor_a *)signalsXd->pointer, &startD, &endD, &outXd);
	return &outXd;
}


static int mergeSignals1D(struct descriptor_a *signalsApd, struct descriptor *startD, 
						  struct descriptor *endD, struct descriptor_xd *outXd)
{
	int numSignals;

	int i, j, totSamples, status;
	EMPTYXD(currSignalXd);
	static EMPTYXD(templateXd);
	EMPTYXD(mergeVarXd);
	EMPTYXD(caccaXd);
	struct descriptor_xd *boundedXds;
	int *samples;
	char mergeExprData[1024];
	char mergeExprDim[1024];
	char mergeExpr[1024];
	struct descriptor mergeExprD = {0, DTYPE_T, CLASS_S, mergeExpr};
	char mergeVarExpr[1024];
	struct descriptor mergeVarExprD = {0, DTYPE_T, CLASS_S, mergeVarExpr};
	struct descriptor_signal **signalsD;

	int *sigDims;
	int numSigDims, numDataArrays;

printf("PARTE MERGE\n");
printDecompiled(signalsApd);


	numSignals = signalsApd->arsize / signalsApd->length;
	if(numSignals == 0)
	{
		MdsCopyDxXd((struct descriptor *)&templateXd, outXd);
		return 1;
	}


	signalsD = (struct descriptor_signal **)signalsApd->pointer;
	boundedXds = malloc(numSignals * sizeof(struct descriptor_xd));
	samples = malloc(numSignals * sizeof(int));
	totSamples = 0;
	for(i = 0; i < numSignals; i++)
	{
		memcpy(&boundedXds[i], &templateXd, sizeof(struct descriptor_xd));
		samples[i] = getBoundedSignal(signalsD[i], startD, endD, &boundedXds[i]);
		totSamples += samples[i];
	}

	if(totSamples > 0)
	{
		if(numSignals > 1) //If different signals must be merged
		{

			sigDims = getShape((struct descriptor *)signalsD[0], &numSigDims);
			if(numSigDims < 2)
			{
				sprintf(mergeExprData, "SET_RANGE(%d,[", totSamples);
				sprintf(mergeExprDim, "SET_RANGE(%d,[", totSamples);
			}
			else
			{

				sprintf(mergeExprData, "SET_RANGE(", totSamples);
				numDataArrays = totSamples;
				for(i = 1; i < numSigDims; i++)
				{
					numDataArrays /= sigDims[i];
				}
		//		sprintf(mergeExprData, "SET_RANGE(%d,", numDataArrays);

				for(i = 1; i < numSigDims; i++)
				{
					sprintf(&mergeExprData[strlen(mergeExprData)], "%d,", sigDims[i]);
				}

				sprintf(&mergeExprData[strlen(mergeExprData)], "%d,[",numDataArrays);
//				sprintf(&mergeExprData[strlen(mergeExprData)], "[");
				sprintf(mergeExprDim, "SET_RANGE(%d,[", numDataArrays);
			}

			if(sigDims) free((char *)sigDims);

			for(i = j = 0; i < numSignals; i++)
			{
				if(samples[i] > 0)
				{
					sprintf(mergeVarExpr, "_MERGE_SIG_DATA%d = DATA($1)", j);
					mergeVarExprD.length = strlen(mergeVarExpr);

					status = TdiCompile(&mergeVarExprD, boundedXds[i].pointer, &mergeVarXd MDS_END_ARG);
					if(status & 1)
						status = TdiData(&mergeVarXd, &mergeVarXd MDS_END_ARG);

					MdsFree1Dx(&mergeVarXd,0);
					sprintf(mergeVarExpr, "_MERGE_SIG_DIM%d = DIM_OF($1)", j);
					mergeVarExprD.length = strlen(mergeVarExpr);

					status = TdiCompile(&mergeVarExprD, boundedXds[i].pointer, &mergeVarXd MDS_END_ARG);
					if(status & 1)
						status = TdiData(&mergeVarXd, &mergeVarXd MDS_END_ARG);

					
					MdsFree1Dx(&mergeVarXd,0);


					if(j == 0)
					{
						sprintf(&mergeExprData[strlen(mergeExprData)], "_MERGE_SIG_DATA%d", j);
						sprintf(&mergeExprDim[strlen(mergeExprDim)], "_MERGE_SIG_DIM%d", j);
					}
					else
					{
						sprintf(&mergeExprData[strlen(mergeExprData)], ",_MERGE_SIG_DATA%d", j);
						sprintf(&mergeExprDim[strlen(mergeExprDim)], ",_MERGE_SIG_DIM%d", j);
					}
					j++;
				}
			}
			sprintf(&mergeExprData[strlen(mergeExprData)], "])");
			sprintf(&mergeExprDim[strlen(mergeExprDim)], "])");


			sprintf(mergeExpr, "MAKE_SIGNAL(%s,,%s)", mergeExprData, mergeExprDim);
			mergeExprD.length = strlen(mergeExpr);
			status = TdiCompile(&mergeExprD, outXd MDS_END_ARG);
			if(status & 1)
				status = TdiEvaluate(outXd, outXd MDS_END_ARG);
		}else //numSignals == 1
		{
			status = MdsCopyDxXd(boundedXds[0].pointer, outXd);
		}
	}
	else
		memcpy(outXd, &templateXd, sizeof(struct descriptor_xd)); //return empty xd
	//Free stuff
	for(i = 0; i < numSignals; i++)
	{
		MdsFree1Dx(&boundedXds[i],0);
	}

	free((char *)boundedXds);
	free((char *)samples);

	if(!(status & 1))
		printf("Merge 1D feiled: %s\n", status);


	return status;
}
	



EXPORT int RTreeSetCurrDimension(int nid, struct descriptor *dimensionD)
{
	EMPTYXD(ser_xd);
	struct descriptor_a *arrPtr;
	int status;

	if(!cache) cache = getCache();
	MdsSerializeDscOut(dimensionD, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;

	status = setCurrDim(nid,  arrPtr->pointer, arrPtr->arsize, cache);
	MdsFree1Dx(&ser_xd, 0);
	return status;
}
	

EXPORT int RTreeAppendData(int nid, struct descriptor *dataD)
{
	EMPTYXD(ser_xd);
	struct descriptor_a *arrPtr;
	int status;

	if(!cache) cache = getCache();
	MdsSerializeDscOut(dataD, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;

	status = appendRecord(nid, arrPtr->pointer, arrPtr->arsize, cache);
	MdsFree1Dx(&ser_xd, 0);
	return status;
}


EXPORT int RTreeGetTimedRecord(int nid, struct descriptor *startD, struct descriptor *endD, 
							   char *squeezeFun, struct descriptor_xd *retXd)
{
	int status, i, j, k;
	int isTimed, numItems;
	char **data = 0, **dims = 0;
	int *sizes = 0, *dimSizes = 0;
	int actNumDims = 0;
	struct descriptor_signal *signals = 0;
	struct descriptor_signal **signalsPtr = 0;
	struct descriptor_xd *dataXds = 0;
	struct descriptor_xd *dimXds = 0;
	DESCRIPTOR_SIGNAL_1(templateSignalD, 0, 0, 0);
	DESCRIPTOR_APD(signalsApd, DTYPE_SIGNAL, 0, 0); 
	static EMPTYXD(templateXd);
	EMPTYXD(currXd);
	EMPTYXD(squeezeXd);
	struct descriptor squeezeExprD = {0, DTYPE_T, CLASS_S, 0};

	unsigned short funCode = OpcExtFunction;
	DESCRIPTOR_R(squeezeFunD, DTYPE_FUNCTION, 5);
	squeezeFunD.length = sizeof(unsigned short);
	squeezeFunD.pointer = (char *)&funCode;


	if(!cache) cache = getCache();


	status = getRecordInfo(nid, &numItems, &isTimed, cache);
	if(!(status & 1)) return status;

	if(!isTimed)   //Handle only timed records
		return 0;

	data = (char **)calloc(numItems, sizeof(char *));
	dims = (char **)calloc(numItems, sizeof(char *));
	sizes = (int *)calloc(numItems, sizeof(int));
	dimSizes = (int *)calloc(numItems, sizeof(int));
	status = getRecords(nid, data, sizes, dims, dimSizes, cache);
	if(status & 1)
	{
	
	//Count actual signal chunks (i.e. actual dimensions) 
		for(i = actNumDims = 0; i < numItems; i++)
		{
			if(dimSizes[i] != 0)
				actNumDims++;
		}

		//allocate signal xds
		signals = (struct descriptor_signal *)malloc(actNumDims * sizeof(struct descriptor_signal));
		signalsPtr = (struct descriptor_signal **)malloc(actNumDims * sizeof(struct descriptor_signal *));
		dataXds = (struct descriptor_xd *)malloc(actNumDims * sizeof(struct descriptor_xd));
		dimXds = (struct descriptor_xd *)malloc(actNumDims * sizeof(struct descriptor_xd));
		for(i = k = 0; k < numItems; i++)
		{
			memcpy(&signals[i], &templateSignalD, sizeof(struct descriptor_signal));
			memcpy(&dataXds[i], &templateXd, sizeof(struct descriptor_xd));
			memcpy(&dimXds[i], &templateXd, sizeof(struct descriptor_xd));
			signalsPtr[i] = &signals[i];
			//Count the number of data items without assocated dimension 
			for(j = 1; k+j < numItems && !dimSizes[k+j]; j++);
			status = packArray(&data[k], &sizes[k], j, &dataXds[i]);

			MdsSerializeDscIn((char *)dims[k], &dimXds[i]);
			signals[i].data = dataXds[i].pointer;
			signals[i].dimensions[0] = dimXds[i].pointer;

			k += j;
		}

		//Create APD of signals
		signalsApd.pointer = (struct descriptor **)signalsPtr;
		signalsApd.arsize = actNumDims * sizeof(struct descriptor_signal *);

		//Build function descriptor taking signals apd, start limit, end limit, is squeezeFun not null
		if(squeezeFun)
		{
			squeezeExprD.length = strlen(squeezeFun);
			squeezeExprD.pointer = squeezeFun;
			squeezeFunD.dscptrs[0] = 0;
			squeezeFunD.dscptrs[1] = &squeezeExprD;
			squeezeFunD.dscptrs[2] = (struct descriptor *)&signalsApd;
			squeezeFunD.dscptrs[3] = startD;
			squeezeFunD.dscptrs[4] = endD;


			status = TdiEvaluate(&squeezeFunD, retXd MDS_END_ARG);
			printf("%s\n", MdsGetMsg(status));

		}
		else //Default 1D squeeze
			status = mergeSignals1D((struct descriptor_a *)&signalsApd, startD, endD, retXd);
			
	}	
	//Free stuff
	for(i = 0; i < actNumDims; i++)
	{
		MdsFree1Dx(&dataXds[i],0);
		MdsFree1Dx(&dimXds[i],0);
	}
	for(i = 0; i < numItems; i++)
	{
		if(data[i]) free((char *)data[i]);
		if(dims[i]) free((char *)dims[i]);
	}
	if(dataXds) free((char *)dataXds);
	if(dimXds) free((char *)dimXds);
	if(signals) free((char *)signals);
	if(signalsPtr) free((char *)signalsPtr);
	if(data) free((char *)data);
	if(dims) free((char *)dims);
	if(sizes) free((char *)sizes);
	if(dimSizes) free((char *)dimSizes);
	return status;
}

/////////////////

EXPORT int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeThrough)
{
	EMPTYXD(ser_xd);

	int status;
	struct descriptor_a *arrPtr;
	if(!cache) cache = getCache();


	

	//Manage Empty descriptor
	if(descriptor_ptr->class == CLASS_XD && ((struct descriptor_xd *)descriptor_ptr)->l_length == 0)
		return putRecord(nid, 0, 0, writeThrough, cache);

	MdsSerializeDscOut(descriptor_ptr, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;
	status = putRecord(nid, arrPtr->pointer, arrPtr->arsize, writeThrough, cache);
	MdsFree1Dx(&ser_xd, 0);
	return status;
}



EXPORT int RTreeFlush()
{
	if(!cache) cache = getCache();
	return flushTree(cache);
}


EXPORT void *RTreeSetCallback(int nid, void (*callback)(int))
{
	if(!cache) cache = getCache();
	return setCallback(nid, callback, cache);
}

EXPORT void RTreeClearCallback(int nid, void *callbackDescr)
{
	if(!cache) cache = getCache();
	clearCallback(nid, callbackDescr, cache);
}




EXPORT int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr)
{
	char *data;
	int dataLen;
	int status;
	int isTimed;
	int numItems;

//printf("RTreeGetRecord\n");
	if(!cache) cache = getCache();


	status = getRecordInfo(nid, &numItems, &isTimed, cache);
	if(!(status & 1)) return status;
	if(isTimed)
		return RTreeGetTimedRecord(nid, 0, 0, 0, dsc_ptr);




	status = getRecord(nid, &data, &dataLen, cache);

//printf("STATUS: %d\n", status);

	if((status & 1))
		MdsSerializeDscIn((char *)data, dsc_ptr);
	return status;
}



int putRecordInternal(int nid, char *data, int size)
{
	int status;
	EMPTYXD(xd);
	status = MdsSerializeDscIn((char *)data, &xd);
	if(status & 1)
		status = TreePutRecord(nid, (struct descriptor *)&xd, 0);
	MdsFree1Dx(&xd, 0);
	return status;
}
