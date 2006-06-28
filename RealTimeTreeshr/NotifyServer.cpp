#include "NotifyServer.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <process.h>


extern "C" void handleEvents(ThreadInfo *info)
{
	while(TRUE)
	{
		WaitForSingleObject(info->handle, INFINITE);
		if(info->killed) 
		{
			_endthread();
		}
		printf("NON MUORO %d %x\n", info->nid, info->callback);
		info->callback(info->nid);
		printf("E CHIAMOP CALLBACK\n");
	}
}




void NotifyServer::setCallback(void (*callback)(int))
{
	this->callback = callback;
	char name[128];
	sprintf((char *)name, "NID_%d", nid);

	eventHandle = CreateEvent(NULL, TRUE, FALSE, (char *)name);
	threadInfo.nid = nid;
	threadInfo.killed = false;
	threadInfo.handle = eventHandle;
	threadInfo.callback = callback;
}

void NotifyServer::startServer()
{

	_beginthread((void (*)(void *))handleEvents, 0, &threadInfo);
}

void NotifyServer::killServer()
{
	threadInfo.killed = true;
	//PulseEvent(threadInfo.handle);
}
#endif