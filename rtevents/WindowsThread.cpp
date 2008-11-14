#include <windows.h>
#include <process.h>
#include "Thread.h"


void Thread::start(Runnable *rtn, void *arg)
{
	lock.initialize();
	withArg = new WithArg;
	withArg->rtn = rtn;
	withArg->arg = arg;
	withArg->lockPtr = &lock;
	lock.lock();
	threadH = (HANDLE) _beginthread((void (__cdecl *)(void *))handlerWithArg, 0, (void *)withArg);
	if(threadH == (HANDLE)-1)
		throw new SystemException("Error activating thread", GetLastError());
}

void Thread::join()
{
	lock.lock();
	lock.unlock();
}

void handlerWithArg(WithArg *withArg)
{
	withArg->rtn->run(withArg->arg);
	withArg->lockPtr->unlock();
	delete withArg;
}

