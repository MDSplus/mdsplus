#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <libroutines.h>
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
extern void *convertFromDsc(void *dscPtr);
extern void freeDsc(void *dscPtr);


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


EXPORT int getTreeData(int nid, void **data, int isCached);
EXPORT int putTreeData(int nid, void *data, int isCached);
EXPORT int deleteTreeData(int nid);
EXPORT int doTreeMethod(int nid, char *method);
EXPORT int beginTreeSegment(int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);

EXPORT int putTreeSegment(int nid, void *dataDsc, int ofs, int isCached, int cachePolicy);
EXPORT int updateTreeSegment(int nid, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
EXPORT int getTreeNumSegments(int nid, int *numSegments, int isCached); 
EXPORT int getTreeSegmentLimits(int nid, void **startDsc, void **endDsc, int isCached);
EXPORT int getTreeSegment(int nid, int segIdx, void **dataDsc, void **timesDsc, int isCached);
EXPORT int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc);//No cache option  
EXPORT int beginTreeTimestampedSegment(int nid, void *dataDsc, int isCached, int cachePolicy);
EXPORT int putTreeTimestampedSegment(int nid, void *dataDsc, _int64 *times, int isCached, int cachePolicy);
EXPORT int putTreeRow(int nid, void *dataDsc, _int64 *time, int isCached, int isLast, int cachePolicy);


EXPORT  int getTreeData(int nid, void **data, int isCached)
{
	EMPTYXD(xd);
	int status;

	if(isCached)
		status = RTreeGetRecord(nid, &xd);
	else
		status = TreeGetRecord(nid, &xd);
	if(!(status & 1)) return status;

	*data = convertFromDsc(&xd);
	MdsFree1Dx(&xd, 0);
	return status;
}

EXPORT int putTreeData(int nid, void *data, int isCached)
{
	struct descriptor_xd *xdPtr;
	int status;

	xdPtr = (struct descriptor_xd *)convertDataToDsc(data);
	if(isCached)
		status = RTreePutRecord(nid, (struct descriptor *)xdPtr, WRITE_BACK);
	else
		status = TreePutRecord(nid, (struct descriptor *)xdPtr, 0);
	freeDsc(xdPtr);	
	return status;
}

EXPORT int deleteTreeData(int nid)
{
	EMPTYXD(xd);
	int status;

	status = TreePutRecord(nid, (struct descriptor *)&xd, 0);
	return status;
}

	
EXPORT void convertTime(int *time, char *retTime)
{
	char timeStr[512];
	int retLen;
	struct descriptor time_dsc = {511, DTYPE_T, CLASS_S, timeStr};

	LibSysAscTim(&retLen, &time_dsc,time);
	timeStr[retLen] = 0;
	strcpy(retTime, timeStr);
}


EXPORT int doTreeMethod(int nid, char *method)
{
	struct descriptor nidD = {sizeof(int), DTYPE_NID, CLASS_S, (char *)&nid};
	struct descriptor methodD = {strlen(method), DTYPE_T, CLASS_S, method};

	return TreeDoMethod(&nidD, &methodD);
}


EXPORT int beginTreeSegment(int nid, void *dataDsc, void *startDsc, void *endDsc, void *dimDsc, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
	struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
	struct descriptor_xd *dimXd = (struct descriptor_xd *)dimDsc;
	int status; 
	
	if(isCached)
	{
		status = RTreeBeginSegment(nid, startXd->pointer, endXd->pointer, dimXd->pointer, 
			(struct descriptor_a *)dataXd->pointer, -1, cachePolicy);
		if(status & 1) status = RTreePutSegment(nid, -1, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}

	else
		status = TreeBeginSegment(nid, startXd->pointer, endXd->pointer, dimXd->pointer, 
			(struct descriptor_a *)dataXd->pointer, -1);

	freeDsc(dataXd);
	freeDsc(startXd);
	freeDsc(endXd);
	freeDsc(dimXd);


	return status;
}

#define PUTROW_BUFSIZE 1000
EXPORT int putTreeRow(int nid, void *dataDsc, _int64 *time, int isCached, int isLast, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
	{
		status = RTreePutRow(nid, PUTROW_BUFSIZE, time, (struct descriptor_a *)dataXd->pointer, (isLast)?WRITE_LAST:WRITE_BUFFER);
	}
	else
		status = TreePutRow(nid, PUTROW_BUFSIZE, time, (struct descriptor_a *)dataXd->pointer);
	freeDsc(dataXd);
	return status;
}


EXPORT int putTreeSegment(int nid, void *dataDsc, int ofs, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
	{
		status = RTreePutSegment(nid, ofs, (struct descriptor_a *)dataXd->pointer, cachePolicy);
	}
	else
		status = TreePutSegment(nid, ofs, (struct descriptor_a *)dataXd->pointer);
	freeDsc(dataXd);
	return status;
}



EXPORT int updateTreeSegment(int nid, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy)
{
	struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
	struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
	struct descriptor_xd *timeXd = (struct descriptor_xd *)timeDsc;
	int status;

	if(isCached)
	{
		status = RTreeUpdateSegment(nid, (struct descriptor *)startXd->pointer, 
			(struct descriptor *)endXd->pointer, (struct descriptor *)timeXd->pointer, -1, cachePolicy);
	}
	else
		status = TreeUpdateSegment(nid, (struct descriptor *)startXd->pointer, 
			(struct descriptor *)endXd->pointer, (struct descriptor *)timeXd->pointer, -1);
	freeDsc(startXd);
	freeDsc(endXd);
	freeDsc(timeXd);
	return status;
}

EXPORT int getTreeNumSegments(int nid, int *numSegments, int isCached)
{
	if(isCached)
		return RTreeGetNumSegments(nid, numSegments);
	else
		return TreeGetNumSegments(nid, numSegments);
}

EXPORT int getTreeSegmentLimits(int nid, void **startPtr, void **endPtr, int isCached)
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
		return RTreeGetSegmentLimits(nid, -1, startXd, endXd);
	else
		return TreeGetSegmentLimits(nid, -1, startXd, endXd);
}




EXPORT int getTreeSegment(int nid, int segIdx, void **dataDsc, void **timeDsc, int isCached)
{
	EMPTYXD(emptyXd);
	struct descriptor_xd *dataXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
	struct descriptor_xd *timeXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));

	*dataXd = emptyXd;
	*dataDsc = dataXd;
	*timeXd = emptyXd;
	*timeDsc = timeXd;

	if(isCached)
		return RTreeGetSegment(nid, segIdx, dataXd, timeXd);
	else
		return TreeGetSegment(nid, segIdx, dataXd, timeXd);
}


EXPORT int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc)
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
EXPORT int beginTreeTimestampedSegment(int nid, void *dataDsc, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
		status = RTreeBeginTimestampedSegment(nid, (struct descriptor_a *)dataXd->pointer, -1, cachePolicy);
	else
		status = TreeBeginTimestampedSegment(nid, (struct descriptor_a *)dataXd->pointer, -1);
	freeDsc(dataXd);
	return status;
}

EXPORT extern int putTreeTimestampedSegment(int nid, void *dataDsc, _int64 *times, int isCached, int cachePolicy)
{
	struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
	int status;

	if(isCached)
		status = RTreePutTimestampedSegment(nid, (struct descriptor_a *)dataXd->pointer, times, cachePolicy);
	else
		status = TreePutTimestampedSegment(nid, times, (struct descriptor_a *)dataXd->pointer);
	freeDsc(dataXd);
	return status;
}
