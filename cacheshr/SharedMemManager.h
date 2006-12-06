#ifndef SHARED_MEM_MANAGER_H
#define SHARED_MEM_MANAGER_H

#ifdef HAVE_WINDOWS_H
#include "windows.h"

class SharedMemManager
{
private:
	HANDLE hFile; 
	int size;

public:
	static void *startAddress;
	void *initialize(int size);
};


#else
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>

class SharedMemManager
{
private:
	int shmid;
	int size;
	
public:
	static void *startAddress;
	void *initialize(int size);
};
#endif

#endif	
