#include <Windows.h>
#include <signal.h>
#include "ExitHandler.h"

Runnable *ExitHandler::handler;

static void winHandler(int reason)
{
	if(ExitHandler::handler)
		ExitHandler::handler->run(0);
}


void ExitHandler::atExit(Runnable *inHandl)
{
	handler = inHandl;
	signal(SIGINT, winHandler);
}

void ExitHandler::dispose()
{
	atExit(0);
}

