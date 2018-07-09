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
#include <tdishr.h>
#include <treeshr.h>

extern unsigned short OpcExtFunction;

//Temporary
//#define TreeGetNumSegments RTreeGetNumSegments
//#define TreeGetSegmentLimits RTreeGetSegmentLimits
//#define TreeGetSegment RTreeGetSegment

static int timedAccessFlag = 0;

#define MAX_FUN_NAMELEN 512

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#endif
extern int XTreeConvertToLongTime(struct descriptor *timeD,
				  int64_t * converted);
extern int XTreeConvertToLongDelta(struct descriptor *deltaD,
				   int64_t * converted);
extern int XTreeMinMaxResample(struct descriptor_signal *inSignalD,
			       struct descriptor *startD,
			       struct descriptor *endD,
			       struct descriptor *deltaD,
			       struct descriptor_xd *outSignalXd);

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

EXPORT void XTreeResetTimedAccessFlag()
{
  timedAccessFlag = 0;
}

EXPORT int XTreeTestTimedAccessFlag()
{
  return timedAccessFlag;
}


//Check if resampled versions of this node exist in case a sampling interval is specified
static int checkResampledVersion(void *dbid, int nid, struct descriptor *deltaD)
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


	if(deltaD == 0)
	    return nid;
	status = XTreeConvertToLongDelta(deltaD, &deltaNs);
	if STATUS_NOT_OK
		return nid;
	status = (dbid) ? _TreeGetXNci(dbid, nid, "ResampleFactor", &xd) : TreeGetXNci(nid, "ResampleFactor", &xd);
	if STATUS_NOT_OK
		return nid;
	status = TdiGetLong((struct descriptor *)&xd,&resampleFactor);
	/*	status = TdiData(&xd, &xd MDS_END_ARG);
	if(status & 1)
		status = TdiLong(&xd, &xd MDS_END_ARG);
	if(!(status & 1))
	{
		MdsFree1Dx(&xd, 0);
		return nid;
	}
	resampleFactor = *(int *)xd.pointer->pointer;
	*/
	MdsFree1Dx(&xd, 0);

	status = (dbid)?_TreeGetSegmentLimits(dbid, nid, 0, &startXd, &endXd):TreeGetSegmentLimits(nid, 0, &startXd, &endXd);
	if STATUS_NOT_OK
		return nid;
	status = XTreeConvertToLongTime(startXd.pointer, &startNs);
	if STATUS_OK status = XTreeConvertToLongTime(endXd.pointer, &endNs);
	MdsFree1Dx(&startXd, 0);
	MdsFree1Dx(&endXd, 0);
	if STATUS_NOT_OK
		return nid;

	status = (dbid)?_TreeGetSegmentInfo(dbid, nid, 0, &dtype, &dimct, dims, &nextRow):TreeGetSegmentInfo(nid, 0, &dtype, &dimct, dims, &nextRow);
	if STATUS_NOT_OK
		return nid;
	numRows = dims[dimct-1];
	actDeltaNs = (endNs - startNs)/numRows;
	if(actDeltaNs <= 0)
                return nid;
	if((int)(deltaNs / actDeltaNs) < resampleFactor)
		return nid;
	status = (dbid) ? _TreeGetXNci(dbid, nid, "ResampleNid", &xd) : TreeGetXNci(nid, "ResampleNid", &xd);
  	if (STATUS_NOT_OK || !xd.pointer || xd.pointer->class != CLASS_S || xd.pointer->dtype != DTYPE_NID)
  	{
		MdsFree1Dx(&xd, 0);
		return nid;
	}
	outNid = *(int *)xd.pointer->pointer;
	MdsFree1Dx(&xd, 0);
	return outNid;
}

EXPORT int _XTreeGetTimedRecord(void *dbid, int inNid, struct descriptor *startD,
				struct descriptor *endD, struct descriptor *minDeltaD,
				struct descriptor_xd *outSignal)
{
  int status, nid;
  int actNumSegments, currSegIdx, numSegments, currIdx, numDimensions;
  int i, nameLen, startIdx, endIdx;
  char resampleFunName[MAX_FUN_NAMELEN], squishFunName[MAX_FUN_NAMELEN];
  char resampleMode[MAX_FUN_NAMELEN];
  EMPTYXD(startTimesXd);
  EMPTYXD(endTimesXd);
  int64_t *startTimes, *endTimes, start, end;

  struct descriptor resampleFunNameD = { 0, DTYPE_T, CLASS_S, resampleFunName };
  struct descriptor squishFunNameD = { 0, DTYPE_T, CLASS_S, squishFunName };
  DESCRIPTOR_R(resampleFunD, DTYPE_FUNCTION, 6);
  DESCRIPTOR_R(squishFunD, DTYPE_FUNCTION, 6);

  struct descriptor_a *startTimesApd;
  struct descriptor_a *endTimesApd;
  struct descriptor_a *currApd;
  EMPTYXD(xd);
  EMPTYXD(emptyXd);
  struct descriptor_xd *resampledXds;
  struct descriptor_xd *dataXds;
  struct descriptor_xd *dimensionXds;

  DESCRIPTOR_SIGNAL(currSignalD, MAX_DIMS, 0, 0);
  DESCRIPTOR_APD(signalsApd, DTYPE_SIGNAL, 0, 0);
  struct descriptor_signal **signals;

//printf("GET TIMED RECORD\n");

  //Check for possible resampled versions
  nid = checkResampledVersion(dbid, inNid, minDeltaD);

  timedAccessFlag = 1;
  //Get names for (possible) user defined  resample and squish funs
  status =
      (dbid) ? _TreeGetXNci(dbid, nid, "ResampleFun", &xd) : TreeGetXNci(nid, "ResampleFun", &xd);
  if (STATUS_OK && xd.pointer)	//If a user defined fun exists
  {
    nameLen = xd.pointer->length;
    if (nameLen >= MAX_FUN_NAMELEN)
      nameLen = MAX_FUN_NAMELEN - 1;
    memcpy(resampleFunName, xd.pointer->pointer, nameLen);
    resampleFunName[nameLen] = 0;
    MdsFree1Dx(&xd, 0);
  } else
    resampleFunName[0] = 0;

  status = (dbid) ? _TreeGetXNci(dbid, nid, "SquishFun", &xd) : TreeGetXNci(nid, "SquishFun", &xd);
  if (STATUS_OK & 1 && xd.pointer)	//If a user defined fun exists
  {
    nameLen = xd.pointer->length;
    if (nameLen >= MAX_FUN_NAMELEN)
      nameLen = MAX_FUN_NAMELEN - 1;
    memcpy(squishFunName, xd.pointer->pointer, nameLen);
    squishFunName[nameLen] = 0;
    MdsFree1Dx(&xd, 0);
  } else
    squishFunName[0] = 0;

	status =
		(dbid) ? _TreeGetXNci(dbid, nid, "ResampleMode", &xd) : TreeGetXNci(nid, "ResampleMode", &xd);
	if (STATUS_OK && xd.pointer)	//If a user defined fun exists
	{
		nameLen = xd.pointer->length;
		if (nameLen >= MAX_FUN_NAMELEN)
		nameLen = MAX_FUN_NAMELEN - 1;
		memcpy(resampleMode, xd.pointer->pointer, nameLen);
		resampleMode[nameLen] = 0;
		MdsFree1Dx(&xd, 0);
  	} else
    resampleMode[0] = 0;

//Evaluate start, end to int64
	if(startD)
	{
		status = XTreeConvertToLongTime(startD, &start);
		if STATUS_NOT_OK return status;
	}
	if(endD)
	{
		status = XTreeConvertToLongTime(endD, &end);
		if STATUS_NOT_OK return status;
	}

//Get segment limits. If not evaluated to 64 bit int, make the required conversion.
// New management based on TreeGetSegmentLimits()
	status = (dbid)?_TreeGetSegmentTimesXd(dbid, nid, &numSegments, &startTimesXd, &endTimesXd):TreeGetSegmentTimesXd(nid, &numSegments, &startTimesXd, &endTimesXd);
	if STATUS_NOT_OK return status;
	// Convert read times into 64 bit representation
	if(startTimesXd.pointer == 0 || endTimesXd.pointer == 0)
		return 0; //Internal error
	startTimesApd = (struct descriptor_a *)startTimesXd.pointer;
	endTimesApd = (struct descriptor_a *)endTimesXd.pointer;
	if((int)(startTimesApd->arsize/startTimesApd->length) != numSegments)
		return 0; //Internal error
	if((int)(endTimesApd->arsize/endTimesApd->length) != numSegments)
		return 0; //Internal error
	startTimes = (int64_t *)malloc(numSegments * sizeof(int64_t));
	endTimes = (int64_t *)malloc(numSegments * sizeof(int64_t));
	for(currSegIdx = 0; currSegIdx < numSegments; currSegIdx++)
	{
		status = XTreeConvertToLongTime(((struct descriptor **)(startTimesApd->pointer))[currSegIdx], &startTimes[currSegIdx]);
		if STATUS_NOT_OK return status;
		status = XTreeConvertToLongTime(((struct descriptor **)(endTimesApd->pointer))[currSegIdx], &endTimes[currSegIdx]);
		if STATUS_NOT_OK return status;
	}
	MdsFree1Dx(&startTimesXd, 0);
	MdsFree1Dx(&endTimesXd, 0);

	if(!startD)     //If no start time specified, take all initial segments
		startIdx = 0;
	else // find first overlapping segment
	    for(startIdx = 0 ; startIdx < numSegments && endTimes[startIdx] < start ; startIdx++);
	if(startIdx == numSegments) {
		MdsCopyDxXd((struct descriptor *)&emptyXd, outSignal);	//return an empty XD
		return 1;
	}
	if(!endD)
		endIdx = numSegments - 1;
	else {
		for (currSegIdx = startIdx; currSegIdx < numSegments ; currSegIdx++) {
			if(endTimes[currSegIdx] >= end) { // Last overlapping segment
				if(startTimes[currSegIdx] > end && currSegIdx > startIdx) currSegIdx--;
				break;
			}
		}
		endIdx = currSegIdx == numSegments ? currSegIdx-1 : currSegIdx; //No segment (section) after end
	}
	actNumSegments = endIdx - startIdx +1;

	free((char *)startTimes);
	free((char *)endTimes);

	signals = (struct descriptor_signal **)malloc(actNumSegments * sizeof(struct descriptor_signal *));
	signalsApd.pointer = (struct descriptor **)signals;
	signalsApd.arsize = actNumSegments * sizeof(struct descriptor_signal *);
	resampledXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	dataXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	dimensionXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	for(i = 0; i < actNumSegments; i++) {
		resampledXds[i] = emptyXd;
		dataXds[i] = emptyXd;
		dimensionXds[i] = emptyXd;
	}

	for(currIdx = startIdx, currSegIdx = 0; currIdx <= endIdx; currIdx++, currSegIdx++)
	{
		status = (dbid)?_TreeGetSegment(dbid, nid, currIdx, &dataXds[currSegIdx], &dimensionXds[currSegIdx]):TreeGetSegment(nid, currIdx, &dataXds[currSegIdx], &dimensionXds[currSegIdx]);

//printf("Read Segment %d\n", currSegIdx);

		if STATUS_NOT_OK
		{
			free((char *)signals);
			for(i = 0; i < actNumSegments; i++)
			{
				MdsFree1Dx(&resampledXds[i], 0);
				MdsFree1Dx(&dataXds[i], 0);
				MdsFree1Dx(&dimensionXds[i], 0);
			}
			free((char *)resampledXds);
			free((char *)dataXds);
			free((char *)dimensionXds);
			return status;
		}

		//Check if returned dimension is an APD for multidimensional dimensions
		if(dimensionXds[currSegIdx].pointer->class == CLASS_APD)
		{
			currApd = (struct descriptor_a *)dimensionXds[currSegIdx].pointer;
			numDimensions = currApd->arsize / currApd->length;
			currSignalD.ndesc = 2 + numDimensions;
			for(i = 0; i < numDimensions; i++)
				currSignalD.dimensions[i] = ((struct descriptor **)currApd->pointer)[i];
		}
		else
		{
			currSignalD.ndesc = 3;
			currSignalD.dimensions[0] = dimensionXds[currSegIdx].pointer;
		}
		currSignalD.data = dataXds[currSegIdx].pointer;

//If defined, call User Provided resampling function, oterwise use default one (XTreeDefaultResample())
		if(resampleFunName[0])
		{
			unsigned short funCode = 162; // OpcExtFunction
			resampleFunD.length = sizeof(unsigned short);
			resampleFunD.pointer = (unsigned char *)&funCode;
			resampleFunNameD.length = strlen(resampleFunName);
			resampleFunNameD.pointer = resampleFunName;
			resampleFunD.dscptrs[0] = 0;
			resampleFunD.dscptrs[1] = &resampleFunNameD;
			resampleFunD.dscptrs[2] = (struct descriptor *)&currSignalD;
			resampleFunD.dscptrs[3] = startD;
			resampleFunD.dscptrs[4] = endD;
			resampleFunD.dscptrs[5] = minDeltaD;
			status = TdiEvaluate((struct descriptor *)&resampleFunD, &resampledXds[currSegIdx] MDS_END_ARG);
//			printf("%s\n", MdsGetMsg(status));
		}
		else
		{
			if(!startD && !endD && !minDeltaD)  //If no resampling required
			{
				status = MdsCopyDxXd((const struct descriptor *)&currSignalD, &resampledXds[currSegIdx]);
			}
			else
			{
				if(!strcmp(resampleMode, "MinMax")){
	      			        status = XTreeMinMaxResample((struct descriptor_signal *)&currSignalD, startD, endD,
					                              minDeltaD, &resampledXds[currSegIdx]);
				} else {
	      			        status = XTreeDefaultResample((struct descriptor_signal *)&currSignalD, startD, endD,
					                               minDeltaD, &resampledXds[currSegIdx]);
				}
			}
		}

//printDecompiled(resampledXds[currSegIdx].pointer);
//scanf("%d", &i);

		if STATUS_NOT_OK
		{
			free((char *)signals);
			for(i = 0; i < actNumSegments; i++)
			{
				MdsFree1Dx(&resampledXds[i], 0);
				MdsFree1Dx(&dataXds[i], 0);
				MdsFree1Dx(&dimensionXds[i], 0);
			}
			free((char *)resampledXds);
			free((char *)dataXds);
			free((char *)dimensionXds);
			return status;
		}

		MdsFree1Dx(&dataXds[currSegIdx], 0);
		MdsFree1Dx(&dimensionXds[currSegIdx], 0);
		signals[currSegIdx] = (struct descriptor_signal *)resampledXds[currSegIdx].pointer;
	}

/****************************/
//Now signalsApd contains the list of selected resampled signals. If User squish fun defined, call it
//Otherwise call default Squish fun XTreeDefaultSquish()

  if (squishFunName[0]) {
    unsigned short funCode = OpcExtFunction;
    squishFunD.length = sizeof(unsigned short);
    squishFunD.pointer = (unsigned char *)&funCode;
    squishFunNameD.length = strlen(squishFunName);
    squishFunNameD.pointer = squishFunName;
    squishFunD.dscptrs[0] = 0;
    squishFunD.dscptrs[1] = &squishFunNameD;
    squishFunD.dscptrs[2] = (struct descriptor *)&signalsApd;
    squishFunD.dscptrs[3] = startD;
    squishFunD.dscptrs[4] = endD;
    squishFunD.dscptrs[5] = minDeltaD;

    status = TdiEvaluate((struct descriptor *)&squishFunD, outSignal MDS_END_ARG);
  } else
    status =
	XTreeDefaultSquish((struct descriptor_a *)&signalsApd, startD, endD, minDeltaD, outSignal);

//Free stuff
  free((char *)signals);
  for (i = 0; i < actNumSegments; i++) {
    MdsFree1Dx(&resampledXds[i], NULL);
  }
  free((char *)resampledXds);
  free((char *)dataXds);
  free((char *)dimensionXds);
  if (outSignal->pointer && (_TreeGetSegmentScale(dbid, inNid, &xd)&1)){
    if (xd.pointer) {
      struct descriptor_signal* sig = (struct descriptor_signal*)outSignal->pointer;
      emptyXd = *outSignal;
      outSignal->pointer = NULL;
      sig->raw  = sig->data;
      sig->data = xd.pointer;
      MdsCopyDxXd((struct descriptor*)sig,outSignal);
      MdsFree1Dx(&xd,NULL);
      MdsFree1Dx(&emptyXd,NULL);
    }
  }
  return status;
}

EXPORT int XTreeGetTimedRecord(int nid, struct descriptor *startD, struct descriptor *endD,
			       struct descriptor *minDeltaD, struct descriptor_xd *outSignal)
{
  return _XTreeGetTimedRecord(0, nid, startD, endD, minDeltaD, outSignal);
}
