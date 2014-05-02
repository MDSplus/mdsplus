#ifndef TIMEOUT_H_
#define TIMEOUT_H_
#include "SystemSpecific.h"

#include <time.h>

class EXPORT MdsTimeout
{
public:
#ifdef HAVE_WINDOWS_H
	typedef timeval Time;
#else
	typedef timespec Time;
#endif

	MdsTimeout(long sec, long nSec) {
		set(sec + nSec / 1000000000, nSec % 1000000000);
	}

	MdsTimeout(long milliSecs) {
		set(milliSecs / 1000, (milliSecs % 1000) * 1000000);
	}

	Time getFutureTime() {
		timespec result;
		if(clock_gettime(CLOCK_REALTIME, &result) < 0) {
			// FIXME: error
		}
		result.tv_sec += timeout.tv_sec;
		result.tv_nsec += timeout.tv_nsec;
		if (result.tv_nsec >= 1000000000L) {
			++result.tv_sec;
			result.tv_nsec -= 1000000000L;
		}
		return result;
	}

	timespec getDuration() { return ts; }

	long getSecs() { return timeout.tv_sec; }
	long getNanoSecs() { return timeout.tv_nsec; }
	long getTotMilliSecs() { return timeout.tv_sec * 1000 + timeout.tv_nsec / 1000000; }

	virtual ~MdsTimeout();

private:
	Time timeout;

	void set(long sec, long nSec) {
		timeout.tv_sec = sec;
		timeout.tv_nsec = nSec;
	}
};


class EXPORT WinTimeout: public MdsTimeout {
public:
	WinTimeout() {
	}

};


#endif /*TIMEOUT_H_*/
