#include <mdsdescrip.h>
#include <treeshr.h>
#include <mds_stdarg.h>
#include <stdio.h>
#include <string.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <../tdishr/opcopcodes.h>
#define MAX_LIMIT 1E10

extern int TdiData();
extern int TdiCompile();

static void compressDataLongX(float *y, long *x, int nSamples, int reqPoints, long xMin, long xMax, int *retPoints, float *retResolution);

static void swap4(char *buf)
{
    char tmp;
    tmp = buf[0];
    buf[0] = buf[3];
    buf[3] = tmp;
    tmp = buf[1];
    buf[1] = buf[2];
    buf[2] = tmp;
}

static void swap8(char *buf)
{
    char tmp;
    tmp = buf[0];
    buf[0] = buf[7];
    buf[7] = tmp;
    tmp = buf[1];
    buf[1] = buf[6];
    buf[6] = tmp;
    swap4(&buf[2]);
}

#define TRUE 1 
#define FALSE 0

static double toDouble(void *ptr, int type)
{
    switch(type)
    {
	case DTYPE_FLOAT:
	    return (double)(*(float *)ptr);
	case DTYPE_DOUBLE:
	    return *(double *)ptr;
	case DTYPE_B:
	case DTYPE_BU:
	    return (double)(*(char *)ptr);
	case DTYPE_W:
	case DTYPE_WU:
	    return (double)(*(short *)ptr);
	case DTYPE_L:
	case DTYPE_LU:
	    return (double)(*(int *)ptr);
	case DTYPE_Q:
	case DTYPE_QU:
	    return (double)(*(long *)ptr);
    }
    printf("Unsupported Type in getData\n");
    return 0;
}





//Perform possible compression on site (do not realloc arrays) return the actual number of points
//the type of X array is unknown, element size is passed in xSize
static void compressData(float *y, char *x, int xSize, int xType, int nSamples, int reqPoints, float xMin, float xMax, int *retPoints, float *retResolution)
{
    int deltaSamples, currSamples;
    int i, outIdx, startIdx, endIdx;
    float minY, maxY;
    if(nSamples < 10 * reqPoints)
    {
//Does not perform any compression 
	*retResolution = 1E12;
	*retPoints = nSamples;
	return;
    }

//From here, consider xMin and xMax
    for(startIdx = 0; startIdx < nSamples && toDouble(&x[startIdx * xSize], xType) < xMin; startIdx++);
    if(startIdx == nSamples) startIdx--;
    for(endIdx = startIdx; endIdx < nSamples && toDouble(&x[endIdx * xSize], xType) < xMax; endIdx++);
    if(endIdx == nSamples) endIdx--;
    if((endIdx - startIdx) < 10 * reqPoints)
	deltaSamples = 1;
    else

//Adjust to a number of samples close to 2 * reqPoints
    	deltaSamples = (endIdx - startIdx + 1) / reqPoints;
    currSamples = 0;
    outIdx = 0;
    if(deltaSamples == 1)
	*retResolution = 1E12;
    else
    {
    	*retResolution = reqPoints/(toDouble(&x[endIdx * xSize], xType) - toDouble(&x[startIdx * xSize], xType));
	
//	printf("Resolution: %f, points: %d, interval: %f\n", *retResolution, reqPoints, (toDouble(&x[endIdx * xSize], xType) - toDouble(&x[startIdx * xSize], xType)));
    }
    currSamples = startIdx;
    while(currSamples < endIdx)
    {
	if(deltaSamples == 1)
	{
	    y[outIdx] = y[currSamples];
	    memcpy(&x[outIdx * xSize], &x[currSamples * xSize], xSize);
	    currSamples++;
	    outIdx++;
	}
	else
	{
	    minY = maxY = y[currSamples];
	    for(i = currSamples; i < nSamples && i < currSamples + deltaSamples; i++)
	    {
	    	if(y[i] < minY) minY = y[i];
	    	if(y[i] > maxY) maxY = y[i];
	    }
	    y[outIdx] = minY;
	    y[outIdx+1] = maxY;
	    memcpy(&x[outIdx * xSize], &x[currSamples * xSize], xSize);
	    memcpy(&x[(outIdx+1) * xSize], &x[currSamples * xSize], xSize);
	    currSamples += deltaSamples;
	    outIdx += 2;
	}
    }
    *retPoints = outIdx;
}


static char *recGetHelp(struct descriptor *dsc)
{
    int nid, numSegments, status, i;
    char *path, *help;
    EMPTYXD(xd);
    struct descriptor_r *rDsc;
    struct descriptor_param *pDsc;
    if(!dsc) return NULL;
    switch (dsc->class) {
	case CLASS_S:
	    if(dsc->dtype == DTYPE_NID)
	    {
	 	nid = *(int *)dsc->pointer;
		status = TreeGetNumSegments(nid, &numSegments);
		if(!(status & 1) || numSegments > 0)
		    return NULL;  
		status = TreeGetRecord(nid, &xd);
		if(status & 1)
		{
		    help = recGetHelp(xd.pointer);
		    MdsFree1Dx(&xd, 0);
		    return help;
		}
		return NULL;
	    }
	    else if(dsc->dtype == DTYPE_PATH)
	    {
		path = malloc(dsc->length + 1);
		memcpy(path, dsc->pointer, dsc->length);
		path[dsc->length] = 0;
		status = TreeFindNode(path, &nid);
		if(!(status & 1))
		    return NULL;
		status = TreeGetNumSegments(nid, &numSegments);
		if(!(status & 1) || numSegments > 0)
		    return NULL;  
		status = TreeGetRecord(nid, &xd);
		if(status & 1)
		{
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
	    if(!((struct descriptor_xd *)dsc)->pointer) return NULL;
	    return recGetHelp(((struct descriptor_xd *)dsc)->pointer);
	case CLASS_R:
	    if(dsc->dtype == DTYPE_PARAM)
	    {
		help = NULL;
		pDsc = (struct descriptor_param *)dsc;
		if(pDsc->help)
		{
		    status = TdiData(pDsc->help, &xd MDS_END_ARG);
		    if((status & 1) && xd.pointer && xd.pointer->class == CLASS_S && xd.pointer->dtype == DTYPE_T)
		    {
			help = malloc(xd.pointer->length + 1);
			memcpy(help, xd.pointer->pointer, xd.pointer->length);
			help[xd.pointer->length] = 0;
		    }
		    MdsFree1Dx(&xd, 0);
		}
		return help;
	    }
	    rDsc = (struct descriptor_r *)dsc;
	    for(i = 0; i < rDsc->ndesc; i++)
	    {
	      if((help = recGetHelp(rDsc->dscptrs[i])))
		    return help;
	    }
	    return NULL;
	default: return NULL;
    }
}

static char *recGetUnits(struct descriptor *dsc, int isX)
{
    int nid, numSegments, status, i, unitsLen;
    char *path, *units, *currUnits;
    char **unitsArr;
    char *mulDiv;
    EMPTYXD(xd);
    struct descriptor_r *rDsc;
    struct descriptor_with_units *uDsc;
    struct descriptor_signal *sDsc;
    struct descriptor_param *pDsc;
    if(!dsc) return NULL;
    switch (dsc->class) {
	case CLASS_S:
	    if(dsc->dtype == DTYPE_NID)
	    {
	 	nid = *(int *)dsc->pointer;
		status = TreeGetNumSegments(nid, &numSegments);
		if(!(status & 1) || numSegments > 0)
		    return NULL;  
		status = TreeGetRecord(nid, &xd);
		if(status & 1)
		{
		    units = recGetUnits(xd.pointer, isX);
		    MdsFree1Dx(&xd, 0);
		    return units;
		}
		return NULL;
	    }
	    else if(dsc->dtype == DTYPE_PATH)
	    {
		path = malloc(dsc->length + 1);
		memcpy(path, dsc->pointer, dsc->length);
		path[dsc->length] = 0;
		status = TreeFindNode(path, &nid);
		if(!(status & 1))
		    return NULL;
		status = TreeGetNumSegments(nid, &numSegments);
		if(!(status & 1) || numSegments > 0)
		    return NULL;  
		status = TreeGetRecord(nid, &xd);
		if(status & 1)
		{
		    units = recGetUnits(xd.pointer, isX);
		    MdsFree1Dx(&xd, 0);
		    return units;
		}
		return NULL;
	    }
	    return NULL;
	case CLASS_A:
	    return NULL;
	case CLASS_XD:
	    if(!((struct descriptor_xd *)dsc)->pointer) return NULL;
	    return recGetUnits(((struct descriptor_xd *)dsc)->pointer, isX);
	case CLASS_R:
	    if(dsc->dtype == DTYPE_WITH_UNITS)
	    {
		units = NULL;
		uDsc = (struct descriptor_with_units *)dsc;
		if(uDsc->units)
		{
		    status = TdiData(uDsc->units, &xd MDS_END_ARG);
		    if((status & 1) && xd.pointer && xd.pointer->class == CLASS_S && xd.pointer->dtype == DTYPE_T)
		    {
			units = malloc(xd.pointer->length + 1);
			memcpy(units, xd.pointer->pointer, xd.pointer->length);
			units[xd.pointer->length] = 0;
		    }
		    MdsFree1Dx(&xd, 0);
		}
		return units;
	    }
	    if(dsc->dtype == DTYPE_SIGNAL)
	    {
		sDsc = (struct descriptor_signal *)dsc;
		if(isX)
		{
		    if(sDsc->ndesc > 2)
		    	return recGetUnits(sDsc->dimensions[0], isX);
		    else
			return NULL;
		}
		else
		{
		    if(sDsc->data)
			return recGetUnits(sDsc->data, isX);
		    else
			return recGetUnits(sDsc->raw, isX);
		}
	    }
	    if(dsc->dtype == DTYPE_PARAM)
	    {
		pDsc = (struct descriptor_param *)dsc;
		return recGetUnits(pDsc->value, isX);
	    }
	    rDsc = (struct descriptor_r *)dsc;
	    if(rDsc->ndesc == 1)
		return recGetUnits(rDsc->dscptrs[0], isX);
	    if(rDsc->dtype == DTYPE_FUNCTION && rDsc->pointer && (*(short *)rDsc->pointer == OpcAdd || *(short *)rDsc->pointer == OpcSubtract))
	    {
		units = recGetUnits(rDsc->dscptrs[0], isX);
		if(!units) return NULL;
	    	for(i = 1; i < rDsc->ndesc; i++)
		{
		    currUnits = recGetUnits(rDsc->dscptrs[i], isX);
		    if(!currUnits || strcmp(units, currUnits)) //Different units
		    {
			free(units);
			free(currUnits);
			return NULL;
		    }
		    free(currUnits);
		}
		return units;
	    }
	    if(rDsc->dtype == DTYPE_FUNCTION && rDsc->pointer && (*(short *)rDsc->pointer == OpcMultiply || *(short *)rDsc->pointer == OpcDivide))
	    {
		mulDiv = (*(short *)rDsc->pointer == OpcMultiply)?"*":"/";
		unitsArr = (char **)malloc(sizeof(char *) * rDsc->ndesc);
		unitsLen = rDsc->ndesc;
	    	for(i = 0; i < rDsc->ndesc; i++)
		{
		    unitsArr[i] = recGetUnits(rDsc->dscptrs[i], isX);
		    if(unitsArr[i])
			unitsLen += strlen(unitsArr[i]);
		}
		units = malloc(unitsLen+1);
		units[0] = 0;
	    	for(i = 0; i < rDsc->ndesc; i++)
		{
		    if(unitsArr[i])
		    {
			if(strlen(units) > 0)
			    sprintf(&units[strlen(units)], "%s%s", mulDiv, unitsArr[i]);
			else
			    sprintf(&units[strlen(units)], "%s", unitsArr[i]);
			free(unitsArr[i]);
		    }
		}
		free((char *)unitsArr);
		return units;
	    }
	    return NULL;
	
	default: return NULL;
    }
}
	



static int recIsSegmented(struct descriptor *dsc)
{
    int nid, numSegments, status, i;
    char *path;
    struct descriptor_r *rDsc;
    if(!dsc) return FALSE;
    switch (dsc->class) {
	case CLASS_S:
	    if(dsc->dtype == DTYPE_NID)
	    {
	 	nid = *(int *)dsc->pointer;
		status = TreeGetNumSegments(nid, &numSegments);
		if((status & 1) && numSegments > 0)
		    return TRUE; 
		return FALSE;
	    }
	    else if(dsc->dtype == DTYPE_PATH)
	    {
		path = malloc(dsc->length + 1);
		memcpy(path, dsc->pointer, dsc->length);
		path[dsc->length] = 0;
		status = TreeFindNode(path, &nid);
		if(status & 1)
		    status = TreeGetNumSegments(nid, &numSegments);
		if((status & 1) && numSegments > 0)
		    return TRUE; 
		return FALSE;
	    }
	    else
		return FALSE;
	case CLASS_A:
	    return FALSE;
	case CLASS_XD:
	    if(!((struct descriptor_xd *)dsc)->pointer) return FALSE;
	    return recIsSegmented(((struct descriptor_xd *)dsc)->pointer);
	case CLASS_R:
	    rDsc = (struct descriptor_r *)dsc;
	    for(i = 0; i < rDsc->ndesc; i++)
	    {
		if(rDsc->dscptrs[i] && recIsSegmented(rDsc->dscptrs[i]))
		    return TRUE;
	    }
	    return FALSE;
	default: return FALSE;
    }
}
	
//Check if the passed expression contains at least one segmented node
int IsSegmented(char *expr)
{
    EMPTYXD(xd);
    struct descriptor exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
    int status, isSegmented;

    status = TdiCompile(&exprD, &xd MDS_END_ARG);
    if(!(status & 1)) return FALSE;
    isSegmented = recIsSegmented(xd.pointer);
    MdsFree1Dx(&xd, 0);
    return isSegmented;
}
    
int TestGetHelp(char *expr)
{
    EMPTYXD(xd);
    struct descriptor exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
    int status;
    char *help;

    status = TdiCompile(&exprD, &xd MDS_END_ARG);
    if(!(status & 1)) return FALSE;
    help = recGetHelp(xd.pointer);
    if(help) printf("%s\n", help);
    MdsFree1Dx(&xd, 0);
    return 1;
}
    
int TestGetUnits(char *expr)
{
    EMPTYXD(xd);
    struct descriptor exprD = {strlen(expr), DTYPE_T, CLASS_S, expr};
    int status;
    char *units;

    status = TdiCompile(&exprD, &xd MDS_END_ARG);
    if(!(status & 1)) return FALSE;
    units = recGetUnits(xd.pointer, 0);
    if(units) printf("%s\n", units);
    MdsFree1Dx(&xd, 0);
    return 1;
}
    


struct descriptor_xd *GetXYSignal(char *inY, char *inX, float *inXMin, float *inXMax, int *reqNSamples)
{
    static EMPTYXD(retXd);
    EMPTYXD(xd);
    EMPTYXD(yXd);
    EMPTYXD(xXd);
    struct descriptor xMinD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
    struct descriptor xMaxD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
    struct descriptor yExpr = {strlen(inY), DTYPE_T, CLASS_S, inY};
    struct descriptor xExpr = {strlen(inX), DTYPE_T, CLASS_S, inX};
    struct descriptor errD = {0, DTYPE_T, CLASS_S, 0};
    char *err;
    char swap;
    char testEndian[4] = {0, 0, 0, 1};
    int nSamples, retSamples;
    int xSampleSize;
    int retSize;
    int idx, i, status;
    char *retArr;
    // float currFloat;
    float retResolution;
    DESCRIPTOR_A(retArrD, 1, DTYPE_B, 0, 0); 
    float xMin = *inXMin;
    float xMax = *inXMax; 
    struct descriptor_a *xArrD, *yArrD;
    float *y;
    double maxX, minX, currX;
    char *title, *xLabel, *yLabel;

//printf("GetXYSignal(%s, %s, %f, %f, %d)\n", inY, inX, *inXMin, *inXMax, *reqNSamples); 


//Set limits if any
    if(xMin <= -MAX_LIMIT && xMax >= MAX_LIMIT)
	status = TreeSetTimeContext(NULL, NULL, NULL);
    else if(xMin <= -MAX_LIMIT)
	status = TreeSetTimeContext(NULL, &xMaxD, NULL);
    else if(xMax >= MAX_LIMIT)
	status = TreeSetTimeContext(&xMinD, NULL, NULL);
    else
	status = TreeSetTimeContext(&xMinD, &xMaxD, NULL);

//Get Y
    status = TdiCompile(&yExpr, &xd MDS_END_ARG);
    if(status & 1)
    {
//Get title and yLabel, if any
    	title = recGetHelp(xd.pointer);
	yLabel = recGetUnits(xd.pointer, 0);
//get Data
	status = TdiData(&xd, &yXd MDS_END_ARG);
    }
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    } 
//Get X
    status = TdiCompile(&xExpr, &xd MDS_END_ARG);
    if(status & 1)
    {
//Get xLabel, if any
	xLabel = recGetUnits(xd.pointer, 1);
//get Data
	status = TdiData(&xd, &xXd MDS_END_ARG);
    }
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
// Y converted to float, X to long or float or double
    err = 0;
    if(yXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    else if(xXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    if(err)
    {
	MdsFree1Dx(&xXd, 0);
	MdsFree1Dx(&yXd, 0);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Number of asmples set to minimum between X and Y
    xArrD = (struct descriptor_a *)xXd.pointer;
    yArrD = (struct descriptor_a *)yXd.pointer;
    nSamples = yArrD->arsize/yArrD->length;
    if(nSamples > xArrD->arsize/xArrD->length)
	nSamples = xArrD->arsize/xArrD->length;
 
    if(yArrD->dtype == DTYPE_FLOAT)
	y = (float *)yArrD->pointer;
    else
    {
	y = (float *)malloc(nSamples * sizeof(float));
	for(i = 0; i < nSamples; i++)
	{
	    switch(yArrD->dtype) {
	    	case DTYPE_B:
	    	case DTYPE_BU:
		    y[i] = *((char *)(&yArrD->pointer[i*yArrD->length]));
		break;
	    	case DTYPE_W:
	    	case DTYPE_WU:
		    y[i] = *((short *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_L:
	    	case DTYPE_LU:
		    y[i] = *((int *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_Q:
	    	case DTYPE_QU:
		    y[i] = *((long *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_DOUBLE:
		    y[i] = *((double *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    }
	}
    }
    compressData(y, xArrD->pointer, xArrD->length, xArrD->dtype, nSamples, *reqNSamples, *inXMin, *inXMax, &retSamples, &retResolution);


/*Assemble result. Format:
-retResolution(float)
-number of samples (minumum between X and Y)
-type of X xamples (byte: long(1), double(2) or float(3))
-y samples (float - big endian)
-x Samples 
*/
	
    swap = *((int *)testEndian) != 1;
    if(xArrD->dtype == DTYPE_Q || xArrD->dtype == DTYPE_QU || xArrD->dtype == DTYPE_DOUBLE)
	xSampleSize = sizeof(long);
    else
	xSampleSize = sizeof(int);

    
    

    retSize = sizeof(float) + sizeof(int) + 1 + retSamples * (sizeof(float) + xSampleSize);
//Add rool for title and labels
    retSize += 3 * sizeof(int);
    if(title)
    {
	retSize += strlen(title);
    }
    if(xLabel)
	retSize += strlen(xLabel);
    if(yLabel)
	retSize += strlen(yLabel);
    retArr = malloc(retSize);
    *((int *)&retArr[4]) = retSamples;
    if(swap)swap4(&retArr[4]);
    if(xArrD->dtype == DTYPE_Q || xArrD->dtype == DTYPE_QU)
    {
	retArr[8] = 1; 
    }
    else if(xArrD->dtype == DTYPE_DOUBLE)
	retArr[8] = 2;
    else
	retArr[8] = 3;

    idx = 9;
    for(i = 0; i < retSamples; i++)
    {
    	*((float *)&retArr[idx]) = y[i];
	if(swap)swap4(&retArr[idx]);
	idx += sizeof(float);
    }
    for(i = 0; i < retSamples; i++)
    {
      //currFloat = 0;
	switch(xArrD->dtype) {
	    case DTYPE_B:
	    case DTYPE_BU:
		*((float *)&retArr[idx]) = *((char *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((char *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_W:
	    case DTYPE_WU:
		*((float *)&retArr[idx]) = *((short *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((short *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_L:
	    case DTYPE_LU:
		*((float *)&retArr[idx]) = *((int *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((int *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_Q:
	    case DTYPE_QU:
		*((long *)&retArr[idx]) = *((long *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap8(&retArr[idx]);
		idx += sizeof(long);
		currX = *((long *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_FLOAT:
		*((float *)&retArr[idx]) = *((float *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((float *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_DOUBLE:
		*((double *)&retArr[idx]) = *((double *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap8(&retArr[idx]);
		idx += sizeof(double);
		currX = *((double *)(&xArrD->pointer[i*xArrD->length]));
		break;
	}
	if(i == 0)
	    minX = maxX = currX;
	else
	{
	    if(currX < minX)
		minX = currX;
	    if(currX > maxX)
		maxX = currX;
	}
    }
//idx is the current index in retArr
//Write title, xLabel, yLabel as length followed by chars
    if(title)
    {
	*(int *)&retArr[idx] = strlen(title);
	if(swap) swap4(&retArr[idx]);
	idx += sizeof(int);
	memcpy(&retArr[idx], title, strlen(title));
	idx += strlen(title);
    }
    else
    {
	*(int *)&retArr[idx] = 0; //no swap required
	idx += 4;
    }
    if(xLabel)
    {
	*(int *)&retArr[idx] = strlen(xLabel);
	if(swap) swap4(&retArr[idx]);
	idx += sizeof(int);
	memcpy(&retArr[idx], xLabel, strlen(xLabel));
	idx += strlen(xLabel);
    }
    else
    {
	*(int *)&retArr[idx] = 0; //no swap required
	idx += 4;
    }
    if(yLabel)
    {
	*(int *)&retArr[idx] = strlen(yLabel);
	if(swap) swap4(&retArr[idx]);
	idx += sizeof(int);
	memcpy(&retArr[idx], yLabel, strlen(yLabel));
	idx += strlen(yLabel);
    }
    else
    {
	*(int *)&retArr[idx] = 0; //no swap required
	idx += 4;
    }

    *((float *)retArr) = retResolution; //resolution
    if(swap)swap4(retArr);



    retArrD.arsize = retSize;
    retArrD.pointer = retArr;
    if(yArrD->dtype != DTYPE_FLOAT)
	free((char *)y);
    MdsCopyDxXd((struct descriptor *)&retArrD, &retXd);
    MdsFree1Dx(&xXd, 0);
    MdsFree1Dx(&yXd, 0);
    free(retArr);
    return &retXd;
}


struct descriptor_xd *GetXYSignalLongTimes(char *inY, char *inX, long *inXMin, long *inXMax, int *reqNSamples)
{
    static EMPTYXD(retXd);
    EMPTYXD(xd);
    EMPTYXD(yXd);
    EMPTYXD(xXd);
    struct descriptor xMinD = {sizeof(float), DTYPE_QU, CLASS_S, (char *)inXMin};
    struct descriptor xMaxD = {sizeof(float), DTYPE_QU, CLASS_S, (char *)inXMax};
    struct descriptor yExpr = {strlen(inY), DTYPE_T, CLASS_S, inY};
    struct descriptor xExpr = {strlen(inX), DTYPE_T, CLASS_S, inX};
    struct descriptor errD = {0, DTYPE_T, CLASS_S, 0};
    char *err;
    char swap;
    char testEndian[4] = {0, 0, 0, 1};
    int nSamples, retSamples;
    int xSampleSize;
    int retSize;
    int idx, i, status;
    char *retArr;
    //float currFloat;
    float retResolution;
    DESCRIPTOR_A(retArrD, 1, DTYPE_B, 0, 0); 
    long xMin = *inXMin;
    long xMax = *inXMax; 
    struct descriptor_a *xArrD, *yArrD;
    float *y;
    //float retInterval;
    double maxX, minX, currX;
    char *title, *xLabel, *yLabel;

//printf("GetXYSignalLongTimes(%s, %s, %d, %d, %d)\n", inY, inX, *inXMin, *inXMax, *reqNSamples); 


//Set limits if any
   if(xMin == 0 && xMax  == 0)
	status = TreeSetTimeContext(NULL, NULL, NULL);
    else if(xMin == 0)
	status = TreeSetTimeContext(NULL, &xMaxD, NULL);
    else if(xMax == 0)
	status = TreeSetTimeContext(&xMinD, NULL, NULL);
    else
    	status = TreeSetTimeContext(&xMinD, &xMaxD, NULL);

//Get Y
    status = TdiCompile(&yExpr, &xd MDS_END_ARG);
    if(status & 1)
    {
//Get title and yLabel, if any
    	title = recGetHelp(xd.pointer);
	yLabel = recGetUnits(xd.pointer, 0);
//get Data
	status = TdiData(&xd, &yXd MDS_END_ARG);
    }
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    } 
//Get X
    status = TdiCompile(&xExpr, &xd MDS_END_ARG);
    if(status & 1)
    {
//Get xLabel, if any
	xLabel = recGetUnits(xd.pointer, 1);
//get Data
	status = TdiData(&xd, &xXd MDS_END_ARG);
    }
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
// Y converted to float, X to long or float or double
    err = 0;
    if(yXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    else if(xXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    if(err)
    {
	MdsFree1Dx(&xXd, 0);
	MdsFree1Dx(&yXd, 0);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Number of asmples set to minimum between X and Y
    xArrD = (struct descriptor_a *)xXd.pointer;
    yArrD = (struct descriptor_a *)yXd.pointer;
    nSamples = yArrD->arsize/yArrD->length;
    if(nSamples > xArrD->arsize/xArrD->length)
	nSamples = xArrD->arsize/xArrD->length;
 
    if(yArrD->dtype == DTYPE_FLOAT)
	y = (float *)yArrD->pointer;
    else
    {
	y = (float *)malloc(nSamples * sizeof(float));
	for(i = 0; i < nSamples; i++)
	{
	    switch(yArrD->dtype) {
	    	case DTYPE_B:
	    	case DTYPE_BU:
		    y[i] = *((char *)(&yArrD->pointer[i*yArrD->length]));
		break;
	    	case DTYPE_W:
	    	case DTYPE_WU:
		    y[i] = *((short *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_L:
	    	case DTYPE_LU:
		    y[i] = *((int *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_Q:
	    	case DTYPE_QU:
		    y[i] = *((long *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_DOUBLE:
		    y[i] = *((double *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    }
	}
    }
    compressDataLongX(y, (long *)xArrD->pointer, nSamples, *reqNSamples, *inXMin, *inXMax, &retSamples, &retResolution);

/*Assemble result. Format:
-retResolution(float)
-number of samples (minumum between X and Y)
-type of X xamples (byte: long(1), double(2) or float(3))
-y samples (float - big endian)
-x Samples 
*/
	
    swap = *((int *)testEndian) != 1;
    if(xArrD->dtype == DTYPE_Q || xArrD->dtype == DTYPE_QU || xArrD->dtype == DTYPE_DOUBLE)
	xSampleSize = sizeof(long);
    else
	xSampleSize = sizeof(int);

    
    

    retSize = sizeof(float) + sizeof(int) + 1 + retSamples * (sizeof(float) + xSampleSize);
//Add rool for title and labels
    retSize += 3 * sizeof(int);
    if(title)
    {
	retSize += strlen(title);
    }
    if(xLabel)
	retSize += strlen(xLabel);
    if(yLabel)
	retSize += strlen(yLabel);
    retArr = malloc(retSize);
    *((int *)&retArr[4]) = retSamples;
    if(swap)swap4(&retArr[4]);
    if(xArrD->dtype == DTYPE_Q || xArrD->dtype == DTYPE_QU)
    {
	retArr[8] = 1; 
    }
    else if(xArrD->dtype == DTYPE_DOUBLE)
	retArr[8] = 2;
    else
	retArr[8] = 3;

    idx = 9;
    for(i = 0; i < retSamples; i++)
    {
    	*((float *)&retArr[idx]) = y[i];
	if(swap)swap4(&retArr[idx]);
	idx += sizeof(float);
    }
    for(i = 0; i < retSamples; i++)
    {
      //currFloat = 0;
	switch(xArrD->dtype) {
	    case DTYPE_B:
	    case DTYPE_BU:
		*((float *)&retArr[idx]) = *((char *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((char *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_W:
	    case DTYPE_WU:
		*((float *)&retArr[idx]) = *((short *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((short *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_L:
	    case DTYPE_LU:
		*((float *)&retArr[idx]) = *((int *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((int *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_Q:
	    case DTYPE_QU:
		*((long *)&retArr[idx]) = *((long *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap8(&retArr[idx]);
		idx += sizeof(long);
		currX = *((long *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_FLOAT:
		*((float *)&retArr[idx]) = *((float *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap4(&retArr[idx]);
		idx += sizeof(float);
		currX = *((float *)(&xArrD->pointer[i*xArrD->length]));
		break;
	    case DTYPE_DOUBLE:
		*((double *)&retArr[idx]) = *((double *)(&xArrD->pointer[i*xArrD->length]));
		if(swap)swap8(&retArr[idx]);
		idx += sizeof(double);
		currX = *((double *)(&xArrD->pointer[i*xArrD->length]));
		break;
	}
	if(i == 0)
	    minX = maxX = currX;
	else
	{
	    if(currX < minX)
		minX = currX;
	    if(currX > maxX)
		maxX = currX;
	}
    }
//idx is the current index in retArr
//Write title, xLabel, yLabel as length followed by chars
    if(title)
    {
	*(int *)&retArr[idx] = strlen(title);
	if(swap) swap4(&retArr[idx]);
	idx += sizeof(int);
	memcpy(&retArr[idx], title, strlen(title));
	idx += strlen(title);
    }
    else
    {
	*(int *)&retArr[idx] = 0; //no swap required
	idx += 4;
    }
    if(xLabel)
    {
	*(int *)&retArr[idx] = strlen(xLabel);
	if(swap) swap4(&retArr[idx]);
	idx += sizeof(int);
	memcpy(&retArr[idx], xLabel, strlen(xLabel));
	idx += strlen(xLabel);
    }
    else
    {
	*(int *)&retArr[idx] = 0; //no swap required
	idx += 4;
    }
    if(yLabel)
    {
	*(int *)&retArr[idx] = strlen(yLabel);
	if(swap) swap4(&retArr[idx]);
	idx += sizeof(int);
	memcpy(&retArr[idx], yLabel, strlen(yLabel));
	idx += strlen(yLabel);
    }
    else
    {
	*(int *)&retArr[idx] = 0; //no swap required
	idx += 4;
    }

    *((float *)retArr) = retResolution; //resolution
    if(swap)swap4(retArr);



    retArrD.arsize = retSize;
    retArrD.pointer = retArr;
    if(yArrD->dtype != DTYPE_FLOAT)
	free((char *)y);
    MdsCopyDxXd((struct descriptor *)&retArrD, &retXd);
    MdsFree1Dx(&xXd, 0);
    MdsFree1Dx(&yXd, 0);
    free(retArr);
    return &retXd;
}




struct descriptor_xd *GetXYWave(char *sigName, float *inXMin, float *inXMax, int *reqNSamples)
{
    static EMPTYXD(retXd);
    EMPTYXD(xd);
    EMPTYXD(yXd);
    EMPTYXD(xXd);
    struct descriptor yExpr = {strlen(sigName), DTYPE_T, CLASS_S, sigName};
    struct descriptor xExpr = {strlen(sigName) + strlen("DIM_OF()"), DTYPE_T, CLASS_S, 0};
    struct descriptor errD = {0, DTYPE_T, CLASS_S, 0};
    struct descriptor xMinD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMin};
    struct descriptor xMaxD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)inXMax};
    char *err;
    int nSamples, retSamples;
    //int xSampleSize;
    //int retSize;
    int i, status;
    //char *retArr;
    //float currFloat;
    float retResolution;
    DESCRIPTOR_A(retDataD, sizeof(float), DTYPE_FLOAT, 0, 0);
    DESCRIPTOR_A(retDimD, 0, 0, 0, 0);
    DESCRIPTOR_SIGNAL_1(retSignalD, &retDataD, 0, &retDimD);
    float xMin = *inXMin;
    float xMax = *inXMax; 
    struct descriptor_a *xArrD, *yArrD;
    float *y;
    //float retInterval;
    //double maxX, minX, currX;
    //char *title, *xLabel, *yLabel;

//printf("GetXYWave(%s, %f, %f, %d)\n", sigName, *inXMin, *inXMax, *reqNSamples); 


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
    if(status & 1)
	status = TdiData(&xd, &yXd MDS_END_ARG);
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    } 

//Get X
    status = TdiCompile(&xExpr, &xd MDS_END_ARG);
    free(xExpr.pointer);
    if(status & 1)
	status = TdiData(&xd, &xXd MDS_END_ARG);
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
// Y converted to float, X to long or float or double
    err = 0;
    if(yXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    else if(xXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    if(err)
    {
	MdsFree1Dx(&xXd, 0);
	MdsFree1Dx(&yXd, 0);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Number of samples set to minimum between X and Y
    xArrD = (struct descriptor_a *)xXd.pointer;
    yArrD = (struct descriptor_a *)yXd.pointer;
    nSamples = yArrD->arsize/yArrD->length;
    if(nSamples > xArrD->arsize/xArrD->length)
	nSamples = xArrD->arsize/xArrD->length;
 
    if(yArrD->dtype == DTYPE_FLOAT)
	y = (float *)yArrD->pointer;
    else
    {
	y = (float *)malloc(nSamples * sizeof(float));
	for(i = 0; i < nSamples; i++)
	{
	    switch(yArrD->dtype) {
	    	case DTYPE_B:
	    	case DTYPE_BU:
		    y[i] = *((char *)(&yArrD->pointer[i*yArrD->length]));
		break;
	    	case DTYPE_W:
	    	case DTYPE_WU:
		    y[i] = *((short *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_L:
	    	case DTYPE_LU:
		    y[i] = *((int *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_Q:
	    	case DTYPE_QU:
		    y[i] = *((long *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_DOUBLE:
		    y[i] = *((double *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    }
	}
    }
    compressData(y, xArrD->pointer, xArrD->length, xArrD->dtype, nSamples, *reqNSamples, *inXMin, *inXMax, &retSamples, &retResolution);

    retDataD.pointer = (char *)y;
    retDataD.arsize = sizeof(float ) * retSamples;
    retDimD.length = xArrD->length;
    retDimD.dtype = xArrD->dtype;
    retDimD.pointer = xArrD->pointer;
    retDimD.arsize = retSamples * xArrD->length;

    MdsCopyDxXd((struct descriptor *)&retSignalD, &retXd);
    MdsFree1Dx(&xXd, 0);
    MdsFree1Dx(&yXd, 0);
    return &retXd;
}




//Perform possible compression on site (do not realloc arrays) return the actual number of points
//the type of X array is unknown, element size is passed in xSize
static void compressDataLongX(float *y, long *x, int nSamples, int reqPoints, long xMin, long xMax, int *retPoints, float *retResolution)
{
    int deltaSamples, currSamples;
    int i, outIdx, startIdx, endIdx;
    float minY, maxY;
    if(nSamples < 10 * reqPoints)
    {
//Does not perform any compression 
	*retResolution = 1E12;
	*retPoints = nSamples;
	return;
    }

//From here, consider xMin and xMax
    for(startIdx = 0; startIdx < nSamples && x[startIdx] < xMin; startIdx++);
    if(startIdx == nSamples) startIdx--;
    for(endIdx = startIdx; endIdx < nSamples && x[endIdx] < xMax; endIdx++);
    if(endIdx == nSamples) endIdx--;
    if((endIdx - startIdx) < 10 * reqPoints)
	deltaSamples = 1;
    else

//Adjust to a number of samples close to 2 * reqPoints
    	deltaSamples = (endIdx - startIdx + 1) / reqPoints;
    currSamples = 0;
    outIdx = 0;
    if(deltaSamples == 1)
	*retResolution = 1E12;
    else
    {
    	*retResolution = reqPoints/(double)(x[endIdx] - x[startIdx]);
    }
    currSamples = startIdx;
    while(currSamples < endIdx)
    {
	if(deltaSamples == 1)
	{
	    y[outIdx] = y[currSamples];
	    memcpy(&x[outIdx], &x[currSamples], sizeof(long));
	    currSamples++;
	    outIdx++;
	}
	else
	{
	    minY = maxY = y[currSamples];
	    for(i = currSamples; i < nSamples && i < currSamples + deltaSamples; i++)
	    {
	    	if(y[i] < minY) minY = y[i];
	    	if(y[i] > maxY) maxY = y[i];
	    }
	    y[outIdx] = minY;
	    y[outIdx+1] = maxY;
	    memcpy(&x[outIdx], &x[currSamples], sizeof(long));
	    memcpy(&x[(outIdx+1)], &x[currSamples], sizeof(long));
	    currSamples += deltaSamples;
	    outIdx += 2;
	}
    }
    *retPoints = outIdx;
}





struct descriptor_xd *GetXYWaveLongTimes(char *sigName, long *inXMin, long *inXMax, int *reqNSamples)
{
    static EMPTYXD(retXd);
    EMPTYXD(xd);
    EMPTYXD(yXd);
    EMPTYXD(xXd);
    struct descriptor yExpr = {strlen(sigName), DTYPE_T, CLASS_S, sigName};
    struct descriptor xExpr = {strlen(sigName) + strlen("DIM_OF()"), DTYPE_T, CLASS_S, 0};
    struct descriptor errD = {0, DTYPE_T, CLASS_S, 0};
    struct descriptor xMinD = {sizeof(long), DTYPE_Q, CLASS_S, (char *)inXMin};
    struct descriptor xMaxD = {sizeof(long), DTYPE_Q, CLASS_S, (char *)inXMax};
    char *err;
    int nSamples, retSamples;
    //int xSampleSize;
    //int retSize;
    int i, status;
    //char *retArr;
    //float currFloat;
    float retResolution;
    DESCRIPTOR_A(retDataD, sizeof(float), DTYPE_FLOAT, 0, 0);
    DESCRIPTOR_A(retDimD, 0, 0, 0, 0);
    DESCRIPTOR_SIGNAL_1(retSignalD, &retDataD, 0, &retDimD);
    //long xMin = *inXMin;
    //long xMax = *inXMax; 
    struct descriptor_a *xArrD, *yArrD;
    float *y;
    //float retInterval;
    //double maxX, minX, currX;
    //char *title, *xLabel, *yLabel;

//printf("GetXYWave(%s, %d, %d, %d)\n", sigName, *inXMin, *inXMax, *reqNSamples); 


//Set limits 
	status = TreeSetTimeContext(&xMinD, &xMaxD, NULL);

//Set correct name for xExpr
    xExpr.pointer = malloc(strlen(sigName) + strlen("DIM_OF()") + 1);
    sprintf(xExpr.pointer, "DIM_OF(%s)", sigName);

//Get Y
    status = TdiCompile(&yExpr, &xd MDS_END_ARG);
    if(status & 1)
	status = TdiData(&xd, &yXd MDS_END_ARG);
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    } 

//Get X
    status = TdiCompile(&xExpr, &xd MDS_END_ARG);
    free(xExpr.pointer);
    if(status & 1)
	status = TdiData(&xd, &xXd MDS_END_ARG);
    MdsFree1Dx(&xd, 0);
    if(!(status & 1))
    {
	err = MdsGetMsg(status);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Check results: must be an array of either type DTYPE_B, DTYPE_BU, DTYPE_W, DTYPE_WU, DTYPE_L, DTYPE_LU, DTYPE_FLOAT, DTYPE_DOUBLE
// Y converted to float, X to long or float or double
    err = 0;
    if(yXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    else if(xXd.pointer->class != CLASS_A)
	err = "Y is not an array";
    if(err)
    {
	MdsFree1Dx(&xXd, 0);
	MdsFree1Dx(&yXd, 0);
	errD.length = strlen(err);
	errD.pointer = err;
	MdsCopyDxXd(&errD, &retXd);
	return &retXd;
    }
//Number of samples set to minimum between X and Y
    xArrD = (struct descriptor_a *)xXd.pointer;
    yArrD = (struct descriptor_a *)yXd.pointer;
    nSamples = yArrD->arsize/yArrD->length;
    if(nSamples > xArrD->arsize/xArrD->length)
	nSamples = xArrD->arsize/xArrD->length;
 
    if(yArrD->dtype == DTYPE_FLOAT)
	y = (float *)yArrD->pointer;
    else
    {
	y = (float *)malloc(nSamples * sizeof(float));
	for(i = 0; i < nSamples; i++)
	{
	    switch(yArrD->dtype) {
	    	case DTYPE_B:
	    	case DTYPE_BU:
		    y[i] = *((char *)(&yArrD->pointer[i*yArrD->length]));
		break;
	    	case DTYPE_W:
	    	case DTYPE_WU:
		    y[i] = *((short *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_L:
	    	case DTYPE_LU:
		    y[i] = *((int *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_Q:
	    	case DTYPE_QU:
		    y[i] = *((long *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    	case DTYPE_DOUBLE:
		    y[i] = *((double *)(&yArrD->pointer[i*yArrD->length]));
		    break;
	    }
	}
    }
    compressDataLongX(y, (long *)xArrD->pointer, nSamples, *reqNSamples, *inXMin, *inXMax, &retSamples, &retResolution);

    retDataD.pointer = (char *)y;
    retDataD.arsize = sizeof(float ) * retSamples;
    retDimD.length = xArrD->length;
    retDimD.dtype = xArrD->dtype;
    retDimD.pointer = xArrD->pointer;
    retDimD.arsize = retSamples * xArrD->length;

    MdsCopyDxXd((struct descriptor *)&retSignalD, &retXd);
    MdsFree1Dx(&xXd, 0);
    MdsFree1Dx(&yXd, 0);
    return &retXd;
}
