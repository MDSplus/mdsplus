#include "SystemSpecific.h"
#include "SystemException.h"
#include "Timeout.h"
#include <Windows.h>

#define MAX_SEM_COUNT 256
class EXPORT NamedSemaphore
{
	HANDLE semHandle;	
public:

	void initialize(int id, int initVal)
	{
		char name[24];
		sprintf(name, "REALTIME_SEM_%d", id);
		semHandle = CreateSemaphore(NULL, initVal, MAX_SEM_COUNT, (LPSTR)name);
		if(semHandle == 0)
			throw new SystemException("Error initializing semaphore", GetLastError());
	}
	void initialize(char *name, int initVal)
	{
		semHandle = CreateSemaphore(NULL, initVal, MAX_SEM_COUNT, (LPCSTR)name);
		if(semHandle == 0)
			throw new SystemException("Error initializing semaphore", GetLastError());
	}
	void  wait() 
	{
        int status = WaitForSingleObject( 
            semHandle,   // handle to semaphore
            INFINITE);           // zero-second time-out interval
		if(status == WAIT_FAILED)
			throw new SystemException("Error waiting Semaphore", GetLastError());
	}
	
	int timedWait(Timeout &timeout)
	{
        int status = WaitForSingleObject( 
            semHandle,   // handle to semaphore
            timeout.getTotMilliSecs());           // zero-second time-out interval
		if(status == WAIT_FAILED)
			throw new SystemException("Error waiting Semaphore", GetLastError());
		return status == WAIT_TIMEOUT;
	}
	
	void  post() //return 0 if seccessful
	{
		if(!ReleaseSemaphore( semHandle, 1, NULL))
			throw new SystemException("Error posting Semaphore", GetLastError());
	}
	
	bool isZero()
	{
       int status = WaitForSingleObject( 
            semHandle,   // handle to semaphore
            0);           // zero-second time-out interval
		if(status == WAIT_FAILED)
			throw new SystemException("Error waiting Semaphore", GetLastError());
		if(status == WAIT_TIMEOUT)
			return true;
		ReleaseSemaphore( semHandle, 1, NULL);
		return false;
	}
	
	void dispose()
	{
		CloseHandle(semHandle);
	}
};

