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
    timer.tv_sec = nsec;
    timer.tv_nsec = millisec * 1000000;
    pthread_delay_np(&timer);
#endif
    sts = 1;
#endif
    return(sts);
   }
