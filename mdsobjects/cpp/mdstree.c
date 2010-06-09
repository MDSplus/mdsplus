#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <libroutines.h>
#include <mdstypes.h>
#include <cacheshr.h>



#ifndef HAVE_WINDOWS_H
#include <mdstypes.h>
#endif
#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif



 
extern void *convertDataToDsc(void *data);
extern void *convertFromDsc(void *dscPtr, void *tree);
extern void freeDsc(void *dscPtr);

/*
extern int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, 
							struct descriptor *dim, struct descriptor_a *initialData, int idx);
extern int TreePutSegment(int nid, int rowidx, struct descriptor_a *data);
extern int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dim, int idx);
extern int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx);
extern int TreePutTimestampedSegment(int nid, _int64 *timestamp, struct descriptor_a *rowdata);
extern int TreePutRow(int nid, int bufsize, _int64 *timestamp, struct descriptor_a *rowdata);
extern int TreeSetTimeContext( struct descriptor *start, struct descriptor *end, struct descriptor *delta);
extern int TreeGetNumSegments(int nid, int *num);
extern int TreeGetSegmentLimits(int nid, int segidx, struct descriptor_xd *start, struct descriptor_xd *end);
extern int TreeGetSegment(int nid, int segidx, struct descriptor_xd *data, struct descriptor_xd *dim);
*/

 int getTreeData(void *dbid, int nid, void **data, void *tree, int isCached);
 int putTreeData(void *dbid, int nid, void *data, int isCached);
 int deleteTreeData(void *dbid, int nid, int isCached, int cachePolicy);
 int doTreeMethod(void *dbid, int nid, char *method);
 int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
 int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int rowsFilled, int isCached, int cachePolicy);

 int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs, int isCached, int cachePolicy);
 int updateTreeSegment(void *dbid, int nid, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
 int getTreeNumSegments(void *dbid, int nid, int *numSegments, int isCached); 
 int getTreeSegmentLimits(void *dbid, int nid, void **startDsc, void **endDsc, int isCached);
 int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, void **timesDsc, int isCached);
 int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc);//No cache option  
 int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int isCached, int cachePolicy);
 int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times, int rowsFilled, int isCached, int cachePolicy);
 int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times, int isCached, int cachePolicy);
 int putTreeRow(void *dbid, int nid, void *dataDsc, _int64 *time, int size, int isCached, int isLast, int cachePolicy);


  int getTreeData(void *dbid, int nid, void **data, void *tree, int isCached)
{
	EMPTYXD(xd);
	int status;

	if(isCached)
		status = _RTreeGetRecord(dbid, nid, &xd);
	else
		status = _TreeGetRecord(dbid, nid, &xd);
	if(!(status & 1)) return status;

	*data = convertFromDsc(&xd, tree);
	MdsFree1Dx(&xd, 0);
	return status;
}

 int putTreeData(void *dbid, int nid, void *data, int isCached)
{
	struct descriptor_xd *xdPtr;
	int status;

	xdPtr = (struct descriptor_xd *)convertDataToDsc(data);
	if(isCached)
		status = _RTreePutRecord(dbid, nid, (struct descriptor *)xdPtr, MDS_WRITE_BACK);
	else
		status = _TreePutRecord(dbid, nid, (struct descriptor *)xdPtr, 0);
	freeDsc(xdPtr);	
	return status;
}

 int deleteTreeData(void *dbid, int nid, int isCached, int cachePolicy)
{
	EMPTYXD(xd);
	int status;

	if(isCached)
		status = _RTreePutRecord(dbid, nid, (struct descriptor *)&xd, cachePolicy);
	else
		status = _TreePutRecord(dbid, nid, (struct descriptor *)&xd, 0);
	return status;
}

	
 void convertTime(int *time, char *retTime)
{
	char timeStr[512];
	int retLen;
	struct descriptor time_dsc = {511, DTYPE_T, CLASS_S, timeStr};

	LibSysAscTim(&retLen, &time_dsc,time);
	timeStr[retLen] = 0;
	strcpy(retTime, timeStr);
}


 int doTreeMethod(void *dbid, int nid, char *method)
{
	struct descriptor nidD = {sizeof(int), DTYPE_NID, CLASS_S, (char *)&nid};
	struct descriptor methodD = {strlen(method), DTYPE_T, CLASS_S, method};

	return TreeDoMethod(dbid, &nidD, &methodD);
}



int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, void *dimDsc, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
	struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
	struct descriptor_xd *dimXd = (struct descriptor_xd *)dimDsc;
	int status; 
	
	if(isCached)
	{
		status = _RTreeBeginSegment(dbid, nid, startXd->pointer, endXd->pointer, dimXd->pointer, 
			(struct descriptor_a *)dataXd->pointer, -1, cachePolicy);
//		if(status & 1) status = _RTreePutSegment(dbid, nid, -1, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}

	else
		status = _TreeBeginSegment(dbid, nid, startXd->pointer, endXd->pointer, dimXd->pointer, 
			(struct descriptor_a *)dataXd->pointer, -1);

	freeDsc(dataXd);
	freeDsc(startXd);
	freeDsc(endXd);
	freeDsc(dimXd);


	return status;
}

int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, void *dimDsc, int rowsFilled, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
	struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
	struct descriptor_xd *dimXd = (struct descriptor_xd *)dimDsc;
	int status, nRows; 
	
	if(isCached)
	{
		status = _RTreeBeginSegment(dbid, nid, startXd->pointer, endXd->pointer, dimXd->pointer, 
			(struct descriptor_a *)dataXd->pointer, -1, cachePolicy);
		if(status & 1) status = _RTreePutSegment(dbid, nid, -1, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}

	else
		status = _TreeMakeSegment(dbid, nid, startXd->pointer, endXd->pointer, dimXd->pointer, 
			(struct descriptor_a *)dataXd->pointer, -1, rowsFilled);

	freeDsc(dataXd);
	freeDsc(startXd);
	freeDsc(endXd);
	freeDsc(dimXd);


	return status;
}

#define PUTROW_BUFSIZE 1000
 int putTreeRow(void *dbid, int nid, void *dataDsc, _int64 *time, int size, int isCached, int isLast, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
	{
		if(cachePolicy == MDS_WRITE_BUFFER)
			status = _RTreePutRow(dbid, nid, size, time, (struct descriptor_a *)dataXd->pointer, (isLast)?MDS_WRITE_LAST:MDS_WRITE_BUFFER);
		else
			status = _RTreePutRow(dbid, nid, size, time, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}
	else
		status = _TreePutRow(dbid, nid, size, time, (struct descriptor_a *)dataXd->pointer);
	freeDsc(dataXd);
	return status;
}


 int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
	{
		status = _RTreePutSegment(dbid, nid, ofs, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}
	else
		status = _TreePutSegment(dbid, nid, ofs, (struct descriptor_a *)dataXd->pointer);
	freeDsc(dataXd);
	return status;
}



 int updateTreeSegment(void *dbid, int nid, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy)
{
	struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
	struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
	struct descriptor_xd *timeXd = (struct descriptor_xd *)timeDsc;
	int status;

	if(isCached)
	{
		status = _RTreeUpdateSegment(dbid, nid, (struct descriptor *)startXd->pointer, 
			(struct descriptor *)endXd->pointer, (struct descriptor *)timeXd->pointer, -1, cachePolicy);
	}
	else
		status = _TreeUpdateSegment(dbid, nid, (struct descriptor *)startXd->pointer, 
			(struct descriptor *)endXd->pointer, (struct descriptor *)timeXd->pointer, -1);
	freeDsc(startXd);
	freeDsc(endXd);
	freeDsc(timeXd);
	return status;
}

 int getTreeNumSegments(void *dbid, int nid, int *numSegments, int isCached)
{
	if(isCached)
		return _RTreeGetNumSegments(dbid, nid, numSegments);
	else
		return _TreeGetNumSegments(dbid, nid, numSegments);
}

 int getTreeSegmentLimits(void *dbid, int nid, void **startPtr, void **endPtr, int isCached)
{
	struct descriptor_xd *startXd, *endXd;
	EMPTYXD(emptyXd);

	startXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
	*startXd = emptyXd;
	endXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
	*endXd = emptyXd;

	*startPtr = startXd;
	*endPtr = endXd;
	if(isCached)
		return _RTreeGetSegmentLimits(dbid, nid, -1, startXd, endXd);
	else
		return _TreeGetSegmentLimits(dbid, nid, -1, startXd, endXd);
}




 int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, void **timeDsc, int isCached)
{
	EMPTYXD(emptyXd);
	struct descriptor_xd *dataXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
	struct descriptor_xd *timeXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));

	*dataXd = emptyXd;
	*dataDsc = dataXd;
	*timeXd = emptyXd;
	*timeDsc = timeXd;

	if(isCached)
		return _RTreeGetSegment(dbid, nid, segIdx, dataXd, timeXd);
	else
		return _TreeGetSegment(dbid, nid, segIdx, dataXd, timeXd);
}


 int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc)
{
	int status;

	status = TreeSetTimeContext((struct descriptor *)startDsc, (struct descriptor *)endDsc, (struct descriptor *)deltaDsc);
	if(startDsc)
		freeDsc(startDsc);
	if(endDsc)
		freeDsc(endDsc);
	if(deltaDsc)
		freeDsc(deltaDsc);

	return status;


}
 int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
		status = _RTreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)dataXd->pointer, -1, cachePolicy);
	else
		status = _TreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)dataXd->pointer, -1);
	freeDsc(dataXd);
	return status;
}

 extern int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
		status = _RTreePutTimestampedSegment(dbid, nid, times, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	else
		status = _TreePutTimestampedSegment(dbid, nid, times, (struct descriptor_a *)dataXd->pointer);
	freeDsc(dataXd);
	return status;
}
 extern int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times, int rowsFilled, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
	{
		status = _RTreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)dataXd->pointer, -1, cachePolicy);
		if(status & 1) status = _RTreePutTimestampedSegment(dbid, nid, times, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}
	else
		status = _TreeMakeTimestampedSegment(dbid, nid, times, (struct descriptor_a *)dataXd->pointer, -1, rowsFilled);
	freeDsc(dataXd);
	return status;
}
