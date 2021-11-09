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
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <pthread_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include <xtreeshr.h>

typedef enum
{
  AVERAGE,
  INTERPOLATION,
  CLOSEST,
  PREVIOUS,
  MINMAX,
} res_mode_t;

// static int lessThan(mdsdsc_t*in1D, mdsdsc_t*in2D, char *less);
// static int getMinMax(mdsdsc_t*dimD, char isMin, mdsdsc_xd_t *outXd);
extern int TdiDecompile();
extern int TdiCompile();
extern int TdiData();
extern int TdiFloat();
extern int TdiEvaluate();
extern int XTreeConvertToDouble(mdsdsc_t *timeD, double *converted);

static int XTreeDefaultResampleMode(mds_signal_t *inSignalD, mdsdsc_t *startD,
                                    mdsdsc_t *endD, mdsdsc_t *deltaD,
                                    const res_mode_t mode,
                                    mdsdsc_xd_t *outSignalXd);

inline static double *convertTimebaseToDouble(mds_signal_t *inSignalD,
                                              int *outSamples, dtype_t *isQ)
{
  if (inSignalD->ndesc < 3)
  {
    ARRAY_COEFF(char *, MAX_DIMS) *dataD = (void *)inSignalD->data;
    if (dataD->dimct == 1)
      *outSamples = dataD->arsize / dataD->length;
    else
      *outSamples = dataD->m[dataD->dimct - 1];
    return NULL;
  }
  double *outPtr = NULL;
  EMPTYXD(currXd);
  EMPTYXD(startXd);
  EMPTYXD(endXd);
  EMPTYXD(deltaXd);

  int numSamples, i;
  mdsdsc_a_t *currDim = (mdsdsc_a_t *)inSignalD->dimensions[0];
  if (currDim->class != CLASS_A)
  {
    //Reset time context when evaluating timebase to avoid errors in case the timebase expression refers to segmented data
    TreeGetTimeContext(&startXd, &endXd, &deltaXd);
    TreeSetTimeContext(NULL, NULL, NULL);
    int status = TdiData(currDim, &currXd MDS_END_ARG);
    TreeSetTimeContext(startXd.pointer, endXd.pointer, deltaXd.pointer);
    MdsFree1Dx(&startXd, NULL);
    MdsFree1Dx(&endXd, NULL);
    MdsFree1Dx(&deltaXd, NULL);
    if (IS_NOT_OK(status))
      goto return_out;
    currDim = (mdsdsc_a_t *)currXd.pointer;
  }
  if (currDim->class != CLASS_A || currDim->arsize == 0)
    goto return_out;
  if (currDim->dtype == DTYPE_DOUBLE)
  {
    numSamples = currDim->arsize / currDim->length;
    outPtr = malloc(currDim->arsize);
    memcpy(outPtr, currDim->pointer, currDim->arsize);
    *outSamples = numSamples;
    goto return_out;
  }
  if (currDim->dtype == DTYPE_Q || currDim->dtype == DTYPE_QU)
  {
    *isQ = currDim->dtype;
    outPtr = malloc(currDim->arsize);
    numSamples = currDim->arsize / currDim->length;
    for (i = 0; i < numSamples; i++)
      outPtr[i] = ((double)((int64_t *)currDim->pointer)[i]) / 1e9;
    *outSamples = numSamples;
    goto return_out;
  }
  if (currDim->dtype != DTYPE_FLOAT)
  {
    if (IS_NOT_OK(TdiFloat(currDim, &currXd MDS_END_ARG)))
      goto return_out;
    currDim = (mdsdsc_a_t *)currXd.pointer;
  }
  if (currDim->class != CLASS_A ||
      (currDim->dtype != DTYPE_FLOAT && currDim->dtype != DTYPE_DOUBLE))
    goto return_out; // Cannot perform conversion
  numSamples = currDim->arsize / currDim->length;
  outPtr = malloc(sizeof(double) * numSamples);
  if (currDim->dtype == DTYPE_FLOAT)
  {
    const float *floatPtr = (float *)currDim->pointer;
    for (i = 0; i < numSamples; i++)
      outPtr[i] = floatPtr[i];
  }
  else // currDim->dtype == DTYPE_DOUBLE)
    memcpy(outPtr, currDim->pointer, currDim->arsize);
  *outSamples = numSamples;
return_out:;
  MdsFree1Dx(&currXd, NULL);
  return outPtr;
}

EXPORT mdsdsc_xd_t *XTreeResampleClosest(mds_signal_t *inSignalD,
                                         mdsdsc_t *startD, mdsdsc_t *endD,
                                         mdsdsc_t *deltaD)
{ // deprecated
  static EMPTYXD(retXd);
  XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, CLOSEST, &retXd);
  return &retXd;
}

EXPORT mdsdsc_xd_t *XTreeResamplePrevious(mds_signal_t *inSignalD,
                                          mdsdsc_t *startD, mdsdsc_t *endD,
                                          mdsdsc_t *deltaD)
{ // deprecated
  static EMPTYXD(retXd);
  XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, PREVIOUS, &retXd);
  return &retXd;
}

static inline res_mode_t get_default()
{
  char *resampleMode = TranslateLogical("MDSPLUS_DEFAULT_RESAMPLE_MODE");
  if (!resampleMode)
    return AVERAGE;
  res_mode_t default_mode;
  int len = strlen(resampleMode);
  if (len == 0)
    default_mode = AVERAGE;
  else if (!strncasecmp(resampleMode, "Average", len))
    default_mode = AVERAGE;
  else if (!strncasecmp(resampleMode, "MinMax", len))
    default_mode = MINMAX;
  else if (!strncasecmp(resampleMode, "Interpolation", len))
    default_mode = INTERPOLATION;
  else if (!strncasecmp(resampleMode, "Closest", len))
    default_mode = CLOSEST;
  else if (!strncasecmp(resampleMode, "Previous", len))
    default_mode = PREVIOUS;
  else
  {
    fprintf(stderr,
            "Error: Resample mode must be one of 'Average', 'MinMax', "
            "'Interpolation', 'Closest', or 'Previous' but was '%s'; using "
            "'Average'.\n",
            resampleMode);
    default_mode = AVERAGE;
  }
  TranslateLogicalFree(resampleMode);
  return default_mode;
}
int XTreeDefaultResample(mds_signal_t *inSignalD, mdsdsc_t *startD,
                         mdsdsc_t *endD, mdsdsc_t *deltaD,
                         mdsdsc_xd_t *outSignalXd)
{
  // get_env is cheap compared to what is comming next, we can affort to reload
  // it
  return XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD,
                                  get_default(), outSignalXd);
}

#define RESAMPLE_FUN(name, mode)                                           \
  int name(mds_signal_t *inSignalD, mdsdsc_t *startD, mdsdsc_t *endD,      \
           mdsdsc_t *deltaD, mdsdsc_xd_t *outSignalXd)                     \
  {                                                                        \
    return XTreeDefaultResampleMode(inSignalD, startD, endD, deltaD, mode, \
                                    outSignalXd);                          \
  }

EXPORT RESAMPLE_FUN(XTreeAverageResample, AVERAGE) EXPORT
    RESAMPLE_FUN(XTreeClosestResample, CLOSEST) EXPORT
    RESAMPLE_FUN(XTreePreviousResample, PREVIOUS) EXPORT
    RESAMPLE_FUN(XTreeInterpResample, INTERPOLATION) EXPORT
    RESAMPLE_FUN(XTreeMinMaxResample, MINMAX)

        extern int getShape(mdsdsc_t *dataD, int *dims, int *numDims);

static int XTreeDefaultResampleMode(mds_signal_t *inSignalD, mdsdsc_t *startD,
                                    mdsdsc_t *endD, mdsdsc_t *deltaD,
                                    const res_mode_t mode,
                                    mdsdsc_xd_t *outSignalXd)
{
  int status;

  // Get shapes(dimensions) for data
  EMPTYXD(dataXd);
  mdsdsc_a_t *dataD;
  if (inSignalD->data->class == CLASS_A)
  {
    dataD = (mdsdsc_a_t *)inSignalD->data;
  }
  else
  {
    status = TdiData(inSignalD->data, &dataXd MDS_END_ARG);
    if (STATUS_NOT_OK)
      return status;
    dataD = (mdsdsc_a_t *)dataXd.pointer;
  }
  int dims[MAX_DIMS];
  int numDims;
  status = getShape((mdsdsc_t *)dataD, dims, &numDims);
  if (STATUS_NOT_OK)
  {
    MdsFree1Dx(&dataXd, 0);
    return status;
  }

  // get timebase
  dtype_t isQ = 0;
  double start, end, delta, *fulltimebase;
  int numTimebase, numData = (numDims <= 1)
                                 ? (int)(dataD->arsize / dataD->length)
                                 : dims[numDims - 1];
  fulltimebase = convertTimebaseToDouble(inSignalD, &numTimebase, &isQ);
  if (!fulltimebase)
  {
    MdsFree1Dx(&dataXd, 0);
    MdsCopyDxXd((mdsdsc_t *)inSignalD, outSignalXd);
    return 3; // Cannot convert timebase to 64 bit int
  }
  if (numData == 0) //If empty segment
  {
    MdsFree1Dx(&dataXd, 0);
    MdsCopyDxXd((mdsdsc_t *)inSignalD, outSignalXd);
    return 1;
  }

  // Check data array too short
  if (numData < numTimebase)
    numTimebase = numData;

  int startIdx = 0;
  if (startD)
  {
    status = XTreeConvertToDouble(startD, &start);
    if (STATUS_NOT_OK)
      return status;
    if (start < fulltimebase[0])
      start = fulltimebase[0];
    for (; startIdx < numTimebase && fulltimebase[startIdx] < start; startIdx++)
      ;
  }
  else
    start = fulltimebase[0];
  if (endD)
  {
    status = XTreeConvertToDouble(endD, &end);
    if (STATUS_NOT_OK)
      return status;
    if (end > fulltimebase[numTimebase - 1])
      end = fulltimebase[numTimebase - 1];
  }
  else
    end = fulltimebase[numTimebase - 1];
  if (deltaD)
  {
    status = XTreeConvertToDouble(deltaD, &delta);
    if (STATUS_NOT_OK)
      return status;
  }
  else
    delta = 0;

  int i, j;
  double prevData, nextData, currData;
  int itemSize = dataD->arsize / numData;
  int numDataItems = itemSize / dataD->length;
  int outItemSize;
  DESCRIPTOR_A_COEFF(outDataArray, 0, 0, 0, MAX_DIMS, 0);
  if (mode == AVERAGE && delta > 0)
  { // use FLOAT or DOUBLE
  }
  else
  {
    outDataArray.length = dataD->length;
    outDataArray.dtype = dataD->dtype;
  }
  // Make sure enough room is allocated
  char *outData;
  double *outDim;
  int timebaseIdx = 0;
  int timebaseSamples = numTimebase - startIdx;
  double *timebase = &fulltimebase[startIdx];
  char *data = &dataD->pointer[startIdx * itemSize];
  int outSamples = 0;
  if (delta > 0 && startIdx < numTimebase)
  {
    double refTime = start;
    double delta1 = delta / 2;
    double delta2 = delta + delta1;
    switch (mode)
    {
    case MINMAX:
    case AVERAGE:
      refTime += delta;
      end += delta;
      break;
    default:
      break;
    }
    int reqSamples = (int)((end - start) / delta + 1.5);
    if (mode == AVERAGE)
    { // use FLOAT or DOUBLE
      if (dataD->length < 8)
      {
        outDataArray.length = sizeof(float);
        outDataArray.dtype = DTYPE_FLOAT;
      }
      else
      {
        outDataArray.length = sizeof(double);
        outDataArray.dtype = DTYPE_DOUBLE;
      }
      outItemSize = itemSize * outDataArray.length / dataD->length;
    }
    else
    {
      if (mode == MINMAX)
        reqSamples *= 2; // Make enough room for MINMAX mode
      outItemSize = itemSize;
      outDataArray.length = dataD->length;
      outDataArray.dtype = dataD->dtype;
    }
    outData = malloc(reqSamples * outItemSize);
    outDim = malloc(reqSamples * sizeof(double));
    int prevTimebaseIdx = timebaseIdx;
    while (refTime <= end)
    {
      while (timebaseIdx < timebaseSamples && timebase[timebaseIdx] < refTime)
        timebaseIdx++;
      switch (mode)
      {
      case CLOSEST: // Select closest sample
        if (timebaseIdx > 0)
        {
          delta1 = timebase[timebaseIdx] - refTime;
          delta2 = refTime - timebase[timebaseIdx - 1];
          if (delta2 < delta1)
            timebaseIdx--;
        }
        memcpy(&outData[outSamples * outItemSize],
               &data[timebaseIdx * itemSize], itemSize);
        break;
      case PREVIOUS: // Select closest sample
        if (timebaseIdx > 0 && timebaseIdx < timebaseSamples)
          timebaseIdx--;
        memcpy(&outData[outSamples * outItemSize],
               &data[timebaseIdx * itemSize], itemSize);
        break;
      case INTERPOLATION:
#define INTERPOLATION_FORLOOP(type)                                            \
  for (i = 0; i < numDataItems; i++)                                           \
  {                                                                            \
    prevData = ((type *)(&data[(timebaseIdx - 1) * itemSize]))[i];             \
    nextData = ((type *)(&data[timebaseIdx * itemSize]))[i];                   \
    currData = prevData +                                                      \
               (nextData - prevData) * (refTime - timebase[timebaseIdx - 1]) / \
                   (timebase[timebaseIdx] - timebase[timebaseIdx - 1]);        \
    ((type *)(&outData[outSamples * outItemSize]))[i] = currData;              \
  }
        if (timebaseIdx <= 0)
          timebaseIdx = 1; // Avoid referring to negative indexes
        switch (dataD->dtype)
        {
        case DTYPE_BU:
          INTERPOLATION_FORLOOP(uint8_t);
          break;
        case DTYPE_B:
          INTERPOLATION_FORLOOP(int8_t);
          break;
        case DTYPE_WU:
          INTERPOLATION_FORLOOP(uint16_t);
          break;
        case DTYPE_W:
          INTERPOLATION_FORLOOP(int16_t);
          break;
        case DTYPE_LU:
          INTERPOLATION_FORLOOP(uint32_t);
          break;
        case DTYPE_L:
          INTERPOLATION_FORLOOP(int32_t);
          break;
        case DTYPE_QU:
          INTERPOLATION_FORLOOP(uint64_t);
          break;
        case DTYPE_Q:
          INTERPOLATION_FORLOOP(int64_t);
          break;
        case DTYPE_FLOAT:
          INTERPOLATION_FORLOOP(float);
          break;
        case DTYPE_DOUBLE:
          INTERPOLATION_FORLOOP(double);
          break;
        default:
          break;
        }
        break;
      case MINMAX: // Two points for every (resampled) sample!!!!!!!!!!!
#define MINMAX_FORLOOP(type)                                               \
  for (i = 0; i < numDataItems; i++)                                       \
  {                                                                        \
    type currData, minData, maxData;                                       \
    minData = maxData = ((type *)(&data[(prevTimebaseIdx)*itemSize]))[i];  \
    for (j = prevTimebaseIdx + 1; j < timebaseIdx && j < numTimebase; j++) \
    {                                                                      \
      currData = ((type *)(&data[j * itemSize]))[i];                       \
      if (currData > maxData)                                              \
        maxData = currData;                                                \
      if (currData < minData)                                              \
        minData = currData;                                                \
    }                                                                      \
    ((type *)(&outData[(outSamples)*outItemSize]))[i] = minData;           \
    ((type *)(&outData[(outSamples + 1) * outItemSize]))[i] = maxData;     \
  }
        switch (dataD->dtype)
        {
        case DTYPE_BU:
          MINMAX_FORLOOP(uint8_t);
          break;
        case DTYPE_B:
          MINMAX_FORLOOP(int8_t);
          break;
        case DTYPE_WU:
          MINMAX_FORLOOP(uint16_t);
          break;
        case DTYPE_W:
          MINMAX_FORLOOP(int16_t);
          break;
        case DTYPE_LU:
          MINMAX_FORLOOP(uint32_t);
          break;
        case DTYPE_L:
          MINMAX_FORLOOP(int32_t);
          break;
        case DTYPE_QU:
          MINMAX_FORLOOP(uint64_t);
          break;
        case DTYPE_Q:
          MINMAX_FORLOOP(int64_t);
          break;
        case DTYPE_FLOAT:
          MINMAX_FORLOOP(float);
          break;
        case DTYPE_DOUBLE:
          MINMAX_FORLOOP(double);
          break;
        default:
          break;
        }
        break;
      case AVERAGE:
#define AVERAGE_FORLOOP(type_out, type)                                  \
  for (i = 0; i < numDataItems; i++)                                     \
  {                                                                      \
    type_out accData = ((type *)(&data[(prevTimebaseIdx)*itemSize]))[i]; \
    for (j = prevTimebaseIdx + 1; j < timebaseIdx; j++)                  \
      accData += ((type *)(&data[j * itemSize]))[i];                     \
    const int range = (timebaseIdx - prevTimebaseIdx);                   \
    ((type_out *)(&outData[outSamples * outItemSize]))[i] =              \
        range > 0 ? accData / range : accData;                           \
  }
        switch (dataD->dtype)
        {
        case DTYPE_BU:
          AVERAGE_FORLOOP(float, uint8_t);
          break;
        case DTYPE_B:
          AVERAGE_FORLOOP(float, int8_t);
          break;
        case DTYPE_WU:
          AVERAGE_FORLOOP(float, uint16_t);
          break;
        case DTYPE_W:
          AVERAGE_FORLOOP(float, int16_t);
          break;
        case DTYPE_LU:
          AVERAGE_FORLOOP(float, uint32_t);
          break;
        case DTYPE_L:
          AVERAGE_FORLOOP(float, int32_t);
          break;
        case DTYPE_QU:
          AVERAGE_FORLOOP(double, uint64_t);
          break;
        case DTYPE_Q:
          AVERAGE_FORLOOP(double, int64_t);
          break;
        case DTYPE_FLOAT:
          AVERAGE_FORLOOP(float, float);
          break;
        case DTYPE_DOUBLE:
          AVERAGE_FORLOOP(double, double);
          break;
        default:
          break;
        }
        break;
      default: // Not able to do interpolation
        if (timebaseIdx > 0 && timebaseIdx < timebaseSamples)
          timebaseIdx--;
        memcpy(&outData[outSamples * outItemSize],
               &data[timebaseIdx * itemSize], itemSize);
        break;
      }
      switch (mode)
      {
      case MINMAX:
      case AVERAGE:
        refTime -= delta1;
        if (mode == MINMAX) // twice the number of points
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
  }
  else
  { // delta <= 0
    while (timebaseIdx < timebaseSamples && timebase[timebaseIdx] <= end)
      timebaseIdx++;
    outData = data;
    outDim = timebase;
    outSamples = timebaseIdx;
    outItemSize = itemSize;
    outDataArray.length = dataD->length;
    outDataArray.dtype = dataD->dtype;
  }

  DESCRIPTOR_A(outDimArray, 0, 0, 0, 0);
  DESCRIPTOR_SIGNAL_1(outSignalD, &outDataArray, 0, &outDimArray);
  // Build array descriptor for out data
  outDataArray.pointer = outDataArray.a0 = outData;
  outDataArray.arsize = outItemSize * outSamples;
  outDataArray.dimct = numDims;
  for (i = 0; i < numDims - 1; i++)
    outDataArray.m[i] = dims[i];
  outDataArray.m[numDims - 1] = outSamples;
  // If originally float, convert  dimension to float
  int64_t *timebaseQ;
  if (isQ)
  {
    timebaseQ = (int64_t *)malloc(outSamples * sizeof(int64_t));
    for (i = 0; i < outSamples; i++)
      timebaseQ[i] = (int64_t)((outDim[i] * 1e9) + 0.5);
    outDimArray.length = sizeof(int64_t);
    outDimArray.arsize = sizeof(int64_t) * outSamples;
    outDimArray.dtype = isQ;
    outDimArray.pointer = outDataArray.a0 = (char *)timebaseQ;
  }
  else
  {
    timebaseQ = NULL;
    outDimArray.length = sizeof(double);
    outDimArray.arsize = sizeof(double) * outSamples;
    outDimArray.dtype = DTYPE_DOUBLE;
    outDimArray.pointer = outDataArray.a0 = (char *)outDim;
  }

  MdsCopyDxXd((mdsdsc_t *)&outSignalD, outSignalXd);
  free(fulltimebase);
  free(timebaseQ);
  if (timebase != outDim)
  {
    free(outData);
    free(outDim);
  }
  MdsFree1Dx(&dataXd, 0);
  return 1;
}
