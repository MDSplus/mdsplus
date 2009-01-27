#include <windows.h>
#include <process.h>
#include "Thread.h"
#include "SystemException.h"


void Thread::start(Runnable *rtn, void *arg, bool deallocate)
{
	withArg = new WithArg;
	withArg->rtn = rtn;
	withArg->arg = arg;
	withArg->deallocate = deallocate;

	semH = CreateSemaphore(NULL, 0, 256, NULL);
	withArg->sem = semH;
	threadH = (HANDLE) _beginthread((void (__cdecl *)(void *))handlerWithArg, 0, (void *)withArg);
	if(threadH == (HANDLE)-1)
		throw new SystemException("Error activating thread", GetLastError());
}

void Thread::join()
{
   WaitForSingleObject(semH, INFINITE);           
}


void handlerWithArg(WithArg *withArg)
{
	withArg->rtn->run(withArg->arg);
	if(withArg->deallocate)
		delete withArg->rtn;
	ReleaseSemaphore(withArg->sem, 1, NULL);
	delete withArg;
}

