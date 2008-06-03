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
			printf("ABANDONED!!!!!!!!!!!!!!!!!\n");
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

#else
#ifdef HAVE_VXWORKS_H
#include <vxWorks.h>
#include <semLib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SEMAPHORES 10



class LockManager
{
    	static SEM_ID *semaphores;
	int thisId;
	
public:
	LockManager(){initialize(0);}
	LockManager(int id)
	{
		initialize(id);
	}
	
	bool initialize(int id)  //Return true if the mutex did not exist before and has been created
	{
	
	    if(!semaphores)
		semaphores = (SEM_ID *)calloc(MAX_SEMAPHORES, sizeof(SEM_ID));
	    if(id >= MAX_SEMAPHORES)
	    {
		printf("Fatal error: id too large in LockManager\n");
		exit(0); //Fatal error
	    }
	    thisId = id;
	    if(!semaphores[id]) //First time the semaphore has been created
	    {
		semaphores[thisId] = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
		if(semaphores[thisId] == NULL)
	    	{
			printf("Fatal error: cannot create binary semaphore in LockManager\n");
			exit(0); //Fatal error
	    	}
		return true;
	    }
	    return false;
	}


	void lock()
    	{
    		semTake(semaphores[thisId], WAIT_FOREVER);
    	}

	 

	void unlock()
	{
    		semGive(semaphores[thisId]);
	}
	

};


#else
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

class LockManager
{
	bool isLocal;
    	sem_t *semaphore;
	pthread_mutex_t pmutex; //Used mutual exclusion within the process (among threads)
	
public:
	LockManager(){initialize();}
	LockManager(int id)
	{
		initialize(id);
	}
	
	bool initialize(int id)  //Return true if the mutex did not exist before and has been created
	{
		isLocal = false;
		char buf[256];
		char buf1[256];
		int oflag = O_CREAT | O_EXCL;
		//mode_t mode = 0777;
		mode_t mode = S_IRWXO | S_IRWXG | S_IRWXU ;
		sprintf(buf, "/mdscachex%d", id);
		sprintf(buf1, "/dev/shm/sem.mdscachex%d", id);
		
		semaphore = sem_open(buf, oflag, mode, 1);
		if(semaphore == SEM_FAILED)
		{
		    semaphore = sem_open(buf, 0, mode, 1);
		    if(semaphore == SEM_FAILED)
		    {
		    	perror("Cannot create Semaphore!\n");
			//exit(0); //Fatal error
		    }
                    //It appears that Linux does not report the correct privileges in sem_open, so do it with chmod
                    chmod(buf1, 0777);
		    return false;
		}
                chmod(buf1, 0777);
		return true;
	}

	//No argument in creator: use a 
	bool initialize()  //Return true if the mutex did not exist before and has been created
	{
		isLocal = true;
		int status = pthread_mutex_init (&pmutex , NULL);
		if(status)
		{
		    perror("Cannot create Semaphore!\n");
			exit(0); //Fatal error
		}
		return true;
	}


    void lock()
    {
	if(isLocal)
		 pthread_mutex_lock (&pmutex);
	else
    		sem_wait(semaphore);
    }

	 

	void unlock()
	{
		if(isLocal)
			 pthread_mutex_unlock (&pmutex);
		else
    			sem_post(semaphore);
	}
	

};


#endif
#endif
#endif
