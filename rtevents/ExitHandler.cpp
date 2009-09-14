//#include <Windows.h>
#include <signal.h>
#include "ExitHandler.h"

#define MAX_HANDLERS 64
static Runnable *exitHandlers[MAX_HANDLERS];
int ExitHandler::handlerIdx = -1;

static Lock *lock;
static void checkLock()
{
	if(!lock) 
	{
		lock = new Lock();
		lock->initialize();
	}
}

static void linHandler(int reason)
{
	for(int i = ExitHandler::handlerIdx; i >= 0; i--)
	{
		exitHandlers[ExitHandler::handlerIdx]->run(0);
	}
	signal(SIGINT, SIG_DFL);
}


void ExitHandler::atExit(Runnable *inHandl)
{
	checkLock();
	if(handlerIdx >= MAX_HANDLERS - 1)
	{
		printf("INTERNAL ERROR: ExitHandler overflow\n");
		return;
	}

	lock->lock();
	exitHandlers[++handlerIdx] = inHandl;
	signal(SIGINT, linHandler);
	lock->unlock();
}

void ExitHandler::dispose()
{
	checkLock();
	lock->lock();
	handlerIdx--;
	if(handlerIdx < 0)
		signal(SIGINT, SIG_DFL);
	lock->unlock();
}



