#include "Thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

void Thread::start(Runnable *rtn, void *arg, bool deallocate)
{
	WithArg *withArg = new WithArg;
	withArg->rtn = rtn;
	withArg->arg = arg;
	withArg->deallocate = deallocate;
	if(attributes)
	{
		withArg->useAttr = true;
		withArg->priority = attributes->priority;
		withArg->policy = attributes->policy;
		memcpy(withArg->cpuMask, attributes->cpuMask, CPU_MASK_SIZE * sizeof(int));
	}
	else
		withArg->useAttr = false;
	int rc = pthread_create(&thread, NULL,(void *(*)(void *))handlerWithArg, (void *)withArg); 
}

void Thread::join()
{
	pthread_join(thread, NULL);
}
void handlerWithArg(WithArg *withArg)
{
	if(withArg->useAttr)
	{
		/* DOES NOT ADHERE TO LINUX STANDARDS
		pid_t pid = getpid();
		status = sched_setaffinity(pid, CPU_MASK_SIZE * sizeof(int), (const cpu_set_t *)withArg->cpuMask);
		if(status == -1)
			printf("Failed to set Thread affinity\n");
		*/
#ifdef HAVE_SCHED_SETSCHEDULER
		struct sched_param param;
		param.sched_priority = withArg->priority;
		int status = sched_setscheduler(pid, withArg->policy, &param);
		if(status == -1)
			printf("Failed to set Thread attributes\n");
#endif
	}

	withArg->rtn->run(withArg->arg);

	if(withArg->deallocate)
	{
		withArg->rtn->dispose();
		delete withArg->rtn;
	}
	delete withArg;
}
