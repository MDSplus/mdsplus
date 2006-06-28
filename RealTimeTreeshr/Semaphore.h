#ifndef SEMAPHORE_H
#define SEMAPHORE_H


#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <process.h>
#endif




class Semaphore
{
#ifdef HAVE_WINDOWS_H
	HANDLE handle;
public:
	Semaphore()
	{
		handle = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	void wait()
	{
		WaitForSingleObject(handle, INFINITE);
	}

	void signal()
	{
		PulseEvent(handle);
	}
#endif  //HAVE_WINDOWS_H

};


#endif