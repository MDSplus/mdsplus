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
#include <string.h>
#include <mdsshr.h>
#include <mdstypes.h>

EXPORT int XTreeConvertToLongTime(struct descriptor *timeD, uint64_t * retTime);
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
