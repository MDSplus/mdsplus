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


static int timedAccessFlag = 0;


#define MAX_DIMENSION_SIGNAL 16
#define MAX_FUN_NAMELEN 512
extern int TdiData(), TdiEvaluate();

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#endif

extern int TdiDecompile();


static void printDecompiled(struct descriptor *inD)
{
	int status;
	EMPTYXD(out_xd);
	char *buf;
	extern int TdiDecompile();

	status = TdiDecompile(inD, &out_xd MDS_END_ARG);
	if(!(status & 1))
	{
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


EXPORT void XTreeResetTimedAccessFlag() {timedAccessFlag = 0; }
EXPORT int XTreeTestTimedAccessFlag() {return timedAccessFlag;}

EXPORT int _XTreeGetTimedRecord(void *dbid, int nid, struct descriptor *startD, struct descriptor *endD, struct descriptor *minDeltaD,
						struct descriptor_xd *outSignal)
{
	int status;
	int actNumSegments, currSegIdx;
	int i, nameLen;
	char resampleFunName[MAX_FUN_NAMELEN], squishFunName[MAX_FUN_NAMELEN];

	struct descriptor resampleFunNameD = {0, DTYPE_T, CLASS_S, resampleFunName};
	struct descriptor squishFunNameD = {0, DTYPE_T, CLASS_S, squishFunName};
	DESCRIPTOR_R(resampleFunD, DTYPE_FUNCTION, 6);
	DESCRIPTOR_R(squishFunD, DTYPE_FUNCTION, 6);

	struct descriptor_a *segmentsApd;
	EMPTYXD(segmentsXd);
	EMPTYXD(xd);
	EMPTYXD(emptyXd);
	struct descriptor_xd *resampledXds;

	DESCRIPTOR_SIGNAL(currSignalD, MAX_DIMENSION_SIGNAL, 0, 0);
	DESCRIPTOR_APD(signalsApd, DTYPE_SIGNAL, 0, 0); 
	struct descriptor_signal **signals;


//printf("GET TIMED RECORD\n");

	timedAccessFlag = 1;
	//Get names for (possible) user defined  resample and squish funs
	status = (dbid)?_TreeGetXNci(dbid, nid, "ResampleFun", &xd):TreeGetXNci(nid, "ResampleFun", &xd);
	if(status & 1 && xd.pointer) //If a user defined fun exists
	{
		nameLen = xd.pointer->length;
		if(nameLen >= MAX_FUN_NAMELEN) nameLen = MAX_FUN_NAMELEN - 1;
		memcpy(resampleFunName, xd.pointer->pointer, nameLen);
		resampleFunName[nameLen] = 0;
		MdsFree1Dx(&xd, 0);
	}
	else
		resampleFunName[0] = 0;

	status = (dbid)?_TreeGetXNci(dbid, nid, "SquishFun", &xd):TreeGetXNci(nid, "SquishFun", &xd);
	if(status & 1 && xd.pointer) //If a user defined fun exists
	{
		nameLen = xd.pointer->length;
		if(nameLen >= MAX_FUN_NAMELEN) nameLen = MAX_FUN_NAMELEN - 1;
		memcpy(squishFunName, xd.pointer->pointer, nameLen);
		squishFunName[nameLen] = 0;
		MdsFree1Dx(&xd, 0);
	}
	else
		squishFunName[0] = 0;

/*** Not Necessary using TreeGetSegments
//Evaluate start, end to int64
	if(startD)
	{
		status = XTreeConvertToLongTime(startD, &start);
		if(!(status & 1)) return status;
	}
	if(endD)
	{
		status = XTreeConvertToLongTime(endD, &end);
		if(!(status & 1)) return status;
	}
***/
	//Get segment limits. If not evaluated to 64 bit int, make the required conversion.

	/********* Old management of start and end times per segment 


	status = (dbid)?_TreeGetNumSegments(dbid, nid, &numSegments):TreeGetNumSegments(nid, &numSegments);
	if(!(status & 1))
		return status;



    startIdx = 0;
	if(!startD)     //If no start time specified, take all initial segments
		startIdx = 0;
	else
	{
		while(startIdx < numSegments)
		{
			status = (dbid)?_TreeGetSegmentLimits(dbid, nid, startIdx, &retStartXd, &retEndXd):TreeGetSegmentLimits(nid, startIdx, &retStartXd, &retEndXd);
			if(!(status & 1)) return status;
			status = XTreeConvertToLongTime(retStartXd.pointer, &currStart);
			status = XTreeConvertToLongTime(retEndXd.pointer, &currEnd);
			MdsFree1Dx(&retStartXd, 0);
			MdsFree1Dx(&retEndXd, 0);
			if(!(status & 1)) 
				return status;

			if(currEnd > start) //First overlapping segment
			{
				if(currStart < start)
					firstSegmentTruncated = 1;
				break;
			}
			startIdx++;
		}
	}
	if(startIdx == numSegments) //All segments antecedent to start time
	{
		MdsCopyDxXd((struct descriptor *)&emptyXd, outSignal);	//return an empty XD
		return 1;
	}


	if(!endD)
		endIdx = numSegments - 1;
	else
	{
		segmentIdx = startIdx;
		while(segmentIdx < numSegments)
		{
			status = (dbid)?_TreeGetSegmentLimits(dbid, nid, segmentIdx, &retStartXd, &retEndXd):TreeGetSegmentLimits(nid, segmentIdx, &retStartXd, &retEndXd);
			if(!(status & 1)) return status;
			status = XTreeConvertToLongTime(retStartXd.pointer, &currStart);
			status = XTreeConvertToLongTime(retEndXd.pointer, &currEnd);
			MdsFree1Dx(&retStartXd, 0);
			MdsFree1Dx(&retEndXd, 0);
			if(!(status & 1)) return status;

			if(currEnd >= end) //Last overlapping segment
			{
				if(currStart > end) //all the segment lies outside the specifid range, it ha to be excluded
				{
					segmentIdx--;
					break;
				}
				if(currStart < end)
					lastSegmentTruncated = 1;
				break;
			}
			segmentIdx++;
		}
		if(segmentIdx == numSegments) //No segment (section) after end
			endIdx = numSegments - 1;
		else
			endIdx = segmentIdx;
	}
*/
/******** New management based on TreeGetSegmentLimits() ***********
	status = (dbid)?_TreeGetSegmentTimes(dbid, nid, &numSegments, &startEndTimes):TreeGetSegmentTimes(nid, &numSegments, &startEndTimes);
	if(!(status & 1)) return status;
    startIdx = 0;
	if(!startD)     //If no start time specified, take all initial segments
		startIdx = 0;
	else
	{
		while(startIdx < numSegments)
		{
			if(startEndTimes[2*startIdx+1] > start) //First overlapping segment
			{
				if(startEndTimes[2*startIdx] < start)
					firstSegmentTruncated = 1;
				break;
			}
			startIdx++;
		}
	}
	if(startIdx == numSegments) //All segments antecedent to start time
	{
		MdsCopyDxXd((struct descriptor *)&emptyXd, outSignal);	//return an empty XD
		return 1;
	}
	if(!endD)
		endIdx = numSegments - 1;
	else
	{
		segmentIdx = startIdx;
		while(segmentIdx < numSegments)
		{
			if(startEndTimes[2*segmentIdx+1] >= end) //Last overlapping segment
			{
				if(startEndTimes[2*segmentIdx] > end) //all the segment lies outside the specifid range, it has to be excluded
				{
					segmentIdx--;
					break;
				}
				if(startEndTimes[2*segmentIdx] < end)
					lastSegmentTruncated = 1;
				break;
			}
			segmentIdx++;
		}
		if(segmentIdx == numSegments) //No segment (section) after end
			endIdx = numSegments - 1;
		else
			endIdx = segmentIdx;
	}
	TreeFree(startEndTimes);
/********************************************************/

/************** Even newer implementation based on TreeGetSegments */

	status = (dbid)?_TreeGetSegments(dbid, nid, startD, endD, &segmentsXd):TreeGetSegments(nid, startD, endD, &segmentsXd);
	if(!(status & 1)) return status;
	segmentsApd = (struct descriptor_a *)segmentsXd.pointer;
	actNumSegments = (segmentsApd->arsize/segmentsApd->length)/2;

	signals = (struct descriptor_signal **)malloc(actNumSegments * sizeof(struct descriptor_signal *));
	signalsApd.pointer = (struct descriptor **)signals;
	signalsApd.arsize = actNumSegments * sizeof(struct descriptor_signal *);

	resampledXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	for(i = 0; i < actNumSegments; i++)
	{
		resampledXds[i] = emptyXd;
	}

	for(currSegIdx = 0; currSegIdx < actNumSegments; currSegIdx++)
	{
		currSignalD.ndesc = 3;
		currSignalD.dimensions[0] = ((struct descriptor **)(segmentsApd->pointer))[2 * currSegIdx + 1];
		currSignalD.data = ((struct descriptor **)(segmentsApd->pointer))[2 * currSegIdx];
//If defined, call User Provided resampling function, oterwise use default one (XTreeDefaultResample())
		if(resampleFunName[0])
		{
//			unsigned short funCode = OpcExtFunction;
			unsigned short funCode = 162;
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
			status = TdiEvaluate(&resampleFunD, &resampledXds[currSegIdx] MDS_END_ARG);
			printf("%s\n", MdsGetMsg(status));
		}
		else
		{
			status = XTreeDefaultResample((struct descriptor_signal *)&currSignalD, startD, endD, 
				minDeltaD, &resampledXds[currSegIdx]);
		}
		if(!(status & 1))
		{
			free((char *)signals);
			for(i = 0; i < actNumSegments; i++)
			{
				MdsFree1Dx(&resampledXds[i], 0);
			}
			free((char *)resampledXds);
			MdsFree1Dx(&segmentsXd, 0);
			return status;
		}
		signals[currSegIdx] = (struct descriptor_signal *)resampledXds[currSegIdx].pointer;
	}
	MdsFree1Dx(&segmentsXd, 0);



/*********************************************************************/

/**** OLD 

	//startIdx and endIdx contain now start and end indexes for valid segments
	actNumSegments = endIdx - startIdx + 1;
	signals = (struct descriptor_signal **)malloc(actNumSegments * sizeof(struct descriptor_signal *));
	signalsApd.pointer = (struct descriptor **)signals;
	signalsApd.arsize = actNumSegments * sizeof(struct descriptor_signal *);

	
	resampledXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	dataXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	dimensionXds = (struct descriptor_xd *)malloc(actNumSegments * sizeof(struct descriptor_xd));
	for(i = 0; i < actNumSegments; i++)
	{
		resampledXds[i] = emptyXd;
		dataXds[i] = emptyXd;
		dimensionXds[i] = emptyXd;
	}

	for(currIdx = startIdx, currSegIdx = 0; currIdx <= endIdx; currIdx++, currSegIdx++)
	{
		status = (dbid)?_TreeGetSegment(dbid, nid, currIdx, &dataXds[currSegIdx], &dimensionXds[currSegIdx]):TreeGetSegment(nid, currIdx, &dataXds[currSegIdx], &dimensionXds[currSegIdx]);
		if(!(status & 1))
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
//			unsigned short funCode = OpcExtFunction;
			unsigned short funCode = 162;
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
			status = TdiEvaluate(&resampleFunD, &resampledXds[currSegIdx] MDS_END_ARG);
			printf("%s\n", MdsGetMsg(status));
		}
		else
		{
			status = XTreeDefaultResample((struct descriptor_signal *)&currSignalD, startD, endD, 
				minDeltaD, &resampledXds[currSegIdx]);
		}

//printDecompiled(resampledXds[currSegIdx].pointer);
//scanf("%d", &i);

		
		if(!(status & 1))
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

****************************/
//Now signalsApd contains the list of selected resampled signals. If User squish fun defined, call it
//Otherwise call default Squish fun XTreeDefaultSquish()

	if(squishFunName[0])
	{
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
			
		status = TdiEvaluate(&squishFunD, outSignal MDS_END_ARG);
	}
	else
		status = XTreeDefaultSquish((struct descriptor_a *)&signalsApd, startD, endD, minDeltaD, outSignal);


//Free stuff
	free((char *)signals);
	for(i = 0; i < actNumSegments; i++)
	{
		MdsFree1Dx(&resampledXds[i], 0);
	}
	free((char *)resampledXds);
//	free((char *)dataXds);
//	free((char *)dimensionXds);

	return status;
}

EXPORT int XTreeGetTimedRecord(int nid, struct descriptor *startD, struct descriptor *endD, struct descriptor *minDeltaD,
						struct descriptor_xd *outSignal)
{
	return _XTreeGetTimedRecord(0, nid, startD, endD, minDeltaD, outSignal);
}
