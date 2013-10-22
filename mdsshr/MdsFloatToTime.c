#include <mdsdescrip.h>
#include <mdstypes.h>
#include <string.h>
#include <mdsshr.h>

//Conversion to Float/Doublee to 64 bit int.

//MDSplus specific time conversion. It is the number of nanoseconds 1 day before time 0.

void MdsFloatToTime(double floatTime, uint64_t *outTime)
{
	int64_t baseTime;
        int64_t currTime;

	baseTime = 1000000000;
	baseTime *= (24 * 3600);
	currTime = (int64_t)(floatTime / 1E-9);
	*outTime = (uint64_t)(baseTime + currTime);
}

void MdsFloatToDelta(double floatTime, uint64_t *outTime)
{
	int64_t currTime;

	currTime = (int64_t)(floatTime / 1E-9);
	*outTime = currTime;
}


void MdsTimeToFloat(uint64_t inTime, float *outFloat)
{
	int64_t baseTime, currTime;

	baseTime = 1000000000;
	baseTime *= (24 * 3600);
	currTime = inTime - baseTime;
	*outFloat = (float)((double)currTime * 1E-9);
}

void MdsTimeToDouble(uint64_t inTime, double *outFloat)
{
	int64_t baseTime, currTime;

	baseTime = 1000000000;
	baseTime *= (24 * 3600);
	currTime = inTime - baseTime;
	*outFloat = (double)currTime * 1E-9;
}











