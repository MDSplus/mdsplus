/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
#define MINMAX 4

//static int lessThan(struct descriptor *in1D, struct descriptor *in2D, char *less);
//static int getMinMax(struct descriptor *dimD, char isMin, struct descriptor_xd *outXd);
extern int TdiDecompile();
extern int TdiCompile();
extern int TdiData();
extern int TdiFloat();
extern int TdiEvaluate();
extern int XTreeConvertToLongTime(struct descriptor *timeD, int64_t * converted);
extern int XTreeConvertToLongDelta(struct descriptor *deltaD, int64_t * converted);

static int XTreeDefaultResampleMode(struct descriptor_signal *inSignalD, struct descriptor *startD,
				    struct descriptor *endD, struct descriptor *deltaD, char mode,
				    struct descriptor_xd *outSignalXd);

/*
static void printDecompiled(struct descriptor *inD)
{
  int status;
  EMPTYXD(out_xd);
  char *buf;

  status = TdiDecompile(inD, &out_xd MDS_END_ARG);
  if (!(status & 1)) {
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
*/

//64 bit time-based resampling function. It is assumed here that the 64 bit representation of time is the count
//of a given, fixed amount of time, starting from a given time in the past
//The closest point is selected as representative for a given time
static void resample(int64_t start, int64_t end, int64_t delta, int64_t * inTimebase,
		     int inTimebaseSamples, int numDims, int *dims, char *inData, int dataSize,
		     char dataType, int mode, char *outData, int64_t * outDim, int *retSamples)
{
	int i, j, timebaseIdx, outIdx, itemSize, outSamples, startIdx, timebaseSamples;
	int64_t refTime, delta1, delta2;
	int64_t *timebase;
	char *data;
	int numDataItems;
	int prevTimebaseIdx;

	double prevData, nextData, currData;

	itemSize = dataSize;
	for (i = 0; i < numDims - 1; i++)
		itemSize *= dims[i];

	numDataItems = itemSize / dataSize;

	if (start < inTimebase[0])
		start = inTimebase[0];
	if (end > inTimebase[inTimebaseSamples - 1])
		end = inTimebase[inTimebaseSamples - 1];

	timebaseIdx = outIdx = outSamples = 0;

	for (startIdx = 0; startIdx < inTimebaseSamples && inTimebase[startIdx] < start; startIdx++) ;

	if (startIdx == inTimebaseSamples)	//Not possible in any case
	{
    	*retSamples = 0;
    	return;
  	}

  	timebase = &inTimebase[startIdx];
  	timebaseSamples = inTimebaseSamples - startIdx;
  	data = &inData[startIdx * itemSize];

  	refTime = start;
	if (delta) {
		prevTimebaseIdx = timebaseIdx;
		while (refTime <= end)
		{
			while (timebaseIdx < timebaseSamples && timebase[timebaseIdx] < refTime)
				timebaseIdx++;
			switch (mode) {
				case CLOSEST_SAMPLE:
			//Select closest sample
					if (timebaseIdx > 0 && timebaseIdx < timebaseSamples) {
						delta1 = timebase[timebaseIdx] - refTime;
						delta2 = refTime - timebase[timebaseIdx - 1];
						if (delta2 < delta1)
							timebaseIdx--;
					}
					memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);
					break;
				case PREVIOUS_SAMPLE:
			//Select closest sample
					if (timebaseIdx > 0 && timebaseIdx < timebaseSamples) {
						timebaseIdx--;
					}
					memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);
					break;
				case INTERPOLATION:
                                        if(timebaseIdx <= 0)
						timebaseIdx = 1;  //Avoid referring to negative indexes
					switch (dataType) {
						case DTYPE_BU:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((unsigned char *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((unsigned char *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned char *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_B:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((char *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((char *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((char *)&outData[outSamples * itemSize])[i] = currData;
							}
							break;
						case DTYPE_WU:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((unsigned short *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((unsigned short *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned short *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_W:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((short *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((short *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((short *)&outData[outSamples * itemSize])[i] = currData;
							}
							break;
						case DTYPE_LU:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((unsigned int *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((unsigned int *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((unsigned int *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_L:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((int *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((int *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((int *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_QU:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((uint64_t *) (&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((uint64_t *) (&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((uint64_t *) (&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_Q:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((int64_t *) (&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((int64_t *) (&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((int64_t *) (&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_FLOAT:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((float *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((float *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((float *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
						case DTYPE_DOUBLE:
							for (i = 0; i < numDataItems; i++) {
								prevData = ((double *)(&data[(timebaseIdx - 1) * itemSize]))[i];
								nextData = ((double *)(&data[timebaseIdx * itemSize]))[i];
								currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1])
								/ (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);
								((double *)(&outData[outSamples * itemSize]))[i] = currData;
							}
							break;
					}
					break;
				case MINMAX:     //Two points for every (resampled) sample!!!!!!!!!!!
					switch (dataType) {
						case DTYPE_BU:
							for (i = 0; i < numDataItems; i++) {
								unsigned char currData, minData, maxData;
								minData = maxData = ((unsigned char *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((unsigned char *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((unsigned char *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((unsigned char *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_B:
							for (i = 0; i < numDataItems; i++) {
								char currData, minData, maxData;
								minData = maxData = ((char *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((char *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((char *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((char *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_WU:
							for (i = 0; i < numDataItems; i++) {
								unsigned short currData, minData, maxData;
								minData = maxData = ((unsigned short *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((unsigned short *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((unsigned short *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((unsigned short *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_W:
							for (i = 0; i < numDataItems; i++) {
								short currData, minData, maxData;
								minData = maxData = ((short *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((short *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((short *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((short *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_LU:
							for (i = 0; i < numDataItems; i++) {
								unsigned int currData, minData, maxData;
								minData = maxData = ((unsigned int *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((unsigned int *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((unsigned int *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((unsigned int *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_L:
							for (i = 0; i < numDataItems; i++) {
								int currData, minData, maxData;
								minData = maxData = ((int *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((int *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((int *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((int *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_QU:
							for (i = 0; i < numDataItems; i++) {
								uint64_t currData, minData, maxData;
								minData = maxData = ((uint64_t *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((uint64_t *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((uint64_t *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((uint64_t *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_Q:
							for (i = 0; i < numDataItems; i++) {
								int64_t currData, minData, maxData;
								minData = maxData = ((int64_t *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((int64_t *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((int64_t *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((int64_t *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_FLOAT:
							for (i = 0; i < numDataItems; i++) {
								float currData, minData, maxData;
								minData = maxData = ((float *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((float *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((float *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((float *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
						case DTYPE_DOUBLE:
							for (i = 0; i < numDataItems; i++) {
								double currData, minData, maxData;
								minData = maxData = ((double *)(&data[(prevTimebaseIdx) * itemSize]))[i];
								for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++)
								{
									currData = ((double *)(&data[j * itemSize]))[i];
									if(currData > maxData)
										maxData = currData;
									if(currData < minData)
										minData = currData;
								}
								((double *)(&outData[2*outSamples * itemSize]))[i] = minData;
								((double *)(&outData[(2*outSamples+1) * itemSize]))[i] = maxData;
							}
							break;
					}
					break;
				default:		//Not able to do interpolation
					if (timebaseIdx > 0 && timebaseIdx < timebaseSamples) {
						timebaseIdx--;
					}
					memcpy(&outData[outSamples * itemSize], &data[timebaseIdx * itemSize], itemSize);
			}
			if(mode == MINMAX) //twice the number of points
			{
    			outDim[2*outSamples] = refTime - delta/2;
    			outDim[2*outSamples+1] = refTime;
			}
			else
   				outDim[outSamples] = refTime;
			outSamples++;
    		refTime += delta;
			prevTimebaseIdx = timebaseIdx;
  		}
		if(mode == MINMAX)
 			*retSamples = 2*outSamples;
		else
 			*retSamples = outSamples;
	}
	else  //delta == NULL
	{
    	while (timebaseIdx < *retSamples && timebase[timebaseIdx] <= end)
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
static int64_t *convertTimebaseToInt64(struct descriptor_signal *inSignalD, int *outSamples, int *isFloat)
{
  struct descriptor_a *currDim;
  double *doublePtr;
  float *floatPtr;
  int64_t *outPtr;
  int numSamples, i, status;
  EMPTYXD(currXd);

  currDim = (struct descriptor_a *)inSignalD->dimensions[0];
  //If timebase already using 64 bit int
  if (currDim->class == CLASS_A && (currDim->dtype == DTYPE_Q || currDim->dtype == DTYPE_QU)) {
    outPtr = malloc(currDim->arsize);
    numSamples = currDim->arsize / currDim->length;
    for (i = 0; i < numSamples; i++)
      outPtr[i] = ((int64_t *) currDim->pointer)[i];
    *outSamples = numSamples;
    return outPtr;
  }
  //otherwise evaluate it
  status = TdiData(currDim, &currXd MDS_END_ARG);
  if (status & 1) {
    currDim = (struct descriptor_a *)currXd.pointer;
    if (currDim->class == CLASS_A && (currDim->dtype == DTYPE_Q || currDim->dtype == DTYPE_QU)) {
      outPtr = malloc(currDim->arsize);
      numSamples = currDim->arsize / currDim->length;
      for (i = 0; i < numSamples; i++)
	outPtr[i] = ((int64_t *) currDim->pointer)[i];
      *outSamples = numSamples;
      MdsFree1Dx(&currXd, 0);
      return outPtr;
    }
    *isFloat = 1;
    status = TdiFloat(&currXd, &currXd MDS_END_ARG);
  }
  currDim = (struct descriptor_a *)currXd.pointer;
  if (!(status & 1) || currDim->class != CLASS_A
      || (currDim->dtype != DTYPE_FLOAT && currDim->dtype != DTYPE_DOUBLE))
    //Cannot perform conversion
  {
    MdsFree1Dx(&currXd, 0);
    return 0;
  }
  numSamples = currDim->arsize / currDim->length;
  outPtr = malloc(8 * numSamples);
  if (currDim->dtype == DTYPE_FLOAT) {
    floatPtr = (float *)currDim->pointer;
    for (i = 0; i < numSamples; i++)
      MdsFloatToTime(floatPtr[i], &outPtr[i]);
  } else			//currDim->dtype == DTYPE_DOUBLE)
  {
    doublePtr = (double *)currDim->pointer;
    for (i = 0; i < numSamples; i++)
      MdsFloatToTime(doublePtr[i], &outPtr[i]);
  }
  *outSamples = numSamples;
  MdsFree1Dx(&currXd, 0);
  return outPtr;
}

//Handle resampling in when dimension is specified as a range
typedef ARRAY_COEFF(char, 256) Array_coeff_type;

EXPORT struct descriptor_xd *XTreeResampleClosest(struct descriptor_signal *inSignalD,
						  struct descriptor *startD,
						  struct descriptor *endD,
						  struct descriptor *deltaD)
{
  static EMPTYXD(retXd);
  XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, CLOSEST_SAMPLE, &retXd);
  return &retXd;
}

EXPORT struct descriptor_xd *XTreeResamplePrevious(struct descriptor_signal *inSignalD,
						   struct descriptor *startD,
						   struct descriptor *endD,
						   struct descriptor *deltaD)
{
  static EMPTYXD(retXd);
  XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, PREVIOUS_SAMPLE, &retXd);
  return &retXd;
}

EXPORT int XTreeDefaultResample(struct descriptor_signal *inSignalD, struct descriptor *startD,
				struct descriptor *endD, struct descriptor *deltaD,
				struct descriptor_xd *outSignalXd)
{
  return XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, INTERPOLATION, outSignalXd);
}

EXPORT int XTreeMinMaxResample(struct descriptor_signal *inSignalD, struct descriptor *startD,
				struct descriptor *endD, struct descriptor *deltaD,
				struct descriptor_xd *outSignalXd)
{
  return XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, MINMAX, outSignalXd);
}

static int getShape(struct descriptor *dataD, int *dims, int *numDims)
{
    int i;
    ARRAY_COEFF(char *, 64) *arrPtr;
    if(dataD->class != CLASS_A)
    {
//	printf("Internal error resample!!!!\n");
	return 0;
    }
    arrPtr = (void *)dataD;
    if(arrPtr->dimct == 1)
    {
        *numDims = 1;
	dims[0] = arrPtr->arsize/arrPtr->length;
    }
    else
    {
        *numDims = arrPtr->dimct;
	for(i = 0; i < arrPtr->dimct; i++)
	    dims[i] = arrPtr->m[i];
    }
    return 1;
}


static int XTreeDefaultResampleMode(struct descriptor_signal *inSignalD, struct descriptor *startD,
				    struct descriptor *endD, struct descriptor *inDeltaD, char mode,
				    struct descriptor_xd *outSignalXd)
{
  //char resampleExpr[64];
  //struct descriptor resampleExprD = { 0, DTYPE_T, CLASS_S, resampleExpr };
  //struct descriptor_a *arrayD;
  //char *shapeExpr = "SHAPE(DATA($1))";
  //struct descriptor shapeExprD = { strlen(shapeExpr), DTYPE_T, CLASS_S, shapeExpr };

  int64_t start64, end64, delta64, *timebase64, *outDim;
//      float *timebaseFloat;
  double *timebaseDouble;
  int dims[64];
  int numDims;
  int numTimebaseSamples;
  int outSamples, itemSize;
  int isFloat = 0;
  int status, i, currIdx;
  int numYSamples;
  char *outData;
  struct descriptor_a *dataD;
  //EMPTYXD(shapeXd);
  EMPTYXD(dataXd);

  DESCRIPTOR_A_COEFF(outDataArray, 0, 0, 0, 255, 0);
  DESCRIPTOR_A(outDimArray, 0, 0, 0, 0);
  DESCRIPTOR_SIGNAL_1(outSignalD, &outDataArray, 0, &outDimArray);
  struct descriptor *deltaD;

  deltaD = inDeltaD;
  if (startD) {
    status = XTreeConvertToLongTime(startD, &start64);
    if (!(status & 1))
      return status;
  }
  if (endD) {
    status = XTreeConvertToLongTime(endD, &end64);
    if (!(status & 1))
      return status;
  }
  if (deltaD) {
    status = XTreeConvertToLongDelta(deltaD, &delta64);
    if (!(status & 1))
      return status;
    if (delta64 == 0)
      deltaD = 0;
  }

//This version handles only 64 bit time format

  timebase64 = convertTimebaseToInt64(inSignalD, &numTimebaseSamples, &isFloat);
  if (!timebase64)
    return 0;			//Cannot convert timebase to 64 bit int

  if (!startD)
    start64 = timebase64[0];
  if (!endD)
    end64 = timebase64[numTimebaseSamples - 1];

  if (start64 < timebase64[0])
    start64 = timebase64[0];
  if (end64 > timebase64[numTimebaseSamples - 1])
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
  numYSamples = dataD->arsize/dataD->length;


  status = getShape((struct descriptor *)dataD, dims, &numDims);
  if (!(status & 1)) {
    MdsFree1Dx(&dataXd, 0);
    return status;
  }

  /*
  status = TdiCompile(&shapeExprD, dataD, &shapeXd MDS_END_ARG);
  if (!(status & 1)) {
    MdsFree1Dx(&dataXd, 0);
    return status;
  }
  status = TdiData(&shapeXd, &shapeXd MDS_END_ARG);
  if (!(status & 1)) {
    MdsFree1Dx(&dataXd, 0);
    return status;
  }
  arrayD = (struct descriptor_a *)shapeXd.pointer;
  numDims = arrayD->arsize / arrayD->length;
  dims = (int *)arrayD->pointer;

*/

  itemSize = dataD->length;
  for (i = 0; i < numDims - 1; i++)
    itemSize *= dims[i];
  //Make sure enough room is allocated
  if (!deltaD) {
    //Count number of copied samples
    for (currIdx = 0; currIdx < numTimebaseSamples && currIdx < numYSamples && timebase64[currIdx] < start64; currIdx++) ;
    for (outSamples = 0; currIdx < numTimebaseSamples && currIdx < numYSamples && timebase64[currIdx] <= end64;
	 currIdx++, outSamples++) ;
  } else
    outSamples = (end64 - start64) / delta64 + 1;

//  outData = malloc(outSamples * itemSize);
  outData = malloc(2* outSamples * itemSize);  //Make enough room for MINMAX mode
//  outDim = malloc(outSamples * 8);
  outDim = malloc(2 * outSamples * 8);

  //Check data array too short
  if ((int)(dataD->arsize / dataD->length) < numTimebaseSamples)
    numTimebaseSamples = dataD->arsize / dataD->length;

  resample(start64, end64, (deltaD) ? delta64 : 0, timebase64, numTimebaseSamples, numDims, dims,
	   dataD->pointer, dataD->length, dataD->dtype, mode, outData, outDim, &outSamples);

  //Build array descriptor for out data
  outDataArray.length = dataD->length;
  outDataArray.dtype = dataD->dtype;
  outDataArray.pointer = outData;
  outDataArray.arsize = itemSize * outSamples;
  outDataArray.dimct = numDims;
  for (i = 0; i < numDims - 1; i++) {
    outDataArray.m[i] = dims[i];
  }
  outDataArray.m[numDims - 1] = outSamples;
  //If originally float, convert  dimension to float
  if (isFloat) {
    timebaseDouble = (double *)malloc(outSamples * sizeof(double));
    for (i = 0; i < outSamples; i++)
      MdsTimeToDouble(outDim[i], &timebaseDouble[i]);
    outDimArray.length = sizeof(double);
    outDimArray.arsize = sizeof(double) * outSamples;
    outDimArray.dtype = DTYPE_DOUBLE;
    outDimArray.pointer = (char *)timebaseDouble;
  }

  else {
    outDimArray.length = 8;
    outDimArray.arsize = 8 * outSamples;
    outDimArray.dtype = DTYPE_Q;
    outDimArray.pointer = (char *)outDim;
  }

  MdsCopyDxXd((struct descriptor *)&outSignalD, outSignalXd);
  free((char *)timebase64);
  free((char *)outDim);
  if (isFloat)
    free((char *)timebaseDouble);
  free((char *)outData);
  //MdsFree1Dx(&shapeXd, 0);
  MdsFree1Dx(&dataXd, 0);

  return 1;
}
