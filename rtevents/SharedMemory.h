#ifndef SHAREDMEMORY_H_
#define SHAREDMEMORY_H_
#include "SystemSpecific.h"
#ifdef HAVE_WINDOWS_H
#include "WindowsSharedMemory.h"
#else
#include <sys/ipc.h>
#include <sys/shm.h>
# include <sys/types.h>
# include <sys/ipc.h>

class EXPORT SharedMemory
{
	int memId;
	char *startAddr;
public:
	bool initialize(int id, int size)
	{
		bool isFirst;
		key_t key = ftok("/tmp", id);
		//First try to create a new memory segment
		memId = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
		if(memId == -1) //Exclusive creation failed, the segment may be already created
		{
			memId = shmget(key, size, 0);
			if(memId == -1)
				throw new SystemException("Cannot get memory segment", errno);
			isFirst = false;
		}
		else
			isFirst = true;
		
		startAddr = (char *)shmat(memId, NULL, 0);
		if(startAddr == (char *)-1)
			throw new SystemException("Cannot attach memory segment", errno);
		return isFirst;
	}
	
	char *getStartAddr() {return startAddr;}
	
	void dispose()
	{
		
	}
};
#endif
#endif

