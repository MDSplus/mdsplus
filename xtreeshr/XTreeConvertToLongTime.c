#include <stdio.h>
#include <mdstypes.h>
#include <xtreeshr.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>

extern int TdiData();
extern int TdiFloat();
//Convert a time expression to 64 bit integer 

int XTreeConvertToLongTime(struct descriptor *timeD, _int64u *retTime)
{	

	int status;
	EMPTYXD(xd);


	status = TdiData(timeD, &xd MDS_END_ARG);
	if(!(status & 1))
		return status;
	
	if(!xd.pointer || xd.pointer->class != CLASS_S)
	{
		MdsFree1Dx(&xd, 0);
		return 0;//InvalidTimeFormat
	}

	if(xd.pointer->dtype == DTYPE_Q && xd.pointer->dtype == DTYPE_QU)
	{
		*retTime = *(_int64 *)xd.pointer->pointer;
		MdsFree1Dx(&xd, 0);
		return 1;
	}

	//Not a 64 bit integer, try to convert it to a float and use MdsFloatToTime
	status = TdiFloat(&xd, &xd MDS_END_ARG);
	if(!(status & 1))
		return status;
	
	status =  MdsFloatToTime(xd.pointer, retTime);
	MdsFree1Dx(&xd, 0);
	return status;
}

