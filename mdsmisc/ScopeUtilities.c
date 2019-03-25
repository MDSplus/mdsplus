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
#include <math.h>
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mds_stdarg.h>
#include <stdio.h>
#include <string.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <mdstypes.h>
#include <tdishr.h>
#include <xtreeshr.h>
#include <ncidef.h>
#define MAX_LIMIT 1E10

static inline void swap2(char *buf,const int a, const int b){
  const char tmp = buf[a];
  buf[a] = buf[b];
  buf[b] = tmp;
}
static inline void swap4(char *buf){
  swap2(buf,0,3);
  swap2(buf,1,2);
}
static inline void swap8(char *buf) {
  swap2(buf,0,7);
  swap2(buf,1,6);
  swap4(buf+2);
}
static double to_double(void *ptr, dtype_t dtype) {
  switch(dtype) {
    case DTYPE_FLOAT:
      return (double)(*( float  *)ptr);
    case DTYPE_DOUBLE:
      return          *( double *)ptr;
    case DTYPE_B:
    case DTYPE_BU:
      return (double)(*( int8_t *)ptr);
    case DTYPE_W:
    case DTYPE_WU:
      return (double)(*(int16_t *)ptr);
    case DTYPE_L:
    case DTYPE_LU:
      return (double)(*(int32_t *)ptr);
    case DTYPE_Q:
    case DTYPE_QU:
      return (double)(*(int64_t *)ptr);
    default:
      printf("Unsupported Type in getData\n");
      return 0;
  }
}
static int recIsSegmented(mdsdsc_t *dsc) {
/* returns nid of the first segmented node found
 * or 0 if none found
 */
  if(!dsc) return 0;
  int nid, numSegments, status, i;
  char *path;
  int retClassLen, retDtypeLen;
  unsigned int nciClass, nciDtype;
  struct nci_itm nciList[] = {{1, NciCLASS, &nciClass, &retClassLen},
    {1, NciDTYPE, &nciDtype, &retDtypeLen},
    {NciEND_OF_LIST, 0, 0, 0}};
  EMPTYXD(xd);
  mdsdsc_r_t *rDsc;
  switch (dsc->class) {
    case CLASS_S: {
      if(dsc->dtype == DTYPE_NID) {
	nid = *(int *)dsc->pointer;
	status = TreeGetNumSegments(nid, &numSegments);
	if (STATUS_OK && numSegments > 0)
	  return nid;
	//Now check if the node contains an expression or a direct nid reference
	status = TreeGetNci(nid, nciList);
	if (STATUS_OK
         &&( nciClass == CLASS_R
          ||(nciClass == CLASS_S
            &&( nciDtype == DTYPE_NID
	      ||nciDtype == DTYPE_PATH)
	 ))) {
	  status = TreeGetRecord(nid, &xd);
	  if (STATUS_OK && xd.l_length > 0) {
	    nid = recIsSegmented(xd.pointer);
	    MdsFree1Dx(&xd, 0);
	    return nid;
	  }
	}
      } else if(dsc->dtype == DTYPE_PATH) {
	path = malloc(dsc->length + 1);
	memcpy(path, dsc->pointer, dsc->length);
	path[dsc->length] = 0;
	status = TreeFindNode(path, &nid);
	if STATUS_OK
	  status = TreeGetNumSegments(nid, &numSegments);
	if (STATUS_OK && numSegments > 0)
	  return nid;
      }
      break;
    }
    case CLASS_A:
      break;
    case CLASS_XD:
      if (dsc->pointer)
	return recIsSegmented((mdsdsc_t*)dsc->pointer);
    case CLASS_R: {
      rDsc = (mdsdsc_r_t*)dsc;
      for(i = 0; i < rDsc->ndesc; i++) {
	if(rDsc->dscptrs[i] && (nid = recIsSegmented(rDsc->dscptrs[i])))
	  return nid;
      }
      break;
    }
    default:
      break;
  }
  return 0;
}

static inline int getSegmentedNid(char *expr){
  int nid;
  INIT_AND_FREEXD_ON_EXIT(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  const int status = TdiCompile(&exprD, &xd MDS_END_ARG);
  nid = STATUS_OK ? recIsSegmented(xd.pointer) : 0;
  FREEXD_NOW(xd);
  return nid;
}

#define NUM_SEGMENTS_THRESHOLD 10
#define NUM_SAMPLES_THRESHOLD 10000000

/**
* Make an estimation of the number of samples in the specified interval. -1 is returned when
* when online minmax reasampling is deemed not necessary. If the number
**/

inline static int64_t estimateNumSamples(mdsdsc_t *dsc, double *xMin, double *xMax, int *estimatedSegmentSamples, double *estimatedDuration) {
/* return the number of samples the signal holds based on meta information
   or -1 if something went wrong
 */
  int numSegments, startIdx, endIdx;
  int64_t startTime, endTime = INT64_MAX, currStartTime, currEndTime;
  char dtype, dimct;
  int dims[64];
  int nextRow, segmentSamples, numActSegments, segmentIdx;
  EMPTYXD(xd);
  int nid = recIsSegmented(dsc);
  if(!nid) return -1;
  int status = TreeGetNumSegments(nid, &numSegments);
  if STATUS_NOT_OK return -1;
  if(xMin != NULL || xMax != NULL) {
    if(xMin) startTime = (int64_t) (*xMin * 1e9);
    if(xMax)   endTime = (int64_t) (*xMax * 1e9);
    if(numSegments < NUM_SEGMENTS_THRESHOLD) return -1;
    startIdx = 0; //If no start time specified, take all initial segments
    if(xMin) {
      while(startIdx < numSegments) {
        status = TreeGetSegmentLimits(nid, startIdx, &xd, NULL);
        if STATUS_NOT_OK goto return_neg1;
        status = XTreeConvertToLongTime(xd.pointer, &currStartTime);
        if STATUS_NOT_OK goto return_neg1;
        if(currStartTime > startTime) //First overlapping segment
          break;
        startIdx++;
      }
    }
    if(startIdx == numSegments) goto return_neg1;//All segments antecedent to start time
    if(!xMax)
      endIdx = numSegments - 1;
    else {
      segmentIdx = startIdx;
      while(segmentIdx < numSegments) {
        status = TreeGetSegmentLimits(nid, segmentIdx, NULL, &xd);
        if STATUS_NOT_OK goto return_neg1;
        status = XTreeConvertToLongTime(xd.pointer, &currEndTime);
        if STATUS_NOT_OK goto return_neg1;
        if(currEndTime >= endTime) //Last overlapping segment
          break;
        segmentIdx++;
      }
      //No segment (section) after end
      endIdx = (segmentIdx == numSegments) ? numSegments - 1 : segmentIdx;
    }
    numActSegments = endIdx - startIdx + 1;
  } else {
    startIdx = 0;
    numActSegments = numSegments;
    endIdx = numSegments - 1;
  }
  status = TreeGetSegmentInfo(nid, startIdx, &dtype, &dimct, dims, &nextRow);
  if STATUS_NOT_OK goto return_neg1;
  segmentSamples = dims[dimct - 1];
  //Compute duration
  status = TreeGetSegmentLimits(nid, startIdx, &xd, NULL);
  if STATUS_OK status = TdiData( (mdsdsc_t*)&xd, &xd MDS_END_ARG);
  if STATUS_OK status = TdiFloat((mdsdsc_t*)&xd, &xd MDS_END_ARG);
  if STATUS_NOT_OK goto return_neg1;
  const double startSig = to_double(xd.pointer->pointer,xd.pointer->dtype);
  status = TreeGetSegmentLimits(nid, endIdx, NULL, &xd);
  if STATUS_OK status = TdiData( (mdsdsc_t*)&xd, &xd MDS_END_ARG);
  if STATUS_OK status = TdiFloat((mdsdsc_t*)&xd, &xd MDS_END_ARG);
  const double endSig = to_double(xd.pointer->pointer,xd.pointer->dtype);
  MdsFree1Dx(&xd, NULL);
  *estimatedDuration = endSig - startSig;
  *estimatedSegmentSamples = segmentSamples;
  return segmentSamples * (int64_t)numActSegments;
return_neg1: ;
  MdsFree1Dx(&xd, NULL);
  return -1;
}

//Perform trim on site (do not realloc arrays) return the actual number of points
//the type of X array is unknown, element size is passed in xSize
static void trimData(float *y, char *x, int xSize, int xType, int nSamples, int reqPoints, double xMin, double xMax, int *retPoints, float *retResolution) {
  if(nSamples < 10 * reqPoints) {
    //Does not perform any compression
    *retResolution = 1E12;
    *retPoints = nSamples;
    return;
  }
  //From here, consider xMin and xMax
  int startIdx, endIdx;
  for(startIdx = 0;        startIdx < nSamples && to_double(&x[startIdx * xSize], xType) < xMin; startIdx++);
  if(startIdx == nSamples) startIdx--;
  for(  endIdx = startIdx;   endIdx < nSamples && to_double(&x[  endIdx * xSize], xType) < xMax;   endIdx++);
  if(  endIdx == nSamples)   endIdx--;
  const int deltaSamples = ((endIdx - startIdx) < 10 * reqPoints) ? 1 : (endIdx - startIdx + 1) / reqPoints;

  int currSamples = startIdx;
  int outIdx = 0;
  if(deltaSamples == 1) {
    *retResolution = 1E12;
    while (currSamples < endIdx) {
      memcpy(&x[outIdx * xSize], &x[currSamples * xSize], xSize);
      y[outIdx++] = y[currSamples++];
    }
  } else {
    *retResolution = reqPoints/(to_double(&x[endIdx * xSize], xType) - to_double(&x[startIdx * xSize], xType));
    // printf("Resolution: %f, points: %d, interval: %f\n", *retResolution, reqPoints, (to_double(&x[endIdx * xSize], xType) - to_double(&x[startIdx * xSize], xType)));
    const double deltaTime = (deltaSamples == 1) ? 0 : (xMax - xMin)/(double)reqPoints;
    double currXDouble;
    float minY, maxY;
    while(currSamples < endIdx) {
      minY = maxY = y[currSamples];
      int i,actSamples = 0;
      const double endXDouble = to_double(&x[currSamples * xSize], xType) + deltaTime;
      for(i = currSamples; i < nSamples && i < currSamples + deltaSamples; i++, actSamples++) {
	currXDouble = to_double(&x[i * xSize], xType);
	if(currXDouble > endXDouble) break; //Handle dual speed clocks
	if(y[i] < minY) minY = y[i];
	if(y[i] > maxY) maxY = y[i];
      }
      memcpy(&x[outIdx * xSize], &x[currSamples * xSize], xSize);
      y[outIdx++] = minY;
      memcpy(&x[outIdx * xSize], &x[currSamples * xSize], xSize);
      y[outIdx++] = maxY;
      currSamples += actSamples;
    }
  }
  *retPoints = outIdx;
}

static char *recGetHelp(mdsdsc_t *dsc) {
  int nid, numSegments, status, i;
  char *path, *help;
  EMPTYXD(xd);
  mdsdsc_r_t *rDsc;
  mds_param_t *pDsc;
  if(!dsc) return NULL;
  switch (dsc->class) {
    case CLASS_S:
      if(dsc->dtype == DTYPE_NID) {
	nid = *(int *)dsc->pointer;
	status = TreeGetNumSegments(nid, &numSegments);
	if(STATUS_NOT_OK || numSegments > 0)
	  return NULL;
	status = TreeGetRecord(nid, &xd);
	if STATUS_OK {
	  help = recGetHelp(xd.pointer);
	  MdsFree1Dx(&xd, 0);
	  return help;
	}
	return NULL;
      } else if(dsc->dtype == DTYPE_PATH) {
	path = malloc(dsc->length + 1);
	memcpy(path, dsc->pointer, dsc->length);
	path[dsc->length] = 0;
	status = TreeFindNode(path, &nid);
	if STATUS_NOT_OK
	  return NULL;
	status = TreeGetNumSegments(nid, &numSegments);
	if (STATUS_NOT_OK || numSegments > 0)
	  return NULL;
	status = TreeGetRecord(nid, &xd);
	if STATUS_OK {
	  help = recGetHelp(xd.pointer);
	  MdsFree1Dx(&xd, 0);
	  return help;
	}
	return NULL;
      }
      return NULL;
    case CLASS_A:
      return NULL;
    case CLASS_XD:
      if(!dsc->pointer) return NULL;
      return recGetHelp((mdsdsc_t*)dsc->pointer);
    case CLASS_R:
      if(dsc->dtype == DTYPE_PARAM) {
	help = NULL;
	pDsc = (mds_param_t *)dsc;
	if(pDsc->help) {
	  status = TdiData(pDsc->help, &xd MDS_END_ARG);
	  if (STATUS_OK && xd.pointer && xd.pointer->class == CLASS_S && xd.pointer->dtype == DTYPE_T) {
	    help = malloc(xd.pointer->length + 1);
	    memcpy(help, xd.pointer->pointer, xd.pointer->length);
	    help[xd.pointer->length] = 0;
	  }
	  MdsFree1Dx(&xd, 0);
	}
	return help;
      }
      rDsc = (mdsdsc_r_t *)dsc;
      for(i = 0; i < rDsc->ndesc; i++) {
	if((help = recGetHelp(rDsc->dscptrs[i]))!=NULL)
	return help;
      }
      return NULL;
    default:
      return NULL;
  }
}

static char *recGetUnits(mdsdsc_t *dsc, int isX) {
  if(!dsc) return NULL;
  int nid, numSegments, status, i, unitsLen;
  char *path, *units, *currUnits;
  char **unitsArr;
  char *mulDiv;
  EMPTYXD(xd);
  mdsdsc_r_t *rDsc;
  mds_with_units_t *uDsc;
  mds_signal_t *sDsc;
  mds_param_t *pDsc;
  switch (dsc->class) {
    case CLASS_S:
      if(dsc->dtype == DTYPE_NID) {
	nid = *(int *)dsc->pointer;
	status = TreeGetNumSegments(nid, &numSegments);
	if (STATUS_NOT_OK || numSegments > 0)
	  return NULL;
	status = TreeGetRecord(nid, &xd);
	if STATUS_OK {
	  units = recGetUnits(xd.pointer, isX);
	  MdsFree1Dx(&xd, 0);
	  return units;
	}
      } else if(dsc->dtype == DTYPE_PATH) {
	path = malloc(dsc->length + 1);
	memcpy(path, dsc->pointer, dsc->length);
	path[dsc->length] = 0;
	status = TreeFindNode(path, &nid);
	if STATUS_NOT_OK
	  return NULL;
	status = TreeGetNumSegments(nid, &numSegments);
	if (STATUS_NOT_OK || numSegments > 0)
	  return NULL;
	status = TreeGetRecord(nid, &xd);
	if STATUS_OK {
	  units = recGetUnits(xd.pointer, isX);
	  MdsFree1Dx(&xd, 0);
	}
      }
      return NULL;
    case CLASS_A:
      return NULL;
    case CLASS_XD:
      if(!dsc->pointer) return NULL;
      return recGetUnits((mdsdsc_t*)dsc->pointer, isX);
    case CLASS_R:
      if(dsc->dtype == DTYPE_WITH_UNITS) {
	units = NULL;
	uDsc = (mds_with_units_t*)dsc;
	if(uDsc->units) {
	  status = TdiData(uDsc->units, &xd MDS_END_ARG);
	  if(STATUS_OK && xd.pointer && xd.pointer->class == CLASS_S && xd.pointer->dtype == DTYPE_T) {
	    units = malloc(xd.pointer->length + 1);
	    memcpy(units, xd.pointer->pointer, xd.pointer->length);
	    units[xd.pointer->length] = 0;
	  }
	  MdsFree1Dx(&xd, 0);
	}
	return units;
      }
      if(dsc->dtype == DTYPE_SIGNAL) {
	sDsc = (mds_signal_t *)dsc;
	if(isX)
	  return (sDsc->ndesc > 2) ? recGetUnits(sDsc->dimensions[0], isX) : NULL;
	if(sDsc->data)
	  return recGetUnits(sDsc->data, isX);
	return recGetUnits(sDsc->raw, isX);
      }
      if(dsc->dtype == DTYPE_PARAM) {
	pDsc = (mds_param_t *)dsc;
	return recGetUnits(pDsc->value, isX);
      }
      rDsc = (mdsdsc_r_t *)dsc;
      if(rDsc->ndesc == 1)
	return recGetUnits(rDsc->dscptrs[0], isX);
      if(rDsc->dtype == DTYPE_FUNCTION && rDsc->pointer
       && (*(opcode_t*)rDsc->pointer == OPC_ADD || *(opcode_t*)rDsc->pointer == OPC_SUBTRACT)) {
	units = recGetUnits(rDsc->dscptrs[0], isX);
	if(!units) return NULL;
        for(i = 1; i < rDsc->ndesc; i++) {
	  currUnits = recGetUnits(rDsc->dscptrs[i], isX);
	  if(!currUnits || strcmp(units, currUnits)) {//Different units
	    free(units);
	    free(currUnits);
	    return NULL;
	  }
	  free(currUnits);
	}
	return units;
      }
      if(rDsc->dtype == DTYPE_FUNCTION && rDsc->pointer
       && (*(opcode_t*)rDsc->pointer == OPC_MULTIPLY || *(opcode_t*)rDsc->pointer == OPC_DIVIDE)) {
	mulDiv = (*(opcode_t*)rDsc->pointer == OPC_MULTIPLY)?"*":"/";
	unitsArr = (char **)malloc(sizeof(char *) * rDsc->ndesc);
	unitsLen = rDsc->ndesc;
	for(i = 0; i < rDsc->ndesc; i++) {
	  unitsArr[i] = recGetUnits(rDsc->dscptrs[i], isX);
	  if(unitsArr[i])
	    unitsLen += strlen(unitsArr[i]);
	}
	units = malloc(unitsLen+1);
	units[0] = 0;
	for(i = 0; i < rDsc->ndesc; i++) {
	  if(unitsArr[i]) {
	    if(strlen(units) > 0)
	      sprintf(&units[strlen(units)], "%s%s", mulDiv, unitsArr[i]);
	    else
	      sprintf(&units[strlen(units)], "%s", unitsArr[i]);
	    free(unitsArr[i]);
	  }
	}
	free(unitsArr);
	return units;
      }
      return NULL;
    default: return NULL;
  }
}

static mdsdsc_xd_t *encodeError(char *error, int line, mdsdsc_xd_t *out_xd) {
/* converts message to packed error message (float res,int len,char msg[len])
 */
  typedef struct __attribute__((__packed__)) {
    int   line;
    int   len;
    char  msg[0];
  } jerr_t;
  const size_t err_ln = strlen(error);
  const size_t err_sz = err_ln + sizeof(jerr_t);
  jerr_t *jerr = malloc(err_sz);{
    jerr->line= line;
    jerr->len = err_ln;
    memcpy(jerr->msg, error, err_ln);
  }
  mdsdsc_t err_d = {err_sz, DTYPE_T, CLASS_S, (char*)jerr};
  MdsCopyDxXd(&err_d, out_xd);
  free(jerr);
  return out_xd;
}

//Check if the passed expression contains at least one segmented node
EXPORT int IsSegmented(char *expr) {
  EMPTYXD(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  if IS_NOT_OK(TdiCompile(&exprD, &xd MDS_END_ARG)) return FALSE;
  int segNid = recIsSegmented(xd.pointer);
  MdsFree1Dx(&xd, NULL);
  return segNid != -1;
}

EXPORT int TestGetHelp(char *expr) {
  EMPTYXD(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  if IS_NOT_OK(TdiCompile(&exprD, &xd MDS_END_ARG)) return FALSE;
  MdsFree1Dx(&xd, 0);
  return TRUE;
}

EXPORT int TestGetUnits(char *expr) {
  EMPTYXD(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  if IS_NOT_OK(TdiCompile(&exprD, &xd MDS_END_ARG)) return FALSE;
  MdsFree1Dx(&xd, 0);
  return TRUE;
}

static int pack_meta(char*title,char*xLabel,char*yLabel,float res,char*retArr,int idx,char swap) {
//idx is the current index in retArr
//Write title, xLabel, yLabel as length followed by chars
  if(title) {
    *(int *)&retArr[idx] = strlen(title);
    if(swap) swap4(&retArr[idx]);
    idx += sizeof(int);
    memcpy(&retArr[idx], title, strlen(title));
    idx += strlen(title);
  } else {
    *(int *)&retArr[idx] = 0; //no swap required
    idx += 4;
  }
  if(xLabel) {
    *(int *)&retArr[idx] = strlen(xLabel);
    if(swap) swap4(&retArr[idx]);
    idx += sizeof(int);
    memcpy(&retArr[idx], xLabel, strlen(xLabel));
    idx += strlen(xLabel);
  } else {
    *(int *)&retArr[idx] = 0; //no swap required
    idx += 4;
  }
  if(yLabel) {
    *(int *)&retArr[idx] = strlen(yLabel);
    if(swap) swap4(&retArr[idx]);
    idx += sizeof(int);
    memcpy(&retArr[idx], yLabel, strlen(yLabel));
    idx += strlen(yLabel);
  } else {
    *(int *)&retArr[idx] = 0; //no swap required
    idx += 4;
  }
  *((float *)retArr) = res; //resolution
  if(swap)swap4(retArr);
  return idx;
}

static float* getFloatArray(mdsdsc_a_t *yArrD, int nSamples) {
  int i;
  float *y;
  switch(yArrD->dtype) {
    case DTYPE_B:
    case DTYPE_BU:
      y = (float *)malloc(nSamples * sizeof(float));
      for(i = 0; i < nSamples; i++)
	y[i] = *((char *)(&yArrD->pointer[i*yArrD->length]));
      return y;
    case DTYPE_W:
    case DTYPE_WU:
      y = (float *)malloc(nSamples * sizeof(float));
      for(i = 0; i < nSamples; i++)
	y[i] = *((short *)(&yArrD->pointer[i*yArrD->length]));
      return y;
    case DTYPE_L:
    case DTYPE_LU:
      y = (float *)malloc(nSamples * sizeof(float));
      for(i = 0; i < nSamples; i++)
	y[i] = *((int *)(&yArrD->pointer[i*yArrD->length]));
      return y;
    case DTYPE_Q:
    case DTYPE_QU:
      y = (float *)malloc(nSamples * sizeof(float));
      for(i = 0; i < nSamples; i++)
	y[i] = *((int64_t *)(&yArrD->pointer[i*yArrD->length]));
      return y;
    case DTYPE_DOUBLE:
      y = (float *)malloc(nSamples * sizeof(float));
      for(i = 0; i < nSamples; i++)
	y[i] = *((double *)(&yArrD->pointer[i*yArrD->length]));
      return y;
    case DTYPE_FLOAT:
      return (float *)yArrD->pointer;
    default:
      return NULL;
  }
}


EXPORT mdsdsc_xd_t* GetXYSignalXd(mdsdsc_xd_t* inY, mdsdsc_xd_t* inX, double *inXMin, double *inXMax, int reqNSamples, mdsdsc_xd_t* retXd) {
  if (!inY || !inY->pointer) return(encodeError("Y data must not be NULL",__LINE__,retXd));
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  //struct  descriptor dimExpr = {strlen("DIM_OF($)"), DTYPE_T, CLASS_S, (char *)"DIM_OF($)"};
  char *err;
  char swap;

  char testEndian[4] = {0, 0, 0, 1};
  int xSampleSize;
  int retSize;
  int idx, i, status;
  char *retArr;
  float retResolution;
  DESCRIPTOR_A(retArrD, 1, DTYPE_B, 0, 0);

  //printf("GetXYSignal(%s, %s, %f, %f, %d)\n", inY, inX, *inXMin, *inXMax, *reqNSamples);

  int estimatedSegmentSamples = 0;
  double estimatedDuration = 0;
  int64_t estimatedSamples = estimateNumSamples(inY->pointer, inXMin, inXMax, &estimatedSegmentSamples, &estimatedDuration);
  //printf("Estimated Samples: %d\n", estimatedSamples);
  //printf("Estimated SegmentSamples: %d\n", estimatedSegmentSamples);
  //printf("Estimated Durations: %f\n", estimatedDuration);
  double delta;
  mdsdsc_t deltaD = {sizeof(double), DTYPE_DOUBLE, CLASS_S, (char *)&delta};
  mdsdsc_t xMinD  = {sizeof(double), DTYPE_DOUBLE, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD  = {sizeof(double), DTYPE_DOUBLE, CLASS_S, (char *)inXMax};
  mdsdsc_t *xMinP, *xMaxP, *deltaP;
  if (estimatedSamples > NUM_SAMPLES_THRESHOLD) {
    //First guess on delta  G
    delta = 100 * estimatedSamples/NUM_SAMPLES_THRESHOLD;
    //Now delta represents the number of samples to be compressed in a min-max mair
    if (delta > estimatedSegmentSamples/10.)
      delta = estimatedSegmentSamples/10.;
    //In any case don't make it too big in respect of the single segment size (at minimum 10 samples )pairs) per segment
    delta *= (estimatedDuration/estimatedSamples);
    deltaP = &deltaD;
  } else deltaP = NULL;
  //Set limits if any
  xMinP = inXMin ? &xMinD : NULL;
  xMaxP = inXMax ? &xMaxD : NULL;
  status = TreeSetTimeContext(xMinP,xMaxP,deltaP);
  char *title, *xLabel, *yLabel;
  status = TdiEvaluate(inY->pointer, &yXd MDS_END_ARG);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,retXd));
  // Get Y, title, and yLabel, if any
  title  = recGetHelp(yXd.pointer);
  yLabel = recGetUnits(yXd.pointer, 0);
  //Get X
  if (!inX || !inX->pointer) //If an explicit expression for X has been given
    status = TdiDimOf(yXd.pointer, &xXd MDS_END_ARG);
  else //Get xLabel, if any
    status = TdiEvaluate(inX->pointer, &xXd MDS_END_ARG);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,retXd));
  xLabel = recGetUnits(xXd.pointer, 1);
  status = TdiData((mdsdsc_t *)&xXd, &xXd MDS_END_ARG);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,retXd));
  status = TdiData((mdsdsc_t *)&yXd, &yXd MDS_END_ARG);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,retXd));
  //Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
  // Y converted to float, X to int64_t or float or double
  if (yXd.pointer->class != CLASS_A)
    err = "Y is not an array";
  else if (xXd.pointer->class != CLASS_A)
    err = "X is not an array";
  else err = 0;
  if (err) {
    MdsFree1Dx(&xXd, 0);
    MdsFree1Dx(&yXd, 0);
    return(encodeError(err,__LINE__,retXd));
  }
  //Number of asmples set to minimum between X and Y
  mdsdsc_a_t *xArrD = (mdsdsc_a_t *)xXd.pointer;
  mdsdsc_a_t *yArrD = (mdsdsc_a_t *)yXd.pointer;
  int nSamples = yArrD->arsize/yArrD->length;
  if (nSamples > (int)(xArrD->arsize/xArrD->length))
    nSamples = xArrD->arsize/xArrD->length;
  //Handle old DTYPE_F format
  if(yArrD->dtype == DTYPE_F) {
    EMPTYXD(currXd);
    if STATUS_OK status = TdiFloat(currXd.pointer,   &currXd MDS_END_ARG);
    if STATUS_NOT_OK {
      MdsFree1Dx(&xXd, 0);
      MdsFree1Dx(&yXd, 0);
      return(encodeError("Cannot Convert Y to IEEE Floating point",__LINE__,retXd));
    }
    MdsFree1Dx(&yXd, 0);
    yXd = currXd;
    yArrD = (mdsdsc_a_t *)yXd.pointer;
  }
  if(xArrD->dtype == DTYPE_F) {
    EMPTYXD(currXd);
    if STATUS_OK status = TdiFloat(currXd.pointer,   &currXd MDS_END_ARG);
    if STATUS_NOT_OK {
      MdsFree1Dx(&xXd, 0);
      MdsFree1Dx(&yXd, 0);
      return(encodeError("Cannot Convert X to IEEE Floating point",__LINE__,retXd));
    }
    MdsFree1Dx(&xXd, 0);
    xXd = currXd;
    xArrD = (mdsdsc_a_t *)xXd.pointer;
  }

  float *y = getFloatArray(yArrD,nSamples);
  int retSamples;
  trimData(y, xArrD->pointer, xArrD->length, xArrD->dtype, nSamples, reqNSamples, inXMin ? *inXMin : -INFINITY, inXMax ? *inXMax : INFINITY, &retSamples, &retResolution);
/*Assemble result. Format:
-retResolution(float)
-number of samples (minumum between X and Y)
-type of X xamples (byte: int64_t(1), double(2) or float(3))
-y samples (float - big endian)
-x Samples
*/
  swap = *((int *)testEndian) != 1;
  if(xArrD->dtype == DTYPE_Q || xArrD->dtype == DTYPE_QU || xArrD->dtype == DTYPE_DOUBLE)
    xSampleSize = sizeof(int64_t);
  else
    xSampleSize = sizeof(int);
  retSize = sizeof(float) + sizeof(int) + 1 + retSamples * (sizeof(float) + xSampleSize);
  //Add rool for title and labels
  retSize += 3 * sizeof(int);
  if(title)  retSize += strlen(title);
  if(xLabel) retSize += strlen(xLabel);
  if(yLabel) retSize += strlen(yLabel);
  retArr = malloc(retSize);
  *((int *)&retArr[4]) = retSamples;
  if(swap)swap4(&retArr[4]);
  if(xArrD->dtype == DTYPE_Q || xArrD->dtype == DTYPE_QU)
    retArr[8] = 1;
  else if(xArrD->dtype == DTYPE_DOUBLE)
    retArr[8] = 2;
  else
    retArr[8] = 3;
  idx = 9;
  for(i = 0; i < retSamples; i++) {
    *((float *)&retArr[idx]) = y[i];
    if(swap)swap4(&retArr[idx]);
    idx += sizeof(float);
  }
//  double maxX, minX, currX;
  for(i = 0; i < retSamples; i++) {
    switch(xArrD->dtype) {
      default:
	err = "Cannot Convert X data dtype";
	free(retArr);
	MdsFree1Dx(&xXd, 0);
	MdsFree1Dx(&yXd, 0);
	return(encodeError(err,__LINE__,retXd));
      case DTYPE_B:
      case DTYPE_BU:
	*((float *)&retArr[idx]) = *((char *)(&xArrD->pointer[i*xArrD->length]));
	if(swap)swap4(&retArr[idx]);
	idx += sizeof(float);
//	currX = *((char *)(&xArrD->pointer[i*xArrD->length]));
	break;
      case DTYPE_W:
      case DTYPE_WU:
	*((float *)&retArr[idx]) = *((short *)(&xArrD->pointer[i*xArrD->length]));
	if(swap)swap4(&retArr[idx]);
	idx += sizeof(float);
//	currX = *((short *)(&xArrD->pointer[i*xArrD->length]));
	break;
      case DTYPE_L:
      case DTYPE_LU:
	*((float *)&retArr[idx]) = *((int *)(&xArrD->pointer[i*xArrD->length]));
	if(swap)swap4(&retArr[idx]);
	idx += sizeof(float);
//	currX = *((int *)(&xArrD->pointer[i*xArrD->length]));
	break;
      case DTYPE_Q:
      case DTYPE_QU:
	*((int64_t *)&retArr[idx]) = *((int64_t *)(&xArrD->pointer[i*xArrD->length]));
	if(swap)swap8(&retArr[idx]);
	idx += sizeof(int64_t);
//	currX = *((int64_t *)(&xArrD->pointer[i*xArrD->length]));
	break;
      case DTYPE_FLOAT:
	*((float *)&retArr[idx]) = *((float *)(&xArrD->pointer[i*xArrD->length]));
	if(swap)swap4(&retArr[idx]);
	idx += sizeof(float);
//	currX = *((float *)(&xArrD->pointer[i*xArrD->length]));
	break;
      case DTYPE_DOUBLE:
	*((double *)&retArr[idx]) = *((double *)(&xArrD->pointer[i*xArrD->length]));
	if(swap)swap8(&retArr[idx]);
	idx += sizeof(double);
//	currX = *((double *)(&xArrD->pointer[i*xArrD->length]));
	break;
    }
//    if(i == 0)
//      minX = maxX = currX;
//    else {
//      if(currX < minX) minX = currX;
//      if(currX > maxX) maxX = currX;
//    }
  }

  idx = pack_meta(title,xLabel,yLabel,retResolution,retArr,idx,swap);

  retArrD.arsize = retSize;
  retArrD.pointer = retArr;
  MdsCopyDxXd((mdsdsc_t *)&retArrD, retXd);
  if ((void*)yArrD->pointer != (void*)y) free(y);
  MdsFree1Dx(&xXd, 0);
  MdsFree1Dx(&yXd, 0);
  free(retArr);
  return retXd;
}
EXPORT mdsdsc_xd_t* GetXYSignal(char *inY, char *inX, float *inXMin, float *inXMax, int *reqNSamples) {
  static EMPTYXD(retXd);
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  double xMin, *pXMin, xMax, *pXMax;
  if (inXMin && *inXMin < -MAX_LIMIT) {
    xMin = (double)*inXMin;
    pXMin= &xMin;
  } else
    pXMin= NULL;
  if (inXMax && *inXMin >  MAX_LIMIT) {
    xMax = (double)*inXMax;
    pXMax= &xMax;
  } else
    pXMax= NULL;
  int status;{
    mdsdsc_t expY = {strlen(inY), DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &yXd MDS_END_ARG);
  }
  if (STATUS_OK && inX){
    mdsdsc_t expX = {strlen(inX), DTYPE_T, CLASS_S, inX};
    status = TdiCompile(&expX, &xXd MDS_END_ARG);
  }
  if STATUS_OK GetXYSignalXd(&yXd,&xXd,pXMin,pXMax,*reqNSamples,&retXd);
  MdsFree1Dx(&yXd, NULL);
  MdsFree1Dx(&xXd, NULL);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,&retXd));
  return &retXd;
}

EXPORT mdsdsc_xd_t* GetXYSignalLongTimes(char *inY, char *inX, int64_t *inXMin, int64_t *inXMax, int *reqNSamples) {
  static EMPTYXD(retXd);
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  double xMin, *pXMin, xMax, *pXMax;
  if (inXMin && *inXMin) {
    xMin = ((double)*inXMin)/1e9;
    pXMin= &xMin;
  } else
    pXMin= NULL;
  if (inXMax && *inXMax) {
    xMax = ((double)*inXMax)/1e9;
    pXMax= &xMax;
  } else
    pXMax= NULL;
  int status;{
    mdsdsc_t expY = {strlen(inY), DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &yXd MDS_END_ARG);
  }
  if (STATUS_OK && inX){
    mdsdsc_t expX = {strlen(inX), DTYPE_T, CLASS_S, inX};
    status = TdiCompile(&expX, &xXd MDS_END_ARG);
  }
  if STATUS_OK GetXYSignalXd(&yXd,&xXd,pXMin,pXMax,*reqNSamples,&retXd);
  MdsFree1Dx(&yXd, NULL);
  MdsFree1Dx(&xXd, NULL);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,&retXd));
  return &retXd;
}

EXPORT mdsdsc_xd_t*GetXYWave(char *sigName, float *inXMin, float *inXMax, int *reqNSamples) {
  static EMPTYXD(retXd);
  EMPTYXD(xd);
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  mdsdsc_t yExpr = {strlen(sigName), DTYPE_T, CLASS_S, sigName};
  mdsdsc_t xExpr = {strlen(sigName) + strlen("DIM_OF()"), DTYPE_T, CLASS_S, 0};
  mdsdsc_t errD = {0, DTYPE_T, CLASS_S, 0};
  mdsdsc_t xMinD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
  char *err;
  int retSamples;
  int status;
  float retResolution;
  DESCRIPTOR_A(retDataD, sizeof(float), DTYPE_FLOAT, 0, 0);
  DESCRIPTOR_A(retDimD, 0, 0, 0, 0);
  DESCRIPTOR_SIGNAL_1(retSignalD, &retDataD, 0, &retDimD);
  float xMin = *inXMin;
  float xMax = *inXMax;

  //Set limits if any
  if(xMin <= -MAX_LIMIT && xMax >= MAX_LIMIT)
    status = TreeSetTimeContext(NULL, NULL, NULL);
  else if(xMin <= -MAX_LIMIT)
    status = TreeSetTimeContext(NULL, &xMaxD, NULL);
  else if(xMax >= MAX_LIMIT)
    status = TreeSetTimeContext(&xMinD, NULL, NULL);
  else
    status = TreeSetTimeContext(&xMinD, &xMaxD, NULL);

  //Set correct name for xExpr
  xExpr.pointer = malloc(strlen(sigName) + strlen("DIM_OF()") + 1);
  sprintf(xExpr.pointer, "DIM_OF(%s)", sigName);

  //Get Y
  status = TdiCompile(&yExpr, &xd MDS_END_ARG);
  if STATUS_OK status = TdiData((mdsdsc_t *)&xd, &yXd MDS_END_ARG);
  MdsFree1Dx(&xd, 0);
  if STATUS_NOT_OK {
    err = MdsGetMsg(status);
    errD.length = strlen(err);
    errD.pointer = err;
    MdsCopyDxXd(&errD, &retXd);
    return &retXd;
  }
  //Get X
  status = TdiCompile(&xExpr, &xd MDS_END_ARG);
  free(xExpr.pointer);
  if STATUS_OK status = TdiData((mdsdsc_t *)&xd, &xXd MDS_END_ARG);
  MdsFree1Dx(&xd, 0);
  if STATUS_NOT_OK {
    err = MdsGetMsg(status);
    errD.length = strlen(err);
    errD.pointer = err;
    MdsCopyDxXd(&errD, &retXd);
    return &retXd;
  }
  // Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
  // Y converted to float, X to int64_t or float or double
  if(yXd.pointer->class != CLASS_A)
    err = "Y is not an array";
  else if(xXd.pointer->class != CLASS_A)
    err = "Y is not an array";
  else err = NULL;
  if(err) {
    MdsFree1Dx(&xXd, 0);
    MdsFree1Dx(&yXd, 0);
    errD.length = strlen(err);
    errD.pointer = err;
    MdsCopyDxXd(&errD, &retXd);
    return &retXd;
  }
  // Number of samples set to minimum between X and Y
  mdsdsc_a_t *xArrD = (mdsdsc_a_t *)xXd.pointer;
  mdsdsc_a_t *yArrD = (mdsdsc_a_t *)yXd.pointer;
  int nSamples = yArrD->arsize/yArrD->length;
  if(nSamples > (int)(xArrD->arsize/xArrD->length))
    nSamples = xArrD->arsize/xArrD->length;

  float *y = getFloatArray(xArrD,nSamples);
  trimData(y, xArrD->pointer, xArrD->length, xArrD->dtype, nSamples, *reqNSamples, *inXMin, *inXMax, &retSamples, &retResolution);

  retDataD.pointer = (char *)y;
  retDataD.arsize = sizeof(float ) * retSamples;
  retDimD.length = xArrD->length;
  retDimD.dtype = xArrD->dtype;
  retDimD.pointer = xArrD->pointer;
  retDimD.arsize = retSamples * xArrD->length;
  MdsCopyDxXd((mdsdsc_t *)&retSignalD, &retXd);
  if ((float*)yArrD->pointer != y) free(y);
  MdsFree1Dx(&xXd, 0);
  MdsFree1Dx(&yXd, 0);
  return &retXd;
}

EXPORT mdsdsc_xd_t*GetXYWaveLongTimes(char *sigName, int64_t *inXMin, int64_t *inXMax, int *reqNSamples) {
  double xMin, *pXMin, xMax, *pXMax;
  if (inXMin && *inXMin) {
    xMin = ((double)*inXMin)/1e9;
    pXMin= &xMin;
  } else
    pXMin= NULL;
  if (inXMax && *inXMax) {
    xMax = ((double)*inXMax)/1e9;
    pXMax= &xMax;
  } else
    pXMax= NULL;
  return GetXYWave(sigName,pXMin,pXMax,reqNSamples);
}
