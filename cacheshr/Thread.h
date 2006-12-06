#ifndef THREAD_H
#define THREAD_H
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#else 
#include <pthread.h>
#endif
#include "Runnable.h"

extern "C" void handler(Runnable *);

class Thread
{
#ifdef HAVE_WINDOWS_H
	HANDLE threadH;
#else
	pthread_t thread;
#endif
public:
	void start(Runnable *rtn);

	friend void handler(Runnable *rtn);
};

#endif

