#ifndef TIMEOUT_H_
#define TIMEOUT_H_
#include "SystemSpecific.h"

#include <time.h>

class EXPORT MdsTimeout
{
public:
	MdsTimeout(long sec, long nSec) {
		set(sec + nSec / 1000000000, nSec % 1000000000);
	}

	MdsTimeout(long milliSecs) {
		set(milliSecs / 1000, (milliSecs % 1000) * 1000000);
	}

	timespec getFutureTime() {
		timespec result;
		if(clock_gettime(CLOCK_REALTIME, &result) < 0) {
			// FIXME: error
		}
		result.tv_sec += ts.tv_sec;
		result.tv_nsec += ts.tv_nsec;
		if (result.tv_nsec >= 1000000000L) {
			++result.tv_sec;
			result.tv_nsec -= 1000000000L;
		}
		return result;
	}

	timespec getDuration() { return ts; }

	long getSecs() { return ts.tv_sec; }
	long getNanoSecs() { return ts.tv_nsec; }
	long getTotMilliSecs() { return ts.tv_sec * 1000 + ts.tv_nsec / 1000000; }

private:
	timespec ts;

	void set(long sec, long nSec) {
		ts.tv_sec = sec;
		ts.tv_nsec = nSec;
	}
};


#endif /*TIMEOUT_H_*/
