#include "SharedMemManager.h"
#include <stdio.h>
#ifdef HAVE_WINDOWS_H
void *SharedMemManager::initialize(int size)
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

	startAddress =  MapViewOfFile(hFile, FILE_MAP_WRITE, 0, 0, 0);
	if(!startAddress)
	{
		DWORD error = GetLastError();
		printf("Error mapping shared file: %d(%x)\n", error, error);
	}
	return startAddress;
}
void *SharedMemManager::startAddress;

#else
void *SharedMemManager::initialize(int size)
{
	this->size = size;
	if(startAddress) return startAddress; //Already mapped
	key_t key = ftok("/tmp", 0);
	if(key == (key_t)-1)
	{
	    perror("Cannot create Unique ID in ftok\n");
	    exit(0);//Fatal error
	}
	
	shmid = shmget(key, size, IPC_CREAT | 0666);
	
	if(shmid == -1)
	{
	    perror("Cannot create shared memory");
	     exit(0); //Fatal error
	}  
	startAddress = shmat(shmid, NULL, 0);
	if(startAddress == (void *)-1)
	{
	    perror("Cannot attach to shared memory");
	     exit(0); //Fatal error
	}  
	return startAddress;
}
void *SharedMemManager::startAddress;



#endif

