#ifndef THREAD_H
#define THREAD_H
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <stdio.h>
#include <process.h>
#else 
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <taskLib.h>
#else
#include <pthread.h>
#endif
#endif
#include "Runnable.h"

extern "C" void handler(Runnable *);

class Thread
{
#ifdef HAVE_WINDOWS_H
	HANDLE threadH;
#else
#ifdef HAVE_VXWORKS_H
	int taskId;
#else
	pthread_t thread;
#endif
#endif
public:
	void start(Runnable *rtn);

	friend void handler(Runnable *rtn);
};

#endif

