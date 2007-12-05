#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

#include "Thread.h"



void Thread::start(Runnable *rtn, void *arg)
{
	WithArg *withArg = new WithArg;
	withArg->rtn = rtn;
	withArg->arg = arg;
#ifdef HAVE_WINDOWS_H
	threadH = (HANDLE)_beginthread((void (__cdecl *)(void *))handlerWithArg, 0, (void *)withArg);
	if(threadH == (HANDLE)-1)
		printf("Error activating thread\n");
#else 
#ifdef HAVE_VXWORKS_H
	taskSpawn(NULL, 55, VX_FP_TASK, 200000, (FUNCPTR)handlerWithArg, (int)withArg,0,0,0,0,0,0,0,0,0); 
#else
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handlerWithArg, (void *)withArg); 
#endif

#endif

}


void handlerWithArg(WithArg *withArg)
{
	withArg->rtn->run(withArg->arg);
	delete withArg;
}
