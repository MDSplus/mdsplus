#include <Windows.h>
#include <signal.h>
#include "ExitHandler.h"

#define MAX_HANDLERS 64
static Runnable *exitHandlers[MAX_HANDLERS];
int ExitHandler::handlerIdx = -1;
Lock *ExitHandler::lock;

static void winHandler(int reason)
{
	for(int i = ExitHandler::handlerIdx; i >= 0; i--)
	{
		printf("EXIT RUN %d\n", i);
		exitHandlers[i]->run(0);
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
	signal(SIGINT, winHandler);
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

