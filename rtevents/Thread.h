#ifndef THREAD_H_
#define THREAD_H_
#ifdef HAVE_WINDOWS_H
#include "WindowsThread.h"
#else
#include <pthread.h>
#include "Runnable.h"
#include "ThreadAttributes.h"
struct  WithArg{
	Runnable *rtn;
	void *arg;
};
extern  "C" void handlerWithArg(WithArg *);

class Thread
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
	void start(Runnable *rtn, void *arg = 0);
	void join();
	friend void handlerWithArg(WithArg *rtn);
};



#endif
#endif /*THREAD_H_*/
