#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mdsshr.h>
#include <xtreeshr.h>

/*
static void printDecompiled(struct descriptor *inD)
{
  int status;
  EMPTYXD(out_xd);
  char *buf;
  extern int TdiDecompile();

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

EXPORT int XTreeDefaultSquish(struct descriptor_a *signalsApd,
			      struct descriptor *startD __attribute__ ((unused)),
			      struct descriptor *endD __attribute__ ((unused)),
			      struct descriptor *minDeltaD __attribute__ ((unused)),
			      struct descriptor_xd *outXd)
{
  EMPTYXD(emptyXd);
  //EMPTYXD(shapeExprXd);
  EMPTYXD(setRangeExprXd);
  struct descriptor_signal *currSignalD;
  struct descriptor_xd *shapesXd;
  struct descriptor_xd *dimensionsXd = 0;
  struct descriptor_a *arrayD;
  struct descriptor_a **arraysD = 0;
  int minNumShapes = 0, maxNumShapes = 0, numShapes;
  int totSize;
  int i, j, status, lastDimension;
  int outShape[64];
  char *shapeExpr = "SHAPE(DATA($1))";
  struct descriptor shapeExprD = { strlen(shapeExpr), DTYPE_T, CLASS_S, shapeExpr };
  char setRangeExpr[512];	//Safe dimension, to avoid useless mallocs
  struct descriptor setRangeExprD = { 0, DTYPE_T, CLASS_S, setRangeExpr };
  DESCRIPTOR_SIGNAL(outSignalD, 64, 0, 0);
  DESCRIPTOR_A(outDimD, 0, 0, 0, 0);
  DESCRIPTOR_A(outDataD, 0, 0, 0, 0);
  DESCRIPTOR_A(beginArrD, 0, 0, 0, 0);
  DESCRIPTOR_A(endingArrD, 0, 0, 0, 0);
  DESCRIPTOR_A(deltavalArrD, 0, 0, 0, 0);
  DESCRIPTOR_RANGE(outRangeD, &beginArrD, &endingArrD, &deltavalArrD);
  char *beginPtr=0, *endingPtr=0, *deltavalPtr=0;
  char *outDataBuf, *outDimBuf=0;

  int numSignals, numDimensions;
  int allDimensionsAreRanges;
  struct descriptor_range *rangeD, *prevRangeD;

  extern int TdiCompile(), TdiData();

  if (signalsApd->class == CLASS_XD)
    signalsApd = (struct descriptor_a *)((struct descriptor_xd *)signalsApd)->pointer;

  numSignals = signalsApd->arsize / signalsApd->length;
  if (numSignals <= 0) {
    MdsCopyDxXd((struct descriptor *)&emptyXd, outXd);
    return 1;
  }
//Check shapes. If n-multidimensional arrays, the first n-1 dimensions must fit 

//Get the shape of all segments
  shapesXd = (struct descriptor_xd *)malloc(numSignals * sizeof(struct descriptor_xd));
  for (i = 0; i < numSignals; i++)
    shapesXd[i] = emptyXd;
  for (i = 0; i < numSignals; i++) {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    status = TdiCompile(&shapeExprD, currSignalD->data, &shapesXd[i] MDS_END_ARG);
    if (!(status & 1))
      break;
    status = TdiData(&shapesXd[i], &shapesXd[i] MDS_END_ARG);
    if (!(status & 1))
      break;
    arrayD = (struct descriptor_a *)shapesXd[i].pointer;
    numShapes = arrayD->arsize / arrayD->length;
    if (i == 0)
      minNumShapes = maxNumShapes = numShapes;
    else {
      if (numShapes < minNumShapes)
	minNumShapes = numShapes;
      if (numShapes > maxNumShapes)
	maxNumShapes = numShapes;
    }
  }
//Check Shapes: maximum and minimum number of shapes can differ at most by one
  if (maxNumShapes - minNumShapes > 1)
    status = InvalidShapeInSegments;

//Check Shapes: the first maxNumShapes - 1 dimensions must fit

  for (i = 1; i < numSignals; i++) {
    for (j = 0; j < maxNumShapes - 1; j++) {
      if (((int *)((struct descriptor_a *)shapesXd[i].pointer)->pointer)[j] !=
	  ((int *)((struct descriptor_a *)shapesXd[0].pointer)->pointer)[j])
	status = InvalidShapeInSegments;
    }
  }

  if (!(status & 1)) {
    for (i = 0; i < numSignals; i++)
      MdsFree1Dx(&shapesXd[i], 0);
    free((char *)shapesXd);
    return status;
  }
//Shapes Ok, build definitive shapes array for this signal;
  arrayD = (struct descriptor_a *)shapesXd[0].pointer;
  for (i = 0; i < minNumShapes; i++)
    outShape[i] = ((int *)arrayD->pointer)[i];

  lastDimension = 0;
  for (i = 0; i < numSignals; i++) {
    arrayD = (struct descriptor_a *)shapesXd[i].pointer;
    numShapes = arrayD->arsize / arrayD->length;
    if (numShapes == maxNumShapes)
      lastDimension += ((int *)arrayD->pointer)[numShapes - 1];
    else
      lastDimension++;
  }
  outShape[maxNumShapes - 1] = lastDimension;
  //The total number of shapes is maxNumShapes
  numShapes = maxNumShapes;

//Free shape stuff      
  for (i = 0; i < numSignals; i++)
    MdsFree1Dx(&shapesXd[i], 0);
  free((char *)shapesXd);

//Check that the number of dimensions in signals is the same
  currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[0];
  numDimensions = currSignalD->ndesc - 2;

  for (i = 1; i < numSignals; i++) {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    if (numDimensions != currSignalD->ndesc - 2)
      status = InvalidDimensionInSegments;
  }
  if (!(status & 1))
    return status;

//Check whether all dimensions are axpressed as a range. In this case a global range is built instead 
//of merging dimension arrays
  allDimensionsAreRanges = 1;
  for (i = 0; i < numSignals && allDimensionsAreRanges; i++) {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    if (currSignalD->dimensions[0]->class != CLASS_R
	|| currSignalD->dimensions[0]->dtype != DTYPE_RANGE)
      allDimensionsAreRanges = 0;
    //Check also that the dtype for start, end, delta are the same
    if (i > 0) {
      rangeD = (struct descriptor_range *)currSignalD->dimensions[0];
      prevRangeD =
	  (struct descriptor_range *)((((struct descriptor_signal **)signalsApd->pointer)[i - 1])->
				      dimensions[0]);
      if (!rangeD->begin || rangeD->begin->class != CLASS_S || prevRangeD->begin->class != CLASS_S
	  || rangeD->begin->dtype != prevRangeD->begin->dtype)
	allDimensionsAreRanges = 0;
      if (!rangeD->ending || rangeD->ending->class != CLASS_S
	  || prevRangeD->ending->class != CLASS_S
	  || rangeD->ending->dtype != prevRangeD->ending->dtype)
	allDimensionsAreRanges = 0;
      if (!rangeD->deltaval || rangeD->deltaval->class != CLASS_S
	  || prevRangeD->deltaval->class != CLASS_S
	  || rangeD->deltaval->dtype != prevRangeD->deltaval->dtype)
	allDimensionsAreRanges = 0;
    }
  }

  if (!allDimensionsAreRanges)	//Not all dimensions are ranges, merge arrays
  {

    //Merge first dimension of each signal. It has to be evaluated before.
    dimensionsXd = (struct descriptor_xd *)malloc(numSignals * sizeof(struct descriptor_xd));
    for (i = 0; i < numSignals; i++)
      dimensionsXd[i] = emptyXd;

    arraysD = (struct descriptor_a **)malloc(numSignals * sizeof(char *));

    //Evaluate first dimension for all segments
    totSize = 0;
    for (i = 0; i < numSignals; i++) {
      currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];

      if (currSignalD->dimensions[0]->class == CLASS_A)	//Data evaluation not needed
      {
	arraysD[i] = arrayD = (struct descriptor_a *)currSignalD->dimensions[0];
      } else {
	status = TdiData(currSignalD->dimensions[0], &dimensionsXd[i] MDS_END_ARG);
	if (!(status & 1))
	  break;
	arraysD[i] = arrayD = (struct descriptor_a *)dimensionsXd[i].pointer;
	if (!arrayD || arrayD->class != CLASS_A)	//Every first dimension must be evaluated to an array
	{
	  status = InvalidDimensionInSegments;
	  break;
	}
      }
      totSize += arrayD->arsize;
    }

    if (!(status & 1)) {
      for (i = 0; i < numSignals; i++)
	MdsFree1Dx(&dimensionsXd[i], 0);
      free((char *)dimensionsXd);
      return status;
    }

    //Merge first dimension in outDim array. It is assumed that the first dimension has been evaluaed to a 1D array
//              memcpy(&outDimD, dimensionsXd[0].pointer, sizeof(struct descriptor_a));
    memcpy(&outDimD, arraysD[0], sizeof(struct descriptor_a));

    outDimBuf = malloc(totSize);
    outDimD.pointer = outDimBuf;
    outDimD.arsize = totSize;
    for (i = j = 0; i < numSignals; i++) {
//                      arrayD = (struct descriptor_a *)dimensionsXd[i].pointer;
      arrayD = arraysD[i];
      memcpy(&outDimBuf[j], arrayD->pointer, arrayD->arsize);
      j += arrayD->arsize;
    }

    outSignalD.dimensions[0] = (struct descriptor *)&outDimD;
  } else			//All dimensions are expressed as range
  {
    if (numSignals == 1) {
      outSignalD.dimensions[0] = currSignalD->dimensions[0];
    } else {
      currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[0];
      rangeD = (struct descriptor_range *)currSignalD->dimensions[0];
      beginPtr = malloc(rangeD->begin->length * numSignals);
      endingPtr = malloc(rangeD->ending->length * numSignals);
      deltavalPtr = malloc(rangeD->deltaval->length * numSignals);
      beginArrD.length = rangeD->begin->length;
      beginArrD.dtype = rangeD->begin->dtype;
      beginArrD.arsize = rangeD->begin->length * numSignals;
      beginArrD.pointer = beginPtr;
      endingArrD.length = rangeD->ending->length;
      endingArrD.dtype = rangeD->ending->dtype;
      endingArrD.arsize = rangeD->ending->length * numSignals;
      endingArrD.pointer = endingPtr;
      deltavalArrD.length = rangeD->deltaval->length;
      deltavalArrD.dtype = rangeD->deltaval->dtype;
      deltavalArrD.arsize = rangeD->deltaval->length * numSignals;
      deltavalArrD.pointer = deltavalPtr;

      for (i = 0; i < numSignals; i++) {
	rangeD =
	    (struct descriptor_range *)((((struct descriptor_signal **)signalsApd->pointer)[i])->
					dimensions[0]);
	memcpy(&beginPtr[i * rangeD->begin->length], rangeD->begin->pointer, rangeD->begin->length);
	memcpy(&endingPtr[i * rangeD->ending->length], rangeD->ending->pointer,
	       rangeD->ending->length);
	memcpy(&deltavalPtr[i * rangeD->deltaval->length], rangeD->deltaval->pointer,
	       rangeD->deltaval->length);
      }
      outSignalD.dimensions[0] = (struct descriptor *)&outRangeD;
    }
  }

//Build remaining outSignal dimension(s)
  outSignalD.ndesc = 2 + numDimensions;
  currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[0];
  for (i = 1; i < numDimensions; i++)
    outSignalD.dimensions[i] = currSignalD->dimensions[i];

//Merge data. The data field of the signals in the APD contain the actual arrays

//Collect total data size
  totSize = 0;
  for (i = 0; i < numSignals; i++) {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    arrayD = (struct descriptor_a *)currSignalD->data;
    totSize += arrayD->arsize;
  }
  outDataBuf = malloc(totSize);
  outDataD.pointer = outDataBuf;
  outDataD.arsize = totSize;
  currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[0];
  arrayD = (struct descriptor_a *)currSignalD->data;
  outDataD.length = arrayD->length;
  outDataD.dtype = arrayD->dtype;
  for (i = j = 0; i < numSignals; i++) {
    currSignalD = ((struct descriptor_signal **)signalsApd->pointer)[i];
    arrayD = (struct descriptor_a *)currSignalD->data;
    memcpy(&outDataBuf[j], arrayD->pointer, arrayD->arsize);
    j += arrayD->arsize;
  }

  //Reshape outData based on outShape information
  sprintf(setRangeExpr, "SET_RANGE(%d", outShape[0]);
  for (i = 1; i < numShapes; i++)
    sprintf(&setRangeExpr[strlen(setRangeExpr)], ",%d", outShape[i]);
  sprintf(&setRangeExpr[strlen(setRangeExpr)], ",$1)");

  setRangeExprD.length = strlen(setRangeExpr);
  status = TdiCompile(&setRangeExprD, &outDataD, &setRangeExprXd MDS_END_ARG);
  status = TdiData(&setRangeExprXd, &setRangeExprXd MDS_END_ARG);
  outSignalD.data = setRangeExprXd.pointer;
  outSignalD.raw = 0;

//Input signals merged now to outSignalD, Copy result back
  status = MdsCopyDxXd((struct descriptor *)&outSignalD, outXd);

//free stuff
  free(outDataBuf);
  if (allDimensionsAreRanges) {
    if (numSignals > 1) {
      if (beginPtr) free(beginPtr);
      if (endingPtr) free(endingPtr);
      if (deltavalPtr) free(deltavalPtr);
    }
  } else {
    for (i = 0; i < numSignals; i++)
      MdsFree1Dx(&dimensionsXd[i], 0);
    if (dimensionsXd) free((char *)dimensionsXd);
    if (outDimBuf) free(outDimBuf);
    if (arraysD) free((char *)arraysD);
  }
  MdsFree1Dx(&setRangeExprXd, 0);
  return status;
}
