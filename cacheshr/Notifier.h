#ifndef NOTIFIER_H
#define NOTIFIER_H
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <stdio.h>
#include <process.h>
#define NUM_HANDLES 10000
static int nids[NUM_HANDLES];
static HANDLE handles[NUM_HANDLES];

struct ThreadInfo
{
	HANDLE handle;
	void (*callback)(int);
	int nid;
	char killed;
};


class Notifier
{
	ThreadInfo info;
	HANDLE getHandle(int nid);

public:
	void initialize(int nid, void (*callback)(int));
	void notify();
	bool isMulticast() {return true;}//Windows signaling mechanism is multicast
	void dispose();
};

#else

#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

struct ThreadInfo
{
	sem_t semaphore;
	void (*callback)(int);
	int nid;
	char killed;
};


class Notifier
{
	ThreadInfo info;

public:
	

	void initialize(int nid, void (*callback)(int));
	void notify();
	bool isMulticast() {return false;}
	void dispose();
};




#endif
#endif
