#include <stdio.h>
#include <mdstypes.h>
#include <xtreeshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <tdishr.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#endif

//Convert a time expression to 64 bit integer 

EXPORT int XTreeConvertToLongTime(struct descriptor *timeD, uint64_t * retTime)
{

  int status;
  EMPTYXD(xd);

  status = TdiData(timeD, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;

  if (!xd.pointer || xd.pointer->class != CLASS_S) {
    MdsFree1Dx(&xd, 0);
    return 0;			//InvalidTimeFormat
  }

  if (xd.pointer->dtype == DTYPE_Q || xd.pointer->dtype == DTYPE_QU) {
    *retTime = *(int64_t *) xd.pointer->pointer;
    MdsFree1Dx(&xd, 0);
    return 1;
  }
  //Not a 64 bit integer, try to convert it to a float and use MdsFloatToTime
  status = TdiFloat((struct descriptor *)&xd, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;

  if (xd.pointer->dtype == DTYPE_DOUBLE)
    MdsFloatToTime(*(double *)xd.pointer->pointer, retTime);
  else
    MdsFloatToTime(*(float *)xd.pointer->pointer, retTime);

//      MdsFloatToTime(*(float *)xd.pointer->pointer, retTime);

  MdsFree1Dx(&xd, 0);
  return status;
}

EXPORT int XTreeConvertToLongDelta(struct descriptor *timeD, uint64_t * retTime)
{

  int status;
  EMPTYXD(xd);

  status = TdiData(timeD, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;

  if (!xd.pointer || xd.pointer->class != CLASS_S) {
    MdsFree1Dx(&xd, 0);
    return 0;			//InvalidTimeFormat
  }

  if (xd.pointer->dtype == DTYPE_Q || xd.pointer->dtype == DTYPE_QU) {
    *retTime = *(int64_t *) xd.pointer->pointer;
    MdsFree1Dx(&xd, 0);
    return 1;
  }
  //Not a 64 bit integer, try to convert it to a float and use MdsFloatToTime
  status = TdiFloat((struct descriptor *)&xd, &xd MDS_END_ARG);
  if (!(status & 1))
    return status;
  if (xd.pointer->dtype == DTYPE_DOUBLE)
    MdsFloatToDelta(*(double *)xd.pointer->pointer, retTime);
  else
    MdsFloatToDelta(*(float *)xd.pointer->pointer, retTime);
  MdsFree1Dx(&xd, 0);
  return status;
}
