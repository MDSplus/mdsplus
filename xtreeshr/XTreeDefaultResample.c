#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <xtreeshr.h>


static int lessThan(struct descriptor *in1D, struct descriptor *in2D, char  *less);
static int getMinMax(struct descriptor *dimD, char isMin, struct descriptor_xd *outXd);
extern int TdiCompile();
extern int TdiData();
extern int TdiEvaluate();

EXPORT int XTreeDefaultResample(struct descriptor_signal *signalD, struct descriptor *startD, struct descriptor *endD, 
						 struct descriptor *minDeltaD, struct descriptor_xd *outSignalXd)
{
	char resampleExpr[64];
	struct descriptor resampleExprD = {0, DTYPE_T, CLASS_S, resampleExpr};

	EMPTYXD(minXd);
	EMPTYXD(maxXd);
	EMPTYXD(compiledXd);
	
	int status;
	char toResample = 0;
	char isLess;
	int varCount;
//Check whether resampling is needed

	if(minDeltaD)
		toResample = 1;
	else
	{
		if(startD)
		{	
			status = getMinMax(signalD->dimensions[0], 1, &minXd);
			if(!(status & 1)) return status;
			status = lessThan(minXd.pointer, startD, &isLess);
			if(!(status & 1)) return status;
			if(isLess)
				toResample = 1;
			MdsFree1Dx(&minXd, 0);
		}
		if(!toResample && endD)
		{	
			status = getMinMax(signalD->dimensions[0], 0, &maxXd);
			if(!(status & 1)) return status;
			status = lessThan(endD, maxXd.pointer, &isLess);
			if(!(status & 1)) return status;
			if(isLess)
				toResample = 1;
			MdsFree1Dx(&maxXd, 0);
		}
	}
	if(!toResample)
	{
		status = MdsCopyDxXd((struct descriptor *)signalD, outSignalXd);
		return status;
	}
	//No way, need resampling here
	varCount = 1;

	sprintf(resampleExpr, "$1[");
	if(startD)
		sprintf(&resampleExpr[strlen(resampleExpr)], "($%d):", ++varCount);
	else
		sprintf(&resampleExpr[strlen(resampleExpr)], "*:");
	if(endD)
		sprintf(&resampleExpr[strlen(resampleExpr)], "($%d):", ++varCount);
	else
		sprintf(&resampleExpr[strlen(resampleExpr)], "*:");
	if(minDeltaD)
		sprintf(&resampleExpr[strlen(resampleExpr)], "($%d)]", ++varCount);
	else
		sprintf(&resampleExpr[strlen(resampleExpr)], "*]");


	resampleExprD.length = strlen(resampleExpr);
	switch(varCount)	{
		case 1:
			status = TdiCompile(&resampleExpr, signalD MDS_END_ARG);
			break;
		case 2:
			if(startD)
				status = TdiCompile(&resampleExprD, signalD, startD, &compiledXd MDS_END_ARG);
			else if(endD)
				status = TdiCompile(&resampleExprD, signalD, endD, &compiledXd MDS_END_ARG);
			else
				status = TdiCompile(&resampleExprD, signalD, minDeltaD, &compiledXd MDS_END_ARG);
			break;
		case 3:
			if(startD && endD)
				status = TdiCompile(&resampleExprD, signalD, startD, endD, &compiledXd  MDS_END_ARG);
			else if(startD && minDeltaD)
				status = TdiCompile(&resampleExprD, signalD, startD, minDeltaD, &compiledXd MDS_END_ARG);
			else
				status = TdiCompile(&resampleExprD, signalD, endD, minDeltaD, &compiledXd MDS_END_ARG);
			break;
		case 4:
			status = TdiCompile(&resampleExprD, signalD, startD, endD, minDeltaD, &compiledXd MDS_END_ARG);
			break;
	}
	if(!(status & 1)) return status;
	status = TdiEvaluate(&compiledXd, outSignalXd MDS_END_ARG);
	MdsFree1Dx(&compiledXd, 0);
	return status;
}


static int lessThan(struct descriptor *in1D, struct descriptor *in2D, char  *less)
{
	char ltExpr[64] = "$1 lt $2";
	struct descriptor ltExprD = {strlen(ltExpr), DTYPE_T, CLASS_S, ltExpr};
	int status;
	EMPTYXD(xd);

	ltExprD.length = strlen(ltExpr);
	status = TdiCompile(&ltExprD, in1D, in2D, &xd MDS_END_ARG);
	if(!(status & 1)) return status;
	status = TdiData(&xd, &xd MDS_END_ARG);
	if(!(status & 1))
	{
		MdsFree1Dx(&xd, 0);
		return status;
	}
	//Expected return type is BU
	*less = *xd.pointer->pointer;
	MdsFree1Dx(&xd, 0);
	return 1;
}

static int getMinMax(struct descriptor *dimD, char isMin, struct descriptor_xd *outXd)
{
	char expr[64] = "XXXXXX($1)";
	struct descriptor exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
	int status;
	EMPTYXD(xd);

	if(isMin)
		memcpy(expr, "MINVAL", 6);
	else
		memcpy(expr, "MAXVAL", 6);

	exprD.length = strlen(expr);
	status = TdiCompile(&exprD, dimD, &xd MDS_END_ARG);
	if(!(status & 1)) return status;
	status = TdiData(&xd, outXd MDS_END_ARG);
	MdsFree1Dx(&xd, 0);
	return status;
}

