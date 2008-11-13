#include "Thread.h"



void Thread::start(Runnable *rtn, void *arg)
{
	WithArg *withArg = new WithArg;
	withArg->rtn = rtn;
	withArg->arg = arg;
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handlerWithArg, (void *)withArg); 
}

void Thread::join()
{
	pthread_join(thread, NULL);
}
void handlerWithArg(WithArg *withArg)
{
	withArg->rtn->run(withArg->arg);
	delete withArg;
}
