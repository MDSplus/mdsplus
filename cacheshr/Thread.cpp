#include "Thread.h"



void Thread::start(Runnable *rtn)
{

#ifdef HAVE_WINDOWS_H
	threadH = (HANDLE)_beginthread((void (__cdecl *)(void *))handler, 0, (void *)rtn);
	if(threadH == (HANDLE)-1)
		printf("Error activating thread\n");
#else
	taskSpawn(NULL, 20, VX_FP_TASK, 200000, (FUNCPTR)handler, (int)rtn,0,0,0,0,0,0,0,0,0); 
#ifdef
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handler, (void *)rtn); 

#endif
#endif

}


void handler(Runnable *rtn)
{
	rtn->run();
}

