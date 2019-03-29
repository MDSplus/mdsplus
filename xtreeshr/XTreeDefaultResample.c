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
#include <treeshr.h>
#include <xtreeshr.h>

typedef enum{
AVERAGE,
INTERPOLATION,
CLOSEST_SAMPLE,
PREVIOUS_SAMPLE,
MINMAX,
} res_mode_t;

//static int lessThan(struct descriptor *in1D, struct descriptor *in2D, char *less);
//static int getMinMax(struct descriptor *dimD, char isMin, struct descriptor_xd *outXd);
extern int TdiDecompile();
extern int TdiCompile();
extern int TdiData();
extern int TdiFloat();
extern int TdiEvaluate();
extern int XTreeConvertToLongTime(struct descriptor *timeD, int64_t * converted);

static int XTreeDefaultResampleMode(struct descriptor_signal *inSignalD, struct descriptor *startD,
	    struct descriptor *endD, struct descriptor *deltaD, const res_mode_t mode,
	    struct descriptor_xd *outSignalXd);

//64 bit time-based resampling function. It is assumed here that the 64 bit representation of time is the count
//of a given, fixed amount of time, starting from a given time in the past
//The closest point is selected as representative for a given time
static void resample(int64_t start, int64_t end, int64_t delta, int64_t * inTimebase,
		     int inTimebaseSamples, int numDims, int *dims, char *inData, int dataSize,
		     char dataType, const res_mode_t mode, char **retData, int64_t ** retDim, int *retSamples)
{
  int i, j;
  double prevData, nextData, currData;
  int itemSize = dataSize;
  for (i = 0; i < numDims - 1; i++)
    itemSize *= dims[i];
  int numDataItems = itemSize/dataSize;
  int outItemSize;
  if (mode == AVERAGE) {// use FLOAT or DOUBLE
    *retSamples += 3;
    outItemSize = dataSize<8 ? itemSize*4/dataSize : itemSize;
  } else {
    if (mode == MINMAX)
      *retSamples *=2;//Make enough room for MINMAX mode
    outItemSize = itemSize;
  }
  // Make sure enough room is allocated

  char    *outData = *retData = malloc(*retSamples * outItemSize);
  int64_t *outDim  = *retDim  = malloc(*retSamples * 8);

  if (start < inTimebase[0])
    start = inTimebase[0];
  if (end > inTimebase[inTimebaseSamples - 1])
    end = inTimebase[inTimebaseSamples - 1];
  int timebaseIdx = 0, outSamples = 0;
  int startIdx = 0;
  for (; startIdx < inTimebaseSamples && inTimebase[startIdx] < start ; startIdx++);
  if (startIdx == inTimebaseSamples) {  //Not possible in any case
    *retSamples = 0;
    return;
  }

  int64_t *timebase = &inTimebase[startIdx];
  int timebaseSamples = inTimebaseSamples - startIdx;
  char *data = &inData[startIdx * itemSize];
  int64_t refTime = start;
  if (delta) {
    int64_t delta1 = (delta  )/2;
    int64_t delta2 = delta+delta1;
    switch (mode) {
      case INTERPOLATION:
      case MINMAX:
      case AVERAGE:
	refTime += delta1;
	end     += delta;
      break;default:break;
    }
    int prevTimebaseIdx = timebaseIdx;
    while (refTime <= end) {
      while (timebaseIdx < timebaseSamples && timebase[timebaseIdx] < refTime)
	timebaseIdx++;
      switch (mode) {
	case CLOSEST_SAMPLE: // Select closest sample
	  if (timebaseIdx > 0 && timebaseIdx < timebaseSamples) {
	    delta1 = timebase[timebaseIdx] - refTime;
	    delta2 = refTime - timebase[timebaseIdx - 1];
	    if (delta2 < delta1)
	      timebaseIdx--;
	  }
	  memcpy(&outData[outSamples * outItemSize], &data[timebaseIdx * itemSize], itemSize);
	  break;
	case PREVIOUS_SAMPLE: //Select closest sample
	  if (timebaseIdx > 0 && timebaseIdx < timebaseSamples)
	    timebaseIdx--;
	  memcpy(&outData[outSamples * outItemSize], &data[timebaseIdx * itemSize], itemSize);
	  break;
	case INTERPOLATION:
#define INTERPOLATION_FORLOOP(type) for (i = 0; i < numDataItems; i++) { \
  prevData = ((type *)(&data[(timebaseIdx - 1) * itemSize]))[i]; \
  nextData = ((type *)(&data[timebaseIdx * itemSize]))[i]; \
  currData = prevData + (nextData - prevData) * (refTime - timebase[timebaseIdx - 1]) / (timebase[timebaseIdx] - timebase[timebaseIdx - 1]); \
  ((type *)(&outData[outSamples * outItemSize]))[i] = currData; \
}
	  if(timebaseIdx <= 0) timebaseIdx = 1;  //Avoid referring to negative indexes
	  switch (dataType) {
	    case DTYPE_BU:	INTERPOLATION_FORLOOP( uint8_t);break;
	    case DTYPE_B:	INTERPOLATION_FORLOOP(  int8_t);break;
	    case DTYPE_WU:	INTERPOLATION_FORLOOP(uint16_t);break;
	    case DTYPE_W:	INTERPOLATION_FORLOOP( int16_t);break;
	    case DTYPE_LU:	INTERPOLATION_FORLOOP(uint32_t);break;
	    case DTYPE_L:	INTERPOLATION_FORLOOP( int32_t);break;
	    case DTYPE_QU:	INTERPOLATION_FORLOOP(uint64_t);break;
	    case DTYPE_Q:	INTERPOLATION_FORLOOP( int64_t);break;
	    case DTYPE_FLOAT:	INTERPOLATION_FORLOOP(   float);break;
	    case DTYPE_DOUBLE:	INTERPOLATION_FORLOOP(  double);break;
	  }
	  break;
	case MINMAX:     //Two points for every (resampled) sample!!!!!!!!!!!
#define MINMAX_FORLOOP(type) for (i = 0; i < numDataItems; i++) { \
type currData, minData, maxData; \
minData = maxData = ((type *)(&data[(prevTimebaseIdx) * itemSize]))[i]; \
for(j = prevTimebaseIdx + 1; j < timebaseIdx && j<inTimebaseSamples ; j++){ \
  currData = ((type *)(&data[j * itemSize]))[i]; \
  if(currData > maxData) maxData = currData; \
  if(currData < minData) minData = currData; \
} \
((type *)(&outData[outSamples * outItemSize]))[i] = minData; \
((type *)(&outData[(outSamples+1) * itemSize]))[i] = maxData; \
}
	  switch (dataType) {
	    case DTYPE_BU:	MINMAX_FORLOOP( uint8_t);break;
	    case DTYPE_B:	MINMAX_FORLOOP(  int8_t);break;
	    case DTYPE_WU:	MINMAX_FORLOOP(uint16_t);break;
	    case DTYPE_W:	MINMAX_FORLOOP( int16_t);break;
	    case DTYPE_LU:	MINMAX_FORLOOP(uint32_t);break;
  	    case DTYPE_L:	MINMAX_FORLOOP( int32_t);break;
	    case DTYPE_QU:	MINMAX_FORLOOP(uint64_t);break;
	    case DTYPE_Q:	MINMAX_FORLOOP( int64_t);break;
	    case DTYPE_FLOAT:	MINMAX_FORLOOP(   float);break;
	    case DTYPE_DOUBLE:	MINMAX_FORLOOP(  double);break;
	  }
	  break;
	case AVERAGE:
#define AVERAGE_FORLOOP(type_out,type) for (i = 0; i < numDataItems; i++) { \
type_out accData = ((type *)(&data[(prevTimebaseIdx) * itemSize]))[i]; \
for(j = prevTimebaseIdx + 1; j < timebaseIdx; j++) \
  accData += ((type *)(&data[j * itemSize]))[i]; \
const int range = (timebaseIdx-prevTimebaseIdx); \
((type_out *)(&outData[outSamples * outItemSize]))[i] = range>0 ? accData / range : accData; \
}
	  switch (dataType) {
	    case DTYPE_BU:	AVERAGE_FORLOOP( float, uint8_t);break;
	    case DTYPE_B:	AVERAGE_FORLOOP( float,  int8_t);break;
	    case DTYPE_WU:	AVERAGE_FORLOOP( float,uint16_t);break;
	    case DTYPE_W:	AVERAGE_FORLOOP( float, int16_t);break;
  	    case DTYPE_L:	AVERAGE_FORLOOP( float, int32_t);break;
	    case DTYPE_QU:	AVERAGE_FORLOOP(double,uint64_t);break;
	    case DTYPE_Q:	AVERAGE_FORLOOP(double, int64_t);break;
	    case DTYPE_FLOAT:	AVERAGE_FORLOOP( float,   float);break;
	    case DTYPE_DOUBLE:	AVERAGE_FORLOOP(double,  double);break;
	  }
	  break;
	default://Not able to do interpolation
	  if (timebaseIdx > 0 && timebaseIdx < timebaseSamples)
	    timebaseIdx--;
	  memcpy(&outData[outSamples * outItemSize], &data[timebaseIdx * itemSize], itemSize);
	  break;
      }
      switch (mode) {
	case INTERPOLATION:
	case MINMAX:
	case AVERAGE:
	refTime -= delta1;
	if(mode == MINMAX) //twice the number of points
	  outDim[outSamples++] = refTime;
        outDim[outSamples++] = refTime;
	refTime += delta2;
	break;
      default:
        outDim[outSamples++] = refTime;
        refTime += delta;
	break;
      }
      prevTimebaseIdx = timebaseIdx;
    }
    *retSamples = outSamples;
  } else { //delta == NULL
    if (mode == AVERAGE && dataType != DTYPE_FLOAT && dataType != DTYPE_DOUBLE) {
      while (timebaseIdx < timebaseSamples && timebase[timebaseIdx] <= end) {
	switch (dataType) {
	  case DTYPE_BU:(*( float*)&outData[timebaseIdx*outItemSize]) = *( uint8_t*)&data[timebaseIdx*itemSize];break;
	  case DTYPE_B: (*( float*)&outData[timebaseIdx*outItemSize]) = *(  int8_t*)&data[timebaseIdx*itemSize];break;
	  case DTYPE_WU:(*( float*)&outData[timebaseIdx*outItemSize]) = *(uint16_t*)&data[timebaseIdx*itemSize];break;
	  case DTYPE_W: (*( float*)&outData[timebaseIdx*outItemSize]) = *( int16_t*)&data[timebaseIdx*itemSize];break;
	  case DTYPE_LU:(*( float*)&outData[timebaseIdx*outItemSize]) = *(uint32_t*)&data[timebaseIdx*itemSize];break;
  	  case DTYPE_L: (*( float*)&outData[timebaseIdx*outItemSize]) = *( int32_t*)&data[timebaseIdx*itemSize];break;
	  case DTYPE_QU:(*(double*)&outData[timebaseIdx*outItemSize]) = *(uint64_t*)&data[timebaseIdx*itemSize];break;
	  case DTYPE_Q: (*(double*)&outData[timebaseIdx*outItemSize]) = *( int64_t*)&data[timebaseIdx*itemSize];break;
        }
        outDim[timebaseIdx] = timebase[timebaseIdx];
        timebaseIdx++;
      }
    } else {
      while (timebaseIdx < timebaseSamples && timebase[timebaseIdx] <= end) {
	memcpy(&outData[timebaseIdx * outItemSize], &data[timebaseIdx * itemSize], itemSize);
	outDim[timebaseIdx] = timebase[timebaseIdx];
	timebaseIdx++;
      }
    }
    *retSamples = timebaseIdx;
  }
}

//The default resample handles int64 timebases
//return 0 if the conversion is  not possible
inline static int64_t *convertTimebaseToInt64(struct descriptor_signal *inSignalD, int *outSamples, int *isFloat){
  if (inSignalD->ndesc<3) {
    ARRAY_COEFF(char *, MAX_DIMS) *dataD = (void*)inSignalD->data;
    if (dataD->dimct == 1)
      *outSamples = dataD->arsize / dataD->length;
    else
      *outSamples = dataD->m[dataD->dimct-1];
    return NULL;
  }
  int64_t *outPtr;
  EMPTYXD(currXd);
  double *doublePtr;
  float *floatPtr;
  int numSamples, i, status;
  struct descriptor_a *currDim = (struct descriptor_a *)inSignalD->dimensions[0];
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
  if (STATUS_NOT_OK || currDim->class != CLASS_A
      || (currDim->dtype != DTYPE_FLOAT && currDim->dtype != DTYPE_DOUBLE)) {
    //Cannot perform conversion
    MdsFree1Dx(&currXd, 0);
    return NULL;
  }
  numSamples = currDim->arsize / currDim->length;
  outPtr = malloc(8 * numSamples);
  if (currDim->dtype == DTYPE_FLOAT) {
    floatPtr = (float *)currDim->pointer;
    for (i = 0; i < numSamples; i++)
      outPtr[i] = (int64_t)(floatPtr[i]*1e9);
  } else {//currDim->dtype == DTYPE_DOUBLE)
    doublePtr = (double *)currDim->pointer;
    for (i = 0; i < numSamples; i++)
      outPtr[i] = (int64_t)(doublePtr[i]*1e9);
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
  return XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, AVERAGE, outSignalXd);
}

EXPORT int XTreeInterpResample(struct descriptor_signal *inSignalD, struct descriptor *startD,
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

extern int getShape(struct descriptor *dataD, int *dims, int *numDims);

static int XTreeDefaultResampleMode(struct descriptor_signal *inSignalD, struct descriptor *startD,
	    struct descriptor *endD, struct descriptor *inDeltaD, const res_mode_t mode,
	    struct descriptor_xd *outSignalXd)
{
  int64_t start64, end64, delta64, *timebase64, *outDim;
  double *timebaseDouble;
  int dims[64];
  int numDims;
  int numTimebaseSamples;
  int isFloat = 0;
  int status, i, currIdx;
  int numYSamples;
  char *outData;
  struct descriptor_a *dataD;
  EMPTYXD(dataXd);

  DESCRIPTOR_A_COEFF(outDataArray, 0, 0, 0, 255, 0);
  DESCRIPTOR_A(outDimArray, 0, 0, 0, 0);
  DESCRIPTOR_SIGNAL_1(outSignalD, &outDataArray, 0, &outDimArray);
  struct descriptor *deltaD;

  deltaD = inDeltaD;
  if (startD) {
    status = XTreeConvertToLongTime(startD, &start64);
    if STATUS_NOT_OK return status;
  }
  if (endD) {
    status = XTreeConvertToLongTime(endD, &end64);
    if STATUS_NOT_OK return status;
  }
  if (deltaD) {
    status = XTreeConvertToLongTime(deltaD, &delta64);
    if STATUS_NOT_OK return status;
    if (delta64 == 0)   deltaD = 0;
  }

  // This version handles only 64 bit time format

  timebase64 = convertTimebaseToInt64(inSignalD, &numTimebaseSamples, &isFloat);
  if (!timebase64) {
    MdsCopyDxXd((struct descriptor *)&inSignalD, outSignalXd);
    return 3;	//Cannot convert timebase to 64 bit int
  }
  if (!startD)  start64 = timebase64[0];
  if (!endD)    end64   = timebase64[numTimebaseSamples - 1];
  if (start64 < timebase64[0])
    start64 = timebase64[0];
  if (end64 > timebase64[numTimebaseSamples - 1])
    end64 = timebase64[numTimebaseSamples - 1];

  // Get shapes(dimensions) for data
  if (inSignalD->data->class == CLASS_A) {
    dataD = (struct descriptor_a *)inSignalD->data;
  } else {
    status = TdiData(inSignalD->data, &dataXd MDS_END_ARG);
    if STATUS_NOT_OK return status;
    dataD = (struct descriptor_a *)dataXd.pointer;
  }
  numYSamples = dataD->arsize/dataD->length;


  status = getShape((struct descriptor *)dataD, dims, &numDims);
  if STATUS_NOT_OK {
    MdsFree1Dx(&dataXd, 0);
    return status;
  }
  //Check data array too short
  if (numYSamples < numTimebaseSamples)
    numTimebaseSamples = numYSamples;

  int outSamples;
  if (!deltaD) {
    // Count number of copied samples
    for (currIdx = 0; currIdx < numTimebaseSamples && timebase64[currIdx] < start64; currIdx++) ;
    for (outSamples = currIdx ; outSamples < numTimebaseSamples && timebase64[outSamples] <= end64; outSamples++) ;
    outSamples = outSamples - currIdx;
  } else
    outSamples = (end64 - start64) / delta64 + 1;

  resample(start64, end64, (deltaD) ? delta64 : 0, timebase64, numTimebaseSamples, numDims, dims,
	   dataD->pointer, dataD->length, dataD->dtype, mode, &outData, &outDim, &outSamples);

  //Build array descriptor for out data
  if (mode == AVERAGE) { // use FLOAT or DOUBLE
    if (dataD->length<8) {
      outDataArray.length = 4;
      outDataArray.dtype  = DTYPE_FLOAT;
    } else {
      outDataArray.length = 8;
      outDataArray.dtype  = DTYPE_DOUBLE;
    }
  } else {
    outDataArray.length = dataD->length;
    outDataArray.dtype = dataD->dtype;
  }
  int itemSize = outDataArray.length;
  for (i = 0; i < numDims - 1; i++)
    itemSize *= dims[i];
  outDataArray.pointer = outDataArray.a0 = outData;
  outDataArray.arsize = itemSize * outSamples;
  outDataArray.dimct = numDims;
  for (i = 0; i < numDims - 1; i++)
    outDataArray.m[i] = dims[i];
  outDataArray.m[numDims - 1] = outSamples;
  //If originally float, convert  dimension to float
  if (isFloat) {
    timebaseDouble = (double *)malloc(outSamples * sizeof(double));
    for (i = 0; i < outSamples; i++)
      timebaseDouble[i] = (double)outDim[i] / 1e9;
    outDimArray.length = sizeof(double);
    outDimArray.arsize = sizeof(double) * outSamples;
    outDimArray.dtype = DTYPE_DOUBLE;
    outDimArray.pointer = outDataArray.a0 = (char *)timebaseDouble;
  } else {
    timebaseDouble = NULL;
    outDimArray.length = 8;
    outDimArray.arsize = 8 * outSamples;
    outDimArray.dtype = DTYPE_Q;
    outDimArray.pointer = outDataArray.a0 = (char *)outDim;
  }

  MdsCopyDxXd((struct descriptor *)&outSignalD, outSignalXd);
  free(timebase64);
  free(outDim);
  free(timebaseDouble);
  free(outData);
  //MdsFree1Dx(&shapeXd, 0);
  MdsFree1Dx(&dataXd, 0);

  return 1;
}
