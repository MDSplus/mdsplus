#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <xtreeshr.h>

#define INTERPOLATION 1
#define CLOSEST_SAMPLE 2
#define PREVIOUS_SAMPLE 3

static int lessThan(struct descriptor *in1D, struct descriptor *in2D, char  *less);
static int getMinMax(struct descriptor *dimD, char isMin, struct descriptor_xd *outXd);
extern int TdiCompile();
extern int TdiData();
extern int TdiFloat();
extern int TdiEvaluate();
extern int XTreeConvertToLongTime(struct descriptor *timeD, _int64u *converted);
extern int XTreeConvertToLongDelta(struct descriptor *deltaD, _int64u *converted);

static int XTreeDefaultResampleMode(struct descriptor_signal *inSignalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *deltaD, char mode, struct descriptor_xd *outSignalXd);


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
static void resample(_int64 start, _int64 end, _int64 delta, _int64 *inTimebase, int inTimebaseSamples, 
					int numDims, int *dims, char *inData, int dataSize, char dataType, int mode, char *outData, _int64 *outDim, int *retSamples)
{
	int i, timebaseIdx, outIdx, itemSize, outSamples, startIdx, timebaseSamples;
	_int64 refTime, delta1, delta2;
	_int64 *timebase;
	char *data;
	int numDataItems;

	double prevData, nextData, currData;



	itemSize = dataSize;
	for(i = 0; i < numDims-1; i++)
		itemSize *= dims[i];

	numDataItems = itemSize/dataSize;

	if(start < inTimebase[0])
		start = inTimebase[0];
	if(end > inTimebase[inTimebaseSamples - 1])
		end = inTimebase[inTimebaseSamples - 1];

	timebaseIdx = outIdx = outSamples = 0;

	
	for(startIdx = 0; startIdx < inTimebaseSamples && inTimebase[startIdx] < start; startIdx++);

	if(startIdx == inTimebaseSamples) //Not possible in any case
	{
		*retSamples = 0;
		return;
	}



	timebase = &inTimebase[startIdx];
	timebaseSamples  = inTimebaseSamples - startIdx;
	data = &inData[startIdx * itemSize];


	refTime = start;
	if(delta)
	{
		while(refTime <= end)
		{
			while(timebaseIdx < timebaseSamples && timebase[timebaseIdx] < refTime)
				timebaseIdx++;
			switch(mode) 
			{
				case CLOSEST_SAMPLE:
				//Select closest sample
					if(timebaseIdx > 0 && timebaseIdx < timebaseSamples)
					{
						delta1 = timebase[timebaseIdx] - refTime;
						delta2 = refTime - timebase[timebaseIdx - 1];
						if(delta2 < delta1)
							timebaseIdx--;
					}
					memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);
					break;
				case PREVIOUS_SAMPLE:
					//Select closest sample
					if(timebaseIdx > 0 && timebaseIdx < timebaseSamples)
					{
							timebaseIdx--;
					}
					memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);
					break;
				case INTERPOLATION:
					switch(dataType) 
					{
						case DTYPE_BU :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((unsigned char *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((unsigned char *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned char *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_B :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((char *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((char *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((char *)&outData[outSamples * itemSize])[i] = currData;
							}
							break;
						case DTYPE_WU :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((unsigned short *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((unsigned short *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned short *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_W :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((short *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((short *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((short *)&outData[outSamples * itemSize])[i] = currData;
							}
							break;
						case DTYPE_LU :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((unsigned int *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((unsigned int *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned int *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_L :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((int *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((int *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned int *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_QU :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((_int64 *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((_int64 *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((_int64u *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_Q :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((_int64 *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((_int64 *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((_int64 *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_FLOAT :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((float *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((float *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((float *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_DOUBLE :
							for(i = 0; i < numDataItems; i++)
							{
								prevData = ((double *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((double *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
									/(timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((double *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						default: //Not able to do interpolation
							if(timebaseIdx > 0 && timebaseIdx < timebaseSamples)
							{
									timebaseIdx--;
							}
							memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);

					}
					break;
			}
			outDim[outSamples] = refTime;
			outSamples++;
			refTime += delta;
		}
		*retSamples = outSamples;
	}
	else
	{
		while(timebaseIdx < *retSamples && timebase[timebaseIdx] <= end)
		{
			memcpy(&outData[timebaseIdx * itemSize], &data[timebaseIdx * itemSize], itemSize);
			outDim[timebaseIdx] = timebase[timebaseIdx];
			timebaseIdx++;
		}
		*retSamples = timebaseIdx;
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
	//otherwise evaluate it
	status = TdiData(currDim, &currXd MDS_END_ARG);
	if(status & 1)
	{
		currDim = (struct descriptor_a *)currXd.pointer;
		if(currDim->class == CLASS_A && (currDim->dtype == DTYPE_Q || currDim->dtype == DTYPE_QU))
		{
			outPtr = malloc(currDim->arsize);
			numSamples = currDim->arsize/currDim->length;
			for(i = 0; i < numSamples; i++)
				outPtr[i] = ((_int64u *)currDim->pointer)[i];
			*outSamples = numSamples;
			MdsFree1Dx(&currXd, 0);
			return outPtr;
		}

		status = TdiFloat(&currXd, &currXd MDS_END_ARG);
	}
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


//Handle resampling in when dimension is specified as a range
typedef ARRAY_COEFF(char, 256) Array_coeff_type;
static int rangeResample(struct descriptor *startD, struct descriptor *endD, struct descriptor *deltaD, 
			struct descriptor *dataD_in, struct descriptor_range *rangeD, 
			char dataType, char mode, struct descriptor_xd *outSignalXd)
{
	int dataSize, numOutSamples, status, i, memSize, currOffset, numInSamples, totInSamples;
	_int64 start64, end64, delta64, rangeStart64, rangeEnd64, rangeDelta64, currTime64, inTime64;
	Array_coeff_type *dataD = (Array_coeff_type *)dataD_in;
	char *signalExpr = "MAKE_SIGNAL($1,,MAKE_RANGE(MAX($2, $3), MIN($4, $5), $6))";
	struct descriptor signalExprD = {strlen(signalExpr), DTYPE_T, CLASS_S, signalExpr};

	char *prevDataPtr, *nextDataPtr;
	double currSample, prevSample, nextSample;
	_int64 prevTime64, nextTime64;
	int numDataItems;
	int previousClosest = 1;

	char *currDataPtr, *outDataPtr, *currOutDataPtr;
	DESCRIPTOR_A_COEFF(outDataArray, 0, 0, 0, 255, 0); 

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
	//Range descriptor must be fully specified
	if(!rangeD->begin || !rangeD->ending || !rangeD->deltaval)
		return 0;
		
	status = XTreeConvertToLongTime(rangeD->begin, &rangeStart64);
	if(!(status & 1))
		return status;
	status = XTreeConvertToLongTime(rangeD->ending, &rangeEnd64);
	if(!(status & 1))
		return status;
	status = XTreeConvertToLongDelta(rangeD->deltaval, &rangeDelta64);
	if(!(status & 1))
		return status;

	dataSize = dataD->length;
	for(i = 0; i < dataD->dimct-1; i++)
		dataSize *= dataD->m[i];
	numDataItems = dataSize/dataD->length;

	totInSamples = dataD->arsize/dataD->length;
	if(!startD || start64 < rangeStart64)
		start64 = rangeStart64;
	if(!endD || end64 > rangeEnd64)
		end64 = rangeEnd64;
	if(!deltaD)
		delta64 = rangeDelta64;

	currDataPtr = dataD->pointer;
	currDataPtr += (dataSize * (start64 - rangeStart64)/rangeDelta64);
	currTime64 = inTime64 = start64;
	memSize = dataSize * ((end64 - start64)/delta64 + 1);
	currOutDataPtr = outDataPtr = malloc(memSize);
	numOutSamples = numInSamples = 0;
	currOffset = 0;
	while(numInSamples < totInSamples && currTime64 < end64)
	{
		prevTime64 = inTime64 - rangeDelta64;
		nextTime64 = inTime64;
		switch(mode) 
		{
			case PREVIOUS_SAMPLE:
				memcpy(currOutDataPtr, currDataPtr, dataSize);
				break;
			case CLOSEST_SAMPLE:
				if(currDataPtr == dataD->pointer)
				{
					memcpy(currOutDataPtr, currDataPtr, dataSize);
					break;
				}
				if((numInSamples < totInSamples) && ((currTime64 - prevTime64) > (nextTime64 - currTime64)))
					memcpy(currOutDataPtr, currDataPtr+dataSize, dataSize);
				else
					memcpy(currOutDataPtr, currDataPtr, dataSize);
				break;

				
			case INTERPOLATION:
				if(currDataPtr == dataD->pointer)
				{
					memcpy(currOutDataPtr, currDataPtr, dataSize);
					break;
				}
				prevDataPtr = currDataPtr - dataSize;
				nextDataPtr = currDataPtr;
				switch(dataType) 
				{
					case DTYPE_BU:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((unsigned char *)prevDataPtr)[i];
							nextSample = ((unsigned char *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((unsigned char *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_B:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((char *)prevDataPtr)[i];
							nextSample = ((char *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((char *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_WU:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((unsigned short *)prevDataPtr)[i];
							nextSample = ((unsigned short *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((unsigned short *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_W:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((short *)prevDataPtr)[i];
							nextSample = ((short *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((short *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_LU:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((unsigned int *)prevDataPtr)[i];
							nextSample = ((unsigned int *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((unsigned int *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_L:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((int *)prevDataPtr)[i];
							nextSample = ((int *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((int *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_QU:
					case DTYPE_Q:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((_int64 *)prevDataPtr)[i];
							nextSample = ((_int64 *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((_int64 *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_FLOAT:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((float *)prevDataPtr)[i];
							nextSample = ((float *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((float *)currOutDataPtr)[i] = currSample;
						}
						break;
					case DTYPE_DOUBLE:
						for(i = 0; i < numDataItems; i++)
						{
							prevSample = ((double *)prevDataPtr)[i];
							nextSample = ((double *)nextDataPtr)[i];
							currSample = prevSample + (nextSample - prevSample)*(currTime64 - prevTime64)/(nextTime64 - prevTime64);
							((double *)currOutDataPtr)[i] = currSample;
						}
						break;
					default: //Not able to do interpolation
						memcpy(currOutDataPtr, currDataPtr, dataSize);
				}
		}
		currOutDataPtr += dataSize;
		currOffset += dataSize;
		currTime64 += delta64;
		while(numInSamples < totInSamples && inTime64 < currTime64)
		{
			inTime64 += rangeDelta64;
			currDataPtr += dataSize;
			numInSamples++;
		}
		numOutSamples++;
	}
	outDataArray.length = dataD->length;
	outDataArray.dtype = dataD->dtype;
	outDataArray.pointer = outDataPtr;
	outDataArray.arsize = dataSize*numOutSamples;
	outDataArray.dimct = dataD->dimct;
	for(i = 0; i < dataD->dimct-1; i++)
	{
		outDataArray.m[i] = dataD->m[i];
	}
	outDataArray.m[dataD->dimct - 1] = numOutSamples;

//Build out signal descriptor, keeping the same data type as passed
	status = TdiCompile(&signalExprD, &outDataArray, (startD)?startD:rangeD->begin, rangeD->begin, 
		(endD)?endD:rangeD->ending, rangeD->ending, 
		(deltaD)?deltaD:rangeD->deltaval, outSignalXd MDS_END_ARG);

	if((status & 1))
		status = TdiEvaluate(outSignalXd, outSignalXd MDS_END_ARG);
	free(outDataPtr);
	return status;
}



EXPORT struct descriptor_xd *XTreeResampleClosest(struct descriptor_signal *inSignalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *deltaD)
{
	static EMPTYXD(retXd);
	int status = XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, CLOSEST_SAMPLE, &retXd);
	return &retXd;
}

EXPORT struct descriptor_xd *XTreeResamplePrevious(struct descriptor_signal *inSignalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *deltaD)
{
	static EMPTYXD(retXd);
	int status = XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, PREVIOUS_SAMPLE, &retXd);
	return &retXd;
}




EXPORT int XTreeDefaultResample(struct descriptor_signal *inSignalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *deltaD, struct descriptor_xd *outSignalXd)
{
	return XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, INTERPOLATION, outSignalXd);
}



static int XTreeDefaultResampleMode(struct descriptor_signal *inSignalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *deltaD, char mode, struct descriptor_xd *outSignalXd)
{
	char resampleExpr[64];
	struct descriptor resampleExprD = {0, DTYPE_T, CLASS_S, resampleExpr};
	struct descriptor_a *arrayD;
	char *shapeExpr = "SHAPE(DATA($1))";
	struct descriptor shapeExprD = {strlen(shapeExpr), DTYPE_T, CLASS_S, shapeExpr};

	_int64u start64, end64, delta64, *timebase64, *outDim;
//	float *timebaseFloat;
	double *timebaseDouble;
	int *dims;
	int numDims;
	int numTimebaseSamples;
	int outSamples, itemSize;
	int isFloat = 0;
	int status, i, currIdx;
	char *outData;
	struct descriptor_a *dataD;
	EMPTYXD(shapeXd);
	EMPTYXD(dataXd);

	DESCRIPTOR_A_COEFF(outDataArray, 0, 0, 0, 255, 0); 
	DESCRIPTOR_A(outDimArray, 0, 0, 0, 0);
	DESCRIPTOR_SIGNAL_1(outSignalD, &outDataArray, 0, &outDimArray);

//Removed Range management due to the inaccurate reconstruction of TDISHR

/*	if(inSignalD->dimensions[0]->class == CLASS_R && inSignalD->dimensions[0]->dtype == DTYPE_RANGE && 
		((struct descriptor_range *)inSignalD->dimensions[0])->begin && 
		((struct descriptor_range *)inSignalD->dimensions[0])->ending && 
		((struct descriptor_range *)inSignalD->dimensions[0])->deltaval)
	{
		dataD = (struct descriptor_a *)inSignalD->data;
		return rangeResample(startD, endD, deltaD, inSignalD->data, (struct descriptor_range *)inSignalD->dimensions[0], 
			dataD->dtype, mode, outSignalXd); 
	}
*/
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
	if (inSignalD->data->class == CLASS_A) {
	  dataD = (struct descriptor_a *)inSignalD->data;
	} else {
	  status = TdiData(inSignalD->data, &dataXd MDS_END_ARG);
	  if (status & 1)
	    dataD = (struct descriptor_a *)dataXd.pointer;
	  else
	    return status;
	}
	status = TdiCompile(&shapeExprD, dataD, &shapeXd MDS_END_ARG);
	if(!(status & 1)) {
	  MdsFree1Dx(&dataXd,0);
	  return status;
	}
	status = TdiData(&shapeXd, &shapeXd MDS_END_ARG);
	if(!(status & 1)) {
	  MdsFree1Dx(&dataXd,0);
	  return status;
	}
	arrayD = (struct descriptor_a *)shapeXd.pointer;
	numDims = arrayD->arsize/arrayD->length;
	dims = (int *)arrayD->pointer;
	itemSize = dataD->length;
	for(i = 0; i < numDims-1; i++)
		itemSize *= dims[i];
	//Make sure enough room is allocated
	if(!deltaD)
	{
		//Count number of copied samples
		for(currIdx = 0; currIdx < numTimebaseSamples && timebase64[currIdx] < start64; currIdx++);
		for(outSamples = 0; currIdx < numTimebaseSamples && timebase64[currIdx] <= end64; currIdx++, outSamples++);
	}
	else 
		outSamples = (end64 - start64)/delta64 + 1;

	outData = malloc(outSamples * itemSize);
	outDim = malloc(outSamples * 8);

	//Check data array too short
	if(dataD->arsize/dataD->length < numTimebaseSamples)
		numTimebaseSamples = dataD->arsize/dataD->length;

	resample(start64, end64, (deltaD)?delta64:0, timebase64, numTimebaseSamples, numDims, dims, 
		dataD->pointer, dataD->length, dataD->dtype, mode, outData, outDim, &outSamples);


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
	outDataArray.m[numDims-1] = outSamples;
/* OLD, Possible loss of precision!!!!
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
*/
	//If originally float, convert  dimension to float
	if(isFloat)
	{
		timebaseDouble = (float *)malloc(outSamples * sizeof(double));
		for(i = 0; i < outSamples; i++)
			MdsTimeToDouble(outDim[i], &timebaseDouble[i]);
		outDimArray.length = sizeof(double);
		outDimArray.arsize = sizeof(double) * outSamples;
		outDimArray.dtype = DTYPE_DOUBLE;
		outDimArray.pointer = (char *)timebaseDouble;
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
		free((char *)timebaseDouble);
	free((char *)outData);
	MdsFree1Dx(&shapeXd, 0);
	MdsFree1Dx(&dataXd, 0);
	

	
	
	return 1;
}

