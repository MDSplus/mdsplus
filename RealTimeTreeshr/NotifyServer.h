#ifndef NOTIFY_SERVER_H
#define NOTIFY_SERVER_H

#include "NotifyClient.h"
#include "CallbackManager.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <process.h>

struct ThreadInfo
{
	HANDLE handle;
	void (*callback)(int);
	int nid;
	bool killed;
};

class NotifyServer
{
	int nid;
	void (*callback)(int);
	HANDLE eventHandle;
	ThreadInfo threadInfo;
	CallbackManager *callbackManager;


public:
	NotifyServer(int nid)
	{
		this->nid = nid;
	}

	void setCallback(void (*callback)(int));
	void startServer();
	void killServer();
	void setCallbackManager(CallbackManager *callbackManager)
	{
		this->callbackManager = callbackManager;
	}

	CallbackManager *getCallbackManager()
	{
		return callbackManager;
	}
};
#endif

#endif