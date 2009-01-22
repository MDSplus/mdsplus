#include "SharedMemory.h"
#include "SystemException.h"
#include <stdio.h>
bool SharedMemory::initialize(int id, int size)
{

	if(startAddress) return false; //Already mapped
	char name[256];
	sprintf(name, "REALTIME_MEM_%d", id);
	bool isFirst = false;
	hFile = OpenFileMapping(
		FILE_MAP_WRITE,
		FALSE,
		name             // name of file-mapping object
	);
	if(hFile == NULL)
	{
		isFirst = true;
		hFile = CreateFileMapping(
			(HANDLE)-1,              // handle to file to map
			NULL,
			PAGE_READWRITE,
			0,
			size,
			name             // name of file-mapping object
		);
		if(hFile == NULL)
			throw new SystemException("Error Creating Shared Memory", GetLastError());
	}
	
	startAddress =  (char *)MapViewOfFile(hFile, FILE_MAP_WRITE, 0, 0, 0);
	if(!startAddress)
	{
		DWORD error = GetLastError();
			throw new SystemException("Error Mapping Shared Memory", GetLastError());
	}
	return isFirst;
}



char *SharedMemory::getStartAddr() {return startAddress;}

char *SharedMemory::startAddress;

