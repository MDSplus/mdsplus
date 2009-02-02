#include "Thread.h"
#include <stdio.h>


void Thread::start(Runnable *rtn, void *arg, bool deallocate)
{
	WithArg *withArg = new WithArg;
	withArg->rtn = rtn;
	withArg->arg = arg;
	withArg->deallocate = deallocate;
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handlerWithArg, (void *)withArg); 
}

void Thread::join()
{
	pthread_join(thread, NULL);
}
void handlerWithArg(WithArg *withArg)
{
	withArg->rtn->run(withArg->arg);

	if(withArg->deallocate)
	{
		withArg->rtn->dispose();
		delete withArg->rtn;
	}
	delete withArg;
}
