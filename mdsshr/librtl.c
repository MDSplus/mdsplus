#include <mdstypes.h>
#include <mdsdescrip.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <librtl_messages.h>
#include <mdsshr.h>
#define _GNU_SOURCE /* glibc2 needs this */
#if defined(__sparc__)
#include "/usr/include/sys/types.h"
#elif !defined(HAVE_WINDOWS_H)
#ifdef HAVE_GETTIMEOFDAY
#include <sys/types.h>
#endif
#endif
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <STATICdef.h>

STATIC_CONSTANT char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$ $Name$";

#ifndef HAVE_VXWORKS_H
STATIC_CONSTANT _int64 addin = LONG_LONG_CONSTANT(0x7c95674beb4000);
#else
STATIC_CONSTANT _int64 addin = 0x7c95674beb4000;
#endif

extern int MdsCopyDxXd();
STATIC_ROUTINE char *GetTdiLogical(char *name);

char *MdsDescrToCstring(struct descriptor *in);
int StrFree1Dx(struct descriptor *out);
int StrGet1Dx(unsigned short *len, struct descriptor *out);
int StrCopyDx(struct descriptor *out, struct descriptor *in);
int StrAppend(struct descriptor *out, struct descriptor *tail);
void TranslateLogicalFree(char *value);

#ifdef HAVE_WINDOWS_H
#pragma warning (disable : 4100 4201 4115 4214 4514)
#include <windows.h>
#include <process.h>

#define RTLD_LAZY 0

STATIC_ROUTINE void *dlopen(char *filename, int flags)
{
  return   (void *)LoadLibrary(filename);
}

STATIC_ROUTINE void *dlsym(void *handle, char *name)
{
  return (void *)GetProcAddress((HINSTANCE)handle, name);
}

STATIC_ROUTINE char *dlerror()
{
  static LPTSTR error_string = 0; /* windows NEED TO MAKE THREADSAFE */
  DWORD last_error=GetLastError();
  if (error_string)
  {
    LocalFree((HLOCAL)error_string);
    error_string = 0;
  }
  FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    0, // Default language
    (LPTSTR) &error_string,
    0,
    NULL 
  );
  return error_string == NULL ? "" : error_string;
}

typedef struct _dir
{
	char *path;
	HANDLE handle;
} DIR;

struct dirent
{
	char *d_name;
};

STATIC_ROUTINE DIR *opendir(char *path)
{
	DIR *dir = 0;
	int info = GetFileAttributes(path);
	if ((info != 0xFFFFFFFF) && ((info & FILE_ATTRIBUTE_DIRECTORY) != 0))
	{
		dir = (DIR *)malloc(sizeof(DIR)+strlen(path)+3);
		dir->path = ((char *)dir)+sizeof(DIR);
		strcpy(dir->path,path);
		strcat(dir->path,(dir->path[strlen(dir->path)-1] == '\\') ? "*" : "\\*");
		dir->handle = 0;
	}
	return dir;
}

STATIC_ROUTINE void closedir(DIR *dir)
{
	if (dir)
	  free(dir);
}

struct dirent *readdir(DIR *dir)
{
  static struct dirent ans;     /* windows NEED TO MAKE THREADSAFE */
  static WIN32_FIND_DATA fdata; /* windows NEED TO MAKE THREADSAFE */
	if (dir->handle == 0)
	{

		dir->handle = FindFirstFile(dir->path, &fdata);
		if (dir->handle == INVALID_HANDLE_VALUE)
			dir->handle = 0;
	}
	else
	{
		if (!FindNextFile(dir->handle, &fdata))
		{
	      FindClose(dir->handle);
		  dir->handle = 0;
		}
	}
	if (dir->handle != 0)
	{
		ans.d_name = fdata.cFileName;
		return &ans;
	}
	else
		return 0;
}

char *index(char *str, char c)
{
	unsigned int pos = strcspn(str,&c);
  return (pos == 0) ? ((str[0] == c) ? str : 0) : ((pos == strlen(str)) ? 0 : &str[pos]);
}


STATIC_ROUTINE char *GetRegistry(char *where, char *pathname)
{
  HKEY regkey1=(HKEY)0;
  HKEY regkey2=(HKEY)0;
  HKEY regkey3=(HKEY)0;
  unsigned char *path = NULL;
  int status1=-1,status2=-1,status3=-1;
  if ( ((status1 = RegOpenKeyEx((HKEY)where,"SOFTWARE",0,KEY_READ,&regkey1)) == ERROR_SUCCESS) &&
       ((status2 = RegOpenKeyEx(regkey1,"MIT",0,KEY_READ,&regkey2)) == ERROR_SUCCESS) &&
       ((status3 = RegOpenKeyEx(regkey2,"MDSplus",0,KEY_READ,&regkey3)) == ERROR_SUCCESS) )
  {
    unsigned long valtype;
    unsigned long valsize;
    if (RegQueryValueEx(regkey3,pathname,0,&valtype,NULL,&valsize) == ERROR_SUCCESS)
    {
      int plen;
	    valsize += 2;
      path = malloc(valsize+1);
      RegQueryValueEx(regkey3,pathname,0,&valtype,path,&valsize);
	    plen = strlen(path);
//	  if (path[plen-1] != '\\')
//	  {
//		  path[plen++] = '\\';
//		  path[plen] = '\0';
//	  }
    }
  }
  if (regkey1) RegCloseKey(regkey1);
  if (regkey2) RegCloseKey(regkey2);
  if (regkey3) RegCloseKey(regkey3);
  return (char *)path;
}

char *TranslateLogical(char *pathname)
{
	char *path = GetRegistry((char *)HKEY_CURRENT_USER, pathname);
	if (!path)
	{
	  path = GetRegistry((char *)HKEY_LOCAL_MACHINE, pathname);
          if (!path)
           // path = GetTdiLogical(char *pathname);
           path = GetTdiLogical(pathname);
	}

	return path;
}

int LibSpawn(struct descriptor *cmd, int waitFlag, int notifyFlag)
{

  char *cmd_c = MdsDescrToCstring(cmd);
  int status;
  char *arglist[255];
  char *tok;
  tok = strtok(cmd_c," ");
  arglist[0] = (char *)6;
  arglist[1] = (char *)(waitFlag ? _P_WAIT :  _P_NOWAIT);
  arglist[2] = "cmd";
  arglist[3] = arglist[2];
  arglist[4] = "/C";
  arglist[5] = tok;
  while ((tok = strtok(0," ")) != 0)
  {

	  if (strlen(tok) > 0)
		arglist[(int)(arglist[0]++)] = tok;
  }
  arglist[((int)arglist[0])] = (char *)0;
  status = LibCallg(arglist,_spawnlp);
  /*if (status != 0) perror("Error doing spawn"); */
  free(cmd_c);

  return status;

}

int LibWait(float *secs)
{
  int msec = (int)(*secs*1000.);
  Sleep(msec);
  return 1;
}

void *LibCallg(void **arglist, FARPROC *routine)
{
  int a_idx;

  unsigned int retval;
  for (a_idx=*(int *)arglist; a_idx > 0; a_idx--)
  {

	  void *arg = arglist[a_idx];
    __asm mov eax, arg

	__asm push eax
  }
  __asm call routine
  __asm mov retval, eax
  for (a_idx=*(int *)arglist; a_idx > 0; a_idx--) __asm pop eax

  return retval;
}
#define ETIMEDOUT 42

int pthread_mutex_init(HANDLE *mutex);
void pthread_mutex_lock(HANDLE *mutex);
void pthread_mutex_unlock(HANDLE *mutex);


void pthread_detach(HANDLE *thread)
{
	return;
}

int pthread_cond_init(HANDLE *cond)
{
  *cond = CreateEvent(NULL,TRUE,FALSE,NULL);
  return (*cond == NULL);
}

BOOL pthread_cond_destroy(HANDLE *cond)
{
   return CloseHandle(*cond);
}

int pthread_cond_signal(HANDLE *cond)
{
  BOOL status;
#ifdef ___DEBUG_IT
  printf("signalling event %p\n",*cond);
#endif
  status = SetEvent(*cond);
#ifdef ___DEBUG_IT
  printf("SetEvent on %p completed with status = %d\n",*cond,status);
#endif
  if (status)
	  status = ResetEvent(*cond);
#ifdef ___DEBUG_IT
  printf("ResetEvent on %p completed with status = %d\n",*cond,status);
#endif
  return status == 0;
}

int pthread_cond_wait(HANDLE *cond, HANDLE *mutex)
{
	int status;
#ifdef ___DEBUG_IT
   printf("waiting for condition %p\n",*cond);
#endif
   pthread_mutex_unlock(mutex);
   status = WaitForSingleObject(*cond,INFINITE);
   pthread_mutex_lock(mutex);
#ifdef ___DEBUG_IT
   printf("got condition %p\n",*cond);
#endif
   return(status == WAIT_FAILED);
}

int pthread_cond_timedwait(HANDLE *cond, HANDLE *mutex, int msec)
{
   int status;
   pthread_mutex_unlock(mutex);
   status = WaitForSingleObject(*cond,msec);
   pthread_mutex_lock(mutex);
   if (status == WAIT_TIMEOUT)
     status = ETIMEDOUT;
   else
     status = 0;
   return status;
}

int pthread_mutex_init(HANDLE *mutex)
{
  *mutex = CreateMutex(0,FALSE,NULL);
  return (*mutex == NULL);
}

BOOL pthread_mutex_destroy(HANDLE *mutex)
{
  return CloseHandle(*mutex);
}

STATIC_THREADSAFE HANDLE global_mutex = NULL;
STATIC_THREADSAFE int global_mutex_initialized = 0;
void pthread_unlock_global_np()
{
  if (!global_mutex_initialized)
  {
    global_mutex_initialized = 1;
    pthread_mutex_init(&global_mutex);
  }
  pthread_mutex_unlock(&global_mutex);

}
void pthread_lock_global_np()
{
  if (!global_mutex_initialized)
  {
    global_mutex_initialized = 1;
    pthread_mutex_init(&global_mutex);
  }
  pthread_mutex_lock(&global_mutex);
}

int pthread_exit(int status)
{
	return status;
}

int pthread_create(unsigned long *thread, void *dummy, void (*rtn)(void *), void *rtn_param)
{
  *thread = _beginthread( rtn, 0, rtn_param);
  return *thread == 0;
}
void pthread_cleanup_pop(){}
void pthread_cleanup_push(){}

void pthread_mutex_lock(HANDLE *mutex)
{
#ifdef ___DEBUG_IT
   printf("Trying to lock mutex %p\n",*mutex);
#endif
   WaitForSingleObject(*mutex,INFINITE);
#ifdef ___DEBUG_IT
   printf("Locked mutex %p\n",*mutex);
#endif
}

void pthread_mutex_unlock(HANDLE *mutex)
{
	int status;
#ifdef ___DEBUG_IT
	printf("Unlocking mutex %p\n",*mutex);
#endif
	status = ReleaseMutex(*mutex);
#ifdef ___DEBUG_IT
	printf("Unlocked mutex %p with status=%d\n",*mutex,status);
#endif
}

void pthread_cancel(HANDLE thread)
{
	printf("Abort not supported");
}

#else /* WIN32 */
#ifdef HAVE_VXWORKS_H

#include <vxWorks.h>
#include <wdLib.h>
#include <semLib.h>
#include <taskLib.h>
#include <ioLib.h>
#include <dirent.h>

int pthread_mutex_init(SEM_ID *mutex);
void pthread_mutex_lock(SEM_ID *mutex);
void pthread_mutex_unlock(SEM_ID *mutex);


void pthread_detach(int *thread)
{
	return;
}

int pthread_cond_init(SEM_ID *cond)
{
  *cond = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  return (*cond ==NULL);
}

BOOL pthread_cond_destroy(SEM_ID *cond)
{
  int status = semDelete(*cond);
  return (status == ERROR);
}

int pthread_cond_signal(SEM_ID *cond)
{
  int status;
  status = semGive(*cond);
  status = semTake(*cond, WAIT_FOREVER);
  return (status == ERROR);
}



int pthread_cond_wait(SEM_ID *cond)
{
  int status;
  status = semTake(*cond, WAIT_FOREVER);
  if(status != ERROR)
    status = semGive(*cond);
  return (status == ERROR);
}

void pthread_cond_timedwait(SEM_ID *cond, int msec)
{
  STATUS status;
  status = semTake(*cond, (sysClkRateGet() * msec) / 1000);
  if(status != ERROR)
    status = semGive(*cond);
}

int pthread_mutex_init(SEM_ID *mutex)
{
  *mutex = semBCreate(SEM_Q_FIFO, SEM_FULL);
  return (*mutex == NULL);
}

BOOL pthread_mutex_destroy(SEM_ID *mutex)
{
  return (semDelete(*mutex) != ERROR);
}

STATIC_THREADSAFE SEM_ID global_mutex = NULL;
STATIC_THREADSAFE int global_mutex_initialized = 0;
void pthread_unlock_global_np()
{
  if (!global_mutex_initialized)
  {
    global_mutex_initialized = 1;
    pthread_mutex_init(&global_mutex);
  }
  pthread_mutex_unlock(&global_mutex);

}
void pthread_lock_global_np()
{
  if (!global_mutex_initialized)
  {
    global_mutex_initialized = 1;
    pthread_mutex_init(&global_mutex);
  }
  pthread_mutex_lock(&global_mutex);
}

int pthread_exit(int status)
{
	return status;
}

int pthread_create(unsigned long *thread, void *dummy, void (*rtn)(void *), void *rtn_param)
{
  *thread = taskSpawn(NULL, 90, 0, 20000, (FUNCPTR)rtn, (int)rtn_param, 0,0,0,0,0,0,0,0,0);
  return (*thread == ERROR);
}

void pthread_cleanup_pop(){}
void pthread_cleanup_push(){}

void pthread_mutex_lock(SEM_ID *mutex)
{
  semTake(*mutex, WAIT_FOREVER);
}

void pthread_mutex_unlock(SEM_ID *mutex)
{
  semGive(*mutex);
}


void pthread_cancel(unsigned long *thread)
{
  taskDelete(*thread);
}

#else /* VXWORKS */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>


#ifdef _AIX
#define pthread_mutexattr_default NULL
#endif

#ifndef HAVE_PTHREAD_LOCK_GLOBAL_NP
#include <inttypes.h>
#include <pthread.h>
STATIC_THREADSAFE pthread_mutex_t GlobalMutex;
STATIC_THREADSAFE int Initialized = 0;
STATIC_THREADSAFE int LockCount = 0;
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#else
#undef select
#endif
STATIC_THREADSAFE pthread_t current_locked_thread = 0;

void pthread_lock_global_np()
{
  pthread_t thread = pthread_self();
  if (!Initialized)
  {
#if !defined(PTHREAD_MUTEX_RECURSIVE)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
#ifndef __sun
#ifdef HAVE_PTHREAD_MUTEXATTR_SETKIND_NP
    pthread_mutexattr_setkind_np(&m_attr,PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE);
#endif
#endif
    pthread_mutex_init(&GlobalMutex,&m_attr);
    Initialized = 1;
  }
#ifdef ___DEBUG_IT
  if (current_locked_thread)
    printf("global currently locked by thread %d\n",current_locked_thread);

  printf("Thread %d is about to lock global\n",thread);
#endif
  pthread_mutex_lock(&GlobalMutex);
#ifdef ___DEBUG_IT
  printf("Global locked - %d\n",++LockCount);
  current_locked_thread = thread;
#endif
}

void pthread_unlock_global_np()
{
  if (!Initialized)
  { 
#ifndef __sun
#if defined(PTHREAD_MUTEX_RECURSIVE) 
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
#ifdef HAVE_PTHREAD_MUTEXATTR_SETKIND_NP
    pthread_mutexattr_setkind_np(&m_attr,PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE);       
#endif
    pthread_mutex_init(&GlobalMutex,&m_attr);
#else 
    pthread_mutex_init(&GlobalMutex,pthread_mutexattr_default);
#endif
#endif
    Initialized = 1;
  }
#ifdef ___DEBUG_IT
  printf("Thread %d is about to unlock global - %d\n",pthread_self(),LockCount--);
#endif
  pthread_mutex_unlock(&GlobalMutex);
#ifdef ___DEBUG_IT
  printf("Unlocked global - %d\n",LockCount);
  if (LockCount == 0)
    current_locked_thread = 0;
#endif
}
#endif

#ifdef HAVE_DL_H
#include <dl.h>
#define RTLD_LAZY BIND_DEFERRED | BIND_NOSTART | DYNAMIC_PATH

STATIC_ROUTINE void *dlopen(char *filename, int flags)
{
  return (void *)shl_load(filename,flags,0);
}

void *dlsym(void *handle, char *name)
{
  void *symbol = NULL;
  int s = shl_findsym((shl_t *)&handle,name,0,&symbol);
  return symbol;
}

#elif HAVE_DLFCN_H
#include <dlfcn.h>

#else 
#error "No supported dynamic library API"
#endif

#endif

STATIC_ROUTINE char *nonblank( char *p)
{
  if (!p) return(0);
  for ( ; *p && isspace(*p) ; p++);
  return(*p ? p : 0);
}

STATIC_ROUTINE char  *blank( char *p)
{
  if (!p) return(0);
  for ( ; *p && !isspace(*p) ; p++);
  return(*p ? p : 0);
}

char *TranslateLogical(char *name)
{
	char *env = getenv(name);
	env = env ? strcpy(malloc(strlen(env)+1),env) : 0;
      
/*
        if (!env)
          env = GetTdiLogical(name);
*/
	return env;
}
void *LibCallg(void **arglist, void * (*routine)())
{
  switch (*(long *)arglist & 0xff)
  {
    case 0:  return (*routine)();
    case 1:  return (*routine)(arglist[1]);
    case 2:  return (*routine)(arglist[1],arglist[2]);
    case 3:  return (*routine)(arglist[1],arglist[2],arglist[3]);
    case 4:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4]);
    case 5:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5]);
    case 6:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6]);
    case 7:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7]);
    case 8:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8]);
    case 9:  return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9]);
    case 10: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10]);
    case 11: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11]);
    case 12: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12]);
    case 13: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13]);
    case 14: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14]);
    case 15: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15]);
    case 16: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16]);
    case 17: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17]);
    case 18: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18]);
    case 19: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19]);
    case 20: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20]);
    case 21: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21]);
    case 22: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22]);
    case 23: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23]);
    case 24: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24]);
    case 25: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25]);
    case 26: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26]);
    case 27: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27]);
    case 28: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28]);
    case 29: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29]);
    case 30: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29],arglist[30]);
    case 31: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29],arglist[30],arglist[31]);
    case 32: return (*routine)(arglist[1],arglist[2],arglist[3],arglist[4],arglist[5],arglist[6],arglist[7],arglist[8],
                    arglist[9],arglist[10],arglist[11],arglist[12],arglist[13],arglist[14],arglist[15],arglist[16],
                    arglist[17],arglist[18],arglist[19],arglist[20],arglist[21],arglist[22],arglist[23],arglist[24],
                    arglist[25],arglist[26],arglist[27],arglist[28],arglist[29],arglist[30],arglist[31],arglist[32]);
    default: printf("Error - currently no more than 32 arguments supported on external calls\n");
  }
  return 0;
}


#ifndef HAVE_VXWORKS_H
STATIC_ROUTINE void  child_done(	/* Return: meaningless sts		*/
    int   sig			/* <r> signal number			*/
   )
   {
    if (sig == SIGCHLD)
        fprintf(stdout,"--> Process completed\n");
    else
        fprintf(stderr,"--> child_done: *NOTE*  sig=0x%08X\n",sig);

    return;
   }

int LibSpawn(struct descriptor *cmd, int waitflag, int notifyFlag)
{
  char *sh = "/bin/sh";
  pid_t  pid,xpid;
  char *cmdstring = MdsDescrToCstring(cmd);
  char *spawn_wrapper = TranslateLogical("MDSPLUS_SPAWN_WRAPPER");
  int   sts=0;
  if (spawn_wrapper)
  {
    char *oldcmdstring = cmdstring;
    cmdstring = strcpy(malloc(strlen(spawn_wrapper)+strlen(oldcmdstring)+5),spawn_wrapper);
    strcat(cmdstring," ");
    strcat(cmdstring,oldcmdstring);
    free(oldcmdstring);
    TranslateLogicalFree(spawn_wrapper);
  }
  signal(SIGCHLD,notifyFlag ? child_done : (waitflag ? SIG_DFL : SIG_IGN));
  pid = fork();
  if (!pid)
  {
  /*-------------> child process: execute cmd	*/
    char  *arglist[4];
    char  *p;
    int i=0;
    signal(SIGCHLD,SIG_DFL);
    arglist[0] = getenv("SHELL");
    if (arglist[0] == 0)
      arglist[0] = sh;
    i++;
    if (cmd->length != 0)
    {
      arglist[i++] = "-c";
      arglist[i++] = nonblank(cmdstring);
    }
    arglist[i] = 0;
    sts = execvp(arglist[0],arglist);
  }
  /*-------------> parent process ...		*/
  if (pid == -1)
  {
    fprintf(stderr,"Error %d from fork()\n",errno);
    return(0);
  }
  if (waitflag || cmd->length == 0)
  {
    for ( ; ; )
    {
      xpid = wait(&sts);
      if (xpid == pid)
      break;
    }
  }
  free(cmdstring);
  /* return sts;*/
  return sts >> 8;
}

#endif
#ifdef HAVE_VXWORKS_H


static WDOG_ID wd;   /* vxworks NEED TO MAKE THREADSAFE */
static SEM_ID sem;   /* vxworks NEED TO MAKE THREADSAFE */

STATIC_ROUTINE void TimerExpired(int par)
{
  semGive(sem);
}

int LibWait(float *secs)
{
  if((sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY)) == NULL)
    return;
  if((wd = wdCreate()) == NULL)
    return;

  wdStart(wd, sysClkRateGet() * *secs, (FUNCPTR)TimerExpired, 0);
  semTake(sem, WAIT_FOREVER);
  wdDelete(wd);
  semDelete(sem);
}

#else


int LibWait(float *secs)
{
  struct timespec ts;
  ts.tv_sec = (unsigned int)*secs;
  ts.tv_nsec = (unsigned int)((*secs - (unsigned int)*secs)*1E9);
  nanosleep(&ts, 0);

  return 1;
}

#endif

#endif

#ifndef va_count
#define  va_count(narg) va_start(incrmtr, first); \
                        for (narg=1; (narg < 256) && (va_arg(incrmtr, struct descriptor *) != MdsEND_ARG); narg++)
#endif /* va_count */

typedef struct _VmList { void *ptr;
			 struct _VmList *next;
		       } VmList;

typedef struct _ZoneList { VmList *vm;
                           struct _ZoneList *next;
			 } ZoneList;

ZoneList *MdsZones=NULL;


int TranslateLogicalXd(struct descriptor *in, struct descriptor_xd *out)
{
  struct descriptor out_dsc = {0,DTYPE_T,CLASS_S,0};
  int status = 0;
  char *in_c = MdsDescrToCstring(in);
  char *out_c = TranslateLogical(in_c);
  if (out_c)
  {
    out_dsc.length = strlen(out_c);
    out_dsc.pointer = out_c;
    status = 1;
  }
  MdsCopyDxXd(&out_dsc,out);
  if (out_c)
    TranslateLogicalFree(out_c);
  free(in_c);
  return status;
}

void MdsFree(void *ptr)
{
  free(ptr);
}

char *MdsDescrToCstring(struct descriptor *in)
{
  char *out = malloc(in->length + 1);
  memcpy(out,in->pointer,in->length);
  out[in->length] = 0;
  return out;
}

int LibSigToRet(){return 1;}

#ifdef HAVE_VXWORKS_H
#include <symLib.h>
#include <taskLib.h>

int LibSpawn(struct descriptor *cmd,int waitFlag, int notifyFlag)
{
/* In vxWorks it is possible to spawn function calls */

    char *funcall = MdsDescrToCstring(cmd);
    extern SYMTAB_ID sysSymTbl;
    int status;
    char *full_funcall;
    SYM_TYPE type;
    FUNCPTR fun;

    full_funcall = malloc(strlen(funcall) + 2);
    full_funcall[0] = '_';
    strcpy(&full_funcall[1], funcall);
    status = symFindByName(sysSymTbl, full_funcall, (char **)fun, &type);
    if(status == ERROR)
	return 0; /* Failed function lookup */
    sp(fun, 0,0,0,0,0,0,0,0,0);
    free(full_funcall);
    free(funcall);
    return 0;
}

int LibFindImageSymbol(struct descriptor *filename, struct descriptor *symbol, void **symbol_value)
{
  extern SYMTAB_ID sysSymTbl;
  int status;
  char *full_symbol, *full_filename;
  SYM_TYPE type;
  char *c_symbol = MdsDescrToCstring(symbol), *c_filename;

  full_symbol = malloc(strlen(c_symbol) + 2);
#ifdef AXPVME
  strcpy(&full_symbol[0], c_symbol);
#else
  full_symbol[0] = '_';
  strcpy(&full_symbol[1], c_symbol);
#endif
  status = symFindByName(sysSymTbl, full_symbol, (char **)symbol_value, &type);
  if(status == ERROR)
    {
      c_filename =  MdsDescrToCstring(filename);
      full_filename = malloc(strlen(c_filename) + 3);
      strcpy(full_filename, c_filename);
      strcpy(&full_filename[strlen(c_filename)], ".o");
      ld(1,0,full_filename);
      status = symFindByName(sysSymTbl, full_symbol, (char **)symbol_value, &type);
      free(c_filename);
      free(full_filename);
    }
  free(full_symbol);
  free(c_symbol);
  if(status == ERROR) {

    return LibKEYNOTFOU;
  } else
    return 1;  
}

char *LibFindImageSymbolErrString()
{
  return 0;
}

#else
STATIC_THREADSAFE char *FIS_Error = 0;

char *LibFindImageSymbolErrString()
{
  return FIS_Error;
}

STATIC_THREADSAFE int dlopen_mutex_initialized = 0;
#ifndef HAVE_WINDOWS_H
STATIC_THREADSAFE pthread_mutex_t dlopen_mutex;
#else
STATIC_THREADSAFE HANDLE dlopen_mutex;
#endif

STATIC_ROUTINE void dlopen_lock()
{

  if(!dlopen_mutex_initialized)
  {
    dlopen_mutex_initialized = 1;
#ifdef HAVE_WINDOWS_H
	pthread_mutex_init(&dlopen_mutex);
#else
	pthread_mutex_init(&dlopen_mutex, pthread_mutexattr_default);
#endif
  }

  pthread_mutex_lock(&dlopen_mutex);
}

STATIC_ROUTINE void dlopen_unlock()
{

  if(!dlopen_mutex_initialized)
  {
    dlopen_mutex_initialized = 1;
#ifdef HAVE_WINDOWS_H
    pthread_mutex_init(&dlopen_mutex);
#else
    pthread_mutex_init(&dlopen_mutex, pthread_mutexattr_default);
#endif
  }

  pthread_mutex_unlock(&dlopen_mutex);
}

int LibFindImageSymbol(struct descriptor *filename, struct descriptor *symbol, void **symbol_value)
{
  char *c_filename = MdsDescrToCstring(filename);
  char *full_filename = malloc(strlen(c_filename) + 10);
  void *handle;
  char *tmp_error1 = 0;
  char *tmp_error2 = 0;
  int dlopen_mode = RTLD_LAZY;
  int lib_offset=3;
  char *old_fis_error;
  
  *symbol_value = NULL;

#ifdef HAVE_WINDOWS_H
  strcpy(full_filename,c_filename);
  lib_offset=0;
#else
  if (strncmp(c_filename,"lib",3))
  {
    strcpy(full_filename,"lib");
    strcat(full_filename,c_filename);
  }
  else
    strcpy(full_filename,c_filename);
#endif
  if (strncmp(c_filename+strlen(c_filename)-strlen(SHARELIB_TYPE),SHARELIB_TYPE,strlen(SHARELIB_TYPE)))
    strcat(full_filename,SHARELIB_TYPE);
  dlopen_lock();
  old_fis_error=FIS_Error;
#ifdef linux
  dlopen_mode = RTLD_NOW /* | RTLD_GLOBAL */;
#endif
  handle = dlopen(full_filename,dlopen_mode);
  if (handle == NULL) {
    tmp_error1 = dlerror();
    if (tmp_error1 == NULL) tmp_error1="";
    tmp_error1 = strcpy((char *)malloc(strlen(tmp_error1)+1),tmp_error1);
    handle = dlopen(c_filename,dlopen_mode);
    if (handle == NULL) {
      tmp_error2 = dlerror();
      if (tmp_error2 == NULL) tmp_error2="";
      tmp_error2 = strcpy((char *)malloc(strlen(tmp_error2)+1),tmp_error2);
      handle = dlopen(&full_filename[lib_offset],dlopen_mode);
    }
  }
  if (handle != NULL)
  {
    char *c_symbol = MdsDescrToCstring(symbol);
    *symbol_value = dlsym(handle,c_symbol);
    if (!(*symbol_value))
    {
      char *tmp = dlerror();
      if (tmp == NULL) tmp="";
      sprintf((FIS_Error = (char *)malloc(strlen(tmp)+strlen("error finding symbol , ")+strlen(c_symbol)+1)),"error finding symbol %s, %s",
			  c_symbol,tmp);
    }
    free(c_symbol);
  }
  else 
  {
    char *tmp = dlerror();
    if (tmp == "") tmp="";
    sprintf((FIS_Error = (char *)malloc(strlen("Error loading library:\n\t %s - %s\n\t %s, %s\n\t%s - %s\n")+
      strlen(full_filename)*3+strlen(tmp)+strlen(tmp_error1)+strlen(tmp_error2)+10)),
      "Error loading library:\n\t %s - %s\n\t %s - %s\n\t%s - %s\n",c_filename,tmp_error1,full_filename,tmp_error2,&full_filename[3],tmp);
  }
  if (old_fis_error != 0 && old_fis_error != FIS_Error)
    free(old_fis_error);
  dlopen_unlock();
  if (tmp_error1)
    free(tmp_error1);
  if (tmp_error2)
    free(tmp_error2);
  free(c_filename);
  free(full_filename);
  if (*symbol_value == NULL)
    return LibKEYNOTFOU;
  else
    return 1;  
}

#endif

int StrConcat( struct descriptor *out, struct descriptor *first, ...)
{
  int i;
  int narg;
  va_list incrmtr;
  int status = StrCopyDx(out,first);
  if (status & 1)
  {
    va_count(narg);
    va_start(incrmtr,first);
    if (out->class == CLASS_D)
    {
      for (i=1;i<narg && (status & 1);i++)
        StrAppend(out,va_arg(incrmtr,struct descriptor *));
    }
    else if (out->class == CLASS_S)
    {
      struct descriptor temp = *out;
      struct descriptor *next;
      for (i=1,
           temp.length = (unsigned short)(out->length - first->length),
           temp.pointer = out->pointer + first->length;
           i<narg && (status & 1) && temp.length > 0;
           i++,
           temp.length = (unsigned short)(temp.length - next->length),
           temp.pointer += next->length)
      {
        next = va_arg(incrmtr,struct descriptor *);
        StrCopyDx(&temp,next);
      }
    }
    else
      status = 0;
  }
  return status;
}

int StrPosition(struct descriptor *source, struct descriptor *substring, int *start)
{
  char *source_c = MdsDescrToCstring(source);
  char *substring_c = MdsDescrToCstring(substring);
  char *search = source_c + ((start && *start > 0) ? (*start - 1) : 0);
  char *found = strstr(search,substring_c);
  int answer = found ? (found - source_c) + 1 : 0;
  free(source_c);
  free(substring_c);
  return answer;
}

int StrCopyR(struct descriptor *dest, unsigned short *len, char *source)
{
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  s.length = *len;
  s.pointer = source;
  return StrCopyDx(dest,&s);
}

int StrLenExtr(struct descriptor *dest, struct descriptor *source, int *start_in, int *len_in)
{
  unsigned short len = (unsigned short)((*len_in < 0) ? 0 : *len_in & 0xffff);
  unsigned short start = (unsigned short)((*start_in > 1) ? *start_in & 0xffff : 1);
  struct descriptor s = {0,DTYPE_T,CLASS_D,0};
  int status = StrGet1Dx(&len, &s);
  int i,j;
  memset(s.pointer,32,len);
  for (i=start-1,j=0; ((i<source->length) && (j < len));i++,j++)
    s.pointer[j]=source->pointer[i];
  status = StrCopyDx(dest,&s);
  StrFree1Dx(&s);
  return status;
}

int StrGet1Dx(unsigned short *len, struct descriptor *out)
{
  if (out->class != CLASS_D) return LibINVSTRDES;
  if (out->length == *len) return 1;
  if (out->length && (out->pointer != NULL))
    free(out->pointer);
  out->length = *len;
  out->pointer = *len ? malloc(*len) : NULL;
  return 1;
}

#ifdef HAVE_VXWORKS_H
static int timezone = 0;   /* vxworks THREADSAFE ? */
#endif



int LibEmul(int *m1, int *m2, int *add, _int64 *prod)
{
  _int64 m1_64 = *m1;
  _int64 m2_64 = *m2;
  _int64 add_64 = *add;
  *prod = m1_64 * m2_64 + add_64;
  return 1;
}

int LibSFree1Dd(struct descriptor *out)
{
  return StrFree1Dx(out);
}
  
int StrTrim(struct descriptor *out, struct descriptor *in, unsigned short *lenout)
{
  struct descriptor tmp = {0,DTYPE_T,CLASS_D,0};
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  unsigned short i;
  for (i=in->length;i>0;i--) if (in->pointer[i-1] != 32 && in->pointer[i-1] != 9) break;
  StrCopyDx(&tmp,in);
  s.length = i;
  s.pointer = tmp.pointer;
  if (lenout != NULL)
    *lenout = s.length;
  StrCopyDx(out,&s);
  return StrFree1Dx(&tmp);
}

int StrCopyDx(struct descriptor *out, struct descriptor *in)
{
  if (out->class == CLASS_D && (in->length != out->length))
    StrGet1Dx(&in->length, out);
  if (out->length && out->pointer != NULL)
  {
    int outlength = (out->class == CLASS_A) ? ((struct descriptor_a *)out)->arsize : out->length;
    int inlength = (in->class == CLASS_A) ? ((struct descriptor_a *)in)->arsize : in->length;
    int len=outlength > inlength ? inlength : outlength;
    char *p1,*p2;
    int i;
    for (i=0,p1=out->pointer,p2=in->pointer;i<len;i++) *p1++=*p2++;
    if (outlength > inlength)
      memset(out->pointer+inlength, 32, outlength - inlength);
  }
  return 1;
}

int StrCompare(struct descriptor *str1, struct descriptor *str2)
{
  char *str1c = MdsDescrToCstring(str1);
  char *str2c = MdsDescrToCstring(str2);
  int ans;
  ans = strcmp(str1c,str2c);
  free(str1c);
  free(str2c);
  return ans;
}

int StrUpcase(struct descriptor *out, struct descriptor *in)
{
  int outlength;
  int i;
  StrCopyDx(out,in);
  outlength = (out->class == CLASS_A) ? ((struct descriptor_a *)out)->arsize : out->length;
  for (i=0; i<outlength; i++)
    out->pointer[i] = (char)toupper(out->pointer[i]);
  return 1;
}

int StrRight(struct descriptor *out, struct descriptor *in, unsigned short *start) 
{
  struct descriptor tmp = {0,DTYPE_T,CLASS_D,0};
  struct descriptor s = {0,DTYPE_T,CLASS_S,0};
  StrCopyDx(&tmp,in);
  s.length = (unsigned short)((int)in->length - *start + 1);
  s.pointer = tmp.pointer+(*start-1);
  StrCopyDx(out,&s);
  return StrFree1Dx(&tmp);
}

int LibCreateVmZone(ZoneList **zone)
{
  ZoneList *list;
  *zone = malloc(sizeof(ZoneList));
  (*zone)->vm = NULL;
  (*zone)->next = NULL;
  if (MdsZones == NULL)
    MdsZones = *zone;
  else {
    for (list = MdsZones; list->next; list = list->next);
    list->next = *zone;
  }
  return (*zone != NULL);
}

int LibDeleteVmZone(ZoneList **zone)
{
  int found = 0;
  ZoneList *list,*prev;
  LibResetVmZone(zone);
  if (*zone == MdsZones)
  {
    found = 1;
    MdsZones = (*zone)->next;
  }
  else
  {
    for (prev=0,list = MdsZones; list && list != *zone; prev=list,list = list->next);
    if (list && prev)
    {
      prev->next = list->next;
      found = 1;
    }
  }
  if (found)
  {
    free(*zone);
    *zone=0;
  }
  return found;
}
   
int LibResetVmZone(ZoneList **zone)
{
  VmList *list;
  while ((list = zone ? (*zone ? (*zone)->vm : NULL) : NULL) != NULL)
    LibFreeVm(0, &list->ptr, zone);
  return 1;
}


int LibFreeVm(unsigned int *len, void **vm, ZoneList **zone)
{
  VmList *list = NULL;
  if (zone != NULL) {
    VmList *prev;
    for (prev = NULL, list = (*zone)->vm; 
         list && (list->ptr != *vm); prev=list, list = list->next);
    if (list) {
      if (prev)
        prev->next = list->next;
      else
	(*zone)->vm = list->next;
    }
  }
  free(*vm);
  if (list) 
    free(list);
  return 1;
}

int libfreevm_(int *len, void **vm, ZoneList **zone)
{
  return(LibFreeVm(len, vm, zone));
}

int libfreevm(int *len, void **vm, ZoneList **zone)
{
  return(LibFreeVm(len, vm, zone));
}

int LibGetVm(unsigned int *len, void **vm, ZoneList **zone)
{
  *vm = malloc(*len);
  if (*vm == NULL) {
    printf("Insufficient virtual memory\n");
  }
  if (zone != NULL) {
    VmList *list = malloc(sizeof(VmList));
    list->ptr = *vm;
    list->next = NULL;
    if ((*zone)->vm) {
      VmList *ptr;
      for (ptr = (*zone)->vm; ptr->next; ptr = ptr->next);
      ptr->next = list;
    } else (*zone)->vm = list;
  }
  return (*vm != NULL);
}

int libgetvm_(unsigned int *len, void **vm, ZoneList **zone)
{
  return(LibGetVm(len, vm, zone));
}


int libgetvm(int *len, void **vm, ZoneList **zone)
{
  return(LibGetVm(len, vm, zone));
}

int LibEstablish()
{
  return 1;
}

#define SEC_PER_DAY (60*60*24)

STATIC_ROUTINE int parsedate(char *asctim, void *dummy)
{
  return time(0);
}
#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))
STATIC_ROUTINE int mds_strcasecmp(char *in1, char *in2)
{
  int ans = -1;
  if (strlen(in1) == strlen(in2))
  {
    int i;
    int len1 = strlen(in1);
    ans = 0;
    for (i=0;i<len1;i++)
    {
      if (__tolower(in1[i]) != __tolower(in2[i]))
      {
        ans = -1;
        break;
      }
    }
  }
  return ans;
}

int LibConvertDateString(char *asc_time, _int64 *qtime)
{
  time_t tim=0;
  char time_out[24];
  int parse_it=1;
  int ctime_it=0;
  
  /* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
  if (asc_time == 0 || mds_strcasecmp(asc_time, "now") == 0) {
    tim=time(NULL);
    parse_it=0;
  } else if (mds_strcasecmp(asc_time, "today") == 0) {
    tim=time(NULL);
    ctime_it=1;
  } else if (mds_strcasecmp(asc_time, "tomorrow") == 0) {
    tim=time(NULL)+SEC_PER_DAY;
    ctime_it=1;
  }else if (mds_strcasecmp(asc_time, "yesterday") == 0) {
    tim=time(NULL)-SEC_PER_DAY;
    ctime_it=1;
  }
  if (parse_it) {
    if (ctime_it) {
      char *time_str;
      time_str = ctime(&tim);
      time_out[0]  = time_str[8];
      time_out[1]  = time_str[9];
      time_out[2]  = '-';
      time_out[3]  = time_str[4];
      time_out[4]  = (char)(time_str[5] & 0xdf);
      time_out[5]  = (char)(time_str[6] & 0xdf);
      time_out[6]  = '-';
      time_out[7]  = time_str[20];
      time_out[8]  = time_str[21];
      time_out[9]  = time_str[22];
      time_out[10] = time_str[23];
      time_out[11] = ' ';
      time_out[12] = '0';
      time_out[13] = '0';
      time_out[14] = ':';
      time_out[15] = '0';
      time_out[16] = '0';
      time_out[17] = ':';
      time_out[18] = '0';
      time_out[19] = '0';
      time_out[20] = '.';
      time_out[21] = '0';
      time_out[22] = '0';
      time_out[23] = 0;
      tim=0;
      asc_time=time_out;
    }
    
#ifndef HAVE_VXWORKS_H
	{
    struct tm tm = {0,0,0,0,0,0,0,0,0};
    char *tmp;
#ifdef HAVE_WINDOWS_H
    unsigned int day,year,hour,minute,second;
	char month[4];
	char *months[]={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
	if (sscanf(asc_time,"%u-%3s-%u %u:%u:%u", &day, &month, 
		&year, &hour, &minute, &second) < 6) {
			return 0;
		}
	_strupr(month);
	for (tm.tm_mon=0;tm.tm_mon<12;tm.tm_mon++) {
		if (strcmp(month,months[tm.tm_mon])==0) {
			break;	
		}
	}
	if ((tm.tm_mon==12) || (day <1) || (day > 31) ||
		(year < 1900) || (year > 2200) ||
		(hour > 23) || (minute > 59) || (second > 59)) return 0;
	tm.tm_mday=day;
	tm.tm_year=year-1900;
	tm.tm_hour=hour;
	tm.tm_min=minute;
	tm.tm_sec=second;
	tmp=asc_time;
#else
    tmp = strptime(asc_time, "%d-%b-%Y %H:%M:%S", &tm);
#endif
    if (tmp) {
      tm.tm_isdst=-1;
      tim = mktime(&tm);
      if ((int)tim == -1) return 0;
#if defined(HAVE_WINDOWS_Hxxxxx)
      _tzset();
      tim -= _timezone;
#endif
    }
    else
      tim = 0;
}
#else
    tim = 0; /*It is vxWorks */
#endif
  }
  if (tim > 0) {
    LibTimeToVMSTime(&tim,qtime);
    return tim > 0;
    *qtime = ((_int64)tim + daylight * 3600)*10000000+addin;
  } else
    *qtime = 0;
  return tim > 0;
}

int LibTimeToVMSTime(time_t *time_in,_int64 *time_out) {
  time_t t;
#ifdef HAVE_GETTIMEOFDAY
  struct timeval tm;
  suseconds_t microseconds=0;
  if (time_in == NULL) {
    gettimeofday(&tm,0);
    t = tm.tv_sec;
    microseconds = tm.tv_usec;
  } else t = *time_in;
#else
  int microseconds=0;
  t = (time_in == NULL) ? t=time(0) : *time_in;
#endif
#if defined(USE_TM_GMTOFF)
  /* this is a suggestion to change all code 
     for this as timezone is depricated unix
     annother alternative is to use gettimeofday */
  { 
    struct tm *tm;
    tm = localtime(&t);
    *time_out = (_int64)((unsigned int)t + tm->tm_gmtoff) * (_int64)10000000 + addin + microseconds * 10;
  }
#else
  tzset();
  *time_out = (_int64)(t - timezone + daylight * 3600) * (_int64)10000000 + addin + microseconds * 10;
#endif
  return 1;
}

time_t LibCvtTim(int *time_in,double *t) 
{
  double t_out;
  time_t bintim = time(&bintim);
#ifndef HAVE_VXWORKS_H
  if (time_in)
  {
    _int64 time_local;
    double time_d;
    struct tm *tmval;
    time_t time_int;
    time_t dummy=0;
    memcpy(&time_local,time_in,sizeof(time_local));
    time_local = (*(_int64 *)time_in - addin);
    if (time_local < 0) time_local=0;
    bintim=time_local/LONG_LONG_CONSTANT(10000000);
    time_d = (double)bintim + (double)(time_local % LONG_LONG_CONSTANT(10000000))*1E-7;
    tmval = localtime(&bintim);
#ifdef USE_TM_GMTOFF
    t_out = (time_d > 0 ? time_d : 0) - tmval->tm_gmtoff; // - (tmval->tm_isdst ? 3600 : 0);
    bintim -= tmval->tm_gmtoff;
#else
    t_out = (time_d > 0 ? time_d : 0) + timezone - daylight * (tmval->tm_isdst ? 3600 : 0);
#endif
    //    bintim = (long)t_out;
  }
  else
    bintim = (long)(t_out = time(0));
#else
  bintim = t_out = time(0);
#endif
  if (t != 0)
    *t = t_out;
  return(bintim);
}
#ifndef HAVE_WINDOWS_H
#ifdef HAVE_VXWORKS_H
#include <time.h>
#else
#include <sys/time.h>
#endif
#endif
int LibSysAscTim(unsigned short *len, struct descriptor *str, int *time_in)
{
  char *time_str;
  char time_out[23];
  unsigned short slen=sizeof(time_out);
  time_t bintim = LibCvtTim(time_in,0);
  _int64 chunks=0;
  _int64 *time_q=(_int64 *)time_in;
#ifndef HAVE_VXWORKS_H
  tzset();
#endif
  if (time_in != NULL) {
#ifdef HAVE_GETTIMEOFDAYx
    _int64 tmp;
    struct timeval tv;
	struct timezone tz;
    gettimeofday(&tv,&tz);
    tmp = (*time_q-0x7c95674beb4000ULL)/10000000+tz.tz_minuteswest*60-(daylight * 3600);
    bintim=(tmp < 0) ? (time_t)0 : (time_t)tmp;
#endif
    chunks = *time_q % 10000000;
  }
  time_str = ctime(&bintim);
  time_out[0]  = time_str[8];
  time_out[1]  = time_str[9];
  time_out[2]  = '-';
  time_out[3]  = time_str[4];
  time_out[4]  = (char)(time_str[5] & 0xdf);
  time_out[5]  = (char)(time_str[6] & 0xdf);
  time_out[6]  = '-';
  time_out[7]  = time_str[20];
  time_out[8]  = time_str[21];
  time_out[9]  = time_str[22];
  time_out[10] = time_str[23];
  time_out[11] = ' ';
  time_out[12] = time_str[11];
  time_out[13] = time_str[12];
  time_out[14] = time_str[13];
  time_out[15] = time_str[14];
  time_out[16] = time_str[15];
  time_out[17] = time_str[16];
  time_out[18] = time_str[17];
  time_out[19] = time_str[18];
  time_out[20] = '.';
  time_out[21] = '0' + (char)(chunks/1000000);
  time_out[22] = '0' + (char)((chunks % 1000000)/100000);
  StrCopyR(str,&slen,time_out);
  if (len) *len = slen;
  return 1;
}

int LibGetDvi(int *code, void *dummy1, struct descriptor *device, int *ans, struct descriptor *ans_string, int *len)
{
  *ans = 132;
  return 1;
}

int StrAppend(struct descriptor *out, struct descriptor *tail)
{
  if (tail->length != 0 && tail->pointer != NULL)
  {
    struct descriptor new = {0,DTYPE_T,CLASS_D,0};
    unsigned short len = (unsigned short)(out->length + tail->length);
    if (((unsigned int)out->length + (unsigned int)tail->length) > 0xffff)
      return StrSTRTOOLON;
    StrGet1Dx(&len,&new);
    memcpy(new.pointer, out->pointer, out->length);
    memcpy(new.pointer + out->length, tail->pointer, tail->length);
    StrFree1Dx(out);
    *out = new;
  }
  return 1;
}
    
int StrFree1Dx(struct descriptor *out)
{
  if (out->class == CLASS_D) {
    if (out->pointer)
      free(out->pointer);
    out->pointer = NULL;
    out->length = 0;
  }
  return 1;
}

int StrFindFirstNotInSet(struct descriptor *source, struct descriptor *set)
{
  int ans = 0;
  if (source->length > 0) {
    char *src = MdsDescrToCstring(source);
    char *s = MdsDescrToCstring(set);
    size_t tmp;
    tmp = strspn(src, s);
    ans = (int)((tmp == strlen(src)) ? 0 : (tmp + 1));
    free(src);
    free(s);
  }
  return ans;
}

int StrFindFirstInSet(struct descriptor *source, struct descriptor *set)
{
  int ans = 0;
  if (source->length > 0) {
    char *src = MdsDescrToCstring(source);
    char *s = MdsDescrToCstring(set);
    char *tmp;
    tmp = (char *)strpbrk(src, s);
    ans = tmp == NULL ? 0 : tmp - src + 1;
    free(src);
    free(s);
  }
  return ans;
}

struct node { void   *left;
              void   *right;
              short bal;};

struct bbtree_info { struct node  *currentnode;
                     char         *keyname;
		     int          (*compare_routine)();
		     int 	  (*alloc_routine)();
		     struct node  *new_node;
		     int           foundintree;
		     int           controlflags;
		     void         *user_context; };

STATIC_ROUTINE int MdsInsertTree();

int LibInsertTree(struct node **treehead,char *symbol_ptr, int *control_flags, int (*compare_rtn)(),
                    int (*alloc_rtn)(), struct node **blockaddr, void *user_data)
{
  struct bbtree_info bbtree;
  bbtree.currentnode = *treehead;
  bbtree.keyname = symbol_ptr;
  bbtree.compare_routine = compare_rtn;
  bbtree.alloc_routine = alloc_rtn;
  bbtree.new_node = NULL;
  bbtree.foundintree = 0;
  bbtree.controlflags = *control_flags;
  bbtree.user_context = user_data;
  MdsInsertTree(&bbtree);
  if (bbtree.foundintree == 1)
    *treehead = bbtree.currentnode;
  *blockaddr = bbtree.new_node;
  return bbtree.foundintree;
}

STATIC_ROUTINE int MdsInsertTree(struct bbtree_info *bbtree_ptr)
{

#define currentNode (bbtree_ptr->currentnode)
#define ALLOCATE    (*(bbtree_ptr->alloc_routine))
/*
#define left_of(node_ptr) (node_ptr->left ? (struct node *)((int)(node_ptr) + node_ptr->left) : 0)
#define right_of(node_ptr) (node_ptr->right ? (struct node *)((int)(node_ptr) + node_ptr->right) : 0)
#define offset_of(node_ptr, offnode_ptr) (offnode_ptr ? ((int)(offnode_ptr)-(int)(node_ptr)) : 0)
*/
#define left_of(node_ptr) node_ptr->left
#define right_of(node_ptr) node_ptr->right
#define offset_of(node_ptr, offnode_ptr) offnode_ptr

    struct node *save_current;
    int in_balance;
    struct node *down_left;
    struct node *down_right;

    if (currentNode == 0)
    {
	if (!(ALLOCATE(bbtree_ptr->keyname, &save_current, bbtree_ptr->user_context) & 1)) return 0;
	currentNode = save_current;
	currentNode->left = 0;
	currentNode->right = 0;
	currentNode->bal = 0;
	bbtree_ptr->new_node = save_current;
	bbtree_ptr->foundintree = 1;
	return 0;
    }
    save_current = currentNode;
    if ((in_balance = (*(bbtree_ptr->compare_routine))(bbtree_ptr->keyname, currentNode, bbtree_ptr->user_context)) <= 0)
    {
	if ( (in_balance == 0) && (!(bbtree_ptr->controlflags & 1)) )
	{
	    bbtree_ptr->new_node = save_current;
	    bbtree_ptr->foundintree = 3;
	    return 1;
	}
	currentNode = left_of(currentNode);
	in_balance = MdsInsertTree(bbtree_ptr);
	if ( (bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0) ) return 1;
	down_left = currentNode;
	currentNode = save_current;
	currentNode->left = offset_of(currentNode,down_left);
	if (in_balance) return 1;
	else
	{
	    currentNode->bal--;
	    if (currentNode->bal == 0) return 1;
	    else
	    {
		if (currentNode->bal & 1) return 0;
		down_left = left_of(currentNode);
		if (down_left->bal < 0)
		{
		    currentNode->left = offset_of(currentNode,right_of(down_left));
		    down_left->right = offset_of(down_left,currentNode);
		    currentNode->bal = 0;
		    currentNode = down_left;
		    currentNode->bal = 0;
		    return 1;
		}
		else
		{
		    down_right = right_of(down_left);
		    down_left->right = offset_of(down_left,left_of(down_right));
		    down_right->left = offset_of(down_right,down_left);
		    currentNode->left = offset_of(currentNode,right_of(down_right));
		    down_right->right = offset_of(down_right,currentNode);
		    currentNode->bal = 0;
		    down_left->bal = 0;
		    if (down_right->bal > 0) down_left->bal = -1;
		    else if (down_right->bal < 0) currentNode->bal = 1;
		    currentNode = down_right;
		    currentNode->bal = 0;
		    return 1;
		}
	    }
	}
    }
    else
    {
	currentNode = right_of(currentNode);
	in_balance = MdsInsertTree(bbtree_ptr);
	if ( (bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0) ) return 1;
	down_right = currentNode;
	currentNode = save_current;
	currentNode->right = offset_of(currentNode,down_right);
	if (in_balance)
	    return 1;
	else
	{
	    currentNode->bal++;
	    if (currentNode->bal == 0)
		return 1;
	    else
	    {
		if (currentNode->bal & 1) return 0;
		down_right = right_of(currentNode);
		if (down_right->bal > 0)
		{
		    currentNode->right = offset_of(currentNode,left_of(down_right));
		    down_right->left = offset_of(down_right,currentNode);
		    currentNode->bal = 0;
		    currentNode = down_right;
		    currentNode->bal = 0;
		    return 1;
		}
		else
		{
		    down_left = left_of(down_right);
		    down_right->left = offset_of(down_right,right_of(down_left));
		    down_left->right = offset_of(down_left,down_right);
		    currentNode->right = offset_of(currentNode,left_of(down_left));
		    down_left->left = offset_of(down_left,currentNode);
		    currentNode->bal = 0;
		    down_right->bal = 0;
		    if (down_left->bal < 0) down_right->bal = 1;
		    else if (down_left->bal > 0) currentNode->bal = -1;
		    currentNode = down_left;
		    currentNode->bal = 0;
		    return 1;
		}
	    }
	}
    }
}
#undef currentNode

int LibLookupTree(struct node **treehead, int *symbolstring, int (*compare_rtn)(), struct node **blockaddr)
{
  int ch_result;
  struct node *currentnode = *treehead;
  while (currentnode != 0)
  {
    if ( (ch_result = (*compare_rtn)(symbolstring,currentnode)) == 0)
    {
      *blockaddr = currentnode;
      return 1;
    }
    else if (ch_result < 0)
      currentnode = left_of(currentnode);
    else
      currentnode = right_of(currentnode);
  }
  return LibKEYNOTFOU;
}

STATIC_ROUTINE int MdsTraverseTree(int (*user_rtn)(), void *user_data, struct node *currentnode);

int LibTraverseTree(struct node **treehead, int (*user_rtn)(), void *user_data)
{ 
    return MdsTraverseTree(user_rtn, user_data, *treehead);
}

STATIC_ROUTINE int MdsTraverseTree(int (*user_rtn)(), void *user_data, struct node *currentnode)
{ 
    struct node *right_subtree;
    int status;
    if (currentnode == 0) return 1;
    if (left_of(currentnode))
    {
	status = MdsTraverseTree(user_rtn,user_data,left_of(currentnode));
	if (!(status & 1)) return status;
    }
    right_subtree = right_of(currentnode);
    status = (*user_rtn)(currentnode,user_data);
    if (!(status & 1)) return status;
    if (right_subtree)
    {
	status = MdsTraverseTree(user_rtn,user_data,right_subtree);
	if (!(status & 1)) return status;
    }
    return 1;
}

int StrCaseBlindCompare(struct descriptor *one, struct descriptor *two)
{
  int ans;
  char *one_c = MdsDescrToCstring(one);
  char *two_c = MdsDescrToCstring(two);
  int i;
  for (i=0;i<one->length;i++)
    one_c[i] = (char)toupper(one_c[i]);
  for (i=0;i<two->length;i++)
    two_c[i] = (char)toupper(two_c[i]);
  ans = strcmp(one_c,two_c);
  free(one_c);
  free(two_c);
  return ans;
}
unsigned int StrMatchWild(struct descriptor *candidate, struct descriptor *pattern)
{
  struct descr {
     int length;
     char *ptr;
  };
  struct descr cand;
  struct descr scand;
  struct descr pat;
  struct descr spat;
  char pc;
  int true;
  true = 1;
  cand.length = candidate->length;
  cand.ptr = candidate->pointer;
  scand = cand;
  pat.length = pattern->length;
  pat.ptr = pattern->pointer;
  spat = pat;
  scand.length = 0;

  while (true) {
    if (--pat.length < 0)
    {
      if (cand.length == 0)
        return StrMATCH;
      else 
      {
        if (--scand.length < 0)
          return StrNOMATCH;
        else {
          scand.ptr++;
          cand = scand;
          pat = spat;
        }
      }
    }
    else 
    {
      if ((pc = *pat.ptr++) == '*')
      {
        if (pat.length == 0) return StrMATCH;
        scand = cand;
        spat = pat;
      }
      else
      {
        if (--cand.length < 0) return StrNOMATCH;
        if (*cand.ptr++ != pc)
        {
          if (pc != '%')
          {
            if (--scand.length < 0)
              return StrNOMATCH;
            else {
              scand.ptr++;
              cand = scand;
              pat = spat;
            }
          }
        }
      } 
    }   
  }
  return StrNOMATCH;
}

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (a) > (b) ? (a) : (b)
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (a) < (b) ? (a) : (b)

int StrElement(struct descriptor *dest, int *num, struct descriptor *delim, struct descriptor *src)
{
  char *src_ptr = src->pointer;
  char *se_ptr = src_ptr+src->length;
  char *e_ptr;
  unsigned short len;
  int cnt;
  int status;
  if (delim->length != 1) return StrINVDELIM;
  for (cnt=0; (cnt<*num)&&(src_ptr<=se_ptr); src_ptr++)
    if (*src_ptr==*delim->pointer) cnt++;
  if (cnt < *num) return StrNOELEM;
  for(e_ptr=src_ptr; src_ptr<se_ptr; src_ptr++)
    if (*src_ptr == *delim->pointer) break;
  len = (unsigned short)(src_ptr-e_ptr);
  status = StrCopyR(dest, &len, e_ptr);
  return status;
}

int StrTranslate(struct descriptor *dest, struct descriptor *src, struct descriptor *tran, struct descriptor *match)
{
  int status = 0;
  if (src->class == CLASS_S || src->class == CLASS_D)
  {
    char *dst = (char *)malloc(src->length);
    int i;
    for (i=0;i<src->length;i++)
    {
      int j;
      int next = 1;
      for (j=0;next && j<match->length;j += next)
        next = (match->pointer[j] != src->pointer[i]) ? 1 : 0;
      dst[i] = (char)(next ? src->pointer[i] : ((j < tran->length) ? tran->pointer[j] : ' '));
    }   
    status = StrCopyR(dest, &src->length, dst);
    free(dst);
  }
  else if ((src->class == CLASS_A) && (dest->class == CLASS_A) && (src->length > 0) && (dest->length > 0) &&
     (((struct descriptor_a *)src)->arsize/src->length == ((struct descriptor_a *)dest)->arsize/dest->length))
  {
    struct descriptor outdsc = {0, DTYPE_T, CLASS_S, 0};
    struct descriptor indsc = {0, DTYPE_T, CLASS_S, 0};
    int num = ((struct descriptor_a *)src)->arsize / src->length;
    int i;
    outdsc.length = dest->length;
    outdsc.pointer = dest->pointer;
    indsc.length = src->length;
    indsc.pointer = src->pointer;
    for (i=0;i<num;i++,outdsc.pointer += outdsc.length,indsc.pointer += indsc.length)
      status = StrTranslate(&outdsc, &indsc, tran, match);
  }
  return status;
}

int StrReplace(struct descriptor *dest, struct descriptor *src, int *start_idx, int *end_idx, struct descriptor *rep)
{
  int status;
  int start;
  int end;
  unsigned short int dstlen;
  char *dst;
  char *sptr;
  char *dptr;
  start = MAX(1,MIN(*start_idx, src->length));
  end = MAX(1,MIN(*end_idx, src->length));
  dstlen = (unsigned short) (start-1+rep->length+src->length-end+1);
  dst = (char *)malloc(dstlen);
  for (sptr=src->pointer, dptr=dst; (dptr-dst) < (start-1) ;*dptr++=*sptr++) ;
  for (sptr=rep->pointer; (sptr-rep->pointer) < rep->length ;*dptr++=*sptr++) ; 
  for (sptr=src->pointer+end; (sptr-src->pointer) < src->length ;*dptr++=*sptr++) ;
  status = StrCopyR(dest, &dstlen, dst);
  free(dst);
  return status;
}

typedef struct {
  char *env;
  char *file;
  struct descriptor wild_descr;
  char **env_strs;
  int  num_env;
  int  next_index;
  int  next_dir_index; /* index intor env_strs to put the next directory to search */
  DIR  *dir_ptr;
} FindFileCtx;


STATIC_ROUTINE int FindFile(struct descriptor *filespec, struct descriptor *result, int **ctx, int recursively, int caseBlind);
STATIC_ROUTINE int FindFileStart(struct descriptor *filespec, FindFileCtx **ctx, int caseBlind);
STATIC_ROUTINE int FindFileEnd(FindFileCtx *ctx);
STATIC_ROUTINE char *_FindNextFile(FindFileCtx *ctx, int recursively, int caseBlind);

extern int LibFindFile(struct descriptor *filespec, struct descriptor *result, int **ctx)
{
  return FindFile(filespec, result, ctx, 0, 0);
}
extern int LibFindFileRecurseCaseBlind(struct descriptor *filespec, struct descriptor *result, int **ctx)
{
  return FindFile(filespec, result, ctx, 1, 1);
}
STATIC_ROUTINE int FindFile(struct descriptor *filespec, struct descriptor *result, int **ctx, int recursively, int caseBlind)
{
  unsigned int status;
  char *ans;
  if (*ctx == 0) {
    status = FindFileStart(filespec, (FindFileCtx **)ctx, caseBlind);
    if ((status&1) ==0)
      return status;
  }
  ans = _FindNextFile((FindFileCtx *)*ctx, recursively, caseBlind);
  if (ans != 0) {
    struct descriptor ansd = {0, DTYPE_T, CLASS_S,0};
    ansd.length = strlen(ans);
    ansd.pointer = ans;
    StrCopyDx(result,&ansd);
    free(ans);
    status = 1;
  }
  else {
    status = 0;
    LibFindFileEnd(ctx);    
  }
  return status;
}

extern int LibFindFileEnd(int **ctx)
{
  int status = FindFileEnd((FindFileCtx *)*ctx);
  if (status) *ctx=NULL;
  return status;
}

STATIC_ROUTINE int FindFileEnd(FindFileCtx *ctx)
{
  int i;
  if (ctx != NULL) {
    if (ctx->dir_ptr) {
      closedir(ctx->dir_ptr);
      ctx->dir_ptr = 0;
    }
    if (ctx->env != 0) free (ctx->env);
    if (ctx->file != 0) free (ctx->file);
    for (i=0; i<ctx->num_env; i++)
      if (ctx->env_strs[i] != 0)
        free(ctx->env_strs[i]);
    if (ctx->env_strs != 0) free(ctx->env_strs);
    free(ctx);
  }
  return 1;
}

#define CSTRING_FROM_DESCRIPTOR(cstring, descr)\
  cstring=strncpy(malloc(descr->length+1),descr->pointer,descr->length);\
  cstring[descr->length] = '\0';

STATIC_ROUTINE int FindFileStart(struct descriptor *filespec, FindFileCtx **ctx, int caseBlind)
{
  FindFileCtx *lctx;
  char *fspec;
  char *colon;
  *ctx = (FindFileCtx *)malloc(sizeof(FindFileCtx));
  memset(*ctx,0,sizeof(FindFileCtx));
  lctx = *ctx;

  CSTRING_FROM_DESCRIPTOR(fspec, filespec)

  lctx->next_index = lctx->next_dir_index = 0;
  colon = (char *)index(fspec, ':');
  if (colon == 0) {
    lctx->env = 0;
    colon = fspec-1;
  }
  else {
    lctx->env = strncpy(malloc(colon-fspec+1),fspec,colon-fspec);
    lctx->env[colon-fspec] = '\0';
  }
  if (strlen(colon+1)==0) {
    if (lctx->env) free(lctx->env);
    free(fspec);
    return 0;    
  }
  else {
    lctx->file = malloc(strlen(colon+1)+1);
    strcpy(lctx->file, colon+1);
    lctx->wild_descr.length = strlen(colon+1);
    lctx->wild_descr.pointer=lctx->file;
    lctx->wild_descr.dtype = DTYPE_T;
    lctx->wild_descr.class = CLASS_S;
    if (caseBlind)
      StrUpcase(&lctx->wild_descr,&lctx->wild_descr);
    free(fspec);
  }
  if (lctx->env != 0) {
    int num = 0;
    char *env;
    char *semi;
	char *env_sav;
	env = env_sav = TranslateLogical(lctx->env);
    if (env != 0) {
      if (env[strlen(env)-1] != ';') {
		  char *tmp = malloc(strlen(env)+2);
		  strcpy(tmp, env);
		  strcat(tmp, ";");
		  env = tmp;
      }
      else {
		  char *tmp = malloc(strlen(env)+1);
		  strcpy(tmp, env);
		  env = tmp;
      }
	  free(env_sav);
      for(semi=(char *)index(env, ';'); semi!= 0; num++, semi=(char *)index(semi+1, ';'));
      if (num > 0) {
		  char *ptr;
		  int i;
		  lctx->num_env = num;
		  lctx->env_strs = (char **)malloc(num*sizeof(char *));
		  for (ptr=env,i=0; i<num; i++) {
			  char *cptr;
			  int len = ((cptr= (char *)index(ptr, ';'))==(char *)0) ? (int)strlen(ptr) : cptr-ptr; 
			  lctx->env_strs[i] = strncpy(malloc(len+1),ptr,len);
			  lctx->env_strs[i][len] = '\0';
			  ptr=cptr+1;
		  }
      }
      free(env);
    }
  }
  lctx->dir_ptr = 0;
  return 1;
}

STATIC_ROUTINE char *_FindNextFile(FindFileCtx *ctx, int recursively, int caseBlind)
{
  char *ans;
  struct dirent *dp;

  int found = 0;
  while (! found) {
    while (ctx->dir_ptr==0)
    {
      if (ctx->next_index < ctx->num_env) {
        ctx->dir_ptr=opendir(ctx->env_strs[ctx->next_index++]);
	ctx->next_dir_index = ctx->next_index;
      }
      else
        return 0;
    }
    dp = readdir(ctx->dir_ptr);
    if (dp != NULL) {
      struct descriptor upname = {0,DTYPE_T,CLASS_D,0};
      DESCRIPTOR_FROM_CSTRING(filename, dp->d_name)
      if (caseBlind)
	{
        StrUpcase(&upname,&filename);
      }
      else {
	StrCopyDx(&upname,&filename);
      }
      found = StrMatchWild(&upname, &ctx->wild_descr)&1;
      StrFree1Dx(&upname);
      if (recursively) {
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
          char *tmp_dirname;
	  DIR *tmp_dir;
	  tmp_dirname = malloc(strlen(ctx->env_strs[ctx->next_index-1])+2+strlen(dp->d_name));
          strcpy(tmp_dirname, ctx->env_strs[ctx->next_index-1]);
          strcat(tmp_dirname, "/");
	  strcat(tmp_dirname, dp->d_name);
	  tmp_dir = opendir(tmp_dirname);
	  if (tmp_dir != NULL) {
	    int i;
	    closedir(tmp_dir);
	    ctx->env_strs = realloc(ctx->env_strs, sizeof(char*)*(ctx->num_env+1));
	    for(i=ctx->num_env-1; i>=ctx->next_dir_index; i--)
	      ctx->env_strs[i+1] = ctx->env_strs[i];
	    ctx->env_strs[ctx->next_dir_index] = tmp_dirname;
	    ctx->num_env++;
	    ctx->next_dir_index++;
	  }
          else
	    free(tmp_dirname);
	}
      }
    }
    else {
     closedir(ctx->dir_ptr);
     ctx->dir_ptr = NULL;
    }
  }
  ans = malloc(strlen(ctx->env_strs[ctx->next_index-1])+1+strlen(dp->d_name)+1);
  strcpy(ans, ctx->env_strs[ctx->next_index-1]);
  strcat(ans, "/");
  strcat(ans, dp->d_name);
  return ans;
}

void TranslateLogicalFree(char *value)
{
	free(value);
}

#ifdef LOBYTE
#undef LOBYTE
#endif
#ifdef HIBYTE
#undef HIBYTE
#endif
#define LOBYTE(x) ((unsigned char)((x) & 0xFF))
#define HIBYTE(x) ((unsigned char)((x) >> 8))

STATIC_ROUTINE unsigned short icrc1(unsigned short crc)
{
  int i;
  unsigned short ans=crc;
  for (i=0;i<8;i++)
  {
    if (ans & 0x8000)
    {
      ans <<= 1;
      ans = ans ^ 4129;
    }
    else
      ans <<= 1;
  }
  return ans;
}

unsigned short Crc(unsigned int len, unsigned char *bufptr)
{
  STATIC_THREADSAFE unsigned short icrctb[256],init=0;
  STATIC_THREADSAFE unsigned char rchr[256];
  STATIC_CONSTANT unsigned it[16]={0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
  unsigned short j,cword=0;
  if (!init)
  {
    init=1;
    for (j=0;j<256;j++)
    {
      icrctb[j]=icrc1((unsigned short)(j << 8));
      rchr[j]=(unsigned char)(it[j & 0xF] << 4 | it[j >> 4]);
    }
  }
  for (j=0;j<len;j++)
    cword = icrctb[bufptr[j] ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;
  return cword;
}

int MdsPutEnv(char *cmd)
{
  int status;
  if (strstr(cmd,"MDSPLUS_SPAWN_WRAPPER") ||
      strstr(cmd,"MDSPLUS_LIBCALL_WRAPPER"))
    status = 0;
  else
  {
    char *tmp=strcpy(malloc(strlen(cmd)+1),cmd);
    putenv(tmp);
    status = 1;
  }
  return status;
}

int libffs(int *position, int *size, char *base, int *find_position)
{
  int i;
  int status = 0;
  int *bits = (int *)(base + (*position)/8); 
  int top_bit_to_check = ((*size)+*position)-((*position)/8)*8;
  for (i=(*position) % 8;i<top_bit_to_check;i++)
  {
    if (*bits & (1 << i))
    {
      *find_position = ((*position)/8)*8 + i;
      status = 1;
      break;
    }
  }
  return status;
}
STATIC_THREADSAFE char RELEASE[512] = {0};
STATIC_THREADSAFE struct descriptor RELEASE_D = {0,DTYPE_T,CLASS_S,RELEASE};


char *MdsRelease()
{
#include <release.h>
  STATIC_CONSTANT const char *tag = MDSPLUS_RELEASE;
  if (RELEASE[0] == 0)
  {
    int major = 0;
    int minor = 0;
    int sub = 0;
    int status = sscanf(&tag[1],"Name: release-%d-%d-%d",&major,&minor,&sub);
    if (status == 0)
      strcpy(RELEASE,"MDSplus, beta version");
    else if (status == 1)
      sprintf(RELEASE,"MDSplus, Version %d",major);
    else if (status == 2)
      sprintf(RELEASE,"MDSplus, Version %d.%d",major,minor);
    else if (status == 3)
      sprintf(RELEASE,"MDSplus, Version %d.%d-%d",major,minor,sub);
    RELEASE_D.length = strlen(RELEASE);
  }
  return RELEASE;
}

struct descriptor *MdsReleaseDsc()
{
  MdsRelease();
  return &RELEASE_D;
}

STATIC_ROUTINE char *GetTdiLogical(char *name)
{
  int status;
  char *ans = 0;
  STATIC_THREADSAFE int (*TdiExecute)()=0;
  STATIC_CONSTANT DESCRIPTOR(exp,"data(ENV($))");
  struct descriptor name_d = {0,DTYPE_T,CLASS_S,0};
  struct descriptor ans_d = {0,DTYPE_T,CLASS_D,0};
  STATIC_CONSTANT DESCRIPTOR(image,"TdiShr");
  STATIC_CONSTANT DESCRIPTOR(routine,"TdiExecute");
  if (TdiExecute == 0)
  {
    status = LibFindImageSymbol((struct descriptor *)&image,(struct descriptor *)&routine,(void **)&TdiExecute);
    if (!(status & 1))
      return 0;
  }
  name_d.length = strlen(name);
  name_d.pointer = name;
  status = (TdiExecute)(&exp,&name_d, &ans_d MDS_END_ARG);
  if (status & 1)
  {
    ans = strncpy(malloc(ans_d.length+1),ans_d.pointer,ans_d.length);
    ans[ans_d.length]=0;
    MdsFree1Dx((struct descriptor_xd *)&ans_d,NULL);
  }
  else
    ans = 0;
  return ans;
}
