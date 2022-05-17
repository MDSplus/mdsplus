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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr.h>
#include <treeshr.h>
#include <xtreeshr.h>

static int timedAccessFlag = 0;

#define MAX_FUN_NAMELEN 512

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#endif
extern int XTreeConvertToLongTime(mdsdsc_t *timeD, int64_t *converted);
extern int XTreeConvertToDouble(mdsdsc_t *timeD, double *converted);
#define RESAMPLE_FUN(name)                                            \
  int name(mds_signal_t *inSignalD, mdsdsc_t *startD, mdsdsc_t *endD, \
           mdsdsc_t *deltaD, mdsdsc_xd_t *outSignalXd)
extern RESAMPLE_FUN(XTreeAverageResample);
extern RESAMPLE_FUN(XTreeMinMaxResample);
extern RESAMPLE_FUN(XTreeInterpResample);
extern RESAMPLE_FUN(XTreeClosestResample);
extern RESAMPLE_FUN(XTreePreviousResample);
extern RESAMPLE_FUN(XTreeDefaultResample);

EXPORT void XTreeResetTimedAccessFlag() { timedAccessFlag = 0; }

EXPORT int XTreeTestTimedAccessFlag() { return timedAccessFlag; }

// Check if resampled versions of this node exist in case a sampling interval is
// specified
static int checkResampledVersion(int nid, mdsdsc_t *deltaD)
{
  EMPTYXD(xd);
  EMPTYXD(startXd);
  EMPTYXD(endXd);
  int status, outNid;
  int resampleFactor;
  int64_t actDeltaNs;
  int64_t deltaNs, startNs, endNs;
  char dtype, dimct;
  int dims[16];
  int nextRow;
  int numRows;

  if (deltaD == 0)
    return nid;
  status = XTreeConvertToLongTime(deltaD, &deltaNs);
  if (STATUS_NOT_OK)
    return nid;
  status = TreeGetXNci(nid, "ResampleFactor", &xd);
  if (STATUS_NOT_OK)
    return nid;
  status = TdiGetLong((mdsdsc_t *)&xd, &resampleFactor);

  MdsFree1Dx(&xd, 0);

  status = TreeGetSegmentLimits(nid, 0, &startXd, &endXd);
  if (STATUS_NOT_OK)
    return nid;
  status = XTreeConvertToLongTime(startXd.pointer, &startNs);
  if (STATUS_OK)
    status = XTreeConvertToLongTime(endXd.pointer, &endNs);
  MdsFree1Dx(&startXd, 0);
  MdsFree1Dx(&endXd, 0);
  if (STATUS_NOT_OK)
    return nid;

  status = TreeGetSegmentInfo(nid, 0, &dtype, &dimct, dims, &nextRow);
  if (STATUS_NOT_OK)
    return nid;
  //  numRows = dims[dimct-1];
  numRows = nextRow;
  actDeltaNs = (endNs - startNs) / numRows;

  if (actDeltaNs <= 0)
    return nid;

  if ((int)(deltaNs / actDeltaNs) < resampleFactor)
    return nid;
  status = TreeGetXNci(nid, "ResampleNid", &xd);
  if (STATUS_NOT_OK || !xd.pointer || xd.pointer->class != CLASS_S ||
      xd.pointer->dtype != DTYPE_NID)
  {
    MdsFree1Dx(&xd, 0);
    return nid;
  }
  outNid = *(int *)xd.pointer->pointer;
  MdsFree1Dx(&xd, 0);
  return outNid;
}

EXPORT int XTreeGetTimedRecord(int inNid, mdsdsc_t *inStartD, mdsdsc_t *inEndD,
                               mdsdsc_t *inMinDeltaD, mdsdsc_xd_t *outSignal)
{
  int status, nid;
  int actNumSegments, currSegIdx, nonEmptySegIdx, numSegments, currIdx,
      numDimensions;
  int i, nameLen, startIdx, endIdx;
  char resampleFunName[MAX_FUN_NAMELEN], squishFunName[MAX_FUN_NAMELEN];
  char resampleMode[MAX_FUN_NAMELEN];
  EMPTYXD(startTimesXd);
  EMPTYXD(endTimesXd);
  double *startTimes, *endTimes, start, end;

  
  //Start, End, Delta MUST be copied becuse they may become invalid in case TreeSetTimeContext is internally called (e.g. in DefaultResample)
  EMPTYXD(startXd);
  EMPTYXD(endXd);
  EMPTYXD(minDeltaXd);
  mdsdsc_t *startD, *endD, *minDeltaD;
  if (inStartD)
  {
    MdsCopyDxXd(inStartD, &startXd);
    startD = startXd.pointer;
  }
  else
    startD = NULL;
  if (inEndD)
  {
    MdsCopyDxXd(inEndD, &endXd);
    endD = endXd.pointer;
  }
  else
    endD = NULL;
  if (inMinDeltaD)
  {
    MdsCopyDxXd(inMinDeltaD, &minDeltaXd);
    minDeltaD = minDeltaXd.pointer;
  }
  else
    minDeltaD = NULL;
  
  
  
  mdsdsc_t resampleFunNameD = {0, DTYPE_T, CLASS_S, resampleFunName};
  mdsdsc_t squishFunNameD = {0, DTYPE_T, CLASS_S, squishFunName};
  DESCRIPTOR_R(resampleFunD, DTYPE_FUNCTION, 6);
  DESCRIPTOR_R(squishFunD, DTYPE_FUNCTION, 6);

  struct descriptor_a *startTimesApd;
  struct descriptor_a *endTimesApd;
  struct descriptor_a *currApd;
  EMPTYXD(xd);
  EMPTYXD(emptyXd);
  mdsdsc_xd_t *resampledXds;
  mdsdsc_xd_t *dataXds;
  mdsdsc_xd_t *dimensionXds;

  DESCRIPTOR_SIGNAL(currSignalD, MAX_DIMS, 0, 0);
  DESCRIPTOR_APD(signalsApd, DTYPE_SIGNAL, 0, 0);
  mds_signal_t **signals;

  // Check for possible resampled versions
  nid = checkResampledVersion(inNid, minDeltaD);

  timedAccessFlag = 1;
  // Get names for (possible) user defined  resample and squish funs
  status = TreeGetXNci(nid, "ResampleFun", &xd);
  if (STATUS_OK && xd.pointer)
  { // If a user defined fun exists
    nameLen = xd.pointer->length;
    if (nameLen >= MAX_FUN_NAMELEN)
      nameLen = MAX_FUN_NAMELEN - 1;
    memcpy(resampleFunName, xd.pointer->pointer, nameLen);
    resampleFunName[nameLen] = 0;
    MdsFree1Dx(&xd, 0);
  }
  else
    resampleFunName[0] = 0;

  status = TreeGetXNci(nid, "SquishFun", &xd);
  if (STATUS_OK & 1 && xd.pointer)
  { // If a user defined fun exists
    nameLen = xd.pointer->length;
    if (nameLen >= MAX_FUN_NAMELEN)
      nameLen = MAX_FUN_NAMELEN - 1;
    memcpy(squishFunName, xd.pointer->pointer, nameLen);
    squishFunName[nameLen] = 0;
    MdsFree1Dx(&xd, 0);
  }
  else
    squishFunName[0] = 0;

  status = TreeGetXNci(nid, "ResampleMode", &xd);
  if (STATUS_OK && xd.pointer)
  { // If a user defined fun exists
    nameLen = xd.pointer->length;
    if (nameLen >= MAX_FUN_NAMELEN)
      nameLen = MAX_FUN_NAMELEN - 1;
    memcpy(resampleMode, xd.pointer->pointer, nameLen);
    resampleMode[nameLen] = 0;
    MdsFree1Dx(&xd, 0);
  }
  else
    resampleMode[0] = 0;

  // Get segment limits. If not evaluated to 64 bit int, make the required
  // conversion.
  // New management based on TreeGetSegmentLimits()
  status = TreeGetSegmentTimesXd(nid, &numSegments, &startTimesXd, &endTimesXd);
  if (STATUS_NOT_OK)
    return status;
  // Convert read times into 64 bit representation
  if (startTimesXd.pointer == 0 || endTimesXd.pointer == 0)
    return 0; // Internal error
  startTimesApd = (struct descriptor_a *)startTimesXd.pointer;
  endTimesApd = (struct descriptor_a *)endTimesXd.pointer;

  status = TdiData(*(mdsdsc_t **)startTimesApd->pointer, &xd MDS_END_ARG);
  if (STATUS_NOT_OK)
    return status;
  MdsFree1Dx(&xd, 0);
  // Evaluate start, end to int64
  if (startD)
  {
    status = XTreeConvertToDouble(startD, &start);
    if (STATUS_NOT_OK)
      return status;
  }
  if (endD)
  {
    status = XTreeConvertToDouble(endD, &end);
    if (STATUS_NOT_OK)
      return status;
  }

  if ((int)(startTimesApd->arsize / startTimesApd->length) != numSegments)
    return 0; // Internal error
  if ((int)(endTimesApd->arsize / endTimesApd->length) != numSegments)
    return 0; // Internal error
  startTimes = (double *)malloc(numSegments * sizeof(double));
  endTimes = (double *)malloc(numSegments * sizeof(double));
  for (currSegIdx = 0; currSegIdx < numSegments; currSegIdx++)
  {
    status = XTreeConvertToDouble(
        ((mdsdsc_t **)(startTimesApd->pointer))[currSegIdx],
        &startTimes[currSegIdx]);
    if (STATUS_NOT_OK)
      return status;
    status =
        XTreeConvertToDouble(((mdsdsc_t **)(endTimesApd->pointer))[currSegIdx],
                             &endTimes[currSegIdx]);
    if (STATUS_NOT_OK)
      return status;
  }
  MdsFree1Dx(&startTimesXd, 0);
  MdsFree1Dx(&endTimesXd, 0);

  if (!startD) // If no start time specified, take all initial segments
    startIdx = 0;
  else // find first overlapping segment
    for (startIdx = 0; startIdx < numSegments && endTimes[startIdx] < start;
         startIdx++)
      ;
  if (startIdx == numSegments)
  {
    MdsCopyDxXd((mdsdsc_t *)&emptyXd, outSignal); // return an empty XD
    return 1;
  }
  if (!endD)
    endIdx = numSegments - 1;
  else
  {
    for (currSegIdx = startIdx; currSegIdx < numSegments; currSegIdx++)
    {
      if (endTimes[currSegIdx] >= end)
      { // Last overlapping segment
        if (startTimes[currSegIdx] > end && currSegIdx > startIdx)
          currSegIdx--;
        break;
      }
    }
    endIdx = currSegIdx == numSegments
                 ? currSegIdx - 1
                 : currSegIdx; // No segment (section) after end
  }
  actNumSegments = endIdx - startIdx + 1;

  free(startTimes);
  free(endTimes);

  signals = (mds_signal_t **)malloc(actNumSegments * sizeof(mds_signal_t *));
  signalsApd.pointer = (mdsdsc_t **)signals;
  signalsApd.arsize = actNumSegments * sizeof(mds_signal_t *);
  resampledXds = (mdsdsc_xd_t *)malloc(actNumSegments * sizeof(mdsdsc_xd_t));
  dataXds = (mdsdsc_xd_t *)malloc(actNumSegments * sizeof(mdsdsc_xd_t));
  dimensionXds = (mdsdsc_xd_t *)malloc(actNumSegments * sizeof(mdsdsc_xd_t));
  for (i = 0; i < actNumSegments; i++)
  {
    resampledXds[i] = emptyXd;
    dataXds[i] = emptyXd;
    dimensionXds[i] = emptyXd;
  }

  for (currIdx = startIdx, currSegIdx = 0, nonEmptySegIdx = 0;
       currIdx <= endIdx; currIdx++, currSegIdx++)
  {
    status = TreeGetSegment(nid, currIdx, &dataXds[currSegIdx],
                            &dimensionXds[currSegIdx]);
    // decompress if compressed
    if (STATUS_OK && dataXds[currSegIdx].pointer->class == CLASS_CA)
    {
      mdsdsc_xd_t compressed = dataXds[currSegIdx];
      dataXds[currSegIdx] = emptyXd;
      status = MdsDecompress((struct descriptor_r *)compressed.pointer,
                             &dataXds[currSegIdx]);
      MdsFree1Dx(&compressed, 0);
    }
    if (STATUS_NOT_OK)
    {
      free(signals);
      for (i = 0; i < actNumSegments; i++)
      {
        MdsFree1Dx(&resampledXds[i], 0);
        MdsFree1Dx(&dataXds[i], 0);
        MdsFree1Dx(&dimensionXds[i], 0);
      }
      free(resampledXds);
      free(dataXds);
      free(dimensionXds);
      return status;
    }

    // Check if returned dimension is an APD for multidimensional dimensions
    if (!dimensionXds[currSegIdx].pointer)
    {
      currSignalD.ndesc = 1;
      currSignalD.dimensions[0] = NULL;
    }
    else if (dimensionXds[currSegIdx].pointer->class == CLASS_APD)
    {
      currApd = (struct descriptor_a *)dimensionXds[currSegIdx].pointer;
      numDimensions = currApd->arsize / currApd->length;
      currSignalD.ndesc = 2 + numDimensions;
      for (i = 0; i < numDimensions; i++)
        currSignalD.dimensions[i] = ((mdsdsc_t **)currApd->pointer)[i];
    }
    else
    {
      currSignalD.ndesc = 3;
      currSignalD.dimensions[0] = dimensionXds[currSegIdx].pointer;
    }
    currSignalD.data = dataXds[currSegIdx].pointer;

    // If defined, call User Provided resampling function, oterwise use default
    // one (XTreeDefaultResample())
    if (resampleFunName[0])
    {
      resampleFunD.length = sizeof(unsigned short);
      resampleFunD.pointer = (unsigned char *)&OpcExtFunction;
      resampleFunNameD.length = strlen(resampleFunName);
      resampleFunNameD.pointer = resampleFunName;
      resampleFunD.dscptrs[0] = 0;
      resampleFunD.dscptrs[1] = &resampleFunNameD;
      resampleFunD.dscptrs[2] = (mdsdsc_t *)&currSignalD;
      resampleFunD.dscptrs[3] = startD;
      resampleFunD.dscptrs[4] = endD;
      resampleFunD.dscptrs[5] = minDeltaD;
      status = TdiEvaluate((mdsdsc_t *)&resampleFunD,
                           &resampledXds[currSegIdx] MDS_END_ARG);
    }
    else if ((!startD && !endD && !minDeltaD) ||
             currSignalD.ndesc == 1) // If no resampling required
      status = MdsCopyDxXd((const mdsdsc_t *)&currSignalD,
                           &resampledXds[currSegIdx]);
    else if (!strcasecmp(resampleMode, "Average"))
      status = XTreeAverageResample((mds_signal_t *)&currSignalD, startD, endD,
                                    minDeltaD, &resampledXds[currSegIdx]);
    else if (!strcasecmp(resampleMode, "MinMax"))
      status = XTreeMinMaxResample((mds_signal_t *)&currSignalD, startD, endD,
                                   minDeltaD, &resampledXds[currSegIdx]);
    else if (!strcasecmp(resampleMode, "Interp"))
      status = XTreeInterpResample((mds_signal_t *)&currSignalD, startD, endD,
                                   minDeltaD, &resampledXds[currSegIdx]);
    else if (!strcasecmp(resampleMode, "Closest"))
      status = XTreeClosestResample((mds_signal_t *)&currSignalD, startD, endD,
                                    minDeltaD, &resampledXds[currSegIdx]);
    else if (!strcasecmp(resampleMode, "Previous"))
      status = XTreePreviousResample((mds_signal_t *)&currSignalD, startD, endD,
                                     minDeltaD, &resampledXds[currSegIdx]);
    else
      status = XTreeDefaultResample((mds_signal_t *)&currSignalD, startD, endD,
                                    minDeltaD, &resampledXds[currSegIdx]);

    if (STATUS_NOT_OK)
    {
      free(signals);
      for (i = 0; i < actNumSegments; i++)
      {
        MdsFree1Dx(&resampledXds[i], 0);
        MdsFree1Dx(&dataXds[i], 0);
        MdsFree1Dx(&dimensionXds[i], 0);
      }
      free(resampledXds);
      free(dataXds);
      free(dimensionXds);
      return status;
    }

    MdsFree1Dx(&dataXds[currSegIdx], 0);
    MdsFree1Dx(&dimensionXds[currSegIdx], 0);
    // After resampling some segments may be empty: consider only nonempty
    // segments
    if (resampledXds[currSegIdx].l_length > 0)
      signals[nonEmptySegIdx++] =
          (mds_signal_t *)resampledXds[currSegIdx].pointer;
  }
  // Update arsize to keep trask of empty segments
  signalsApd.arsize = nonEmptySegIdx * sizeof(mds_signal_t *);

  /****************************/
  // Now signalsApd contains the list of selected resampled signals. If User
  // squish fun defined, call it Otherwise call default Squish fun
  // XTreeDefaultSquish()

  if (squishFunName[0])
  {
    unsigned short funCode = OpcExtFunction;
    squishFunD.length = sizeof(unsigned short);
    squishFunD.pointer = (unsigned char *)&funCode;
    squishFunNameD.length = strlen(squishFunName);
    squishFunNameD.pointer = squishFunName;
    squishFunD.dscptrs[0] = 0;
    squishFunD.dscptrs[1] = &squishFunNameD;
    squishFunD.dscptrs[2] = (mdsdsc_t *)&signalsApd;
    squishFunD.dscptrs[3] = startD;
    squishFunD.dscptrs[4] = endD;
    squishFunD.dscptrs[5] = minDeltaD;

    status = TdiEvaluate((mdsdsc_t *)&squishFunD, outSignal MDS_END_ARG);
  }
  else
    status = XTreeDefaultSquish((struct descriptor_a *)&signalsApd, startD,
                                endD, minDeltaD, outSignal);

  // Free stuff
  free(signals);
  for (i = 0; i < actNumSegments; i++)
  {
    MdsFree1Dx(&resampledXds[i], NULL);
  }
  free(resampledXds);
  free(dataXds);
  free(dimensionXds);
  if (outSignal->pointer && (TreeGetSegmentScale(inNid, &xd) & 1))
  {
    if (xd.pointer)
    {
      mds_signal_t *sig = (mds_signal_t *)outSignal->pointer;
      emptyXd = *outSignal;
      outSignal->pointer = NULL;
      sig->raw = sig->data;
      sig->data = xd.pointer;
      MdsCopyDxXd((struct descriptor *)sig, outSignal);
      MdsFree1Dx(&xd, NULL);
      MdsFree1Dx(&emptyXd, NULL);
    }
  }
  MdsFree1Dx(&startXd, NULL);
  MdsFree1Dx(&endXd, NULL);
  MdsFree1Dx(&minDeltaXd, NULL);

  return status;
}

EXPORT int _XTreeGetTimedRecord(void *dbid, int nid, mdsdsc_t *startD,
                                mdsdsc_t *endD, mdsdsc_t *minDeltaD,
                                mdsdsc_xd_t *outSignal)
{
  int status;
  CTX_PUSH(&dbid);
  status = XTreeGetTimedRecord(nid, startD, endD, minDeltaD, outSignal);
  CTX_POP(&dbid);
  return status;
}
