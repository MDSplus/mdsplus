#include <mdsdescrip.h>
#include <mdstypes.h>
#include <string.h>
#include <mdsshr.h>

//Conversion to Float/Doublee to 64 bit int.

//MDSplus specific time conversion. It is the number of nanoseconds 1 day before time 0.

void MdsFloatToTime(double floatTime, _int64u *outTime)
{
	_int64u baseTime, currTime;

	baseTime = 1000000000;
	baseTime *= (24 * 3600);
	currTime = (_int64u)(floatTime / 1E-9);
	*outTime = baseTime + currTime;
}

void MdsFloatToDelta(double floatTime, _int64u *outTime)
{
	_int64 currTime;

	currTime = (_int64)(floatTime / 1E-9);
	*outTime = currTime;
}


void MdsTimeToFloat(_int64u inTime, float *outFloat)
{
	_int64 baseTime, currTime;

	baseTime = 1000000000;
	baseTime *= (24 * 3600);
	currTime = inTime - baseTime;
	*outFloat = (float)((double)currTime * 1E-9);
}

void MdsTimeToDouble(_int64u inTime, double *outFloat)
{
	_int64 baseTime, currTime;

	baseTime = 1000000000;
	baseTime *= (24 * 3600);
	currTime = inTime - baseTime;
	*outFloat = (double)currTime * 1E-9;
}











