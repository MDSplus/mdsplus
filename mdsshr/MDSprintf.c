/*
* MDSprintf.c --
*
* Functions to allow re-direction of
*    "printf(...)" and
*    "fprintf(stderr, ...)"
*
* Usage:
*   int MDSprintf( char *fmt , ... )
*     - Use like printf()
*
*   int MDSfprintf( FILE *fp , char *fmt , ... )
*     - Use like fprintf().  Affects only stderr and stdout
*
*   void  MdsSetOutputFunctions(
*       int (*NEWvprintf)()
*      ,int (*NEWvfprintf)()
*      )
*     - Set addr of functions called via MDSprintf and MDSfprintf.
*       Special values:
*          0 :  suppress output
*         -1 :  restore default (vprintf / vfprintf)
*
*       Functions, if specified, must handle variable-length arg lists.
*       Defaults are:
*         o vprintf( char *fmt , va_list ap )           and
*         o vfprintf( FILE *fp , char *fmt , va_list ap )
*             Note: called only for "fp" stderr and stdout.
*       Any user-written function should follow these prototypes.
*
*   void  MdsGetOutputFunctions(
*        void **CURvprintf
*       ,void **CURvfprintf
*       )
*      - Get addresses of current functions (see above).
*
* History:
*  14-Feb-2001  TRG  Create.
*
*********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


		/*=====================================================
		 * Static variables ...
		 *====================================================*/
static int  (*MDSvprintf)() = vprintf;
static int  (*MDSvfprintf)() = vfprintf;



	/******************************************************************
	 * MDSprintf:
	 ******************************************************************/
int   MDSprintf( char *fmt , ... )
   {
    va_list  ap;

    if (!MDSvprintf)
        return(0);
    va_start(ap,fmt);		/* initialize "ap"		*/
    return((*MDSvprintf)(fmt,ap));
   }



	/******************************************************************
	 * MDSfprintf:
	 ******************************************************************/
int   MDSfprintf( FILE *fp , char *fmt , ... )
   {
    va_list  ap;

    va_start(ap,fmt);		/* initialize "ap"		*/
    if (fp!=stderr && fp!=stdout)
        return(vfprintf(fp,fmt,ap));
    if (!MDSvfprintf)
        return(0);
    return((*MDSvfprintf)(fp,fmt,ap));
   }



	/***************************************************************
	 * MdsSetOutputFunctions:
	 * MdsGetOutputFunctions:
	 ***************************************************************/
void  MdsSetOutputFunctions(
    int (*NEWvprintf)(const char *, void *)
   ,int (*NEWvfprintf)(FILE *,const char *,void *)
   )
   {
    MDSvprintf =  ((void *)NEWvprintf ==(void *)-1) ? (int (*)())vprintf : (int (*)())NEWvprintf;
    MDSvfprintf = ((void *)NEWvfprintf==(void *)-1) ? (int (*)())vfprintf : (int (*)())NEWvfprintf;
    return;
   }

void  MdsGetOutputFunctions( void **CURvprintf , void **CURvfprintf )
   {
    if (CURvprintf)  *CURvprintf = (void *)MDSvprintf;
    if (CURvfprintf)  *CURvfprintf = (void *)MDSvfprintf;
    return;
   }



#ifdef Main
	/****************************************************************
	 * main:
	 ****************************************************************/


static int   woof( char *fmt , va_list ap )
   {
    char  xxfmt[80];

    sprintf(xxfmt,"\nWOOF: %s",fmt);
    return(vprintf(xxfmt,ap));
   }


static int   tweet( FILE *fp , char *fmt , va_list ap )
   {
    char  xxfmt[80];

    sprintf(xxfmt,"\nTWEET: %s\n",fmt);
    return(vfprintf(fp,xxfmt,ap));
   }

int   main( int argc , void *argv[] )
   {
    static void  *save_vprintf;
    static void  *save_vfprintf;
    MDSprintf("woof %d %d %d\n",1,2,3);
    MDSfprintf(stderr,"tweet %d %d %d\n",1,2,3);

    MdsGetOutputFunctions(&save_vprintf,&save_vfprintf);
    MdsSetOutputFunctions(woof,tweet);
    MDSprintf("woof %d %d %d\n",1,2,3);
    MDSfprintf(stderr,"tweet %d %d %d\n",1,2,3);

    MdsSetOutputFunctions(save_vprintf,save_vfprintf);
    MDSprintf("woof %d %d %d\n",1,2,3);
    MDSfprintf(stderr,"tweet %d %d %d\n",1,2,3);

    MdsGetOutputFunctions(&save_vprintf,&save_vfprintf);
    MdsSetOutputFunctions(woof,tweet);
    MDSprintf("woof %d %d %d\n",1,2,3);
    MDSfprintf(stderr,"tweet %d %d %d\n",1,2,3);

    MdsSetOutputFunctions((void *)-1,(void *)-1);
    MDSprintf("woof %d %d %d\n",1,2,3);
    MDSfprintf(stderr,"tweet %d %d %d\n",1,2,3);
   }

#endif
