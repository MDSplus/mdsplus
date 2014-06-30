#ifndef THREAD_H_
#define THREAD_H_
#include "SystemSpecific.h"
#ifdef _MSC_VER
#include "WindowsThread.h"
#else
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include "Runnable.h"
#include "ThreadAttributes.h"
struct  WithArg{
	Runnable *rtn;
	void *arg;
	bool deallocate;
	int stackSize;
	bool useAttr;
	int priority;
	int policy;
	int cpuMask[CPU_MASK_SIZE];

};
extern  "C" void handlerWithArg(WithArg *);

class EXPORT Thread
{
	pthread_t thread;
	ThreadAttributes *attributes;
public:
	Thread()
	{
		attributes = NULL;
	}
	Thread(ThreadAttributes *attr)
	{
		if(attr)
		{
			attributes = new ThreadAttributes;
			*attributes = *attr;
		}
		else
			attributes = NULL;
	}
	~Thread()
	{
		if(attributes)
			delete attributes;
	}
	void setAttributes(ThreadAttributes *attr)
	{
		attributes = new ThreadAttributes;
		*attributes = *attr;
	}
	void start(Runnable *rtn, void *arg = 0, bool deallocate = true);
	void join();
	friend void handlerWithArg(WithArg *rtn);
};



#endif
#endif /*THREAD_H_*/
