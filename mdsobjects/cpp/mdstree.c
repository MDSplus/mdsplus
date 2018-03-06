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
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <libroutines.h>
#include <mdstypes.h>

#ifndef _WIN32
#include <mdstypes.h>
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
extern int TreePutTimestampedSegment(int nid, int64_t *timestamp, struct descriptor_a *rowdata);
extern int TreePutRow(int nid, int bufsize, int64_t *timestamp, struct descriptor_a *rowdata);
extern int _TreeSetTimeContext(void* dbid, struct descriptor *start, struct descriptor *end, struct descriptor *delta);
extern int TreeGetNumSegments(int nid, int *num);
extern int TreeGetSegmentLimits(int nid, int segidx, struct descriptor_xd *start, struct descriptor_xd *end);
extern int TreeGetSegment(int nid, int segidx, struct descriptor_xd *data, struct descriptor_xd *dim);
*/

int getTreeData(void *dbid, int nid, void **data, void *tree);
int putTreeData(void *dbid, int nid, void *data);
int deleteTreeData(void *dbid, int nid);
int doTreeMethod(void *dbid, int nid, char *method);
int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc,
		     void *timeDsc);
int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc,
		    void *timeDsc, int rowsFilled);

int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs);
int updateTreeSegment(void *dbid, int nid, int segIdx, void *startDsc, void *endDsc, void *timeDsc);
int getTreeNumSegments(void *dbid, int nid, int *numSegments);
int getTreeSegmentLimits(void *dbid, int nid, int idx, void **startDsc, void **endDsc);
int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, void **timesDsc);
int setTreeTimeContext(void* dbid, void *startDsc, void *endDsc, void *deltaDsc);
int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc);
int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int64_t * times, int rowsFilled);
int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int64_t * times);
int putTreeRow(void *dbid, int nid, void *dataDsc, int64_t * time, int size);
int setTreeXNci(void *dbid, int nid, const char *name, void *dataDsc);

int getTreeData(void *dbid, int nid, void **data, void *tree)
{
  EMPTYXD(xd);
  int status;

  status = _TreeGetRecord(dbid, nid, &xd);
  if (!(status & 1))
    return status;

  *data = convertFromDsc(&xd, tree);
  MdsFree1Dx(&xd, 0);
  return status;
}

int putTreeData(void *dbid, int nid, void *data)
{
  struct descriptor_xd *xdPtr;
  int status;

  xdPtr = (struct descriptor_xd *)convertDataToDsc(data);
  status = _TreePutRecord(dbid, nid, (struct descriptor *)xdPtr, 0);
  freeDsc(xdPtr);
  return status;
}

int deleteTreeData(void *dbid, int nid)
{
  EMPTYXD(xd);
  int status;

  status = _TreePutRecord(dbid, nid, (struct descriptor *)&xd, 0);
  return status;
}

void convertTime(int *time, char *retTime)
{
  char timeStr[512];
  unsigned short retLen;
  struct descriptor time_dsc = { 511, DTYPE_T, CLASS_S, timeStr };

  LibSysAscTim(&retLen, &time_dsc, time);
  timeStr[retLen] = 0;
  strcpy(retTime, timeStr);
}

int doTreeMethod(void *dbid, int nid, char *method)
{
  struct descriptor nidD = { sizeof(int), DTYPE_NID, CLASS_S, (char *)&nid };
  struct descriptor methodD = { strlen(method), DTYPE_T, CLASS_S, method };

  return TreeDoMethod(dbid, &nidD, &methodD);
}

int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, void *dimDsc)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
  struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
  struct descriptor_xd *dimXd = (struct descriptor_xd *)dimDsc;
  int status;

  status = _TreeBeginSegment(dbid, nid, startXd->pointer, endXd->pointer, dimXd->pointer,
			     (struct descriptor_a *)dataXd->pointer, -1);

  freeDsc(dataXd);
  freeDsc(startXd);
  freeDsc(endXd);
  freeDsc(dimXd);

  return status;
}

int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, void *dimDsc,
		    int rowsFilled)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
  struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
  struct descriptor_xd *dimXd = (struct descriptor_xd *)dimDsc;
  int status;

  status = _TreeMakeSegment(dbid, nid, startXd->pointer, endXd->pointer, dimXd->pointer,
			    (struct descriptor_a *)dataXd->pointer, -1, rowsFilled);

  freeDsc(dataXd);
  freeDsc(startXd);
  freeDsc(endXd);
  freeDsc(dimXd);

  return status;
}

#define PUTROW_BUFSIZE 1000
int putTreeRow(void *dbid, int nid, void *dataDsc, int64_t * time, int size)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  int status;

  status = _TreePutRow(dbid, nid, size, time, (struct descriptor_a *)dataXd->pointer);
  freeDsc(dataXd);
  return status;
}

int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  int status;

  status = _TreePutSegment(dbid, nid, ofs, (struct descriptor_a *)dataXd->pointer);
  freeDsc(dataXd);
  return status;
}

int updateTreeSegment(void *dbid, int nid, int segIdx, void *startDsc, void *endDsc, void *timeDsc)
{
  struct descriptor_xd *startXd = (struct descriptor_xd *)startDsc;
  struct descriptor_xd *endXd = (struct descriptor_xd *)endDsc;
  struct descriptor_xd *timeXd = (struct descriptor_xd *)timeDsc;
  int status, numSegments, segmentIdx;

  if (segIdx == -1) {
    status = _TreeGetNumSegments(dbid, nid, &numSegments);
    if (!(status & 1))
      return status;
    segmentIdx = numSegments - 1;
  } else
    segmentIdx = segIdx;
  status = _TreeUpdateSegment(dbid, nid, (struct descriptor *)startXd->pointer,
			      (struct descriptor *)endXd->pointer,
			      (struct descriptor *)timeXd->pointer, segmentIdx);
  freeDsc(startXd);
  freeDsc(endXd);
  freeDsc(timeXd);
  return status;
}

int getTreeNumSegments(void *dbid, int nid, int *numSegments)
{
  return _TreeGetNumSegments(dbid, nid, numSegments);
}

int getTreeSegmentLimits(void *dbid, int nid, int idx, void **startPtr, void **endPtr)
{
  struct descriptor_xd *startXd, *endXd;
  EMPTYXD(emptyXd);

  startXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *startXd = emptyXd;
  endXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *endXd = emptyXd;

  *startPtr = startXd;
  *endPtr = endXd;
  return _TreeGetSegmentLimits(dbid, nid, idx, startXd, endXd);
}

int getTreeSegmentInfo(void *dbid, int nid, int segIdx, char *dtype, char *dimct, int *dims,
		       int *nextRow)
{
  return _TreeGetSegmentInfo(dbid, nid, segIdx, dtype, dimct, dims, nextRow);
}

int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, void **timeDsc)
{
  EMPTYXD(emptyXd);
  struct descriptor_xd *timeXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *timeXd = emptyXd;
  *timeDsc = timeXd;
  struct descriptor_xd *dataXd;
  if (dataDsc) {
    dataXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
    *dataXd = emptyXd;
    *dataDsc = dataXd;
  } else dataXd = NULL;
  return _TreeGetSegment(dbid, nid, segIdx, dataXd, timeXd);
}

int setTreeTimeContext(void* dbid, void *startDsc, void *endDsc, void *deltaDsc)
{
  int status;

  status =
      _TreeSetTimeContext(dbid, (struct descriptor *)startDsc, (struct descriptor *)endDsc,
			 (struct descriptor *)deltaDsc);
  if (startDsc)
    freeDsc(startDsc);
  if (endDsc)
    freeDsc(endDsc);
  if (deltaDsc)
    freeDsc(deltaDsc);

  return status;

}

int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  int status;

  status = _TreeBeginTimestampedSegment(dbid, nid, (struct descriptor_a *)dataXd->pointer, -1);
  freeDsc(dataXd);
  return status;
}

int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int64_t * times)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  int status;

  status = _TreePutTimestampedSegment(dbid, nid, times, (struct descriptor_a *)dataXd->pointer);
  freeDsc(dataXd);
  return status;
}

int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int64_t * times,
				      int rowsFilled)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  int status;

  status =
      _TreeMakeTimestampedSegment(dbid, nid, times, (struct descriptor_a *)dataXd->pointer, -1,
				  rowsFilled);
  freeDsc(dataXd);
  return status;
}

int setTreeXNci(void *dbid, int nid, const char *name, void *dataDsc)
{
  struct descriptor_xd *dataXd = (struct descriptor_xd *)dataDsc;
  int status;

  status = _TreeSetXNci(dbid, nid, name, (struct descriptor *)dataXd);
	freeDsc(dataXd);
  return status;
}

int getTreeSegmentScale(void *dbid, int nid, void **sclDsc)
{
  EMPTYXD(emptyXd);
  struct descriptor_xd *sclXd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
  *sclXd  = emptyXd;
  *sclDsc = sclXd;
  return _TreeGetSegmentScale(dbid, nid, sclXd);
}
int setTreeSegmentScale(void *dbid, int nid, void *sclDsc)
{
  struct descriptor_xd *sclXd = (struct descriptor_xd *)sclDsc;
  int status = _TreeSetSegmentScale(dbid, nid, sclXd->pointer);
  freeDsc(sclXd);
  return status;
}
