#if defined(vms)
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#include        <signal.h>
#include        <sys/time.h>
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
    struct itimerval  value;		/* Two "timeval" structs	*/
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
#else

    getitimer(ITIMER_REAL,&value);

    value.it_value.tv_sec = nsec;
    value.it_value.tv_usec = millisec;
    setitimer(ITIMER_REAL,&value,0);

    getitimer(ITIMER_REAL,&value);
    signal(SIGALRM,no_op);		/* declare handler routine	*/
    sigpause(SIGALRM);
#endif
    sts = 1;
#endif
    return(sts);
   }
