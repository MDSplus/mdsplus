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
#include <status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xtreeshr.h>

int getShape(struct descriptor *dataD, int *dims, int *numDims)
{
  /*
   * returns 1-D and N-D shaped arrays
   */
  ARRAY_COEFF(char *, MAX_DIMS) * arrPtr;
  if (dataD->class != CLASS_A)
  {
    *numDims = 1;
    dims[0] = 0;
    return 0;
  }
  arrPtr = (void *)dataD;
  if (arrPtr->dimct == 1)
  {
    *numDims = 1;
    dims[0] = arrPtr->arsize / arrPtr->length;
  }
  else
    memcpy(dims, arrPtr->m, (*numDims = arrPtr->dimct) * sizeof(*arrPtr->m));
  return 1;
}

static inline int checkShapes(struct descriptor_a *signalsApd, int *totShapes,
                              int *outShape)
{
  /*
   * checks if data shapes are fit for concatenation
   */

  // Get the shape of all segments
  struct descriptor_signal *currSignalD;
  int i, j, numSignals = signalsApd->arsize / signalsApd->length;
  int minNumShapes = 0, maxNumShapes = 0;
  int *numShapes = (int *)malloc(numSignals * sizeof(int));
  int **shapes = (int **)malloc(numSignals * sizeof(int *));
  for (i = 0; i < numSignals; i++)
  {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    shapes[i] = (int *)malloc(54 * sizeof(int));
    getShape((struct descriptor *)(currSignalD->data), shapes[i],
             &numShapes[i]);
    if (i == 0)
      minNumShapes = maxNumShapes = numShapes[i];
    else
    {
      if (numShapes[i] < minNumShapes)
        minNumShapes = numShapes[i];
      if (numShapes[i] > maxNumShapes)
        maxNumShapes = numShapes[i];
    }
  }

  // Check Shapes: maximum and minimum number of shapes can differ at most by
  // one
  int status = 1;
  if (maxNumShapes - minNumShapes > 1 || maxNumShapes > MAX_DIMS)
    status = InvalidShapeInSegments;
  else
    // Check Shapes: the first maxNumShapes - 1 dimensions must fit
    for (i = 1; i < numSignals; i++)
      for (j = 0; j < maxNumShapes - 1; j++)
        if (shapes[i][j] != shapes[0][j])
          status = InvalidShapeInSegments;

  if (STATUS_OK)
  {
    // Shapes Ok, build definitive shapes array for this signal;
    for (i = 0; i < minNumShapes; i++)
      outShape[i] = shapes[0][i];
    int lastDimension = 0;
    for (i = 0; i < numSignals; i++)
    {
      if (numShapes[i] == maxNumShapes)
        lastDimension += shapes[i][numShapes[i] - 1];
      else
        lastDimension++;
    }
    *totShapes = maxNumShapes;
    outShape[*totShapes - 1] = lastDimension;
  }
  else
    *totShapes = 0;

  // Free shape stuff
  for (i = 0; i < numSignals; i++)
    free(shapes[i]);
  free(shapes);
  free(numShapes);
  return status;
}

static inline int checkNumDimensions(struct descriptor_a *signalsApd,
                                     int *numDimensions)
{
  /*
   * checks if dimensions are fit for merge
   */
  int numDesc = ((struct descriptor_signal **)signalsApd->pointer)[0]->ndesc;
  int i, numSignals = signalsApd->arsize / signalsApd->length;
  for (i = 1; i < numSignals; i++)
    if (numDesc != ((struct descriptor_signal **)signalsApd->pointer)[i]->ndesc)
      return InvalidDimensionInSegments;
  *numDimensions = numDesc - 2;
  return 1;
}

static inline int checkRanges(struct descriptor_a *signalsApd)
{
  /*
   * checks if class and dtypes of dimensions are fit for mergeRanges
   */
  struct descriptor_signal *currSignalD =
      ((struct descriptor_signal **)signalsApd->pointer)[0];
  struct descriptor_range *firstRangeD =
      (struct descriptor_range *)currSignalD->dimensions[0];
  // Check if dimension is of type RANGE
  if (firstRangeD->class != CLASS_R || firstRangeD->dtype != DTYPE_RANGE ||
      !firstRangeD->begin || firstRangeD->begin->class != CLASS_S ||
      !firstRangeD->ending || firstRangeD->ending->class != CLASS_S)
    return B_FALSE;
  if (firstRangeD->ndesc == 3 &&
      (!firstRangeD->begin || firstRangeD->begin->class != CLASS_S))
    return B_FALSE; // delta given but invalid
  int i, numSignals = signalsApd->arsize / signalsApd->length;
  for (i = 1; i < numSignals; i++)
  {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    struct descriptor_range *currRangeD =
        (struct descriptor_range *)currSignalD->dimensions[0];
    // Check if dimension is of type RANGE
    if (currRangeD->class != CLASS_R || currRangeD->dtype != DTYPE_RANGE ||
        currRangeD->ndesc != firstRangeD->ndesc // either all have delta or none
        || !currRangeD->begin || currRangeD->begin->class != CLASS_S ||
        !currRangeD->ending || currRangeD->ending->class != CLASS_S ||
        currRangeD->begin->dtype != firstRangeD->begin->dtype ||
        currRangeD->ending->dtype != firstRangeD->ending->dtype ||
        (currRangeD->ndesc == 3 // check delta if given
         && (!currRangeD->deltaval || currRangeD->deltaval->class != CLASS_S ||
             currRangeD->deltaval->dtype != firstRangeD->deltaval->dtype)))
      return B_FALSE;
  }
  return B_TRUE;
}

static inline struct descriptor *
mergeRanges(struct descriptor_a *signalsApd,
            struct descriptor_range *outRangeD)
{
  /*
   * combines all Range dimensions into one total dimension
   */
  int numSignals = signalsApd->arsize / signalsApd->length;
  if (numSignals == 1)
    return ((struct descriptor_signal **)signalsApd->pointer)[0]->dimensions[0];
  struct descriptor_range *rangeD =
      (struct descriptor_range *)((struct descriptor_signal **)
                                      signalsApd->pointer)[0]
          ->dimensions[0];
  ((struct descriptor_a *)outRangeD->begin)->length = rangeD->begin->length;
  ((struct descriptor_a *)outRangeD->begin)->dtype = rangeD->begin->dtype;
  ((struct descriptor_a *)outRangeD->begin)->arsize =
      rangeD->begin->length * numSignals;
  ((struct descriptor_a *)outRangeD->begin)->pointer =
      malloc(rangeD->begin->length * numSignals);
  ((struct descriptor_a *)outRangeD->ending)->length = rangeD->ending->length;
  ((struct descriptor_a *)outRangeD->ending)->dtype = rangeD->ending->dtype;
  ((struct descriptor_a *)outRangeD->ending)->arsize =
      rangeD->ending->length * numSignals;
  ((struct descriptor_a *)outRangeD->ending)->pointer =
      malloc(rangeD->ending->length * numSignals);
  if (rangeD->ndesc > 2)
  {
    ((struct descriptor_a *)outRangeD->deltaval)->length =
        rangeD->deltaval->length;
    ((struct descriptor_a *)outRangeD->deltaval)->dtype =
        rangeD->deltaval->dtype;
    ((struct descriptor_a *)outRangeD->deltaval)->arsize =
        rangeD->deltaval->length * numSignals;
    ((struct descriptor_a *)outRangeD->deltaval)->pointer =
        malloc(rangeD->deltaval->length * numSignals);
  }
  else
    outRangeD->ndesc = 2;
  int i;
  for (i = 0; i < numSignals; i++)
  {
    rangeD = (struct descriptor_range *)((((struct descriptor_signal **)
                                               signalsApd->pointer)[i])
                                             ->dimensions[0]);
    memcpy(&(((struct descriptor_a *)outRangeD->begin)
                 ->pointer)[i * rangeD->begin->length],
           rangeD->begin->pointer, rangeD->begin->length);
    memcpy(&(((struct descriptor_a *)outRangeD->ending)
                 ->pointer)[i * rangeD->ending->length],
           rangeD->ending->pointer, rangeD->ending->length);
    if (outRangeD->ndesc > 2)
      memcpy(&(((struct descriptor_a *)outRangeD->deltaval)
                   ->pointer)[i * rangeD->deltaval->length],
             rangeD->deltaval->pointer, rangeD->deltaval->length);
  }
  return (struct descriptor *)outRangeD;
}

static EMPTYXD(emptyXd);
static inline int mergeArrays(struct descriptor_a *signalsApd,
                              struct descriptor_xd **dimensionsXd,
                              struct descriptor_a *outDimD, char **arraysBuf,
                              char **outDimBuf)
{
  /*
   * converts all dimensions into arrays and concatenates to total array
   */
  int status = 1;
  int i, j, numSignals = signalsApd->arsize / signalsApd->length;
  // Merge first dimension of each signal. It has to be evaluated before.
  struct descriptor_a *arrayD;
  struct descriptor_a **arraysD;
  *dimensionsXd =
      (struct descriptor_xd *)malloc(numSignals * sizeof(struct descriptor_xd));
  for (i = 0; i < numSignals; i++)
    (*dimensionsXd)[i] = emptyXd;
  size_t arraysDsize = numSignals * sizeof(char *);
  *arraysBuf = malloc(arraysDsize);
  arraysD = (struct descriptor_a **)(*arraysBuf);
  memset(arraysD, 0, arraysDsize);
  // Evaluate first dimension for all segments
  int totSize = 0;
  for (i = 0; i < numSignals; i++)
  {
    struct descriptor_signal *currSignalD =
        ((struct descriptor_signal **)signalsApd->pointer)[i];
    if (currSignalD->dimensions[0]->class ==
        CLASS_A) // Data evaluation not needed
      arraysD[i] = arrayD = (struct descriptor_a *)currSignalD->dimensions[0];
    else
    {
      extern int TdiData();
      status =
          TdiData(currSignalD->dimensions[0], &(*dimensionsXd)[i] MDS_END_ARG);
      if (STATUS_NOT_OK)
        break;
      arraysD[i] = arrayD = (struct descriptor_a *)(*dimensionsXd)[i].pointer;
      if (!arrayD ||
          arrayD->class !=
              CLASS_A)
      { // Every first dimension must be evaluated to an array
        status = InvalidDimensionInSegments;
        break;
      }
    }
    totSize += arrayD->arsize;
  }
  if (STATUS_NOT_OK)
    return status;
  // Merge first dimension in outDim array. It is assumed that the first
  // dimension has been evaluaed to a 1D array
  memcpy(outDimD, arraysD[0], sizeof(struct descriptor_a));
  outDimD->aflags.coeff = 0;
  *outDimBuf = malloc(totSize);
  outDimD->pointer = *outDimBuf;
  outDimD->arsize = totSize;
  for (i = j = 0; i < numSignals; i++)
  {
    arrayD = arraysD[i];
    memcpy(&(*outDimBuf)[j], arrayD->pointer, arrayD->arsize);
    j += arrayD->arsize;
  }
  return status;
}

EXPORT int XTreeDefaultSquish(struct descriptor_a *signalsApd,
                              struct descriptor *startD __attribute__((unused)),
                              struct descriptor *endD __attribute__((unused)),
                              struct descriptor *minDeltaD
                              __attribute__((unused)),
                              struct descriptor_xd *outXd)
{
  if (signalsApd->class == CLASS_XD)
    signalsApd =
        (struct descriptor_a *)((struct descriptor_xd *)signalsApd)->pointer;
  int numSignals = signalsApd->arsize / signalsApd->length;
  if (numSignals <= 0)
  {
    MdsCopyDxXd((struct descriptor *)&emptyXd, outXd);
    return 1;
  }
  // Get the shape of all segments
  int totShapes;
  int outShape[MAX_DIMS];
  int status = checkShapes(signalsApd, &totShapes, outShape);
  if (STATUS_NOT_OK)
    return status;
  // Check that the number of dimensions in signals is the same
  int numDimensions = 0;
  status = checkNumDimensions(signalsApd, &numDimensions);
  if (STATUS_NOT_OK)
    return status;
  DESCRIPTOR_SIGNAL(outSignalD, MAX_DIMS, 0, 0);

  // Check whether all dimensions are expressed as a range.
  int canDoMergeRanges = checkRanges(signalsApd);
  // stack for mergeRange
  DESCRIPTOR_A(beginArrD, 0, 0, 0, 0);
  DESCRIPTOR_A(endingArrD, 0, 0, 0, 0);
  DESCRIPTOR_A(deltavalArrD, 0, 0, 0, 0);
  DESCRIPTOR_RANGE(outRangeD, &beginArrD, &endingArrD, &deltavalArrD);

  // stack for mergeArrays
  struct descriptor_xd *dimensionsXd = 0;
  DESCRIPTOR_A(outDimD, 0, 0, 0, 0);
  char *outDimBuf = 0, *arraysBuf = 0;

  if (canDoMergeRanges) // merge in total range
    outSignalD.dimensions[0] = mergeRanges(signalsApd, &outRangeD);
  else
  { // Not all dimensions are ranges, merge arrays
    status =
        mergeArrays(signalsApd, &dimensionsXd, (struct descriptor_a *)&outDimD,
                    &arraysBuf, &outDimBuf);
    if (STATUS_OK)
      outSignalD.dimensions[0] = (struct descriptor *)&outDimD;
    else
    {
      numDimensions = 0;
    }
  }

  // Build remaining outSignal dimension(s)
  int i, j;
  outSignalD.ndesc = 2 + numDimensions;
  struct descriptor_signal *currSignalD =
      ((struct descriptor_signal **)signalsApd->pointer)[0];
  for (i = 1; i < numDimensions; i++)
    outSignalD.dimensions[i] = currSignalD->dimensions[i];

  // Merge data. The data field of the signals in the APD contain the actual
  // arrays
  // Collect total data size
  int totSize = 0;
  for (i = 0; i < numSignals; i++)
  {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    totSize += ((struct descriptor_a *)currSignalD->data)->arsize;
  }
  DESCRIPTOR_A_COEFF(outDataD, 0, 0, 0, MAX_DIMS, 0);
  char *outDataBuf = malloc(totSize);
  outDataD.pointer = outDataD.a0 = outDataBuf;
  outDataD.arsize = totSize;
  currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[0];
  struct descriptor_a *arrayD = (struct descriptor_a *)currSignalD->data;
  outDataD.length = arrayD->length;
  outDataD.dtype = arrayD->dtype;
  for (i = j = 0; i < numSignals; i++)
  {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    arrayD = (struct descriptor_a *)currSignalD->data;
    memcpy(&outDataBuf[j], arrayD->pointer, arrayD->arsize);
    j += arrayD->arsize;
  }
  outDataD.dimct = totShapes;
  for (i = 0; i < totShapes; i++)
    outDataD.m[i] = outShape[i];
  outSignalD.data = (struct descriptor *)&outDataD;

  // Input signals merged now to outSignalD, Copy result back
  status = MdsCopyDxXd((struct descriptor *)&outSignalD, outXd);

  // free stuff
  free(outDataBuf);
  if (canDoMergeRanges)
  {
    if (numSignals > 1)
    {
      free(beginArrD.pointer);
      free(endingArrD.pointer);
      free(deltavalArrD.pointer);
    }
  }
  else
  {
    for (i = 0; i < numSignals; i++)
      MdsFree1Dx(&dimensionsXd[i], 0);
    free(dimensionsXd);
    free(outDimBuf);
    free(arraysBuf);
  }
  return status;
}
