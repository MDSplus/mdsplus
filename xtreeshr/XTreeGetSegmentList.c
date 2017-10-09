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
#include <mdstypes.h>
#include <xtreeshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <string.h>
#include <stdlib.h>
#include <treeshr.h>

extern unsigned short OpcExtFunction;

//Temporary
//#define TreeGetNumSegments RTreeGetNumSegments
//#define TreeGetSegmentLimits RTreeGetSegmentLimits
//#define TreeGetSegment RTreeGetSegment

//static int timedAccessFlag = 0;

#define MAX_DIMENSION_SIGNAL 16
#define MAX_FUN_NAMELEN 512
extern int TdiData(), TdiEvaluate();

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#endif

extern int TdiDecompile();
extern int TdiExecute();

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

static int check(char *compExpr, struct descriptor *time1Dsc, struct descriptor *time2Dsc, char *answ)
{
  struct descriptor compExprDsc = { strlen(compExpr), DTYPE_T, CLASS_S, compExpr };
  int status;
  EMPTYXD(ansXd);
  status = TdiExecute(&compExprDsc, time1Dsc, time2Dsc, &ansXd MDS_END_ARG);
  if (!(status & 1))
    return status;
  *answ = *ansXd.pointer->pointer;
  MdsFree1Dx(&ansXd, 0);
  return 1;
}
static int checkGreaterOrEqual(struct descriptor *time1Dsc, struct descriptor *time2Dsc, char *answ){
  return check("$ >= $",time1Dsc,time2Dsc,answ);
}
static int checkGreater(struct descriptor *time1Dsc, struct descriptor *time2Dsc, char *answ){
  return check("$ > $",time1Dsc,time2Dsc,answ);
}

static void freeResources(struct descriptor_xd *startTimeXds, struct descriptor_xd *endTimeXds,
			  int numSegments)
{
  int i;
  for (i = 0; i < numSegments; i++) {
    MdsFree1Dx(&startTimeXds[i], 0);
    MdsFree1Dx(&endTimeXds[i], 0);
  }
  free((char *)startTimeXds);
  free((char *)endTimeXds);
}

EXPORT int _XTreeGetSegmentList(void *dbid, int nid, struct descriptor *startDsc,
				struct descriptor *endDsc, struct descriptor_xd *outSignalXd)
{
  int status;
  char isGreater;
  int numSegments;
  int startIdx, endIdx, currIdx, segmentIdx;
  int i;
  EMPTYXD(emptyXd);
  struct descriptor_xd *startTimeXds, *endTimeXds;
  DESCRIPTOR_APD(startTimesApd, DTYPE_SIGNAL, 0, 0);
  DESCRIPTOR_APD(endTimesApd, DTYPE_SIGNAL, 0, 0);
  DESCRIPTOR_A(startTimesArray, 0, 0, 0, 0);
  DESCRIPTOR_A(endTimesArray, 0, 0, 0, 0);
  struct descriptor startIdxDsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)&startIdx };
  struct descriptor endIdxDsc = { sizeof(int), DTYPE_L, CLASS_S, (char *)&endIdx };
  DESCRIPTOR_RANGE(idxDsc, &startIdxDsc, &endIdxDsc, 0);
  DESCRIPTOR_SIGNAL_2(retSignalDsc, (struct descriptor *)&idxDsc, 0, 0, 0);
  char currType;
  int isSigCandidate;
  char *startTimesBuf, *endTimesBuf;

  status =
      (dbid) ? _TreeGetNumSegments(dbid, nid, &numSegments) : TreeGetNumSegments(nid, &numSegments);
  if STATUS_NOT_OK
    return status;

  startTimeXds = (struct descriptor_xd *)malloc(numSegments * sizeof(struct descriptor_xd));
  endTimeXds = (struct descriptor_xd *)malloc(numSegments * sizeof(struct descriptor_xd));
  for (i = 0; i < numSegments; i++) {
    startTimeXds[i] = emptyXd;
    endTimeXds[i] = emptyXd;
  }
  for (currIdx = 0; currIdx < numSegments; currIdx++) {
    status =
	(dbid) ? _TreeGetSegmentLimits(dbid, nid, currIdx, &startTimeXds[currIdx],
				       &endTimeXds[currIdx]) : TreeGetSegmentLimits(nid, currIdx,
										    &startTimeXds
										    [currIdx],
										    &endTimeXds
										    [currIdx]);
    if STATUS_NOT_OK {
      freeResources(startTimeXds, endTimeXds, numSegments);
      return status;
    }
  }

  startIdx = 0;
  currIdx = 0;
  if (!startDsc)		//If no start time specified, take all initial segments
    startIdx = 0;
  else {
    while (startIdx < numSegments) {
      //if(currEnd > start) //First overlapping segment
      status = checkGreaterOrEqual((struct descriptor *)&endTimeXds[startIdx], startDsc, &isGreater);
      if STATUS_NOT_OK {
	freeResources(startTimeXds, endTimeXds, numSegments);
	return status;
      }
      if (isGreater)
	break;
      startIdx++;
    }
  }
  if (startIdx == numSegments)	//All segments antecedent to start time
  {
    freeResources(startTimeXds, endTimeXds, numSegments);
    MdsCopyDxXd((struct descriptor *)&emptyXd, outSignalXd);	//return an empty XD
    return 1;
  }

  if (!endDsc)
    endIdx = numSegments - 1;
  else {
    segmentIdx = startIdx;
    while (segmentIdx < numSegments) {
      status = checkGreaterOrEqual((struct descriptor *)&endTimeXds[segmentIdx], endDsc, &isGreater);
      if STATUS_NOT_OK {
	freeResources(startTimeXds, endTimeXds, numSegments);
	return status;
      }
      //if(currEnd >= end) //Last overlapping segment
      if (isGreater) {
	status = checkGreater((struct descriptor *)&startTimeXds[segmentIdx], endDsc, &isGreater);
	if STATUS_NOT_OK {
	  freeResources(startTimeXds, endTimeXds, numSegments);
	  return status;
	}
	//if(currStart > end) //all the segment lies outside the specifid range, it has to be excluded
	if (isGreater && segmentIdx > startIdx)  segmentIdx--;
	break;
      }
      segmentIdx++;
    }
    if (segmentIdx == numSegments)	//No segment (section) after end
      endIdx = numSegments - 1;
    else
      endIdx = segmentIdx;
  }
  //startIdx and endIdx contain now start and end indexes for valid segments

  //Check if startTimes  are of the same CLASS_S type. If yes, convert them to an array
  currType = startTimeXds[startIdx].pointer->dtype;
  isSigCandidate = 1;
  for (currIdx = startIdx; currIdx <= endIdx; currIdx++) {
    if (!startTimeXds[currIdx].pointer || startTimeXds[currIdx].pointer->class != CLASS_S) {
      isSigCandidate = 0;
      break;
    }
    if (startTimeXds[currIdx].pointer->dtype != currType) {
      isSigCandidate = 0;
      break;
    }
  }

  if (isSigCandidate) {
    startTimesBuf = malloc((endIdx - startIdx + 1) * startTimeXds[startIdx].pointer->length);
    for (currIdx = startIdx; currIdx <= endIdx; currIdx++) {
      memcpy(&startTimesBuf[(currIdx - startIdx) * startTimeXds[startIdx].pointer->length],
	     startTimeXds[currIdx].pointer->pointer, startTimeXds[startIdx].pointer->length);
    }
    startTimesArray.dtype = currType;
    startTimesArray.length = startTimeXds[startIdx].pointer->length;
    startTimesArray.arsize = startTimeXds[startIdx].pointer->length * (endIdx - startIdx + 1);
    startTimesArray.pointer = startTimesBuf;
    retSignalDsc.dimensions[0] = (struct descriptor *)&startTimesArray;
  } else			//build an APD
  {
    startTimesBuf = malloc((endIdx - startIdx + 1) * sizeof(struct descriptor *));
    for (currIdx = startIdx; currIdx <= endIdx; currIdx++) {
      memcpy(&startTimesBuf[(currIdx - startIdx) * sizeof(struct descriptor *)],
	     &startTimeXds[currIdx].pointer, sizeof(struct descriptor *));
    }
    startTimesApd.arsize = sizeof(struct descriptor *) * (endIdx - startIdx + 1);
    startTimesApd.pointer = (struct descriptor **)startTimesBuf;
    retSignalDsc.dimensions[0] = (struct descriptor *)&startTimesApd;
  }

  //The same for endTimes
  currType = endTimeXds[startIdx].pointer->dtype;
  isSigCandidate = 1;
  for (currIdx = startIdx; currIdx <= endIdx; currIdx++) {
    if (!endTimeXds[currIdx].pointer || endTimeXds[currIdx].pointer->class != CLASS_S) {
      isSigCandidate = 0;
      break;
    }
    if (endTimeXds[currIdx].pointer->dtype != currType) {
      isSigCandidate = 0;
      break;
    }
  }

  if (isSigCandidate) {
    endTimesBuf = (char *)malloc((endIdx - startIdx + 1) * endTimeXds[startIdx].pointer->length);
    for (currIdx = startIdx; currIdx <= endIdx; currIdx++) {
      memcpy(&endTimesBuf[(currIdx - startIdx) * endTimeXds[startIdx].pointer->length],
	     endTimeXds[currIdx].pointer->pointer, endTimeXds[startIdx].pointer->length);
    }
    endTimesArray.dtype = currType;
    endTimesArray.length = endTimeXds[startIdx].pointer->length;
    endTimesArray.arsize = endTimeXds[startIdx].pointer->length * (endIdx - startIdx + 1);
    endTimesArray.pointer = endTimesBuf;
    retSignalDsc.dimensions[1] = (struct descriptor *)&endTimesArray;
  } else			//build an APD
  {
    endTimesBuf = malloc((endIdx - startIdx + 1) * sizeof(struct descriptor *));
    for (currIdx = startIdx; currIdx <= endIdx; currIdx++) {
      memcpy(&endTimesBuf[(currIdx - startIdx) * sizeof(struct descriptor *)],
	     &endTimeXds[currIdx].pointer, sizeof(struct descriptor *));
    }
    endTimesApd.arsize = sizeof(struct descriptor *) * (endIdx - startIdx + 1);
    endTimesApd.pointer = (struct descriptor **)endTimesBuf;
    retSignalDsc.dimensions[1] = (struct descriptor *)&endTimesApd;
  }

  MdsCopyDxXd((struct descriptor *)&retSignalDsc, (struct descriptor_xd *)outSignalXd);
  //Free stuff
  freeResources(startTimeXds, endTimeXds, numSegments);
  free(startTimesBuf);
  free(endTimesBuf);
  return 1;
}

EXPORT int XTreeGetSegmentList(int nid, struct descriptor *startD, struct descriptor *endD,
			       struct descriptor_xd *outSignal)
{
  return _XTreeGetSegmentList(0, nid, startD, endD, outSignal);
}
