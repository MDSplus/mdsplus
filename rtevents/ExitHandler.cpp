//#include <Windows.h>
#include <signal.h>
#include "ExitHandler.h"

Runnable *ExitHandler::handler;

static void linHandler(int reason)
{
	if(ExitHandler::handler)
		ExitHandler::handler->run(0);
	signal(SIGINT, SIG_DFL);
}


void ExitHandler::atExit(Runnable *inHandl)
{
	handler = inHandl;
	signal(SIGINT, linHandler);
}

void ExitHandler::dispose()
{
	signal(SIGINT, SIG_DFL);

}

