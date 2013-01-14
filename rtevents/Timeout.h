#ifndef TIMEOUT_H_
#define TIMEOUT_H_
#include "SystemSpecific.h"
class EXPORT Timeout
{
	long secs;
	long nanoSecs;
public:
	Timeout(long secs, long nanoSecs)
	{
		this->secs = secs + nanoSecs / 1000000000;	
		this->nanoSecs = nanoSecs % 1000000000;
	}
	Timeout(long milliSecs)
	{
		this->secs = milliSecs / 1000;	
		this->nanoSecs = (milliSecs % 1000) * 1000000;
	}
	long getSecs(){return secs;}	
	long getNanoSecs(){return nanoSecs;}
	long getTotMilliSecs(){return secs * 1000 + nanoSecs / 1000000;}
};


#endif /*TIMEOUT_H_*/
