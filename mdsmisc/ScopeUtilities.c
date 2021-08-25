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
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdstypes.h>
#include <ncidef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr.h>
#include <treeshr.h>
#include <xtreeshr.h>

#ifndef INFINITY
#define INFINITY 1.0 / 0.0
#endif

#ifdef WORD_BIGENDIAN
#define SWAP32(out, in) memcpy((char *)(out), (char *)(in), 4)
#define SWAP64(out, in) memcpy((char *)(out), (char *)(in), 8)
#else
#define SWAP(out, in, a, b)               \
  ((char *)(out))[a] = ((char *)(in))[b]; \
  ((char *)(out))[b] = ((char *)(in))[a]
#define SWAP32(out, in)  \
  do                     \
  {                      \
    SWAP(out, in, 0, 3); \
    SWAP(out, in, 2, 1); \
  } while (0)
#define SWAP64(out, in)  \
  do                     \
  {                      \
    SWAP(out, in, 0, 7); \
    SWAP(out, in, 2, 5); \
    SWAP(out, in, 4, 3); \
    SWAP(out, in, 6, 1); \
  } while (0)
#endif
static double to_doublex(const void *const ptr, const dtype_t dtype,
                         const double defval, const int is_time)
{
  switch (dtype)
  {
  case DTYPE_FLOAT:
    return (double)*(float *)ptr;
  case DTYPE_DOUBLE:
    return *(double *)ptr;
  case DTYPE_B:
  case DTYPE_BU:
    return (double)*(int8_t *)ptr;
  case DTYPE_W:
  case DTYPE_WU:
    return (double)*(int16_t *)ptr;
  case DTYPE_L:
  case DTYPE_LU:
    return (double)*(int32_t *)ptr;
  case DTYPE_Q:
  case DTYPE_QU:
    if (is_time)
      return ((double)*(int64_t *)ptr);
    return (double)*(int64_t *)ptr;
  default:
    printf("Unsupported Type in getData\n");
    return defval;
  }
}

static int recIsSegmented(const mdsdsc_t *const dsc)
{
  /* returns nid of the first segmented node found
   * or 0 if none found
   */
  if (!dsc)
    return 0;
  int nid, numSegments, status, i;
  int retClassLen, retDtypeLen;
  EMPTYXD(xd);
  mdsdsc_r_t *rDsc;

  switch (dsc->class)
  {
  case CLASS_S:
  {
    if (dsc->dtype == DTYPE_NID)
    {
      nid = *(int *)dsc->pointer;
      status = TreeGetNumSegments(nid, &numSegments);
      if (STATUS_OK && numSegments > 0)
        return nid;
      // Now check if the node contains an expression or a direct nid reference
      unsigned int nciClass = 0, nciDtype = 0;
      struct nci_itm nciList[] = {{1, NciCLASS, &nciClass, &retClassLen},
                                  {1, NciDTYPE, &nciDtype, &retDtypeLen},
                                  {NciEND_OF_LIST, 0, 0, 0}};
      status = TreeGetNci(nid, nciList);
      if (STATUS_OK && (nciClass == CLASS_R ||
                        (nciClass == CLASS_S &&
                         (nciDtype == DTYPE_NID || nciDtype == DTYPE_PATH))))
      {
        status = TreeGetRecord(nid, &xd);
        if (STATUS_OK && xd.l_length > 0)
        {
          nid = recIsSegmented(xd.pointer);
          MdsFree1Dx(&xd, 0);
          return nid;
        }
      }
    }
    else if (dsc->dtype == DTYPE_PATH)
    {
      char *path = malloc(dsc->length + 1);
      memcpy(path, dsc->pointer, dsc->length);
      path[dsc->length] = 0;
      status = TreeFindNode(path, &nid);
      free(path);
      if (STATUS_OK)
      {
        status = TreeGetNumSegments(nid, &numSegments);
        if (STATUS_OK)
        {
          if (numSegments > 0)
          {
            return nid;
          }
          status = TreeGetRecord(nid, &xd);
          if (STATUS_OK && xd.l_length > 0)
          {
            nid = recIsSegmented(xd.pointer);
            MdsFree1Dx(&xd, 0);
            return nid;
          }
        }
      }
    }
    break;
  }
  case CLASS_A:
    break;
  case CLASS_XD:
    if (dsc->pointer)
      return recIsSegmented((mdsdsc_t *)dsc->pointer);
    break;
  case CLASS_R:
  {
    rDsc = (mdsdsc_r_t *)dsc;
    if (rDsc->dtype == DTYPE_SIGNAL)
      break; // If the expression includes a Signal descriptor, the time base
             // may be different from that of the segmented node
    for (i = 0; i < rDsc->ndesc; i++)
    {
      if (rDsc->dscptrs[i] && (nid = recIsSegmented(rDsc->dscptrs[i])))
        return nid;
    }
    break;
  }
  default:
    break;
  }
  return 0;
}

#define NUM_SEGMENTS_THRESHOLD 2
#define NUM_SAMPLES_THRESHOLD 500000

/**
 * Make an estimation of the number of samples in the specified interval. -1 is
 *returned when when online minmax reasampling is deemed not necessary. If the
 *number
 **/

#define MAX64 0x7FFFFFFFFFFFFFFFL
inline static int64_t
estimateNumSamples(const mdsdsc_t *const dsc, mdsdsc_t *const xMin,
                   mdsdsc_t *const xMax, int *const estimatedSegmentSamples,
                   double *const dMin, double *const dMax, double *const sMin,
                   double *const sMax, int *const dIsLong)
{
  /* return the number of samples the signal holds based on meta information
     or -1 if something went wrong
   */
  int numSegments, startIdx, endIdx;
  int64_t startTime = -MAX64 - 1, endTime = MAX64, currEnd;
  char dtype, dimct;
  int dims[64];
  int nextRow, segmentSamples, numActSegments, segmentIdx;
  EMPTYXD(xd);
  int nid = recIsSegmented(dsc);
  if (!nid)
    goto return_neg1;
  int status = TreeGetNumSegments(nid, &numSegments);
  if (STATUS_NOT_OK)
    goto return_neg1;
  status = TreeGetSegmentLimits(nid, 0, NULL, &xd);
  if (STATUS_NOT_OK)
    goto return_neg1;
  status = TdiData(xd.pointer, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto return_neg1;
  *dIsLong = xd.pointer &&
             (xd.pointer->dtype == DTYPE_Q || xd.pointer->dtype == DTYPE_QU);
  if (xMin != NULL || xMax != NULL)
  {
    if (xMin)
      XTreeConvertToLongTime(xMin, &startTime);
    if (xMax)
      XTreeConvertToLongTime(xMax, &endTime);
    if (numSegments < NUM_SEGMENTS_THRESHOLD)
      goto return_neg1;
    startIdx = 0; // If no start time specified, take all initial segments
    if (xMin)
    {
      while (startIdx < numSegments)
      {
        status = TreeGetSegmentLimits(nid, startIdx, NULL, &xd);
        if (STATUS_NOT_OK)
          goto return_neg1;
        status = XTreeConvertToLongTime(xd.pointer, &currEnd);
        if (STATUS_NOT_OK)
          goto return_neg1;
        if (currEnd >= startTime) // First overlapping segment
          break;
        startIdx++;
      }
    }
    if (startIdx == numSegments)
      goto return_neg1; // All segments antecedent to start time
    if (!xMax)
      endIdx = numSegments - 1;
    else
    {
      segmentIdx = startIdx;
      while (segmentIdx < numSegments)
      {
        status = TreeGetSegmentLimits(nid, segmentIdx, NULL, &xd);
        if (STATUS_NOT_OK)
          goto return_neg1;
        status = XTreeConvertToLongTime(xd.pointer, &currEnd);
        if (STATUS_NOT_OK)
          goto return_neg1;
        if (currEnd >= endTime) // Last overlapping segment
          break;
        segmentIdx++;
      }
      // No segment (section) after end
      endIdx = (segmentIdx == numSegments) ? numSegments - 1 : segmentIdx;
    }
    numActSegments = endIdx - startIdx + 1;
  }
  else
  {
    startIdx = 0;
    numActSegments = numSegments;
    endIdx = numSegments - 1;
  }
  status = TreeGetSegmentInfo(nid, startIdx, &dtype, &dimct, dims, &nextRow);
  if (STATUS_NOT_OK)
    goto return_neg1;
  segmentSamples = nextRow;
  // Compute duration
  status = TreeGetSegmentLimits(nid, startIdx, &xd, NULL);
  if (STATUS_OK)
    status = TdiData(xd.pointer, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto return_neg1;
  *sMin = to_doublex(xd.pointer->pointer, xd.pointer->dtype, -INFINITY, TRUE);
  status = TreeGetSegmentLimits(nid, endIdx, NULL, &xd);
  if (STATUS_OK)
    status = TdiData((mdsdsc_t *)&xd, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto return_neg1;
  *sMax = to_doublex(xd.pointer->pointer, xd.pointer->dtype, INFINITY, TRUE);
  MdsFree1Dx(&xd, NULL);
  *dMin =
      xMin ? to_doublex(xMin->pointer, xMin->dtype, -INFINITY, TRUE) : *sMin;
  *dMax = xMax ? to_doublex(xMax->pointer, xMax->dtype, INFINITY, TRUE) : *sMax;
  if (numActSegments == 1 && xMin && xMax) //Handle the case of a single (likely large) segment
  {
    double givenTimeSpan = to_doublex(xMax->pointer, xMax->dtype, -INFINITY, TRUE) - to_doublex(xMin->pointer, xMin->dtype, -INFINITY, TRUE);
    double segmentTimeSpan = *sMax - *sMin;
    if (segmentTimeSpan > 0 && segmentTimeSpan > givenTimeSpan)
      return (int64_t)(segmentSamples * givenTimeSpan / segmentTimeSpan);
  }
  *estimatedSegmentSamples = segmentSamples;
  return segmentSamples * (int64_t)numActSegments;
return_neg1:;
  if (xMin && IS_OK(TdiData(xMin, &xd MDS_END_ARG)))
  {
    *sMin = to_doublex(xd.pointer->pointer, xd.pointer->dtype, -INFINITY, TRUE);
    *dMin = to_doublex(xMin->pointer, xMin->dtype, -INFINITY, TRUE);
  }
  else
  {
    *sMin = -INFINITY;
    *dMin = -INFINITY;
  }
  if (xMax && IS_OK(TdiData(xMax, &xd MDS_END_ARG)))
  {
    *sMax = to_doublex(xd.pointer->pointer, xd.pointer->dtype, INFINITY, TRUE);
    *dMax = to_doublex(xMax->pointer, xMax->dtype, INFINITY, TRUE);
  }
  else
  {
    *sMax = INFINITY;
    *dMax = INFINITY;
  }
  MdsFree1Dx(&xd, NULL);
  return -1;
}

// Perform trim on site (do not realloc arrays) return the actual number of
// points the type of X array is unknown, element size is passed in xSize
static inline void trimData(float *const y, mdsdsc_a_t *const x,
                            const int nSamples, const int reqPoints,
                            const double xMin, const double xMax,
                            int *const retPoints, float *const retResolution,
                            int treeshrResampled)
{

  /*  if(nSamples < 10 * reqPoints) {
    //Does not perform any compression
    *retResolution = 1E12;
    *retPoints = nSamples;
  if(treeshrResampled)
  {
      *retResolution = reqPoints/(to_doublex(&x->pointer[endIdx * x->length],
  x->dtype,INFINITY,TRUE) - to_doublex(&x->pointer[startIdx * x->length],
  x->dtype,-INFINITY,TRUE)); printf("AMENDED RESOLUTION: %f\n", *retResolution);
  }
    return;
  }
  */
  // From here, consider xMin and xMax
  int startIdx, endIdx;

  for (startIdx = 0;
       startIdx < nSamples && to_doublex(&x->pointer[startIdx * x->length],
                                         x->dtype, xMin, TRUE) <= xMin;
       startIdx++)
    ;
  if (startIdx == nSamples)
    startIdx--;
  for (endIdx = startIdx;
       endIdx < nSamples &&
       to_doublex(&x->pointer[endIdx * x->length], x->dtype, xMax, TRUE) < xMax;
       endIdx++)
    ;
  if (endIdx == nSamples)
    endIdx--;
  const int deltaIdx = ((endIdx - startIdx) < 10 * reqPoints)
                           ? 1
                           : (endIdx - startIdx + 1) / reqPoints;

  if (nSamples < 10 * reqPoints)
  {
    // Does not perform any compression
    *retResolution = 1E12;
    *retPoints = nSamples;
    if (treeshrResampled &&
        endIdx != startIdx) // If resampling has been performed by treeshr
      *retResolution =
          reqPoints / (to_doublex(&x->pointer[endIdx * x->length], x->dtype,
                                  INFINITY, TRUE) -
                       to_doublex(&x->pointer[startIdx * x->length], x->dtype,
                                  -INFINITY, TRUE));
    return;
  }

  int curIdx = startIdx;
  int outIdx = 0;
  if (deltaIdx == 1)
  {
    *retResolution = 1E12;
    if (outIdx < curIdx)
    { // check if not src == dst
      while (curIdx < endIdx)
      {
        memcpy(&x->pointer[outIdx * x->length], &x->pointer[curIdx * x->length],
               x->length);
        y[outIdx++] = y[curIdx++];
      }
    }
    else // but in this case the number of points must be reported!!!
    {
      outIdx = endIdx - startIdx;
    }
  }
  else
  {
    *retResolution = reqPoints / (to_doublex(&x->pointer[endIdx * x->length],
                                             x->dtype, INFINITY, TRUE) -
                                  to_doublex(&x->pointer[startIdx * x->length],
                                             x->dtype, -INFINITY, TRUE));
    const double deltaTime =
        (deltaIdx == 1) ? 0 : (xMax - xMin) / (double)reqPoints;
    float minY, maxY;
    while (curIdx < endIdx)
    {
      minY = maxY = y[curIdx];
      int i, actSamples = 0;
      const double endXDouble = to_doublex(&x->pointer[curIdx * x->length],
                                           x->dtype, INFINITY, TRUE) +
                                deltaTime;
      for (i = curIdx; i < nSamples && i < curIdx + deltaIdx;
           i++, actSamples++)
      {
        if (to_doublex(&x->pointer[i * x->length], x->dtype, -INFINITY, TRUE) >
            endXDouble)
          break; // Handle dual speed clocks
        if (y[i] < minY)
          minY = y[i];
        if (y[i] > maxY)
          maxY = y[i];
      }
      curIdx += (actSamples + 1) / 2; // half step ensures outIdx != curIdx
      if (outIdx < curIdx)
        memcpy(&x->pointer[outIdx * x->length], &x->pointer[curIdx * x->length],
               x->length);
      y[outIdx++] = minY;
      if (outIdx < curIdx)
        memcpy(&x->pointer[outIdx * x->length], &x->pointer[curIdx * x->length],
               x->length);
      y[outIdx++] = maxY;
      curIdx += actSamples / 2;
    }
  }
  *retPoints = outIdx;
  if (treeshrResampled &&
      endIdx != startIdx) // If resampling has been performed by treeshr
    *retResolution = reqPoints / (to_doublex(&x->pointer[endIdx * x->length],
                                             x->dtype, INFINITY, TRUE) -
                                  to_doublex(&x->pointer[startIdx * x->length],
                                             x->dtype, -INFINITY, TRUE));
}

static int recGetXxxx(const mdsdsc_t *const dsc_in, mdsdsc_xd_t *const xd_out,
                      const int getHelp)
{
  const mdsdsc_t *dsc = dsc_in;
again:;
  if (!dsc)
    return MDSplusERROR;
  int status;
  switch (dsc->class)
  {
  case CLASS_S:
    if (dsc->dtype == DTYPE_NID || dsc->dtype == DTYPE_PATH)
    {
      int nid;
      if (dsc->dtype == DTYPE_NID)
        nid = *(int *)dsc->pointer;
      else
      { // if(dsc->dtype == DTYPE_PATH)
        char *path = malloc(dsc->length + 1);
        memcpy(path, dsc->pointer, dsc->length);
        path[dsc->length] = 0;
        status = TreeFindNode(path, &nid);
        free(path);
        if (STATUS_NOT_OK)
          goto status_not_ok_out;
      }
      int numSegments;
      status = TreeGetNumSegments(nid, &numSegments);
      if (STATUS_NOT_OK || numSegments > 0)
        goto error_out;
      EMPTYXD(xd);
      status = TreeGetRecord(nid, &xd);
      if (STATUS_OK)
        status = recGetXxxx(xd.pointer, xd_out, getHelp);
      MdsFree1Dx(&xd, NULL);
      goto status_out;
    }
    break;
  case CLASS_A:
    break;
  case CLASS_XD:
    if (!dsc->pointer)
      break;
    dsc = (mdsdsc_t *)dsc->pointer;
    goto again;
  case CLASS_R:
    if (getHelp)
    {
      if (dsc->dtype == DTYPE_PARAM)
      {
        mds_param_t *pDsc = (mds_param_t *)dsc;
        if (pDsc->help)
        {
          status = TdiData(pDsc->help, xd_out MDS_END_ARG);
          if (STATUS_NOT_OK)
            goto status_not_ok_out;
          if (xd_out->pointer && xd_out->pointer->class == CLASS_S &&
              xd_out->pointer->dtype == DTYPE_T)
            goto status_out;
        }
        break;
      }
      mdsdsc_r_t *rDsc = (mdsdsc_r_t *)dsc;
      int i;
      for (i = 0; i < rDsc->ndesc; i++)
      {
        status = recGetXxxx(rDsc->dscptrs[i], xd_out, getHelp);
        if (STATUS_OK)
          goto status_out;
      }
    }
    else
    {
      if (dsc->dtype == DTYPE_WITH_UNITS)
      {
        mds_with_units_t *uDsc = (mds_with_units_t *)dsc;
        if (uDsc->units)
        {
          status = TdiData(uDsc->units, xd_out MDS_END_ARG);
          if (STATUS_NOT_OK)
            goto status_not_ok_out;
          if (xd_out->pointer && xd_out->pointer->class == CLASS_S &&
              xd_out->pointer->dtype == DTYPE_T)
            goto status_out;
        }
        break;
      }
      if (dsc->dtype == DTYPE_PARAM)
      {
        dsc = ((mds_param_t *)dsc)->value;
        ;
        goto again;
      }
      if (dsc->dtype == DTYPE_SIGNAL)
      {
        dsc = ((mds_signal_t *)dsc)->data;
        goto again;
      }
      mdsdsc_r_t *rDsc = (mdsdsc_r_t *)dsc;
      if (rDsc->ndesc == 1)
      {
        dsc = rDsc->dscptrs[0];
        goto again;
      }
      if (rDsc->dtype == DTYPE_FUNCTION && rDsc->pointer)
      {
        int i;
        if (*(opcode_t *)rDsc->pointer == OPC_ADD ||
            *(opcode_t *)rDsc->pointer == OPC_SUBTRACT)
        {
          status = recGetXxxx(rDsc->dscptrs[0], xd_out, getHelp);
          if (STATUS_NOT_OK)
            goto status_not_ok_out;
          EMPTYXD(xd);
          for (i = 1; i < rDsc->ndesc; i++)
          {
            status = recGetXxxx(rDsc->dscptrs[i], &xd, getHelp);
            if (STATUS_NOT_OK ||
                xd.pointer->length != xd_out->pointer->length ||
                strncmp(xd.pointer->pointer, xd_out->pointer->pointer,
                        xd.pointer->length))
            {
              MdsFree1Dx(&xd, NULL);
              goto error_out; // Different units
            }
          }
          MdsFree1Dx(&xd, NULL);
          goto status_out;
        }
        if (*(opcode_t *)rDsc->pointer == OPC_MULTIPLY ||
            *(opcode_t *)rDsc->pointer == OPC_DIVIDE)
        {
          mdsdsc_t da = {0, DTYPE_T, CLASS_D, NULL};
          char mulDivC =
              (*(opcode_t *)rDsc->pointer == OPC_MULTIPLY) ? '*' : '/';
          mdsdsc_t mulDiv = {1, DTYPE_T, CLASS_S, &mulDivC};
          EMPTYXD(xd);
          for (i = 0; i < rDsc->ndesc; i++)
          {
            status = recGetXxxx(rDsc->dscptrs[i], &xd, getHelp);
            if (STATUS_OK)
            {
              if (da.pointer)
              {
                status = StrConcat(&da, &da, &mulDiv, xd.pointer MDS_END_ARG);
                if (STATUS_NOT_OK)
                {
                  MdsFree1Dx(&xd, NULL);
                  goto status_not_ok_out;
                }
              }
              else
              {
                da.length = xd.pointer->length;
                da.pointer = strdup(xd.pointer->pointer);
              }
            }
          }
          MdsFree1Dx(&xd, NULL);
          if (da.pointer)
          {
            da.class = CLASS_S;
            MdsCopyDxXd((mdsdsc_t *)&da, xd_out);
            free(da.pointer);
            goto success;
          }
        }
      }
    }
    break;
  default:
    break;
  }
error_out:;
  status = MDSplusERROR;
status_not_ok_out:;
  MdsFree1Dx(xd_out, NULL);
status_out:;
  return status;
success:;
  return MDSplusSUCCESS;
}

static inline int recGetHelp(const mdsdsc_t *const dsc,
                             mdsdsc_xd_t *const xd_out)
{
  return recGetXxxx(dsc, xd_out, TRUE);
}

static inline int recGetUnits(const mdsdsc_t *const dsc,
                              mdsdsc_xd_t *const xd_out)
{
  return recGetXxxx(dsc, xd_out, FALSE);
}

// Check if the passed expression contains at least one segmented node
EXPORT int IsSegmented(char *const expr)
{
  EMPTYXD(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  if (IS_NOT_OK(TdiCompile(&exprD, &xd MDS_END_ARG)))
    return FALSE;
  int segNid = recIsSegmented(xd.pointer);
  MdsFree1Dx(&xd, NULL);
  return segNid;
}

// Check if the passed expression contains at least one segmented node
EXPORT struct descriptor_xd *GetPathOf(int *nid)
{
  static EMPTYXD(retXd);
  mdsdsc_t pathD = {0, DTYPE_T, CLASS_S, 0};
  char *path = TreeGetPath(*nid);
  if (!path)
    return NULL;
  pathD.length = strlen(path);
  pathD.pointer = path;
  MdsCopyDxXd(&pathD, &retXd);
  TreeFree(path);
  return &retXd;
}

EXPORT int TestGetHelp(char *const expr)
{
  EMPTYXD(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  if (IS_NOT_OK(TdiCompile(&exprD, &xd MDS_END_ARG)))
    return FALSE;
  MdsFree1Dx(&xd, 0);
  return TRUE;
}

EXPORT int TestGetUnits(char *const expr)
{
  EMPTYXD(xd);
  mdsdsc_t exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
  if (IS_NOT_OK(TdiCompile(&exprD, &xd MDS_END_ARG)))
    return FALSE;
  MdsFree1Dx(&xd, 0);
  return TRUE;
}

static inline int pack_meta(const mdsdsc_t *const title,
                            const mdsdsc_t *const xLabel,
                            const mdsdsc_t *const yLabel, const float res,
                            char *const retArr, int idx)
{
  // idx is the current index in retArr
  // Write title, xLabel, yLabel as length followed by chars
  int len;
  if (title)
  {
    len = title->length & 0xffff;
    SWAP32(&retArr[idx], &len);
    idx += sizeof(int);
    memcpy(&retArr[idx], title->pointer, len);
    idx += len;
  }
  else
  {
    *(int *)&retArr[idx] = 0; // no swap required
    idx += 4;
  }
  if (xLabel)
  {
    len = xLabel->length & 0xffff;
    SWAP32(&retArr[idx], &len);
    idx += sizeof(int);
    memcpy(&retArr[idx], xLabel->pointer, len);
    idx += len;
  }
  else
  {
    *(int *)&retArr[idx] = 0; // no swap required
    idx += 4;
  }
  if (yLabel)
  {
    len = yLabel->length & 0xffff;
    SWAP32(&retArr[idx], &len);
    idx += sizeof(int);
    memcpy(&retArr[idx], yLabel->pointer, len);
    idx += len;
  }
  else
  {
    *(int *)&retArr[idx] = 0; // no swap required
    idx += 4;
  }
  SWAP32(retArr, &res); // resolution
  return idx;
}

inline static int getNSamples(const mdsdsc_xd_t *const yXd,
                              const mdsdsc_xd_t *const xXd,
                              int *const nSamples)
{
  if (yXd->pointer->class != CLASS_A)
    return TdiINVCLADSC;
  if (xXd->pointer->class != CLASS_A)
    return TdiINVCLADSC;
  if (yXd->pointer->dtype == DTYPE_F)
  {
    const int status = TdiFloat(yXd->pointer, yXd MDS_END_ARG);
    if (STATUS_NOT_OK)
      return status;
  }
  if (xXd->pointer->dtype == DTYPE_F)
  {
    const int status = TdiFloat(xXd->pointer, xXd MDS_END_ARG);
    if (STATUS_NOT_OK)
      return status;
  }
  // Number of samples set to minimum between X and Y
  const int ySamples =
      ((mdsdsc_a_t *)yXd->pointer)->arsize / yXd->pointer->length;
  const int xSamples =
      ((mdsdsc_a_t *)xXd->pointer)->arsize / xXd->pointer->length;
  *nSamples = xSamples < ySamples ? xSamples : ySamples;
  return MDSplusSUCCESS;
}

inline static float *getFloatArray(const mdsdsc_a_t *const yArrD,
                                   const int nSamples)
{
  int i;
  float *y;
  switch (yArrD->dtype)
  {
  case DTYPE_B:
  case DTYPE_BU:
    y = (float *)malloc(nSamples * sizeof(float));
    for (i = 0; i < nSamples; i++)
      y[i] = *((char *)(&yArrD->pointer[i * yArrD->length]));
    return y;
  case DTYPE_W:
  case DTYPE_WU:
    y = (float *)malloc(nSamples * sizeof(float));
    for (i = 0; i < nSamples; i++)
      y[i] = *((short *)(&yArrD->pointer[i * yArrD->length]));
    return y;
  case DTYPE_L:
  case DTYPE_LU:
    y = (float *)malloc(nSamples * sizeof(float));
    for (i = 0; i < nSamples; i++)
      y[i] = *((int *)(&yArrD->pointer[i * yArrD->length]));
    return y;
  case DTYPE_Q:
  case DTYPE_QU:
    y = (float *)malloc(nSamples * sizeof(float));
    for (i = 0; i < nSamples; i++)
      y[i] = *((int64_t *)(&yArrD->pointer[i * yArrD->length]));
    return y;
  case DTYPE_DOUBLE:
    y = (float *)malloc(nSamples * sizeof(float));
    for (i = 0; i < nSamples; i++)
      y[i] = *((double *)(&yArrD->pointer[i * yArrD->length]));
    return y;
  case DTYPE_FLOAT:
    return (float *)yArrD->pointer;
  default:
    return NULL;
  }
}

static inline int getXArray(const mdsdsc_a_t *const xArrD, const int retSamples,
                            char *const retArr, int idx)
{
  int i;
  switch (xArrD->dtype)
  {
  default:
    return -1;
  case DTYPE_B:
  case DTYPE_BU:
    for (i = 0; i < retSamples; i++, idx += 4)
    {
      const float tmp = *((int8_t *)(&xArrD->pointer[i * xArrD->length]));
      SWAP32(&retArr[idx], &tmp);
    }
    break;
  case DTYPE_W:
  case DTYPE_WU:
    for (i = 0; i < retSamples; i++, idx += 4)
    {
      const float tmp = *((int16_t *)(&xArrD->pointer[i * xArrD->length]));
      SWAP32(&retArr[idx], &tmp);
    }
    break;
  case DTYPE_L:
  case DTYPE_LU:
    for (i = 0; i < retSamples; i++, idx += 4)
    {
      const float tmp = *((int32_t *)(&xArrD->pointer[i * xArrD->length]));
      SWAP32(&retArr[idx], &tmp);
    }
    break;
  case DTYPE_Q:
  case DTYPE_QU:
    for (i = 0; i < retSamples; i++, idx += 8)
    {
      SWAP64(&retArr[idx], &xArrD->pointer[i * xArrD->length]);
    }
    break;
  case DTYPE_FLOAT:
    for (i = 0; i < retSamples; i++, idx += 4)
    {
      SWAP32(&retArr[idx], &xArrD->pointer[i * xArrD->length]);
    }
    break;
  case DTYPE_DOUBLE:
    for (i = 0; i < retSamples; i++, idx += 8)
    {
      SWAP64(&retArr[idx], &xArrD->pointer[i * xArrD->length]);
    }
    break;
  }
  return idx;
}

EXPORT int GetXYSignalXd(mdsdsc_t *const inY, mdsdsc_t *const inX,
                         mdsdsc_t *const inXMin, mdsdsc_t *const inXMax,
                         const int reqNSamples, mdsdsc_xd_t *const retXd)
{
  if (!inY)
    return TdiNULL_PTR;
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  EMPTYXD(title);
  EMPTYXD(xLabel);
  EMPTYXD(yLabel);
  int estimatedSegmentSamples = 0;
  int isLong = FALSE;
  double xmin = -INFINITY, xmax = INFINITY; // requested
  double smin = -INFINITY, smax = INFINITY; // actual
  double delta;
  mdsdsc_t *xMinP, *xMaxP, *deltaP,
      deltaD = {sizeof(double), DTYPE_DOUBLE, CLASS_S, (char *)&delta};

  int64_t estimatedSamples =
      estimateNumSamples(inY, inXMin, inXMax, &estimatedSegmentSamples, &xmin,
                         &xmax, &smin, &smax, &isLong);
  const double estimatedDuration = smax - smin;
  xMinP = (xmin > -INFINITY) ? inXMin : NULL;
  xMaxP = (xmax < INFINITY) ? inXMax : NULL;

  //printf("ESTIMATED SAMPLES: %d  THRESHOLD: %d\n", estimatedSamples,
  //NUM_SAMPLES_THRESHOLD);

  if (estimatedSamples > NUM_SAMPLES_THRESHOLD)
  {
    delta = estimatedSamples / reqNSamples;
    if (xmin > -INFINITY && xmax < INFINITY && smax > smin)
      delta *= (xmax - xmin) / (smax - smin);

    // Now delta represents the number of samples to be compressed in a min-max
    // mair
    if (delta > estimatedSegmentSamples / 10.)
      delta = estimatedSegmentSamples / 10.;
    // In any case don't make it too big in respect of the single segment size
    // (at minimum 10 samples )pairs) per segment

    delta *= (estimatedDuration / estimatedSamples);
    if (isLong)
      delta /= 1e9; // quick compensation of xtreeshr conversion

    //printf("DELTA: %e\n", delta);

    deltaP = (delta > 1e-9) ? &deltaD : NULL;
  }
  else
    deltaP = NULL;
  // Set limits if any

  int status = TreeSetTimeContext(xMinP, xMaxP, deltaP);

  if (STATUS_OK)
    status = TdiEvaluate(inY, &yXd MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto return_err;
  // Get Y, title, and yLabel, if any
  recGetHelp(yXd.pointer, &title);
  recGetUnits(yXd.pointer, &yLabel);
  // Get X
  if (!inX) // If an explicit expression for X has been given
    status = TdiDimOf(yXd.pointer, &xXd MDS_END_ARG);
  else // Get xLabel, if any
    status = TdiEvaluate(inX, &xXd MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto return_err;

  recGetUnits(xXd.pointer, &xLabel);
  if (STATUS_OK)
    status = TdiData((mdsdsc_t *)&xXd, &xXd MDS_END_ARG);
  if (STATUS_OK)
    status = TdiData((mdsdsc_t *)&yXd, &yXd MDS_END_ARG);

  int nSamples = 0;
  if (STATUS_OK)
    status = getNSamples(&yXd, &xXd, &nSamples);
  if (STATUS_NOT_OK)
    goto return_err;
  mdsdsc_a_t *xArrD = (mdsdsc_a_t *)xXd.pointer;
  mdsdsc_a_t *yArrD = (mdsdsc_a_t *)yXd.pointer;
  float *y = getFloatArray(yArrD, nSamples);
  int retSamples;
  float retResolution;

  trimData(y, xArrD, nSamples, reqNSamples, xmin, xmax, &retSamples,
           &retResolution, deltaP != NULL);

  DESCRIPTOR_A(yData, sizeof(float), DTYPE_FLOAT, (char *)y,
               sizeof(float) * retSamples);
  DESCRIPTOR_WITH_UNITS(yDataU, &yData, yLabel.pointer);
  mdsdsc_t *yObj = yLabel.pointer ? (mdsdsc_t *)&yDataU : (mdsdsc_t *)&yData;

  DESCRIPTOR_A(xData, xArrD->length, xArrD->dtype, xArrD->pointer,
               xArrD->length * retSamples);
  DESCRIPTOR_WITH_UNITS(xDataU, &xData, xLabel.pointer);
  mdsdsc_t *xObj = xLabel.pointer ? (mdsdsc_t *)&xDataU : (mdsdsc_t *)&xData;
  mdsdsc_t resD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)&retResolution};
  DESCRIPTOR_SIGNAL_1(Signal, yObj, &resD, xObj);
  DESCRIPTOR_PARAM(SignalH, &Signal, title.pointer, NULL);
  mdsdsc_t *signal = title.pointer ? (mdsdsc_t *)&SignalH : (mdsdsc_t *)&Signal;

  MdsCopyDxXd(signal, retXd);
  if ((float *)yArrD->pointer != y)
    free(y);
return_err:;
  MdsFree1Dx(&title, NULL);
  MdsFree1Dx(&xLabel, NULL);
  MdsFree1Dx(&yLabel, NULL);
  MdsFree1Dx(&xXd, NULL);
  MdsFree1Dx(&yXd, NULL);
  TreeSetTimeContext(NULL, NULL, NULL); // reset timecontext
  return status;
}
EXPORT int _GetXYSignalXd(void **const ctx, mdsdsc_t *const y,
                          mdsdsc_t *const x, mdsdsc_t *const xmin,
                          mdsdsc_t *const xmax, const int num,
                          mdsdsc_xd_t *const retXd)
{
  int status;
  void *ps = TreeCtxPush(ctx);
  pthread_cleanup_push(TreeCtxPop, ps);
  status = GetXYSignalXd(y, x, xmin, xmax, num, retXd);
  pthread_cleanup_pop(1);
  return status;
}
EXPORT int GetXYWaveXd(mdsdsc_t *sig, mdsdsc_t *xmin, mdsdsc_t *xmax, int num,
                       mdsdsc_xd_t *retXd)
{
  return GetXYSignalXd(sig, NULL, xmin, xmax, num, retXd);
}

static mdsdsc_xd_t *encodeError(const char *error, const int line,
                                mdsdsc_xd_t *out_xd)
{
  /* converts message to packed error message (float res,int len,char msg[len])
   */
  typedef struct __attribute__((__packed__))
  {
    int line;
    int len;
    char msg[0];
  } jerr_t;
  const size_t err_ln = strlen(error);
  const size_t err_sz = err_ln + sizeof(jerr_t);
  jerr_t *jerr = malloc(err_sz);
  {
    jerr->line = line;
    jerr->len = err_ln;
    memcpy(jerr->msg, error, err_ln);
  }
  mdsdsc_t err_d = {err_sz, DTYPE_T, CLASS_S, (char *)jerr};
  MdsCopyDxXd(&err_d, out_xd);
  free(jerr);
  return out_xd;
}

static mdsdsc_xd_t *getPackedDsc(mdsdsc_xd_t *retXd)
{
  /*Assemble result. Format:
  -retResolution(float)
  -number of samples (minumum between X and Y)
  -type of X xamples (byte: int64_t(1), double(2) or float(3))
  -y samples (float - big endian)
  -x Samples
  */
  mdsdsc_t *title, *xLabel, *yLabel;
  mdsdsc_t const *sig = retXd->pointer;
  if (sig->dtype == DTYPE_PARAM)
  {
    title = ((mdsdsc_r_t *)sig)->dscptrs[1];
    sig = ((mdsdsc_r_t *)sig)->dscptrs[0];
  }
  else
    title = NULL;
  mdsdsc_t const *dat = ((mdsdsc_r_t *)sig)->dscptrs[0];
  if (dat->dtype == DTYPE_WITH_UNITS)
  {
    yLabel = ((mdsdsc_r_t *)dat)->dscptrs[1];
    dat = ((mdsdsc_r_t *)dat)->dscptrs[0];
  }
  else
    yLabel = NULL;
  mdsdsc_t const *dim = ((mdsdsc_r_t *)sig)->dscptrs[2];
  if (dim->dtype == DTYPE_WITH_UNITS)
  {
    xLabel = ((mdsdsc_r_t *)dim)->dscptrs[1];
    dim = ((mdsdsc_r_t *)dim)->dscptrs[0];
  }
  else
    xLabel = NULL;
  float retResolution = *(float *)(((mdsdsc_r_t *)sig)->dscptrs[1]->pointer);
  const int retSamples = ((mdsdsc_a_t *)dat)->arsize / dat->length;
  int xSampleSize;
  if (dim->dtype == DTYPE_Q || dim->dtype == DTYPE_QU ||
      dim->dtype == DTYPE_DOUBLE)
    xSampleSize = sizeof(int64_t);
  else
    xSampleSize = sizeof(int);
  int retSize = sizeof(float) + sizeof(int) + 1 +
                retSamples * (sizeof(float) + xSampleSize);
  // Add rool for title and labels
  retSize += 3 * sizeof(int);
  if (title)
    retSize += title->length;
  if (xLabel)
    retSize += xLabel->length;
  if (yLabel)
    retSize += yLabel->length;
  char *retArr = malloc(retSize);
  SWAP32(&retArr[4], &retSamples);
  if (dim->dtype == DTYPE_Q || dim->dtype == DTYPE_QU)
    retArr[8] = 1;
  else if (dim->dtype == DTYPE_DOUBLE)
    retArr[8] = 2;
  else
    retArr[8] = 3;
  int i, idx = 9;
  for (i = 0; i < retSamples; i++, idx += sizeof(float))
  {
    SWAP32(&retArr[idx], &((float *)dat->pointer)[i]);
  }
  idx = getXArray((mdsdsc_a_t *)dim, retSamples, retArr, idx);
  if (idx < 0)
  {
    encodeError("Cannot Convert X data dtype", __LINE__, retXd);
    return retXd;
  }
  idx = pack_meta(title, xLabel, yLabel, retResolution, retArr, idx);
  DESCRIPTOR_A(retArrD, 1, DTYPE_B, retArr, retSize);
  MdsCopyDxXd((mdsdsc_t *)&retArrD, retXd);

  return retXd;
}

EXPORT mdsdsc_xd_t *GetXYSignal(char *inY, char *inX, float *inXMin,
                                float *inXMax, int *reqNSamples)
{
  static EMPTYXD(retXd);
  const size_t len = strlen(inY);
  // printf("GET XY SIGNAL %s xmin: %f  xmax: %f Req samples: %d  \n", inY,
  // *inXMin, *inXMax, *reqNSamples);
  if (len == 0)
    return (encodeError("Y data must not be NULL or empty.", __LINE__, &retXd));
  EMPTYXD(yXd);
  EMPTYXD(xXd);
  mdsdsc_t xMinD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
  int status;
  {
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &yXd MDS_END_ARG);
  }
  if (STATUS_OK && inX && *inX)
  {
    mdsdsc_t expX = {strlen(inX), DTYPE_T, CLASS_S, inX};
    status = TdiCompile(&expX, &xXd MDS_END_ARG);
  }
  if (STATUS_OK)
    status =
        GetXYSignalXd(yXd.pointer, xXd.pointer, inXMin ? &xMinD : NULL,
                      inXMax ? &xMaxD : NULL, *reqNSamples, &retXd);
  MdsFree1Dx(&yXd, NULL);
  MdsFree1Dx(&xXd, NULL);
  if (STATUS_NOT_OK)
    return (encodeError(MdsGetMsg(status), __LINE__, &retXd));
  return getPackedDsc(&retXd);
}

EXPORT mdsdsc_xd_t *GetXYSignalLongTimes(char *inY, char *inX, int64_t *inXMin,
                                         int64_t *inXMax, int *reqNSamples)
{
  static EMPTYXD(xd);

  const size_t len = strlen(inY);
  if (len == 0)
    return (encodeError("Y data must not be NULL or empty.", __LINE__, &xd));
  EMPTYXD(yXd);
  mdsdsc_t xMinD = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMax};
  int status;
  {
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &yXd MDS_END_ARG);
  }
  if (STATUS_OK && inX && *inX)
  {
    mdsdsc_t expX = {strlen(inX), DTYPE_T, CLASS_S, inX};
    status = TdiCompile(&expX, &xd MDS_END_ARG);
  }
  if (STATUS_OK)
    status =
        GetXYSignalXd(yXd.pointer, xd.pointer, inXMin ? &xMinD : NULL,
                      inXMax ? &xMaxD : NULL, *reqNSamples, &xd);
  MdsFree1Dx(&yXd, NULL);
  if (STATUS_NOT_OK)
    return (encodeError(MdsGetMsg(status), __LINE__, &xd));
  return getPackedDsc(&xd);
}

EXPORT mdsdsc_xd_t *GetXYWave(char *inY, float *inXMin, float *inXMax,
                              int *reqNSamples)
{
  static EMPTYXD(xd);
  const size_t len = strlen(inY);
  if (len == 0)
    return (encodeError("Y data must not be NULL or empty.", __LINE__, &xd));
  mdsdsc_t xMinD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
  int status;
  {
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &xd MDS_END_ARG);
  }
  if (STATUS_OK)
    status = GetXYSignalXd(xd.pointer, NULL, inXMin ? &xMinD : NULL,
                           inXMax ? &xMaxD : NULL, *reqNSamples, &xd);
  if (STATUS_NOT_OK)
    return (encodeError(MdsGetMsg(status), __LINE__, &xd));
  return &xd;
}

EXPORT mdsdsc_xd_t *GetXYWaveLongTimes(char *inY, int64_t *inXMin,
                                       int64_t *inXMax, int *reqNSamples)
{
  static EMPTYXD(xd);
  const size_t len = strlen(inY);
  if (len == 0)
    return (encodeError("Y data must not be NULL or empty.", __LINE__, &xd));
  mdsdsc_t xMinD = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMin};
  mdsdsc_t xMaxD = {sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)inXMax};
  int status;
  {
    mdsdsc_t expY = {len, DTYPE_T, CLASS_S, inY};
    status = TdiCompile(&expY, &xd MDS_END_ARG);
  }
  if (STATUS_OK)
    status = GetXYSignalXd(xd.pointer, NULL, inXMin ? &xMinD : NULL,
                           inXMax ? &xMaxD : NULL, *reqNSamples, &xd);
  if (STATUS_NOT_OK)
    return (encodeError(MdsGetMsg(status), __LINE__, &xd));
  return &xd;
}
