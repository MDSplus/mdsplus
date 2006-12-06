#include "Thread.h"
#include <stdio.h>
#ifdef HAVE_WINDOWS_H
#include <process.h>
#endif



void Thread::start(Runnable *rtn)
{

#ifdef HAVE_WINDOWS_H
	threadH = (HANDLE)_beginthread((void (__cdecl *)(void *))handler, 0, (void *)rtn);
	if(threadH == (HANDLE)-1)
		printf("Error activating thread\n");
#else
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handler, (void *)rtn); 

#endif

}


void handler(Runnable *rtn)
{
	rtn->run();
}

