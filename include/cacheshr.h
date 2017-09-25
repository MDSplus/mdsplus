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
#include <mdsdescrip.h>

#define MDS_WRITE_THROUGH 1
#define MDS_WRITE_BACK 2
#define MDS_WRITE_BUFFER 3
#define MDS_WRITE_LAST 4

extern int RTreeOpen(char *expName, int shot);
extern int RTreeClose(char *expName, int shot);
extern int _RTreeOpen(void **dbid, char *expName, int shot);
extern int _RTreeClose(void *dbid, char *expName, int shot);
extern int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
			     struct descriptor *dimension, struct descriptor_a *initialValue,
			     int idx, int writeMode);
extern void RTreeSetShotNum(int inShotNum);
extern int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
			     struct descriptor *dimension, struct descriptor_a *initialValue,
			     int idx, int writeMode);
extern int RTreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx,
					int writeMode);
extern int RTreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end,
			      struct descriptor *dimension, int idx, int writeMode);
extern int RTreePutSegment(int nid, int segIdx, struct descriptor_a *dataD, int writeMode);
extern int RTreePutTimestampedSegment(int nid, int64_t * timestamps, struct descriptor_a *dataD,
				      int writeMode);
extern int RTreePutRow(int nid, int bufSize, int64_t * timestamp, struct descriptor_a *rowD,
		       int writeMode);
extern int RTreeGetNumSegments(int nid, int *numSegments);
extern int RTreeGetSegment(int nid, int idx, struct descriptor_xd *retData,
			   struct descriptor_xd *retDim);
extern int RTreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart,
				 struct descriptor_xd *retEnd);
extern int RTreeGetSegmentInfo(int nid, int idx, char *dtype, char *dimct, int *dims,
			       int *leftItems, int *leftRows);
extern int RTreeDiscardOldSegments(int nid, int64_t timestamp);
extern int RTreeDiscardData(int nid);
extern int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeMode);
extern int RTreeFlush();
extern int RTreeFlushNode(int nid);
extern int RTreeFlushNodeShot(char *expName, int shot, int nid);
extern char *RTreeSetCallback(int nid, void *argument, void (*callback) (int, void *));
extern int RTreeClearCallback(int nid, char *callbackDescr);
extern int RTreeSetWarm(int nid, int warm);
extern int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr);
extern void RTreeCacheReset();
extern void RTreeSynch();
extern void RTreeConfigure(int shared, int size);

extern int _RTreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
			      struct descriptor *dimension, struct descriptor_a *initialValue,
			      int idx, int writeMode);
extern int _RTreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end,
			      struct descriptor *dimension, struct descriptor_a *initialValue,
			      int idx, int writeMode);
extern int _RTreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue,
					 int idx, int writeMode);
extern int _RTreeUpdateSegment(void *dbid, int nid, struct descriptor *start,
			       struct descriptor *end, struct descriptor *dimension, int idx,
			       int writeMode);
extern int _RTreePutSegment(void *dbid, int nid, int segIdx, struct descriptor_a *dataD,
			    int writeMode);
extern int _RTreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamps,
				       struct descriptor_a *dataD, int writeMode);
extern int _RTreePutRow(void *dbid, int nid, int bufSize, int64_t * timestamp,
			struct descriptor_a *rowD, int writeMode);
extern int _RTreeGetNumSegments(void *dbid, int nid, int *numSegments);
extern int _RTreeGetSegment(void *dbid, int nid, int idx, struct descriptor_xd *retData,
			    struct descriptor_xd *retDim);
extern int _RTreeGetSegmentLimits(void *dbid, int nid, int idx, struct descriptor_xd *retStart,
				  struct descriptor_xd *retEnd);
extern int _RTreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims,
				int *leftItems, int *leftRows);
extern int _RTreeDiscardOldSegments(void *dbid, int nid, int64_t timestamp);
extern int _RTreeDiscardData(void *dbid, int nid);
extern int _RTreePutRecord(void *dbid, int nid, struct descriptor *descriptor_ptr, int writeMode);
extern int _RTreeFlush(void *dbid);
extern int _RTreeFlushNode(void *dbid, int nid);
extern int _RTreeFlushNodeShot(void *dbid, char *expName, int shot, int nid);
extern char *_RTreeSetCallback(void *dbid, int nid, void *argument, void (*callback) (int, void *));
extern int _RTreeClearCallback(void *dbid, int nid, char *callbackDescr);
extern int _RTreeGetRecord(void *dbid, int nid, struct descriptor_xd *dsc_ptr);
