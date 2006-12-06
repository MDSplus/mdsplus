#include <stdio.h>
#include <mdstypes.h>
#include <xtreeshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <string.h>
#include <stdlib.h>

extern unsigned short OpcExtFunction;


#define MAX_DIMENSION_SIGNAL 16
#define MAX_FUN_NAMELEN 512
extern int TdiData(), TdiEvaluate();

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#endif


static void printDecompiled(struct descriptor *inD)
{
	int status;
	EMPTYXD(out_xd);
	char *buf;

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





EXPORT int XTreeGetTimedRecord(int nid, struct descriptor *startD, struct descriptor *endD, struct descriptor *minDeltaD,
						struct descriptor_xd *outSignal)
{
	int status;
	_int64u start, end;
	int numSegments;
	_int64u currStart, currEnd;
	int segmentIdx, startIdx, endIdx, actNumSegments, currIdx, currSegIdx;
	int numDimensions;
	int i, nameLen;
	int firstSegmentTruncated = 0, lastSegmentTruncated = 0;
	char resampleFunName[MAX_FUN_NAMELEN], squishFunName[MAX_FUN_NAMELEN];

	struct descriptor resampleFunNameD = {0, DTYPE_T, CLASS_S, resampleFunName};
	struct descriptor squishFunNameD = {0, DTYPE_T, CLASS_S, squishFunName};
	DESCRIPTOR_R(resampleFunD, DTYPE_FUNCTION, 6);
	DESCRIPTOR_R(squishFunD, DTYPE_FUNCTION, 6);

	EMPTYXD(xd);
	EMPTYXD(retStartXd);
	EMPTYXD(retEndXd);
	EMPTYXD(emptyXd);
	struct descriptor_xd *resampledXds, *dataXds, *dimensionXds;

	DESCRIPTOR_SIGNAL(currSignalD, MAX_DIMENSION_SIGNAL, 0, 0);
	struct descriptor_a *currApd;
	DESCRIPTOR_APD(signalsApd, DTYPE_SIGNAL, 0, 0); 
	struct descriptor_signal **signals;


	//Get names for (possible) user defined  resample and squish funs
	status = TreeGetXNci(nid, "ResampleFun", &xd);
	if(!(status & 1)) return status;
	if(xd.pointer) //If a user defined fun exists
	{
		nameLen = xd.pointer->length;
		if(nameLen >= MAX_FUN_NAMELEN) nameLen = MAX_FUN_NAMELEN - 1;
		memcpy(resampleFunName, xd.pointer->pointer, nameLen);
		resampleFunName[nameLen] = 0;
		MdsFree1Dx(&xd, 0);
	}
	else
		resampleFunName[0] = 0;

	status = TreeGetXNci(nid, "SquishFun", &xd);
	if(!(status & 1)) return status;
	if(xd.pointer) //If a user defined fun exists
	{
		nameLen = xd.pointer->length;
		if(nameLen >= MAX_FUN_NAMELEN) nameLen = MAX_FUN_NAMELEN - 1;
		memcpy(squishFunName, xd.pointer->pointer, nameLen);
		squishFunName[nameLen] = 0;
		MdsFree1Dx(&xd, 0);
	}
	else
		squishFunName[0] = 0;


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

	//Get segment limits. If not evaluated to 64 bit int, make the required conversion.
	status = TreeGetNumSegments(nid, &numSegments);
	if(!(status & 1))
		return status;


    startIdx = 0;
	if(!startD)     //If no start time specified, take all initial segments
		startIdx = 0;
	else
	{
		while(startIdx < numSegments)
		{
			status = TreeGetSegmentLimits(nid, startIdx, &retStartXd, &retEndXd);
			if(!(status & 1)) return status;
			status = XTreeConvertToLongTime(retStartXd.pointer, &currStart);
			status = XTreeConvertToLongTime(retEndXd.pointer, &currEnd);
			MdsFree1Dx(&retStartXd, 0);
			MdsFree1Dx(&retEndXd, 0);
			if(!(status & 1)) return status;

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
			status = TreeGetSegmentLimits(nid, segmentIdx, &retStartXd, &retEndXd);
			if(!(status & 1)) return status;
			status = XTreeConvertToLongTime(retStartXd.pointer, &currStart);
			status = XTreeConvertToLongTime(retEndXd.pointer, &currEnd);
			MdsFree1Dx(&retStartXd, 0);
			MdsFree1Dx(&retEndXd, 0);
			if(!(status & 1)) return status;

			if(currEnd > end) //Last overlapping segment
			{
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
		status = TreeGetSegment(nid, currIdx, &dataXds[currSegIdx], &dimensionXds[currSegIdx]);
		if(!(status & 1))
		{
			free((char *)signals);
			if(resampleFunName[0] || minDeltaD)
			{
				for(i = 0; i < actNumSegments; i++)
				{
					MdsFree1Dx(&resampledXds[i], 0);
					MdsFree1Dx(&dataXds[i], 0);
					MdsFree1Dx(&dimensionXds[i], 0);
				}
				free((char *)resampledXds);
				free((char *)dataXds);
				free((char *)dimensionXds);
			}
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
			unsigned short funCode = OpcExtFunction;
			resampleFunD.length = sizeof(unsigned short);
			resampleFunD.pointer = (char *)&funCode;
			resampleFunNameD.length = strlen(resampleFunName);
			resampleFunNameD.pointer = resampleFunName;
			resampleFunD.dscptrs[0] = 0;
			resampleFunD.dscptrs[1] = &resampleFunNameD;
			resampleFunD.dscptrs[2] = (struct descriptor *)&currSignalD;
			resampleFunD.dscptrs[3] = startD;
			resampleFunD.dscptrs[4] = endD;
			resampleFunD.dscptrs[5] = minDeltaD;
			status = TdiEvaluate(&resampleFunD, &resampledXds[currSegIdx] MDS_END_ARG);
		}
		else
		{
			if(minDeltaD || (currIdx == startIdx && firstSegmentTruncated) || (currIdx == endIdx && lastSegmentTruncated))
				status = XTreeDefaultResample((struct descriptor_signal *)&currSignalD, startD, endD, 
					minDeltaD, &resampledXds[currSegIdx]);
		}

		if(!(status & 1))
		{
			free((char *)signals);
			if(resampleFunName[0] || minDeltaD)
			{
				for(i = 0; i < actNumSegments; i++)
				{
					MdsFree1Dx(&resampledXds[i], 0);
					MdsFree1Dx(&dataXds[i], 0);
					MdsFree1Dx(&dimensionXds[i], 0);
				}
				free((char *)resampledXds);
				free((char *)dataXds);
				free((char *)dimensionXds);
			}
			return status;
		}
		if(resampleFunName[0] || minDeltaD || (currIdx == startIdx && firstSegmentTruncated) || (currIdx == endIdx && lastSegmentTruncated))
			signals[currSegIdx] = (struct descriptor_signal *)resampledXds[currSegIdx].pointer;
		else
		{
			signals[currSegIdx] = (struct descriptor_signal *)malloc(sizeof(currSignalD));
			memcpy(signals[currSegIdx], (struct descriptor *)&currSignalD, sizeof(currSignalD));

		}
	}


//Now signalsApd contains the list of selected resampled signals. If User squish fun defined, call it
//Otherwise call default Squish fun XTreeDefaultSquish()

	if(squishFunName[0])
	{
		unsigned short funCode = OpcExtFunction;
		squishFunD.length = sizeof(unsigned short);
		squishFunD.pointer = (char *)&funCode;
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
	for(i = 0; i < actNumSegments; i++)
	{
		if(!(resampleFunName[0] || minDeltaD || (i == startIdx && firstSegmentTruncated) || (i == endIdx && lastSegmentTruncated)))
			free((char *)signals[i]);
	}

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

