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

#ifdef WORD_BIGENDIAN
#define swap2(...)
#define swap4(...)
#define swap8(...)
#else
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
#endif
static double to_doublex(const void *ptr, const dtype_t dtype, const double defval, const int is_time) {
  switch(dtype) {
    case DTYPE_FLOAT:
      return (double)*( float  *)ptr;
    case DTYPE_DOUBLE:
      return         *( double *)ptr;
    case DTYPE_B:
    case DTYPE_BU:
      return (double)*( int8_t *)ptr;
    case DTYPE_W:
    case DTYPE_WU:
      return (double)*(int16_t *)ptr;
    case DTYPE_L:
    case DTYPE_LU:
      return (double)*(int32_t *)ptr;
    case DTYPE_Q:
    case DTYPE_QU:
      if (is_time) return ((double)*(int64_t *)ptr)/1e9;
      return (double)*(int64_t *)ptr;
    default:
      printf("Unsupported Type in getData\n");
      return defval;
  }
}
inline static double to_double(const void *ptr, const dtype_t dtype) {
  return to_doublex(ptr,dtype,0,FALSE);
}
static int recIsSegmented(mdsdsc_t *dsc) {
/* returns nid of the first segmented node found
 * or 0 if none found
 */
  if(!dsc) return 0;
  int nid, numSegments, status, i;
  char *path;
  int retClassLen, retDtypeLen;
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
	unsigned int nciClass = 0, nciDtype = 0;
	struct nci_itm nciList[] = {
	  {1, NciCLASS, &nciClass, &retClassLen},
	  {1, NciDTYPE, &nciDtype, &retDtypeLen},
	  {NciEND_OF_LIST, 0, 0, 0}
	};
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
      break;
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

#define MAX64 0x7FFFFFFFFFFFFFFFL
inline static int64_t estimateNumSamples(mdsdsc_t *dsc, mdsdsc_t *xMin, mdsdsc_t *xMax, int *estimatedSegmentSamples, double *dMin, double *dMax) {
/* return the number of samples the signal holds based on meta information
   or -1 if something went wrong
 */
  int numSegments, startIdx, endIdx;
  int64_t startTime = -MAX64-1, endTime = MAX64, currStartTime, currEndTime;
  char dtype, dimct;
  int dims[64];
  int nextRow, segmentSamples, numActSegments, segmentIdx;
  EMPTYXD(xd);
  int nid = recIsSegmented(dsc);
  if(!nid) return -1;
  int status = TreeGetNumSegments(nid, &numSegments);
  if STATUS_NOT_OK return -1;
  if(xMin != NULL || xMax != NULL) {
    if(xMin) XTreeConvertToLongTime(xMin, &startTime);
    if(xMax) XTreeConvertToLongTime(xMax,   &endTime);
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
  if STATUS_OK status = TdiData((mdsdsc_t*)&xd, &xd MDS_END_ARG);
  if STATUS_NOT_OK goto return_neg1;
  const double xmin = to_doublex(xd.pointer->pointer,xd.pointer->dtype,-INFINITY,TRUE);
  status = TreeGetSegmentLimits(nid, endIdx, NULL, &xd);
  if STATUS_OK status = TdiData((mdsdsc_t*)&xd, &xd MDS_END_ARG);
  if STATUS_NOT_OK goto return_neg1;
  const double xmax = to_doublex(xd.pointer->pointer,xd.pointer->dtype, INFINITY,TRUE);
  MdsFree1Dx(&xd, NULL);
  *dMin = xmin;
  *dMax = xmax;
  *estimatedSegmentSamples = segmentSamples;
  return segmentSamples * (int64_t)numActSegments;
return_neg1: ;
  MdsFree1Dx(&xd, NULL);
  return -1;
}

//Perform trim on site (do not realloc arrays) return the actual number of points
//the type of X array is unknown, element size is passed in xSize
static void trimData(float *y, mdsdsc_a_t *x, int nSamples, int reqPoints, double xMin, double xMax, int *retPoints, float *retResolution) {
  if(nSamples < 10 * reqPoints) {
    //Does not perform any compression
    *retResolution = 1E12;
    *retPoints = nSamples;
    return;
  }
  //From here, consider xMin and xMax
  int startIdx, endIdx;
  for(startIdx = 0;        startIdx < nSamples && to_double(&x->pointer[startIdx * x->length], x->dtype) < xMin; startIdx++);
  if(startIdx == nSamples) startIdx--;
  for(  endIdx = startIdx;   endIdx < nSamples && to_double(&x->pointer[  endIdx * x->length], x->dtype) < xMax;   endIdx++);
  if(  endIdx == nSamples)   endIdx--;
  const int deltaIdx = ((endIdx - startIdx) < 10 * reqPoints) ? 1 : (endIdx - startIdx + 1) / reqPoints;

  int curIdx = startIdx;
  int outIdx = 0;
  if(deltaIdx == 1) {
    *retResolution = 1E12;
    if (outIdx < curIdx) { // check if not src == dst
      while (curIdx < endIdx) {
	memcpy(&x->pointer[outIdx * x->length], &x->pointer[curIdx * x->length], x->length);
	y[outIdx++] = y[curIdx++];
      }
    }
  } else {
    *retResolution = reqPoints/(to_doublex(&x->pointer[endIdx * x->length], x->dtype,INFINITY,TRUE) - to_doublex(&x->pointer[startIdx * x->length], x->dtype,-INFINITY,TRUE));
    const double deltaTime = (deltaIdx == 1) ? 0 : (xMax - xMin)/(double)reqPoints;
    float minY, maxY;
    char *xtmp = malloc(x->length);
    while(curIdx < endIdx) {
      minY = maxY = y[curIdx];
      int i,actSamples = 0;
      const double endXDouble = to_double(&x->pointer[curIdx * x->length], x->dtype) + deltaTime;
      for(i = curIdx; i < nSamples && i < curIdx + deltaIdx; i++, actSamples++) {
	if(to_double(&x->pointer[i * x->length], x->dtype) > endXDouble) break; //Handle dual speed clocks
	if(y[i] < minY) minY = y[i];
	if(y[i] > maxY) maxY = y[i];
      }
      curIdx += (actSamples+1)/2; // half step ensures outIdx != curIdx
      memcpy(&x->pointer[outIdx * x->length], &x->pointer[curIdx * x->length], x->length);
      y[outIdx++] = minY;
      if (outIdx<curIdx)
        memcpy(&x->pointer[outIdx * x->length], &x->pointer[curIdx * x->length], x->length);
      y[outIdx++] = maxY;
      curIdx +=  actSamples/2;
    }
    free(xtmp);
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
  return segNid != 0;
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

static inline int pack_meta(mdsdsc_t*title,mdsdsc_t*xLabel,mdsdsc_t*yLabel,float res,char*retArr,int idx) {
//idx is the current index in retArr
//Write title, xLabel, yLabel as length followed by chars
  if(title) {
    *(int *)&retArr[idx] = title->length;
    swap4(&retArr[idx]);
    idx += sizeof(int);
    memcpy(&retArr[idx], title->pointer, title->length);
    idx += title->length;
  } else {
    *(int *)&retArr[idx] = 0; //no swap required
    idx += 4;
  }
  if(xLabel) {
    *(int *)&retArr[idx] = xLabel->length;
    swap4(&retArr[idx]);
    idx += sizeof(int);
    memcpy(&retArr[idx], xLabel->pointer, xLabel->length);
    idx += xLabel->length;
  } else {
    *(int *)&retArr[idx] = 0; //no swap required
    idx += 4;
  }
  if(yLabel) {
    *(int *)&retArr[idx] = yLabel->length;
    swap4(&retArr[idx]);
    idx += sizeof(int);
    memcpy(&retArr[idx], yLabel->pointer, yLabel->length);
    idx += yLabel->length;
  } else {
    *(int *)&retArr[idx] = 0; //no swap required
    idx += 4;
  }
  *((float *)retArr) = res; //resolution
  swap4(retArr);
  return idx;
}

static int getNSamples(mdsdsc_xd_t*yXd,mdsdsc_xd_t*xXd,char**err) {
  // Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
  // Y converted to float, X to int64_t or float or double
  if(yXd->pointer->class != CLASS_A) {
    *err = "Y is not an array";
    return __LINE__;
  }
  if(xXd->pointer->class != CLASS_A) {
    *err = "Y is not an array";
    return __LINE__;
  }
  //Handle old DTYPE_F format
  if(yXd->pointer->dtype == DTYPE_F) {
    if IS_NOT_OK(TdiFloat(yXd->pointer, &yXd MDS_END_ARG)) {
      *err = "Cannot Convert Y to IEEE Floating point";
      return __LINE__;
    }
  }
  if(xXd->pointer->dtype == DTYPE_F) {
    if IS_NOT_OK(TdiFloat(xXd->pointer, &xXd MDS_END_ARG)) {
      *err = "Cannot Convert X to IEEE Floating point";
      return __LINE__;
    }
  }
  //Number of samples set to minimum between X and Y
  const int ySamples = ((mdsdsc_a_t*)yXd->pointer)->arsize/yXd->pointer->length;
  const int xSamples = ((mdsdsc_a_t*)xXd->pointer)->arsize/xXd->pointer->length;
  return xSamples<ySamples ? xSamples : ySamples;
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

static inline int getXArray(mdsdsc_a_t *xArrD, int retSamples, char *retArr, int idx) {
  int i;
  switch(xArrD->dtype) {
    default:
      return -1;
    case DTYPE_B:
    case DTYPE_BU:
      for(i = 0; i < retSamples; i++, idx += 4) {
	*((float  *)&retArr[idx]) = *(( int8_t*)(&xArrD->pointer[i*xArrD->length]));
	swap4(&retArr[idx]);
      }
      break;
    case DTYPE_W:
    case DTYPE_WU:
      for(i = 0; i < retSamples; i++, idx += 4) {
	*((float  *)&retArr[idx]) = *((int16_t*)(&xArrD->pointer[i*xArrD->length]));
	swap4(&retArr[idx]);
      }
      break;
    case DTYPE_L:
    case DTYPE_LU:
      for(i = 0; i < retSamples; i++, idx += 4) {
	*((float  *)&retArr[idx]) = *((int32_t*)(&xArrD->pointer[i*xArrD->length]));
	swap4(&retArr[idx]);
      }
      break;
    case DTYPE_Q:
    case DTYPE_QU:
      for(i = 0; i < retSamples; i++, idx += 8) {
	*((int64_t*)&retArr[idx]) = *((int64_t*)(&xArrD->pointer[i*xArrD->length]));
	swap8(&retArr[idx]);
      }
      break;
    case DTYPE_FLOAT:
      for(i = 0; i < retSamples; i++, idx += 4) {
	*((float  *)&retArr[idx]) = *((float  *)(&xArrD->pointer[i*xArrD->length]));
	swap4(&retArr[idx]);
      }
      break;
    case DTYPE_DOUBLE:
      for(i = 0; i < retSamples; i++, idx += 8) {
	*((double *)&retArr[idx]) = *((double *)(&xArrD->pointer[i*xArrD->length]));
	swap8(&retArr[idx]);
	break;
      }
  }
  return idx;
}

EXPORT int GetXYSignalXd(mdsdsc_t *inY, mdsdsc_t *inX, mdsdsc_t *inXMin, mdsdsc_t *inXMax, int reqNSamples, mdsdsc_xd_t* retXd) {
  if (!inY) return TdiNULL_PTR;
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  int estimatedSegmentSamples = 0;
  double xmin = -INFINITY, xmax = INFINITY, delta;
  mdsdsc_t *xMinP,*xMaxP,*deltaP, deltaD = {0, 0, CLASS_S, 0};
  int64_t estimatedSamples = estimateNumSamples(inY, inXMin, inXMax, &estimatedSegmentSamples, &xmin, &xmax);
  const double estimatedDuration = xmax - xmin;
  xMinP = (xmin > -INFINITY) ? inXMin : NULL;
  xMaxP = (xmax <  INFINITY) ? inXMax : NULL;
  if (estimatedSamples > NUM_SAMPLES_THRESHOLD) {
    //First guess on delta
    delta = 100 * estimatedSamples/NUM_SAMPLES_THRESHOLD;
    //Now delta represents the number of samples to be compressed in a min-max mair
    if (delta > estimatedSegmentSamples/10.)
      delta = estimatedSegmentSamples/10.;
    //In any case don't make it too big in respect of the single segment size (at minimum 10 samples )pairs) per segment
    delta *= (estimatedDuration/estimatedSamples);
    deltaP = (delta>1e-9) ? &deltaD : NULL;
  } else deltaP = NULL;
  //Set limits if any
  int status = TreeSetTimeContext(xMinP,xMaxP,deltaP);
  char *title, *xLabel, *yLabel;
  status = TdiEvaluate(inY, &yXd MDS_END_ARG);
  if STATUS_NOT_OK goto return_err;
  // Get Y, title, and yLabel, if any
  title  = recGetHelp(yXd.pointer);
  yLabel = recGetUnits(yXd.pointer, 0);
  //Get X
  if (!inX) //If an explicit expression for X has been given
    status = TdiDimOf(yXd.pointer, &xXd MDS_END_ARG);
  else //Get xLabel, if any
    status = TdiEvaluate(inX, &xXd MDS_END_ARG);
  if STATUS_NOT_OK goto return_err;
  xLabel = recGetUnits(xXd.pointer, 1);
  if STATUS_OK status = TdiData((mdsdsc_t *)&xXd, &xXd MDS_END_ARG);
  status = TdiData((mdsdsc_t *)&yXd, &yXd MDS_END_ARG);
  if STATUS_NOT_OK goto return_err;
  //Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
  // Y converted to float, X to int64_t or float or double
  char *err = NULL;
  int nSamples = getNSamples(&yXd,&xXd,&err);
  if (err) {
    encodeError(err,nSamples,retXd);
    status = MDSplusERROR;
    goto return_err;
  }
  mdsdsc_a_t *xArrD = (mdsdsc_a_t *)xXd.pointer;
  mdsdsc_a_t *yArrD = (mdsdsc_a_t *)yXd.pointer;
  float *y = getFloatArray(yArrD,nSamples);
  int retSamples;
  float retResolution;
  trimData(y, xArrD, nSamples, reqNSamples, xmin, xmax, &retSamples, &retResolution);

  DESCRIPTOR_A(yData, sizeof(float), DTYPE_FLOAT, (char *)y,     sizeof(float ) * retSamples);
  mdsdsc_t yUnits = {yLabel ? strlen(yLabel) : 0,DTYPE_T,CLASS_S,yLabel};
  DESCRIPTOR_WITH_UNITS(yDataU,&yData,&yUnits);
  mdsdsc_t *yObj = yLabel ? (mdsdsc_t *)&yDataU : (mdsdsc_t *)&yData;

  DESCRIPTOR_A(xData, xArrD->length, xArrD->dtype, xArrD->pointer, xArrD->length * retSamples);
  mdsdsc_t xUnits = {xLabel ? strlen(xLabel) : 0,DTYPE_T,CLASS_S,xLabel};
  DESCRIPTOR_WITH_UNITS(xDataU,&xData,&xUnits);
  mdsdsc_t *xObj = xLabel ? (mdsdsc_t *)&xDataU : (mdsdsc_t *)&xData;
  mdsdsc_t resD  = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char*)&retResolution};
  DESCRIPTOR_SIGNAL_1(Signal, yObj, &resD, xObj);
  mdsdsc_t Help = {title ? strlen(title) : 0,DTYPE_T,CLASS_S,title};
  DESCRIPTOR_PARAM(SignalH,&Signal,&Help,NULL);
  mdsdsc_t *signal = title ? (mdsdsc_t *)&SignalH : (mdsdsc_t *)&Signal;

  MdsCopyDxXd(signal, retXd);
  if ((float*)yArrD->pointer != y) free(y);
return_err: ;
  MdsFree1Dx(&xXd, NULL);
  MdsFree1Dx(&yXd, NULL);
  return status;
}
EXPORT int _GetXYSignalXd(void**ctx, mdsdsc_t *y,  mdsdsc_t *x, mdsdsc_t *xmin, mdsdsc_t *xmax, int num, mdsdsc_xd_t *retXd) {
  int status;
  void* ps = TreeCtxPush(ctx);
  pthread_cleanup_push(TreeCtxPop,ps);
  status = GetXYSignalXd(y, x, xmin, xmax, num, retXd);
  pthread_cleanup_pop(1);
  return status;
}

static mdsdsc_xd_t*getPackedDsc(mdsdsc_xd_t*retXd){
/*Assemble result. Format:
-retResolution(float)
-number of samples (minumum between X and Y)
-type of X xamples (byte: int64_t(1), double(2) or float(3))
-y samples (float - big endian)
-x Samples
*/
  mdsdsc_t *title, *xLabel, *yLabel;
  mdsdsc_t const *sig = retXd->pointer;
  if (sig->dtype == DTYPE_PARAM) {
    title = ((mdsdsc_r_t*)sig)->dscptrs[1];
    sig   = ((mdsdsc_r_t*)sig)->dscptrs[0];
  } else
    title = NULL;
  mdsdsc_t const *dat = ((mdsdsc_r_t*)sig)->dscptrs[0];
  if (dat->dtype == DTYPE_WITH_UNITS) {
    yLabel = ((mdsdsc_r_t*)dat)->dscptrs[1];
    dat    = ((mdsdsc_r_t*)dat)->dscptrs[0];
  } else
    yLabel = NULL;
  mdsdsc_t const *dim = ((mdsdsc_r_t*)sig)->dscptrs[2];
  if (dim->dtype == DTYPE_WITH_UNITS) {
    xLabel = ((mdsdsc_r_t*)dim)->dscptrs[1];
    dim    = ((mdsdsc_r_t*)dim)->dscptrs[0];
  } else
    xLabel = NULL;
  float retResolution = *(float*)(((mdsdsc_r_t*)sig)->dscptrs[1]->pointer);
  const int retSamples = ((mdsdsc_a_t*)dat)->arsize/dat->length;
  int xSampleSize;
  if(dim->dtype == DTYPE_Q || dim->dtype == DTYPE_QU || dim->dtype == DTYPE_DOUBLE)
    xSampleSize = sizeof(int64_t);
  else
    xSampleSize = sizeof(int);
  int retSize = sizeof(float) + sizeof(int) + 1 + retSamples * (sizeof(float) + xSampleSize);
  //Add rool for title and labels
  retSize += 3 * sizeof(int);
  if(title)  retSize += title->length;
  if(xLabel) retSize += xLabel->length;
  if(yLabel) retSize += yLabel->length;
  char *retArr = malloc(retSize);
  *((int *)&retArr[4]) = retSamples;
  swap4(&retArr[4]);
  if(dim->dtype == DTYPE_Q || dim->dtype == DTYPE_QU)
    retArr[8] = 1;
  else if(dim->dtype == DTYPE_DOUBLE)
    retArr[8] = 2;
  else
    retArr[8] = 3;
  int i,idx = 9;
  for(i = 0; i < retSamples; i++, idx += sizeof(float)) {
    *((float *)&retArr[idx]) = ((float *)dat->pointer)[i];
    swap4(&retArr[idx]);
  }
  idx = getXArray((mdsdsc_a_t*)dim, retSamples, retArr, idx);
  if (idx<0) {
    encodeError("Cannot Convert X data dtype",__LINE__,retXd);
     return retXd;
  }
  idx = pack_meta(title,xLabel,yLabel,retResolution,retArr,idx);
  DESCRIPTOR_A(retArrD, 1, DTYPE_B, retArr, retSize);
  MdsCopyDxXd((mdsdsc_t *)&retArrD, retXd);
  return retXd;
}

EXPORT int GetXYWaveXd(mdsdsc_t *sig,  mdsdsc_t *xmin, mdsdsc_t *xmax, int num, mdsdsc_xd_t *retXd) {
  return GetXYSignalXd(sig, NULL, xmin, xmax, num, retXd);
}

EXPORT mdsdsc_xd_t* GetXYSignal(char *inY, char *inX, float *inXMin, float *inXMax, int *reqNSamples) {
  static EMPTYXD(retXd);
  const size_t len = strlen(inY);
  if (len==0) return(encodeError("Y data must not be NULL or empty.",__LINE__,&retXd));
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  mdsdsc_t xMinD  = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD  = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
  int status;{
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &yXd MDS_END_ARG);
  }
  if (STATUS_OK && inX && *inX){
    mdsdsc_t expX = {strlen(inX), DTYPE_T, CLASS_S, inX};
    status = TdiCompile(&expX, &xXd MDS_END_ARG);
  }
  if STATUS_OK status = GetXYSignalXd(yXd.pointer,xXd.pointer,inXMin ? &xMinD : NULL,inXMax ? &xMaxD : NULL,*reqNSamples,&retXd);
  MdsFree1Dx(&yXd, NULL);
  MdsFree1Dx(&xXd, NULL);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,&retXd));
  return getPackedDsc(&retXd);
}

EXPORT mdsdsc_xd_t* GetXYSignalLongTimes(char *inY, char *inX, int64_t *inXMin, int64_t *inXMax, int *reqNSamples) {
  static EMPTYXD(xd);
  const size_t len = strlen(inY);
  if (len==0) return(encodeError("Y data must not be NULL or empty.",__LINE__,&xd));
  EMPTYXD(yXd);
  mdsdsc_t xMinD  = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD  = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMax};
  int status;{
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &yXd MDS_END_ARG);
  }
  if (STATUS_OK && inX && *inX){
    mdsdsc_t expX = {strlen(inX), DTYPE_T, CLASS_S, inX};
    status = TdiCompile(&expX, &xd MDS_END_ARG);
  }
  if STATUS_OK status = GetXYSignalXd(yXd.pointer,xd.pointer,inXMin ? &xMinD : NULL,inXMax ? &xMaxD : NULL,*reqNSamples,&xd);
  MdsFree1Dx(&yXd, NULL);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,&xd));
  return getPackedDsc(&xd);
}

EXPORT mdsdsc_xd_t* GetXYWave(char *inY, float *inXMin, float *inXMax, int *reqNSamples) {
  static EMPTYXD(xd);
  const size_t len = strlen(inY);
  if (len==0) return(encodeError("Y data must not be NULL or empty.",__LINE__,&xd));
  mdsdsc_t xMinD  = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD  = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
  int status;{
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &xd MDS_END_ARG);
  }
  if STATUS_OK status = GetXYSignalXd(xd.pointer,NULL,inXMin ? &xMinD : NULL,inXMax ? &xMaxD : NULL,*reqNSamples,&xd);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,&xd));
  return &xd;
}

EXPORT mdsdsc_xd_t* GetXYWaveLongTimes(char *inY, int64_t *inXMin, int64_t *inXMax, int *reqNSamples) {
  static EMPTYXD(xd);
  const size_t len = strlen(inY);
  if (len==0) return(encodeError("Y data must not be NULL or empty.",__LINE__,&xd));
  mdsdsc_t xMinD  = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD  = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMax};
  int status;{
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &xd MDS_END_ARG);
  }
  if STATUS_OK status = GetXYSignalXd(xd.pointer,NULL,inXMin ? &xMinD : NULL,inXMax ? &xMaxD : NULL,*reqNSamples,&xd);
  if STATUS_NOT_OK return(encodeError(MdsGetMsg(status),__LINE__,&xd));
  return &xd;
}
