#if defined(vms)
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#include        <signal.h>
#include        <sys/time.h>
#include        <pthread.h>
#endif
#include        <stdio.h>
#include        "mdsdcl.h"

/***********************************************************************
* MDSDCL_WAIT.C --
*
************************************************************************/


#if defined(vms)
int   sys$bintim();
int   sys$setimr();
int   sys$waitfr();
#else
static void  no_op()  {return;}
#endif


	/**************************************************************
	 * mdsdcl_wait:
	 **************************************************************/
int   mdsdcl_wait()		/* Return:  status			*/
   {
    int   k;
    int   sts;
#if defined(vms)
    int   efn;
    unsigned int   bintim[2];
#else
    int   day,hr,min,sec,millisec;
    int   nsec;
    float fsec;
#if !defined(_WIN32)
#ifdef _NO_THREADS
    struct itimerval  value;		/* Two "timeval" structs	*/
#else
    struct timespec timer;
#endif
#endif
#endif
    static DYNAMIC_DESCRIPTOR(dsc_deltatime);

    sts = cli_get_value("DELTA_TIME",&dsc_deltatime);
    if (~sts & 1)
        return(sts);

#if defined(vms)
    sts = sys$bintim(&dsc_deltatime,bintim);
    if (sts & 1)
       {
        lib$get_ef(&efn);
        sys$setimr(efn,bintim,0,0);
        sys$waitfr(efn);
        lib$free_ef(&efn);
       }
#else
    k = sscanf(dsc_deltatime.dscA_pointer,"%d %d:%d:%f",
                &day,&hr,&min,&fsec);
    if (k != 4)
       {
        MdsMsg(0,"--> invalid time string");
        return(CLI_STS_BADLINE);
       }
    sec = (int)fsec;
    millisec = (int)((fsec - (float)sec) * 1000.);
    nsec = sec + 60*(min + 60*(hr + 24*day));
#if defined(_WIN32)
	millisec += nsec * 1000;
	Sleep(millisec);
#elif defined(_NO_THREADS)

    getitimer(ITIMER_REAL,&value);

    value.it_value.tv_sec = nsec;
    value.it_value.tv_usec = millisec;
    setitimer(ITIMER_REAL,&value,0);

    getitimer(ITIMER_REAL,&value);
    signal(SIGALRM,no_op);		/* declare handler routine	*/
    sigpause(SIGALRM);
#else
    {
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_condattr_default NULL
#define pthread_mutexattr_default NULL
#endif
      static pthread_cond_t wait_cond;
      static pthread_mutex_t wait_mutex;
      static int initialized = 0;
      struct timespec delta_time = {nsec,millisec},abstime;
      if (!initialized)
      {
        pthread_cond_init(&wait_cond,pthread_condattr_default);
        pthread_mutex_init(&wait_mutex,pthread_mutexattr_default);
        initialized = 1;
      }
      pthread_get_expiration_np(&delta_time,&abstime);
      pthread_cond_timedwait(&wait_cond,&wait_mutex,&abstime);
    }
    /*
    timer.tv_sec = nsec;
    timer.tv_nsec = millisec * 1000000;
    pthread_delay_np(&timer);
    */
#endif
    sts = 1;
#endif
    return(sts);
   }
