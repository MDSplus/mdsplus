#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif
#include "SharedMemManager.h"
#include <stdio.h>
#ifdef HAVE_WINDOWS_H
char *SharedMemManager::initialize(int size)
{

	if(startAddress) return startAddress; //Already mapped

	hFile = OpenFileMapping(
		FILE_MAP_WRITE,
		FALSE,
		"MDS_SHARED_TREE"             // name of file-mapping object
	);
	if(hFile == NULL)
	{
		hFile = CreateFileMapping(
			(HANDLE)-1,              // handle to file to map
			NULL,
			PAGE_READWRITE,
			0,
			size,
			"MDS_SHARED_TREE"             // name of file-mapping object
		);
		if(hFile == NULL)
			return  NULL;
	}

	startAddress =  (char *)MapViewOfFile(hFile, FILE_MAP_WRITE, 0, 0, 0);
	if(!startAddress)
	{
		DWORD error = GetLastError();
		printf("Error mapping shared file: %d(%x)\n", error, error);
	}
	return startAddress;
}
char *SharedMemManager::startAddress;

#else
#ifdef HAVE_VXWORKS_H

char *SharedMemManager::initialize(int size)
{
	this->size = size;
	if(startAddress) return startAddress; //Already mapped

	startAddress = (char *)malloc(size);

	return startAddress;
}
char *SharedMemManager::startAddress;

#else
char *SharedMemManager::initialize(int size)
{
	this->size = size;
	if(startAddress) return startAddress; //Already mapped
	//key_t key = ftok("/tmp", 0);
	key_t key = ftok("/tmp", 1);
	if(key == (key_t)-1)
	{
	    perror("Cannot create Unique ID in ftok\n");
	    exit(0);//Fatal error
	}
	
	printf("INITIALIZE MEMORY\n");
	
	shmid = shmget(key, size, IPC_CREAT | 0666);
	
	if(shmid == -1)
	{
            char buf[256];
            sprintf(buf, "Cannot create shared memory for %d bytes", size);
	    perror(buf);
	     exit(0); //Fatal error
	}  
	startAddress = (char *)shmat(shmid, NULL, 0);
	if(startAddress == (char *)-1)
	{
	    perror("Cannot attach to shared memory");
	     exit(0); //Fatal error
	}  
	return startAddress;
}
char *SharedMemManager::startAddress;


#endif
#endif

