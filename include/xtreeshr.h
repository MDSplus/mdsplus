#include <mdsdescrip.h>
#include <string.h>
#include <mdsshr.h>
#include <mdstypes.h>

EXPORT int XTreeConvertToLongTime(struct descriptor *timeD, int64_t * retTime);
EXPORT int XTreeGetTimedRecord(int nid, struct descriptor *startD, struct descriptor *endD,
			       struct descriptor *minDeltaD, struct descriptor_xd *outSignal);
EXPORT int XTreeDefaultResample(struct descriptor_signal *currSignal, struct descriptor *startD,
				struct descriptor *endD, struct descriptor *minDeltaD,
				struct descriptor_xd *outSignal);
EXPORT int XTreeDefaultSquish(struct descriptor_a *signalApd, struct descriptor *startD,
			      struct descriptor *endD, struct descriptor *minDeltaD,
			      struct descriptor_xd *outSignal);

//Temporary -- to me integrated into MDSplus error framework
#define InvalidTimeFormat 2
#define InvalidShapeInSegments 4
#define InvalidDimensionInSegments 6

//Temporary -- use CacheShr for segmented stuff for now
//#define TreeGetNumSegments RTreeGetNumSegments
//#define TreeGetSegmentLimits RTreeGetSegmentLimits
//#define TreeGetSegment RTreeGetSegment

//Temporary -- to be integrated in treeshr NCI
//Return an empty descriptor so the default squish and resample are used
//static int TreeGetXNci(int nid, char *nciname, struct descriptor_xd *retValue)
//{
//      static EMPTYXD(emptyXd);
//      MdsCopyDxXd((struct descriptor *)&emptyXd, retValue);
//      return 1;
//}
