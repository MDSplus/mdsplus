#ifndef TIMEOUT_H_
#define TIMEOUT_H_
class Timeout
{
	long secs;
	long nanoSecs;
public:
	Timeout(long secs, long nanoSecs)
	{
		this->secs = secs + nanoSecs / 1000000000;	
		this->nanoSecs = nanoSecs % 1000000000;
	}
	long getSecs(){return secs;}	
	long getNanoSecs(){return nanoSecs;}
};


#endif /*TIMEOUT_H_*/
