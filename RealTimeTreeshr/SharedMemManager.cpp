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
void *SharedMemManager::initialize(int size){}
void *SharedMemManager::startAddress;


#endif

