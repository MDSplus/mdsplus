#ifndef DELAY_H_
#define DELAY_H_
#include <time.h>

class Delay
{
	struct timespec delayTime;
public:
	Delay(int milliSecs)
	{
		delayTime.tv_sec = milliSecs / 1000;
		delayTime.tv_nsec = (milliSecs % 1000)*1000000;
	}
	void wait()
	{
		struct timespec remTime;
		int status = nanosleep(&delayTime, &remTime);
		if(status)
			printf("Error in nanosleep\n");
	}
};
#endif /*DELAY_H_*/
