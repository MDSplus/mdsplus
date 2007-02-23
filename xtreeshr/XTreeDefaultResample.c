#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <xtreeshr.h>


static int lessThan(struct descriptor *in1D, struct descriptor *in2D, char  *less);
static int getMinMax(struct descriptor *dimD, char isMin, struct descriptor_xd *outXd);
extern int TdiCompile();
extern int TdiData();
extern int TdiFloat();
extern int TdiEvaluate();
extern int XTreeConvertToLongTime(struct descriptor *timeD, _int64u *converted);
extern int XTreeConvertToLongDelta(struct descriptor *deltaD, _int64u *converted);

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




//64 bit time-based resampling function. It is assumed here that the 54 bit representation of time is the count
//of a given, fixed amount of time, starting from a given time in the past
//The closest point is selected as representative for a given time
static void resample(_int64u start, _int64u end, _int64u delta, _int64u *timebase, int timebaseSamples, 
					int numDims, int *dims, char *data, int dataSize, char *outData, _int64u *outDim, int *retSamples)
{
	int i, timebaseIdx, outIdx, itemSize, outSamples;
	_int64u refTime, delta1, delta2;

	itemSize = dataSize;
	for(i = 0; i < numDims - 1; i++)
		itemSize *= dims[i];


	if(start < timebase[0])
		start = timebase[0];
	if(end > timebase[timebaseSamples - 1])
		end = timebase[timebaseSamples - 1];

	timebaseIdx = outIdx = outSamples = 0;
	refTime = start;
	while(refTime <= end)
	{
		while(timebaseIdx < timebaseSamples && timebase[timebaseIdx] < refTime)
			timebaseIdx++;
		//Select closest sample
		if(timebaseIdx > 0 && timebaseIdx < timebaseSamples)
		{
			delta1 = timebase[timebaseIdx] - refTime;
			delta2 = refTime - timebase[timebaseIdx - 1];
			if(delta2 < delta1)
				timebaseIdx--;
		}
		memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);
		outDim[outSamples] = refTime;
		outSamples++;
		if(delta > 0)
			refTime += delta;
		else
		{
			if(timebaseIdx < timebaseSamples - 1)
				refTime = timebase[timebaseIdx + 1];
			else
				refTime += 1; //Just to pass over end
		}
	}

}




//The default resample handles int64 timebases
//return 0 if the conversion is  not possible
static _int64u *convertTimebaseToInt64(struct descriptor_signal *inSignalD, int *outSamples)
{
	struct descriptor_a *currDim;
	double *doublePtr;
	float *floatPtr;
	_int64u *outPtr;
	int numSamples, i, status;
	EMPTYXD(currXd);



	currDim = (struct descriptor_a *)inSignalD->dimensions[0];
	//If timebase already using 64 bit int
	if(currDim->class == CLASS_A && (currDim->dtype == DTYPE_Q || currDim->dtype == DTYPE_QU))
	{
		outPtr = malloc(currDim->arsize);
		numSamples = currDim->arsize/currDim->length;
		for(i = 0; i < numSamples; i++)
			outPtr[i] = ((_int64u *)currDim->pointer)[i];
		*outSamples = numSamples;
		return outPtr;
	}

	status = TdiData(currDim, &currXd MDS_END_ARG);
	if(status & 1)
		status = TdiFloat(&currXd, &currXd MDS_END_ARG);

	currDim = (struct descriptor_a *)currXd.pointer;
	if(!(status & 1) || currDim->class != CLASS_A || (currDim->dtype != DTYPE_FLOAT && currDim->dtype != DTYPE_DOUBLE))
		//Cannot perform conversion
	{
		MdsFree1Dx(&currXd, 0);
		return 0;
	}
	numSamples = currDim->arsize/currDim->length;
	outPtr = malloc(8 * numSamples);
	if(currDim->dtype == DTYPE_FLOAT)
	{
		floatPtr = (float *)currDim->pointer;
		for(i = 0; i < numSamples; i++)
			MdsFloatToTime(floatPtr[i], &outPtr[i]);
	}
	else //currDim->dtype == DTYPE_DOUBLE)
	{
		doublePtr = (double *)currDim->pointer;
		for(i = 0; i < numSamples; i++)
			MdsFloatToTime(doublePtr[i], &outPtr[i]);
	}
	*outSamples = numSamples;
	MdsFree1Dx(&currXd, 0);
	return outPtr;
}



EXPORT int XTreeDefaultResample(struct descriptor_signal *inSignalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *deltaD, struct descriptor_xd *outSignalXd)
{
	char resampleExpr[64];
	struct descriptor resampleExprD = {0, DTYPE_T, CLASS_S, resampleExpr};
	struct descriptor_a *arrayD;
	char *shapeExpr = "SHAPE(DATA($1))";
	struct descriptor shapeExprD = {strlen(shapeExpr), DTYPE_T, CLASS_S, shapeExpr};

	_int64u start64, end64, delta64, *timebase64, *outDim;
	float *timebaseFloat;
	int *dims;
	int numDims;
	int numTimebaseSamples;
	int outSamples, itemSize;
	int isFloat = 0;
	int status, i;
	char *outData;
	struct descriptor_a *dataD;
	EMPTYXD(shapeXd);

	DESCRIPTOR_A_COEFF(outDataArray, 0, 0, 0, 255, 0); 
	DESCRIPTOR_A(outDimArray, 0, 0, 0, 0);
	DESCRIPTOR_SIGNAL_1(outSignalD, &outDataArray, 0, &outDimArray);

	if(startD)
	{
		status = XTreeConvertToLongTime(startD, &start64);
		if(!(status & 1))
			return status;
	}
	if(endD)
	{
		status = XTreeConvertToLongTime(endD, &end64);
		if(!(status & 1))
			return status;
	}
	if(deltaD)
	{
		status = XTreeConvertToLongDelta(deltaD, &delta64);
		if(!(status & 1))
			return status;
	}

//This version handles only 64 bit time format
	if(inSignalD->dimensions[0]->dtype != DTYPE_Q && inSignalD->dimensions[0]->dtype != DTYPE_QU)
		isFloat = 1;

	timebase64 = convertTimebaseToInt64(inSignalD, &numTimebaseSamples);
	if(!timebase64) return 0; //Cannot convert timebase to 64 bit int

	if(!startD)
		start64 = timebase64[0];
	if(!endD)
		end64 = timebase64[numTimebaseSamples - 1];

	if(start64 < timebase64[0])
		start64 = timebase64[0];
	if(end64 > timebase64[numTimebaseSamples - 1])
		end64 = timebase64[numTimebaseSamples - 1];

//Get shapes(dimensions) for data
	dataD = (struct descriptor_a *)inSignalD->data;
	status = TdiCompile(&shapeExprD, dataD, &shapeXd MDS_END_ARG);
	if(!(status & 1)) return status;;
	status = TdiData(&shapeXd, &shapeXd MDS_END_ARG);
	if(!(status & 1)) return status;
	arrayD = (struct descriptor_a *)shapeXd.pointer;
	numDims = arrayD->arsize/arrayD->length;
	dims = (int *)arrayD->pointer;
	//Make sure enough room is allocated
	if(!deltaD)
		outSamples = dataD->arsize / dataD->length;
	else 
		outSamples = (end64 - start64)/delta64 + 1;

	itemSize = dataD->length;
	for(i = 0; i < numDims-1; i++)
		itemSize *= dims[i];
	outData = malloc(outSamples * itemSize);
	outDim = malloc(outSamples * 8);


	resample(start64, end64, (deltaD)?delta64:0, timebase64, numTimebaseSamples, numDims, dims, 
		dataD->pointer, dataD->length, outData, outDim, &outSamples);

	//Build array descriptor for out data
	outDataArray.length = dataD->length;
	outDataArray.dtype = dataD->dtype;
	outDataArray.pointer = outData;
	outDataArray.arsize = itemSize*outSamples;
	outDataArray.dimct = numDims;
	for(i = 0; i < numDims-1; i++)
	{
		outDataArray.m[i] = dims[i];
	}
	outDataArray.m[numDims - 1] = outSamples;

	//If originally float, convert  dimension to float
	if(isFloat)
	{
		timebaseFloat = (float *)malloc(outSamples * sizeof(float));
		for(i = 0; i < outSamples; i++)
			MdsTimeToFloat(outDim[i], &timebaseFloat[i]);
		outDimArray.length = sizeof(float);
		outDimArray.arsize = sizeof(float) * outSamples;
		outDimArray.dtype = DTYPE_FLOAT;
		outDimArray.pointer = (char *)timebaseFloat;
	}
	else
	{
		outDimArray.length = 8;
		outDimArray.arsize = 8 * outSamples;
		outDimArray.dtype = DTYPE_QU;
		outDimArray.pointer = (char *)outDim;
	}

	
	MdsCopyDxXd((struct descriptor *)&outSignalD, outSignalXd);
	free((char *)timebase64);
	free((char *)outDim);
	if(isFloat)
		free((char *)timebaseFloat);
	free((char *)outData);
	MdsFree1Dx(&shapeXd, 0);
	

	
	
	return 1;
}

