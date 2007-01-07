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
	taskSpawn(NULL, 20, VX_FP_TASK, 200000, (FUNCPTR)handler, (int)rtn,0,0,0,0,0,0,0,0,0); 
#ifdef
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handler, (void *)rtn); 

#endif
#endif

}


void handlerWithArg(WithArg *withArg)
{
	withArg->rtn->run(withArg->arg);
	delete withArg;
}