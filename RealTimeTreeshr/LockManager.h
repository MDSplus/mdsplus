#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

//Lock IDs used thorough
#define GLOBAL_LOCK 1
#define SHARED_LOCK  2
#define TREE_LOCK 3
#define DATA_LOCK  4
#define TREE_WRITER_LOCK 5
#define CACHE_LOCK 6

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <stdio.h>


class LockManager
{

	HANDLE hMutex;
	
public:
	LockManager(){initialize();}
	LockManager(int id)
	{
		initialize(id);
	}
	bool initialize(int id)  //Return true if the mutex did not exist before and has been created
	{
		char buf[256];
		DWORD lastError;
		sprintf(buf, "%d", id);
		
		hMutex = CreateMutex( 
			NULL,                             // no security attributes
			FALSE,                            // initially not owned
			buf);        // name of mutex

		if (hMutex == NULL) 
		{
            printf("Error creating Mutex: %s\n", buf);
			exit(0); //Cannot handle the error now
		}
		lastError = GetLastError();
		if(lastError == ERROR_ALREADY_EXISTS)
			return false;
		return true;
	}


	bool initialize()
	{
		DWORD lastError;
		hMutex = CreateMutex( 
			NULL,                             // no security attributes
			FALSE,                            // initially not owned
			NULL);        

		if (hMutex == NULL) 
		{
            printf("Error creating Mutex\n");
			exit(0); //Cannot handle the error now
		}
		lastError = GetLastError();
		if(lastError == ERROR_ALREADY_EXISTS)
			return false;
		return true;
	}


	int lock()
	{
		
      DWORD dwWaitResult; 

 
      dwWaitResult = WaitForSingleObject( 
         hMutex,        // handle of mutex
         INFINITE);        // five-second time-out interval
 
      switch (dwWaitResult) 
      {
         case WAIT_OBJECT_0: 
			 return true;
                        // Cannot get mutex ownership due to time-out.
        case WAIT_TIMEOUT: 
            return FALSE; 

                        // Got ownership of the abandoned mutex object.
        case WAIT_ABANDONED: 
            return FALSE; 
      }

      return TRUE;
	}
	 
	int lock(long timeout)
	{
		
      DWORD dwWaitResult; 

 
      dwWaitResult = WaitForSingleObject( 
         hMutex,        // handle of mutex
         timeout);        // five-second time-out interval
 
      switch (dwWaitResult) 
      {
         case WAIT_OBJECT_0: 
			 return true;
                        // Cannot get mutex ownership due to time-out.
        case WAIT_TIMEOUT: 
            return FALSE; 

                        // Got ownership of the abandoned mutex object.
        case WAIT_ABANDONED: 
            return FALSE; 
      }

      return TRUE;
	}


	void unlock()
	{
		ReleaseMutex(hMutex) ;
	}

};





#endif
#endif