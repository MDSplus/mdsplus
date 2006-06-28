#ifndef NOTIFY_CLIENT_H
#define NOTIFY_CLIENT_H
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <stdio.h>
#define NUM_HANDLES 10000
static int nids[NUM_HANDLES];
static HANDLE handles[NUM_HANDLES];

class NotifyClient
{
#ifdef HAVE_WINDOWS_H
	
	
	int nid;

	HANDLE getHandle(int nid)
	{
		int i;
		for(i = 0; i < NUM_HANDLES; i++)
		{
			if(nids[i] == 0 || nids[i] == nid)
				break;
		}
		if(i < NUM_HANDLES)
		{
			if(!nids[i])
			{
				char *name[128];
				sprintf((char *)name, "NID_%d", nid);
				handles[i] =  OpenEvent(
					EVENT_ALL_ACCESS,
					FALSE,
					(char *)name);
				nids[i] = nid;
			}
			return handles[i];
		}
		return 0;
	}



public:
	

	NotifyClient(int nid)
	{
		this->nid = nid;
	}

	void notify()
	{
		HANDLE handle = getHandle(nid);
		
		if(handle)
			PulseEvent(handle);
	}
	bool isMulticast()
	{
		return true; //Windows signaling mechanism is multicast
	}

};
#endif
#endif
