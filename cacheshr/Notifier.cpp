#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif
#include "Notifier.h"

#ifdef HAVE_WINDOWS_H
#include "Notifier.h"

#define NUM_HANDLES 10000

extern "C" void handleEvents(ThreadInfo *info)
{
	while(TRUE)
	{
		WaitForSingleObject(info->handle, INFINITE);
		if(info->killed) 
		{
			_endthread();
		}
		info->callback(info->nid, info->argument);
	}
}



HANDLE Notifier::getHandle(int nid)
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

void Notifier::initialize(int nid, void *argument, void (*callback)(int, void *))
{
    info.nid = nid;
	info.argument = argument;
    info.killed = 0;
    info.callback = callback;
	info.argument = argument;
    
    char name[128];
    sprintf((char *)name, "NID_%d", nid);

    HANDLE eventHandle = CreateEvent(NULL, TRUE, FALSE, (char *)name);
    info.handle = eventHandle;
    _beginthread((void (*)(void *))handleEvents, 0, &info);
}

void Notifier::notify()
{
    char name[128];
    sprintf((char *)name, "NID_%d", info.nid);

    HANDLE eventHandle = OpenEvent(EVENT_MODIFY_STATE, FALSE, (char *)name);

    int status = PulseEvent(eventHandle);
	if(status == 0) printf("Error in PulseEvent. Windows Error Code: %x\n ", GetLastError());

}

void Notifier::dispose()
{
    info.killed = 1;
    notify();
}

#else

#ifdef HAVE_VXWORKS_H
extern "C" void handleEvents(ThreadInfo *info)
{
	while(1)
	{
		semTake(info->semaphore, WAIT_FOREVER);
		if(info->killed) 
		{
			return;
		}
		info->callback(info->nid, info->argument);
	}
}

void Notifier::initialize(int nid, void *argument, void (*callback)(int, void *))
{
    info.nid = nid;
	info.argument = argument;
    info.killed = 0;
    info.callback = callback;
    info.semaphore = semBCreate(SEM_Q_FIFO, SEM_EMPTY); 
    if(info.semaphore == (void *)ERROR)
    {
	perror("Cannot create semaphore");
	exit(0); //Fatal error
    }
    //100KBytes stack size
    int status = taskSpawn(NULL, 55, VX_FP_TASK, 100000, (FUNCPTR)handleEvents, (int)&info,0,0,0,0,0,0,0,0,0);
    if(status == ERROR)
    {
	perror("Cannot create task");
	exit(0); //Fatal error
    }
}



void Notifier::notify()
{
	semGive(info.semaphore);
}    
    
void Notifier::dispose()
{
    info.killed = 1;
    notify();
}    



#else

extern "C" void handleEvents(ThreadInfo *info)
{
	while(1)
	{
		int status = sem_wait(&info->semaphore);
      if(status != 0)
      {
         perror("Error waiting Notifier semaphore\n");
         return;
      }
		if(info->killed) 
		{
			return;
		}
		info->callback(info->nid, info->argument);
	}
}


void Notifier::initialize(int nid, void *argument, void (*callback)(int, void *))
{
    info.nid = nid;
	info.argument = argument;
    info.killed = 0;
    info.callback = callback;
    int sts = sem_init(&info.semaphore, 1, 0);
    if(sts == -1)
    {
	perror("Cannot create semaphore");
	exit(0); //Fatal error
    }
    pthread_t thread;
    int rc = pthread_create(&thread, NULL,(void *(*)(void *))handleEvents, (void *)&info); 
}

void Notifier::notify()
{
	sem_post(&info.semaphore);
}    
    
void Notifier::dispose()
{
    info.killed = 1;
    notify();
}    

#endif
#endif
