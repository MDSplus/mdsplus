#include        <errno.h>
#include        <stdarg.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        "dasutil.h"

/**********************************************************************
* dasmsg.c
*
* History:
*  10-Feb-1998  TRG  Use vsprintf.  "sun" version of sprintf used to
*                     return ptr to string.  Now it behaves correctly.
*  19-Jan-1993  TRG   Create.  Copied from VAX.
*
***********************************************************************/


#ifdef vms
extern noshare char  *sys_errlist[];
extern noshare int  sys_nerr;
#else
extern char  *sys_errlist[];
extern int  sys_nerr;
#endif


	/***************************************************************
	 * msgText:
	 * Dummy routine, simulates routine in DASUTIL lib on VAX
	 ***************************************************************/
static char  *msgText(sts)
int   sts;
   {
    static char  text[33];

    sprintf(text,"Error code %d (0x%04X)",sts,sts);
    return(text);
   }



	/****************************************************************
	 * dasmsg:
	 * Print message in standardized format, including a translation
	 *  of the 'sts' parameter via $GETMSG.
	 * The function returns 'sts' as its value.
	 ***************************************************************/
int   dasmsg(			/* Return: status from user		*/
    int   sts			/* <r> status from user			*/
   ,char  fmt[]			/* <r> format specification		*/
   , ...			/* <r> variable arg list		*/
   )
   {
    char  txt[240];
    int   i;
    va_list  ap;		/* argument ptr				*/
    static int   nerr;

    if (!nerr)
#ifdef __ERRNO_MAX
        nerr = __ERRNO_MAX;
#elif vms
       {
        nerr = &(char  **)sys_nerr - sys_errlist;	/* ..strange..	*/
        if (nerr != 36)
            printf("\ndasmsg: nerr=%d\n\n",nerr);
       }
#else
        nerr = sys_nerr;
#endif

    va_start(ap,fmt);		/* Initialize ap			*/
    i = vsprintf(txt,fmt,ap);
    if (i >= sizeof(txt))
        exit(dasmsg(2,"dasmsg(): message too long for buffer!"));

    if (sts)
        fprintf(stderr,"\r%s:  %s\n    sts=%.70s\n",pgmname(),txt,
            (sts>0 && sts<nerr) ? sys_errlist[sts] : msgText(sts));
    else
        fprintf(stderr,"\r%s:  %s\n",pgmname(),txt);
    return(sts);
   }
#include        <string.h>
#include        "dasutil.h"

/**********************************************************************
* PGMNAME.C --
*
* History:
*  02-Oct-1996  TRG  Add "include" file.
*  00-Jan-1900  TRG  Create in the dim dark past.
*
***********************************************************************/

static char  name[32] = "xxx";


char  *pgmname()
   {
    return(name);
   }


void set_pgmname(ptr)
char  *ptr;
   {
    int   i,k;
    char  *p,*p2;

#ifdef vms
    p = strrchr(ptr,']');
#else
    p = strrchr(ptr,'/');
#endif
    p = p ? (p+1) : ptr;
    if (p2 = strchr(p,'.'))
        k = p2 - p;
    else
        k = strlen(p);

    if (k >= (sizeof(name)-1))
        k = sizeof(name) - 1;
    strncpy(name,p,k);
    return;
   }
#include        <ctype.h>
#include        <stdarg.h>
#include        <stdlib.h>
#include        <stdio.h>
#include        <string.h>
#include        "dasutil.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#endif

/**********************************************************************
* STR_UTIL.C --
*
* Emulations of certain VMS "STR$" routines.
*
* History:
*  05-Nov-1997  TRG  Create.
*
***********************************************************************/


#define MAX_ARGS    31		/* Max num args for variable argList[]	*/



	/****************************************************************
	 * str_free1_dx:
	 ****************************************************************/
int   str_free1_dx(			/* Return: status		*/
    struct descriptor  *dsc		/* <m> descriptor to free	*/
   )
   {
    int   sts;
    char  *p;

    if (!is_ddescr(dsc))
       {
        fprintf(stderr,"str_free1_dx: *WARN* non-dynamic string\n");
#ifdef vms
        lib$signal(SS$_DEBUG);
#endif
        return(1);
       }

    p = dsc->dscA_pointer;
    if (p)
       {
        free(p);
        dsc->dscA_pointer = 0;
        dsc->dscW_length = 0;
       }
    return(1);
   }



	/****************************************************************
	 * str_trim:
	 ****************************************************************/
int   str_trim(				/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *optsrc			/* <r:opt> source: cstring or dsc */
   )
   {
    int   i,k;
    char  *p;
    void  *src;
    struct descriptor  *dsc;

		/*-------------------------------------------------------
		 * Prepare "src" ...
		 *------------------------------------------------------*/
    src = optsrc ? optsrc : dsc_ret;
    if (is_cdescr(src) || is_ddescr(src))
       {
        dsc = src;
        p = dsc->dscA_pointer;
        k = dsc->dscW_length;
       }
    else
       {
        p = src;
        k = p ? strlen(p) : 0;
       }

		/*--------------------------------------------------------
		 * Trim trailing blanks and nulls ...
		 *-------------------------------------------------------*/
    for ( ; k && (!p[k-1] || isspace(p[k-1])) ; k--)
        ;

    if (!optsrc)
       {
        p[k] = '\0';
        return(1);			/*--------------------> return	*/
       }

		/*-------------------------------------------------------
		 * Prepare dsc_ret to hold 'k' chars ...
		 *------------------------------------------------------*/
    if (is_ddescr(dsc_ret))
       {
        if (dsc_ret->dscW_length != k)
           {
            if (dsc_ret->dscA_pointer)
                free(dsc_ret->dscA_pointer);
            dsc_ret->dscA_pointer = malloc(k+1);
            dsc_ret->dscW_length = k;
            if (!dsc_ret->dscA_pointer)
               {
                printf("Out of space!");
#ifdef vms
                lib$signal(SS$_DEBUG);
#endif
                exit(0);
               }
           }
       }
    if (k > dsc_ret->dscW_length)
        k = dsc_ret->dscW_length;
    strncpy(dsc_ret->dscA_pointer,p,k);
    if (k < dsc_ret->dscW_length)
        dsc_ret->dscA_pointer[k] = '\0';

    return(1);
   }



	/****************************************************************
	 * str_copy_dx:
	 ****************************************************************/
int   str_copy_dx(			/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *source			/* <r> source: c-string or dsc	*/
   )
   {
    int   i,k;
    int   maxbytes;
    char  *p;
    struct descriptor  *dsc;

		/*-------------------------------------------------------
		 * Prepare "source" ...
		 *------------------------------------------------------*/
    if (is_cdescr(source) || is_ddescr(source))
       {
        dsc = source;
        p = dsc->dscA_pointer;
        k = p ? strlen(p) : 0;
        if (k > dsc->dscW_length)
            k = dsc->dscW_length;
       }
    else
       {
        p = source;
        k = p ? strlen(p) : 0;
       }

		/*-------------------------------------------------------
		 * Prepare dsc_ret ...
		 *------------------------------------------------------*/
    if (is_ddescr(dsc_ret))
       {			/* if dsc_ret is a dynamic descriptor ...*/
        if (!k)
           {
            str_free1_dx(dsc_ret);
            return(1);			/*---------------------> return	*/
           }
        if (k != dsc_ret->dscW_length)
           {
            if (dsc_ret->dscA_pointer)
                dsc_ret->dscA_pointer = realloc(dsc_ret->dscA_pointer,k+1);
            else
                dsc_ret->dscA_pointer = malloc(k+1);
            if (!dsc_ret->dscA_pointer)
               {
                printf("Out of space!");
                exit(0);
               }
            dsc_ret->dscW_length = k;
           }
        dsc_ret->dscA_pointer[k] = '\0';
       }

    if (k > dsc_ret->dscW_length)
        k = dsc_ret->dscW_length;
    if (k)
        strncpy(dsc_ret->dscA_pointer,p,k);
    if (k < dsc_ret->dscW_length)		/* i.e., a static dsc	*/
        dsc_ret->dscA_pointer[k] = '\0';

    return(1);
   }



	/****************************************************************
	 * str_replace:
	 ****************************************************************/
int   str_replace(			/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *source			/* <r> Source: c-string or dsc	*/
   ,int   offsetStart			/* <r> start pos <0-based>	*/
   ,int   offsetEnd			/* <r> end pos <0-based>	*/
   ,void  *replaceString		/* <r> replacement: c-str or dsc*/
   )
   {
    int   i,k;
    int   len,vlen;
    int   sts;
    char  *p;
    char  *v;
    char  *tmp;
    struct descriptor  *dsc;

    if (offsetStart > offsetEnd)
        return(0);		/* invalid parameters			*/
    if (!(is_ddescr(dsc_ret) || is_cdescr(dsc_ret)))
       {
        fprintf(stderr,"str_replace: *ERR* dsc_ret must be a descriptor\n");
        exit(1);
       }

    if (is_cdescr(source) || is_ddescr(source))
        p = ((struct descriptor *)source)->dscA_pointer;
    else
        p = source;

    if (is_cdescr(replaceString) || is_ddescr(replaceString))
        v = ((struct descriptor *)replaceString)->dscA_pointer;
    else
        v = replaceString;

    if (!p)  p = "";
    if (!v)  v = "";
    vlen = strlen(v);
    len = strlen(p) - (offsetEnd-offsetStart) + vlen;
    tmp = malloc(len+1);
    if (!tmp)
       {
        fprintf(stderr,"Out of space!\n\n");
        exit(1);
       }
    if (offsetStart)
        strncpy(tmp,p,offsetStart);
    strcpy(tmp+offsetStart,v);
    strcpy(tmp+offsetStart+vlen,p+offsetEnd);

    sts = str_copy_dx(dsc_ret,tmp);
    free(tmp);
    return(sts);
   }



	/****************************************************************
	 * str_prefix:
	 ****************************************************************/
int   str_prefix(
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *prefix			/* <r> Prefix: cstring or dsc	*/
   )
   {
    return(str_replace(dsc_ret,dsc_ret,0,0,prefix));
   }



	/****************************************************************
	 * str_append:
	 ****************************************************************/
int   str_append(			/* Return: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,void  *source			/* <r> source: c-string or dsc	*/
   )
   {
    int   i,k;
    int   maxbytes;
    int   len;
    int   nchar;
    char  *p;
    struct descriptor  *dsc;

		/*-------------------------------------------------------
		 * Prepare "source" ...
		 *------------------------------------------------------*/
    if (is_cdescr(source) || is_ddescr(source))
       {
        dsc = source;
        p = dsc->dscA_pointer;
        k = p ? strlen(p) : 0;
        if (k > dsc->dscW_length)
            k = dsc->dscW_length;
       }
    else
       {
        p = source;
        k = p ? strlen(p) : 0;
       }

		/*-------------------------------------------------------
		 * Prepare dsc_ret ...
		 *------------------------------------------------------*/
    len = dsc_ret->dscA_pointer ? strlen(dsc_ret->dscA_pointer) : 0;
    if (len > dsc_ret->dscW_length)
        len = dsc_ret->dscW_length;

    nchar = len+k;		/* num chars in new string		*/
    if (is_ddescr(dsc_ret))
       {			/* if dsc_ret is a dynamic descriptor ...*/
        if (dsc_ret->dscA_pointer)
            dsc_ret->dscA_pointer = realloc(dsc_ret->dscA_pointer,nchar+1);
        else
            dsc_ret->dscA_pointer = malloc(nchar+1);
        if (!dsc_ret->dscA_pointer)
           {
            printf("Out of space!");
            exit(0);
           }
        dsc_ret->dscW_length = nchar;
        dsc_ret->dscA_pointer[nchar] = '\0';
       }

    if (nchar > dsc_ret->dscW_length)
        k = dsc_ret->dscW_length - len;
    if (k > 0)
        strncpy(dsc_ret->dscA_pointer+len,p,k);
    if (nchar < dsc_ret->dscW_length)		/* i.e., a static dsc	*/
        dsc_ret->dscA_pointer[nchar] = '\0';

    return(1);
   }



	/****************************************************************
	 * str_concat:
	 ****************************************************************/
char  *str_concat(		/* Returns: ptr to null-terminated string*/
    struct descriptor  *dsc_dest 	/* <w> Destination string dsc	*/
   ,...				/* <r> source strings: dsc or c-string	*/
   )
   {
    int   i,k,kk;
    int   argc;
    int   ichar;
    int   nchar;
    char  *p;
    char  *tmp;		/* Ptr to new concatenated string	*/
    void  *v;
    struct descriptor  *dsc;		/* Utility dsc			*/
    va_list  ap;		/* Ptr to traverse variable arg list	*/
    void  *argList[MAX_ARGS];	/* Argument list			*/

		/*=======================================================
		 * First arg is destination string:  a descriptor
		 *======================================================*/
    if (!(is_ddescr(dsc_dest) || is_cdescr(dsc_dest)))
        return((char *)(dasmsg(0,"str_concat: dest must be descriptor")));

		/*=======================================================
		 * Set up argList ...
		 *======================================================*/
    va_start(ap,dsc_dest);			/* initialize ap	*/
    for (argc=0 ; argc<MAX_ARGS && (v=va_arg(ap, void *)) ; argc++)
        argList[argc] = v;			/* fill argList[]	*/
    va_end(ap);

		/*======================================================
		 * Get length of source strings ...
		 *=====================================================*/
    nchar = 0;
    for (i=0 ; i<argc ; i++)
       {
        if (is_cdescr(argList[i]) || is_ddescr(argList[i]))
           {
            dsc = argList[i];
            p = dsc->dscA_pointer;
            kk = dsc->dscW_length;
            for (k=0 ; k<kk && p[k] ; k++)
                ;		/* null may occur before "kk" chars	*/
           }
        else
           {
            p = argList[i];
            k = p ? strlen(p) : 0;
           }
        nchar += k;
       }

		/*======================================================
		 * Allocate tmp string of sufficient size;
		 * Copy substrings to new tmp string ...
		 *=====================================================*/
    tmp = malloc(nchar+1);

    ichar = 0;
    for (i=0 ; i<argc && ichar<nchar ; i++,ichar+=k)
       {
        if (is_cdescr(argList[i]) || is_ddescr(argList[i]))
           {
            dsc = argList[i];
            p = dsc->dscA_pointer;
            kk = dsc->dscW_length;
            for (k=0 ; k<kk && p[k] ; k++)
                ;		/* actual strlen may be < "kk" chars	*/
           }
        else
           {
            p = argList[i];
            k = p ? strlen(p) : 0;
           }
        strncpy(tmp+ichar,p,k);
       }
    tmp[nchar] = '\0';

		/*=======================================================
		 * Prepare dsc_dest ...
		 *======================================================*/
    if (is_ddescr(dsc_dest))
       {			/* if dsc_dest is a dynamic descriptor ...*/
        str_free1_dx(dsc_dest);

        dsc_dest->dscA_pointer = tmp;	/* valid even for zero-length str */
        dsc_dest->dscW_length = nchar;
       }
    else
       {			/* else, dsc_ret is a static descriptor	*/
        if (nchar > dsc_dest->dscW_length)
            nchar = dsc_dest->dscW_length;

        strncpy(dsc_dest->dscA_pointer,tmp,nchar);
        free(tmp);

        if (nchar < dsc_dest->dscW_length)
            dsc_dest->dscA_pointer[nchar] = '\0';
       }

    return(dsc_dest->dscA_pointer);
   }



	/****************************************************************
	 * str_element:
	 * Return "idx"th element (0-based) of input string
	 ****************************************************************/
int   str_element(			/* Returns: status		*/
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,int   ielement			/* <r> element num, 0-based	*/
   ,char  delimiter			/* <r> delimiter character	*/
   ,void  *source			/* <r> source: dsc or c-string	*/
   )
   {
    int   i,k;
    char  *p,*p2;
    char  *srcBase;
    struct descriptor  *dsc;
    static struct descriptor  dsc_substring = {
                            0,DSC_K_DTYPE_T,DSC_K_CLASS_S,0} ;

		/*======================================================
		 * Set up substring descriptor ...
		 *=====================================================*/
    if (is_cdescr(source) || is_ddescr(source))
       {
        dsc = source;
        srcBase = dsc->dscA_pointer;
        k = srcBase ? strlen(srcBase) : 0;
        if (k > dsc->dscW_length)
            k = dsc->dscW_length;
       }
    else
       {
        srcBase = source;
        k = srcBase ? strlen(srcBase) : 0;
       }
		/*-------------------------------------------------------
		 * find "i"th delimiter ...
		 *------------------------------------------------------*/
    for (i=0,p=srcBase ; i<ielement && (p=strchr(p,delimiter)) ; i++)
       {
        if ((p-srcBase) > k)
           {
            p = 0;  break;
           }
        p++;				/* Bump 'p' past delimiter char	*/
       }
    if (!p)
        return(0);			/*---------> return: not found	*/

		/*-------------------------------------------------------
		 * Find end "p2" of substring.  Make sure that end is
		 *  completely within "source" string ...
		 *------------------------------------------------------*/
    p2 = strchr(p,delimiter);
    if (!p2)
        p2 = p + strlen(p);
    if ((p2 - srcBase) > k)
        p2 = srcBase + k;

    dsc_substring.dscA_pointer = p;
    dsc_substring.dscW_length = p2 - p;
    return(str_copy_dx(dsc_ret,&dsc_substring));
   }



	/****************************************************************
	 * str_dupl_char:
	 * Duplicate character a number of times ...
	 ****************************************************************/
char  *str_dupl_char(			/* Returns: dsc_ret->dscA_pointer */
    struct descriptor  *dsc_ret		/* <w> Destination string	*/
   ,int   icnt				/* <r> duplication count	*/
   ,char  c				/* <r> character to duplicate	*/
   )
   {
    int   i,k;
    char  *p;

    if (!(is_ddescr(dsc_ret) || is_cdescr(dsc_ret)))
        return((char *)dasmsg(0,"str_dupl_char: 1st arg not a descriptor"));

    p = malloc(icnt+1);
    if (!p)
        exit(dasmsg(0,"Out of space"));

    memset(p,c,icnt);
    p[icnt] = '\0';
    str_copy_dx(dsc_ret,p);
    free(p);
    return(dsc_ret->dscA_pointer);
   }
