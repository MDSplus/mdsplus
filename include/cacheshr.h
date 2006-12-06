#include <mdsdescrip.h>

extern int RTreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dimension, 
							 struct descriptor_a *initialValue, int idx, int writeThrough);
extern int RTreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, 
							  struct descriptor *dimension, int idx, int writeThrough);
extern int RTreePutSegment(int nid, int idx, struct descriptor *dataD, int segIdx);
extern int RTreeGetNumSegments(int nid, int *numSegments);
extern int RTreeGetSegment(int nid, int idx, struct descriptor_xd *retData, struct descriptor_xd *retDim);
extern int RTreeGetSegmentLimits(int nid, int idx, struct descriptor_xd *retStart, struct descriptor_xd *retEnd);
extern int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int writeThrough);
extern int RTreeFlush();
extern void *RTreeSetCallback(int nid, void (*callback)(int));
extern void RTreeClearCallback(int nid, void *callbackDescr);
extern int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr);
extern void cacheReset();

extern void RTreeSetGetRecord(int (*treeGetRecordIn)(int nid, struct descriptor_xd *outXd));
extern void RTreeSetPutRecord(int (*treePutRecordIn)(int nid, struct descriptor *data, int));
extern void RTreeSetGetTimedRecord(int (*getTimedRecordIn)(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *minDelta,
							 struct descriptor_xd *outSig));
