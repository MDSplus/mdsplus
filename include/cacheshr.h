#include <mdsdescrip.h>

#define WRITE_THROUGH 1
#define WRITE_BACK 2
#define WRITE_BUFFER 3
#define WRITE_LAST 4

extern int RTreeOpen(char *expName, int shot);
extern int RTreeClose(char *expName, int shot);
extern int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode);
extern void RTreeSetShotNum(int inShotNum);
extern int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeMode);
extern int RTreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx, int writeMode);
extern int RTreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, 
							  struct descriptor *dimension, int idx, int writeMode);
extern int RTreePutSegment(int nid, nt segIdx, struct descriptor *dataD, iint writeMode);
extern int RTreePutTimestampedSegment(int nid, struct descriptor *dataD, _int64 *timestamps, int writeMode);
extern int RTreePutRow(int nid, int bufSize, _int64 *timestamp, struct descriptor_a *rowD, int writeMode);
extern int RTreeGetNumSegments(int nid, int *numSegments);
extern int RTreeGetSegment(int nid, int idx, struct descriptor_xd *retData, struct descriptor_xd *retDim);
extern int RTreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd);
extern int RTreeGetSegmentInfo(int nid, char *dtype, char *dimct, int *dims, int *leftItems, int *leftRows);
extern int RTreeDiscardOldSegments(int nid, _int64 timestamp);
extern int RTreeDiscardData(int nid);
extern int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeMode);
extern int RTreeFlush();
extern int RTreeFlushNode(int nid);
extern char *RTreeSetCallback(int nid, void (*callback)(int));
extern int RTreeClearCallback(int nid, char *callbackDescr);
extern int RTreeSetWarm(int nid, int warm);
extern int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr);
extern void RTreeCacheReset();
extern void RTreeCacheSynch();
