#include        <ctype.h>
#include        <stdio.h>
#include        <string.h>
#include        "dasutil.h"

/**********************************************************************
*
* CMD_LOOKUP.C --
*
* Function which looks up a "command" string in a user-provided list.
* The return value of the function is the value indicated in the
*  structure.
* This function supports both abbreviated commands and "wildcard"
*  commands, with the following restrictions:
*       Abbreviations
*             - Do not contain wild-card characters ('*' and '%')
*             - Match must be unique
*       Wild-cards
*             - "Context" must be provided
*             - Abbreviations are not handled -- user can provide
*                a trailing '*' or '%' if desired.
*       "Context"
*             - Command line ptr will not be bumped if context is
*               provided.
*
* Modifications:
*  18-Oct-1996  TRG  Use COLWIDTH when displaying available commands.
*  10-Dec-1987  TRG  Set "noerrmsg" flag during successful "context" srch.
*  23-Jun-1987  TRG  Add 'context' and 'wildcard' stuff.
*
**********************************************************************/


#define COLWIDTH     12		/*"Available" list: width of columns	*/
#define FIRST_COLUMN  6		/*..position of first column		*/
#define LINEWIDTH    70		/*..width of display line		*/


/*struct cmd_struct {int   cmdL_id;
/*                  char  *cmdA_string;
/*                 };					/*  */


/*#define NOMSG   1		/* "flags" mask, for no errmsg		*/
/*#define NOWILD 2		/* "flags" mask, ignores wildcards	*/


static int strncmp_nocase();



	/****************************************************************
	 * cmd_lookup:  If command is valid, return the command id#.
	 *    Otherwise, return 0 and, unless otherwise indicated,
	 *     complain about it.
	 ****************************************************************/

int cmd_lookup(
	char  **s,		/* <m> Ptr within command string	*/
	struct cmd_struct  cmdlist[], /* <r> Command-list struct	*/
	char  parentString[],	/* <r:opt> for use with error msg	*/
	long  flags,		/* <r:opt> 1=NoErrMsg			*/
	long  *ucontext		/* <m:opt> Allows continuation of "lookup"*/
	)
   {
    int   i,j,k,isave,icount,ilast;
    int   ispace;
    int   len;
    int   noerrmsg;
    int   nowildcard;
    char  *wc;			/* Flag: set if wildcards are present	*/
    char  *cmd;
    long  *context;		/* Local copy of the optional "ucontext" */
    int   first;		/* first idx, adjusted by "context"	*/

    if (!s)
        return(0);

		/*========================================================
		 * Set flags, "first", "last", and such --
		 *  "noerrmsg" suppresses certain error messages.
		 *=======================================================*/
    noerrmsg = flags & NOMSG;
    nowildcard = flags & NOWILD;	/* "context" only */
    context = ucontext;
    if (context && *context)
        noerrmsg = 1;
    first = 1 + (context ? *context : 0);
    ilast = (long)cmdlist->cmdA_string;	/* # of entries in list		*/
    isave = 0;

		/*======================================================
		 * Check cmd string:  if it's null then "no match"
		 * "wc" indicates whether or not wildcards are present.
		 *=====================================================*/
    cmd = *s = nonblank(*s);	/* Addr of command string		*/
    k = tknlen(cmd,0);		/* Length of cmd string (abbreviated?)	*/

    if (!k || *cmd==';')	/* Zero-length command?  Comment?	*/
       {
        *s = 0;			/* - indicated end-of-cmdline		*/
        return(0);
       }
    if (nowildcard)
        wc = 0;
    else if (wc = strpbrk(cmd,"*%"))
        wc = (char *)((cmd+k) > wc);	/* Wildcard char within cmd ?	*/

		/*========================================================
		 * If not a wildcard search, but *context is nonzero, then
		 *  we assume that a match has already been returned, so
		 *  we want to return "no match" this time.
		 * If wildcard, then context must have been provided.
		 *=======================================================*/
    if (!wc && context && *context)
        first = ilast;
    else if (wc && !context)
       {
        printf("  cmd_lookup:  Illegal wildcard specification\n");
        return(0);
       }

		/*======================================================
		 * Look for matches in user-provided cmd struct.
		 *=====================================================*/
    for (i=first,icount=0 ; i<ilast ; i++)
       {
        char  *cmdA_string = (cmdlist+i)->cmdA_string;

        if (!cmdA_string)
            continue;

        if (wc)
           {
            if (wildcard_match(cmd,cmdA_string,0,0,0))
               {
                icount = 1;
                isave = i;
                break;
               }
           }
        else if (!strncmp_nocase(cmd,cmdA_string,k))
           {
			/*------------------------------------------------
			 * Exact match ?
			 *-----------------------------------------------*/
            if (k == strlen(cmdA_string))
               {
                icount = 1;
                isave = i;
                break;
               }
			/*------------------------------------------------
			 * If this is just an alias for another command,
			 *  ignore it.
			 *-----------------------------------------------*/
            if (icount && (cmdlist+i)->cmdL_id==(cmdlist+isave)->cmdL_id)
                continue;
			/*-----------------------------------------------
			 * Find all commands which match user's input
			 *----------------------------------------------*/
            if (icount++)
               {			/* More than 1 match ?		*/
                if (icount == 2)	/* -- 1st time ...		*/
                    printf("Be more specific :\n\t%s\n",
                        (cmdlist+isave)->cmdA_string);
                printf("\t%s\n",(cmdlist+i)->cmdA_string);
                isave = 0;		/* "No match" return		*/
               }
            else
               isave = i;		/* Save "match" idx		*/
           }
       }
    if (icount == 0)
       {
        k = 0;			/* Set "token length" to 0	*/
        if (!noerrmsg)
           {
            if (parentString)
                printf("Valid '%s' subcommands are :\n",parentString);
            else
                printf("Valid commands are:\n");
            for (i=1,j=0 ; i<ilast ; i++)
               {			/* Print out commands		*/
                if (!(cmdlist+i)->cmdA_string)
                    continue;
                ispace = j ? (COLWIDTH - (j-FIRST_COLUMN)%COLWIDTH) :
                            FIRST_COLUMN;
                len = strlen((cmdlist+i)->cmdA_string);
                if (j+len+ispace > LINEWIDTH)
                   {
                    printf("\n");
                    j = 0;
                    ispace = FIRST_COLUMN;
                   }
			/*-----------------------------------------------
			 * Space over to next column ...
			 *----------------------------------------------*/
                for ( ; ispace-- ; j++)
                    putc(' ',stdout);

                printf("%s",(cmdlist+i)->cmdA_string);
                j += len;
               }
            if (j)
                printf("\n");
           }
        }
		/*=======================================================
		 * If "context" was provided, update it.
		 * Otherwise, bump "command-line" ptr beyond the command
		 *  just processed.
		 *======================================================*/
    if (context)
        *context = isave;
    else
        *s = nonblank(*s + k);	/* Bump "command-line" ptr	*/;
    return((cmdlist+isave)->cmdL_id);
   }



	/**************************************************************
	 * strncmp_nocase():
	 * Like strncmp(), but is not case sensitive.
	 **************************************************************/
static int strncmp_nocase(s,t,n)
char  *s,*t;
   {
    register int   i;

    for ( ; n ; n--,s++,t++)
        if (i = toupper(*s) - toupper(*t))  break;
    return(i);
   }
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
    char  *getmsg();
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
            (sts>0 && sts<nerr) ? sys_errlist[sts] : getmsg(sts));
    else
        fprintf(stderr,"\r%s:  %s\n",pgmname(),txt);
    return(sts);
   }



	/***************************************************************
	 * getmsg:
	 * Dummy routine, simulates routine in DASUTIL lib on VAX
	 ***************************************************************/
char  *getmsg(sts)
int   sts;
   {
    static char  text[33];

    sprintf(text,"Error code %d (0x%04X)",sts,sts);
    return(text);
   }
#include        <stdio.h>
#include        "dasutil.h"

/*************************************************************************
* init_timer.c
*
* Routines to show resource usage:  init_timer, show_timer.
*
* History:
*  10-Mar-1993  TRG   Create.
*
**************************************************************************/


#ifdef vms
#include        <lib$routines.h>
#else
#include        <sys/time.h>
#include        <sys/resource.h>
#include        <errno.h>

static struct rusage  base;
static struct rusage  rnow;
static struct timeval  startTime;
static struct timeval  nowTime;
#endif



	/*****************************************************************
	 * init_timer:
	 *****************************************************************/
int   init_timer()
   {
#ifdef vms
    lib$init_timer();
#else
    gettimeofday(&startTime,0);
    if (getrusage(RUSAGE_SELF,&base))
        dasmsg(errno,"init_timer:  *ERR* from getrusage()");
#endif
    return(0);
   }



	/****************************************************************
	 * show_timer:
	 ****************************************************************/
int   show_timer()
   {
#ifdef vms
    lib$show_timer();
    printf("\n");
#else
    long emsec,umsec,smsec;		/* milliseconds			*/

    gettimeofday(&nowTime,0);
    if (getrusage(RUSAGE_SELF,&rnow))
        return(dasmsg(errno,"show_timer:  *ERR* from getrusage()"));

    if (nowTime.tv_usec < startTime.tv_usec)
        --nowTime.tv_sec, nowTime.tv_usec+=1000000;
    if (rnow.ru_utime.tv_usec < base.ru_utime.tv_usec)
        --rnow.ru_utime.tv_sec, rnow.ru_utime.tv_usec+=1000000;
    if (rnow.ru_stime.tv_usec < base.ru_stime.tv_usec)
        --rnow.ru_stime.tv_sec, rnow.ru_stime.tv_usec+=1000000;
    emsec = (nowTime.tv_usec-startTime.tv_usec) / 10000;
    umsec = (rnow.ru_utime.tv_usec-base.ru_utime.tv_usec) / 10000;
    smsec = (rnow.ru_stime.tv_usec-base.ru_stime.tv_usec) / 10000;
    fprintf(stderr,"elpased=%d.%02d user=%d.%02d sys=%d.%02d sf=%d hf=%d\n",
        nowTime.tv_sec-startTime.tv_sec,emsec,
        rnow.ru_utime.tv_sec-base.ru_utime.tv_sec,umsec,
        rnow.ru_stime.tv_sec-base.ru_stime.tv_sec,smsec,
        rnow.ru_minflt-base.ru_minflt,rnow.ru_majflt-base.ru_majflt);
#endif
    return(0);
   }
#include        <ctype.h>
#include        "dasutil.h"

/***********************************************************************
* l2u.c
*
* Character string conversion and whatnot.
*
* History:
*  19-Jan-1993  TRG   Create.  Copied from VAX.
*
************************************************************************/



	/****************************************************************
	 * l2u:
	 * Convert ASCII string to all upper case
	 ****************************************************************/
char  *l2u(
    char  *s		/* <m> string to modify				*/
   ,char  *t		/* <r:opt> source string			*/
   )
   {
    char  *in;
    char  *out;

    out = s;
    in = t ? t : s;		/* Default is to use "s" as in and out	*/

    for ( ; *out++=toupper(*in++) ; )
        ;
    return(s);
   }



	/****************************************************************
	 * l2un:
	 * Like l2u(), but copies specified number of characters.
	 ****************************************************************/
char  *l2un(
    char  to[],		/* <w> destination string			*/
    char  from[],	/* <r> source string				*/
    int   n		/* <r> number of characters in from[]		*/
   )
   {
    char  *out;

    for (out=to ; n-- && (*out++=toupper(*from++)) ; )
        ;
    return(to);
   }



	/****************************************************************
	 * u2l:
	 * Convert ASCII string to all lower case
	 ****************************************************************/
char  *u2l(
    char  *s		/* <m> string to modify				*/
   ,char  *t		/* <r:opt> source string			*/
   )
   {
    char  *in;
    char  *out;

    out = s;
    in = t ? t : s;		/* Default is to use "s" as in and out	*/

    for ( ; *out++=tolower(*in++) ; )
        ;
    return(s);
   }



	/****************************************************************
	 * u2ln:
	 * Like u2l(), but copies specified number of characters.
	 ****************************************************************/
char  *u2ln(
    char  to[],		/* <w> destination string			*/
    char  from[],	/* <r> source string				*/
    int   n		/* <r> number of characters in from[]		*/
   )
   {
    char  *out;

    for (out=to ; n-- && (*out++=tolower(*from++)) ; )
        ;
    return(to);
   }



	/*****************************************************************
	 * bcd2i:  Convert BCD (Binary Coded Decimal) value to int.
	 * i2bcd:  Convert int to BCD.
	 *****************************************************************/
long  bcd2i(
    unsigned long  bcd	/* <r> bcd val, 4 bits per digit, max 8 digits	*/
   )
   {
    int   i,ival;

    if (bcd)
        ival = bcd2i(bcd>>4)*10 + (bcd & 0x0F);
    else
        ival = 0;
    return(ival);
   }


long  i2bcd(		/* bcd val, 4 bits per digit, max 8 digits	*/
    unsigned long  ival	/* <r> integer value to convert			*/
   )
   {
    unsigned int   bcd;

    if (!ival)
        return(0);
    if (ival > 99999999)
        return(dasmsg(0,"i2bcd: *WARN* ival too large (%d)",ival));

    bcd = (ival%10) | (i2bcd(ival/10) << 4);
    return(bcd);
   }
#include        <ctype.h>
#include        <string.h>
#include        "dasutil.h"

/***********************************************************************
* nonblank.c --
*
* Various utility routines for working with character strings.
*
* History:
*  19-Jan-1993  TRG   Create.  Subset of nonblank.c from VAX.
*
************************************************************************/



	/*****************************************************************
	 * nonblank:
	 * Return addr of first nonblank char in string
	 *****************************************************************/
char  *nonblank(
    char  *p	/* <r> Addr of start of string			*/
   )
   {
    if (!p)
        return(0);
    for ( ; *p && isspace(*p) ; p++)
        ;
    return(*p ? p : 0);
   }




	/*****************************************************************
	 * blank:
	 * Return addr of first blank (white space) char.
	 *****************************************************************/
char  *blank(
    char  *p	/* <r> Addr of start of string			*/
   )
   {
    if (!p)
        return(0);
    for ( ; *p && !isspace(*p) ; p++)
        ;
    return(*p ? p : 0);
   }



	/****************************************************************
	 * remove_commas:
	 ****************************************************************/
void  remove_commas(
    char  str[]		/* <m> string to operate upon		*/
   )
   {
    char  *p;

    if (!str)
        return;

    for (p=str ; *p ; p++)
       {
        if (*p == ',')
            *p = ' ';
       }
    return;
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
        fprintf(stderr,"str_free_dx: *WARN* non-dynamic string\n");
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
        k = strlen(src);
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
        k = strlen(source);
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
        k = strlen(source);
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
            k = strlen(p);
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
            k = strlen(p);
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
        k = strlen(source);
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
#include        "dasutil.h"
#include        <ctype.h>
#include        <string.h>

/***********************************************************************
* STRTRIM.C --
*
* Trim white-space off end of string ...
*
* History:
*  05-Nov-1992  TRG   Create.
*
************************************************************************/



	/****************************************************************
	 * strtrim:
	 * Trim trailing whitespace, return length of trimmed string.
	 ****************************************************************/
int   strtrim(
    char  to[]		/* <w/m> Destination string			*/
   ,char  from[]	/* <r:opt> Source string			*/
   )
   {
    char  *s,*t;
    int   i,k;
    t = to;
    s = from ? from : to;

    k = strlen(s);
    for ( ; k && (isspace(s[k-1]) || !s[k-1])  ; k--)
        ;

    if (k && s!=t)
        strncpy(t,s,k);

    t[k] = '\0';
    return(k);
   }



	/*****************************************************************
	 * strntrim:
	 * Trim trailing whitespace, return length of trimmed string.
	 *****************************************************************/
int   strntrim(
    char  to[]		/* <w/m> Destination string			*/
   ,char  from[]	/* <r:opt> Source string			*/
   ,int   fromLen	/* <r> Length of source string			*/
   )
   {
    char  *s,*t;
    int   i,k;

    t = to;
    s = from ? from : to;
    k = fromLen;

    for ( ; k && (isspace(s[k-1]) || !s[k-1]) ; k--)
        ;

    if (k && s!=t)
        strncpy(t,s,k);

    t[k] = '\0';
    return(k);
   }
#include        <ctype.h>
#include        <stdio.h>
#include        <string.h>
#include        "dasutil.h"

/***********************************************************************
* tokens.c
*
* Utility functions for working with "tokens" from char strings.
*
* History:
*  29-Oct-1997  TRG  deltatimeToken: new function.
*  19-Jan-1993  TRG   Create.  Copied from nonblank.c on VAX.
*
************************************************************************/


#define isWild(C)          (strchr(wc,C))
#define isAcceptable(C)    (strchr(wc,C) || strchr(acceptAlph,C))


	/****************************************************************
	 * tknlen:
	 * Return length of token starting at 's'.
	 * Token is terminated by white-space, or any of (/,=,-,+)
	 ****************************************************************/
int   tknlen(
    char  *s,			/* <r> addr of start of token		*/
    char  usrTerminators[]	/* <r:opt> string of "terminator" chars	*/
   )
   {
    char  *t;
    char  *terminators;
    static char  defaultTerminators[] = "/=-+";

    terminators = usrTerminators ? usrTerminators : defaultTerminators;
    if (!s || !*s)
        return(0);
    for (t=s+1 ; *t ; t++)
        if (isspace(*t) || strchr(terminators,*t))
            break;
    return(t-s);
   }



	/*****************************************************************
	 * ascToken:
	 *****************************************************************/
int   ascToken(
    char  **s		/* <m> Addr of ptr to input string		*/
   ,struct descriptor  *dsc_token	/* <w> return token here	*/
   ,long  *utknLen	/* <w:opt> token length				*/
   ,char  optAlph[]	/* <r:opt> User-defined alph extensions		*/
   )
   {
    int   sts;
    long  tknLen;
    char  *p;
    static char  defaultAlph[] = "0123456789_-$";
    char  *alphExtensions;

    alphExtensions = optAlph ? optAlph : defaultAlph;

    if (!s || !(p=nonblank(*s)))
       {
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
    sts = nextToken(&p,dsc_token,&tknLen,0,alphExtensions);
    if (sts & 1)
        *s = p;			/* Return new ptr value to caller	*/

    if (utknLen)
        *utknLen = tknLen;

    return(sts);
   }



	/****************************************************************
	 * nextToken:
	 ****************************************************************/
int   nextToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    char  wildcards[],		/* <r:opt> str of acceptable wildcard chrs*/
    char  alphExtensions[]	/* <r:opt> extensions to default alphnum */
   )
   {
    char  *p,*p2;
    int   n;
    char  tmp[1024];
    unsigned int   maxLen;
    unsigned long  tknLen;
    char  quoteMark;
    char  *wc;
    char  *acceptAlph;
    static char  defaultExtensions[] = "$_";

    maxLen = sizeof(tmp) - 1;
    p = *s;
    p = nonblank(p);
    if (!p)
       {
        *s = 0;
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
		/*=======================================================
		 * Set "wildcard" string ptr
		 *======================================================*/
    wc = wildcards ? wildcards : "";
    acceptAlph = alphExtensions ? alphExtensions : defaultExtensions;

		/*=======================================================
		 * Extract token, depending upon type of 1st char...
		 *======================================================*/
    if (isalpha(*p) || isAcceptable(*p))
       {
        for (p2=p+1 ; *p2 && (isalnum(*p2) || isAcceptable(*p2)) ; p2++)
            ;
        tknLen = p2 - p;
        if (tknLen > maxLen)
           {
            dasmsg(0,"nextToken: *WARN* tkn truncated from %d to %d chars",
                tknLen,maxLen);
            tknLen = maxLen;
           }
        strncpy(tmp,p,tknLen);
       }
     else if (isdigit(*p))
       {
        for (p2=p+1 ; *p2 && (isdigit(*p2) || isWild(*p2)) ; p2++)
            ;
        tknLen = p2 - p;
        if (tknLen > maxLen)
           {
            dasmsg(0,"nextToken: *WARN* tkn truncated from %d to %d chars",
                tknLen,maxLen);
            tknLen = maxLen;
           }
        strncpy(tmp,p,tknLen);
       }
    else if (*p=='"' || *p=='\'')	/* double or single quotation-mark*/
       {
        quoteMark = *p;

		/*--------------------------------------------------------
		 * Some fancy footwork to extract text string ...
		 *-------------------------------------------------------*/
        tknLen = 0;
        for ( ; *p==quoteMark ; p=p2)
           {
            p++;		/* Bump 'p' inside of quotes		*/
            p2 = strchr(p,quoteMark);
            if (p2)
               {		/* Found a matching quoteMark...	*/
                n = p2 - p;		/* string length, could be zero	*/
                if (*(p2+1) == quoteMark)
                    n++;		/*..it's a double quoteMark	*/
               }
            else
               {		/*..no balancing quoteMark		*/
                n = strlen(p);		/* Go to end of string		*/
                p2 = p + n;
               }
			/*-------------------------------------------------
			 * Copy string (or portion of string) to tmp[].
			 * Bump p2 past its quoteMark.
			 *------------------------------------------------*/
            if (tknLen+n > maxLen)
               {
                if (tknLen < maxLen)
                    dasmsg(0,"nextToken: *WARN* truncating to %d chars",
                        maxLen);
                n = maxLen - tknLen;
               }
            if (n)
               {
                strncpy(tmp+tknLen,p,n);
                tknLen += n;
               }
            if (*p2)
                p2++;
           }
       }
    else
       {
        tknLen = 1;
        tmp[0] = *p;
        p2 = p+1;
       }

    tmp[tknLen] = '\0';
    *s = *p2 ? p2 : 0;
    str_copy_dx(dsc_token,tmp);
    if (utknLen)
        *utknLen = tknLen;
    return(1);
   }



	/****************************************************************
	 * longToken:
	 ****************************************************************/
int   longToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    long  *uval			/* <w:opt> return long value here	*/
   )
   {
    char  *p,*p2;
    int   n;
    char  *token;
    long  tknLen;
    char  plusSign;

    token = dsc_token->dscA_pointer;
    p = *s;
    p = nonblank(p);
    if (!p)
       {
        *s = 0;
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
		/*=======================================================
		 * Extract integer token.
		 *======================================================*/
    plusSign = 0;
    tknLen = 0;
    if (*p=='-' || *p=='+')
       {
        tknLen = 1;
        token[0] = *p;
        plusSign = (*p == '+');
        p++;
       }
    if (!isdigit(*p))
       {
        if (utknLen)
            *utknLen = 0;
        if (uval)
            *uval = 0;
        return(0);
       }
    for (p2=p+1 ; *p2 && isdigit(*p2) ; p2++)
        ;
    n = p2 - p;
    strncpy(token+tknLen,p,n);
    tknLen += n;

    token[tknLen] = '\0';
    *s = *p2 ? p2 : 0;

    if (utknLen)
        *utknLen = tknLen;

    if (uval)
        sscanf(token+plusSign,"%d",uval);

    return(1);
   }



	/***************************************************************
	 * doubleToken:
	 * Extract double (dbl precision floating pt) value from line.
	 ***************************************************************/
int   doubleToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    double *uval		/* <w:opt> return double value here	*/
   )
   {
    char  *p,*p2;
    int   n;
    char  *token;
    long  tknLen;
    char  utilString[80];
    static struct descriptor  dsc_utilString = {
            sizeof(utilString)-1,DSC_K_DTYPE_T,DSC_K_CLASS_S,0 };

    dsc_utilString.dscA_pointer = utilString;
    token = dsc_token->dscA_pointer;
    p = *s;
    p = nonblank(p);
    if (!p)
       {
        *s = 0;
        return(0);
       }
		/*======================================================
		 * Extract ascii int text:  left side of decimal pt
		 *=====================================================*/
    if (*p == '.')
       {
        utilString[0] = '0';		/* place leading zero		*/
        tknLen = 1;
       }
    else if (!longToken(&p,&dsc_utilString,&tknLen,NIL))
        return(0);

		/*------------------------------------------------------
		 *..Now get decimal pt and rest of number (ascii)
		 *-----------------------------------------------------*/
    if (p && *p=='.')
       {		/*..Decimal pt is present ...			*/
        utilString[tknLen++] = '.';
        for (p++ ; isdigit(*p) ; p++)
           {
            if (tknLen < sizeof(utilString)-1)
                utilString[tknLen++] = *p;
           }
       }
    utilString[tknLen] = '\0';

		/*=======================================================
		 * Return info to user
		 *======================================================*/
    strncpy(token,utilString,tknLen);

    if (utknLen)
        *utknLen = tknLen;

    if (uval)
        sscanf(utilString,"%lf",uval);

    *s = p;
    return(1);
   }



	/***************************************************************
	 * main:
	 ***************************************************************/
/*main()
/*   {
/*    static char  token[128];
/*    static char  line[128];
/*    static DESCRIPTOR(dsc_line,line);
/*    static DESCRIPTOR(dsc_token,token);
/*    static int   flag;			/* for lib$get_foreign		*/
/*    long  tknLen;
/*    int   sts;
/*    char  *p;
/*    char  *wc;
/*    int   val;
/*    static char wildcards[] = "*%#";
/*
/*    printf("Wildcards?  <N>  ");
/*    wc = yesno(0) ? wildcards : 0;
/*
/*    for ( ; ; )
/*       {
/*        printf("\nEnter >  ");
/*        if (!fgets(line,sizeof(line),stdin))
/*           {
/*            printf(" Quitting?  ");
/*            if (yesno(0))
/*                exit();
/*           }
/*        for (p=line ; ; )
/*           {
/*            if (longToken(&p,&dsc_token,&tknLen,&val))
/*                printf("   length=%-3d '%s'  integer=%d\n",tknLen,token,val);
/*            else if (nextToken(&p,&dsc_token,&tknLen,wc,NIL))
/*                printf("   length=%-3d '%s'\n",tknLen,token);
/*            else
/*                break;			/* out of loop			*/
/*           }
/*       }
/*   }						/*  */



	/***************************************************************
	 * ascFilename:
	 ***************************************************************/
int   ascFilename(
    char  **s			/* <m> addr from which to start search	*/
   ,struct descriptor  *dsc		/* <w> return token here	*/
   ,long  *utknLen		/* <w:opt> length of token		*/
   )
   {
#ifdef vms
    static char  legitimateChars[] = "$[]_-.:;0123456789";
#else
    static char  legitimateChars[] = "./_-0123456789";
#endif

    return(ascToken(s,dsc,utknLen,legitimateChars));
   }



	/****************************************************************
	 * deltatimeToken:
	 ****************************************************************/
int   deltatimeToken(		/* Returns: status			*/
    char  **pp			/* <m> addr of ptr to char string	*/
   ,struct descriptor *dsc	/* <w> return token here		*/
   ,long  *utknlen		/* <w:opt> token length <longword>	*/
   ,long  *uval			/* <w:opt> return deltatime <sec> here	*/
   )
   {
    int   i,k;
    long  kk;
    int   day,hr,min,sec,hund;
    int   sts;
    char  *p;
    char  hundString[8];
    char  utilString[80];
    static struct descriptor  dsc_utilString = {
            sizeof(utilString)-1,DSC_K_DTYPE_T,DSC_K_CLASS_S,0 };
    static char  token[20];
    static DESCRIPTOR(dsc_token,token);

    dsc_utilString.dscA_pointer = utilString;
    sts = ascToken(pp,&dsc_utilString,0,"0123456789-:.");
    if (!sts)
        return(0);
    p = nonblank(utilString);
    if (!p)
        return(0);			/*----------------> return: err	*/

			/*------------------- day...--------------------*/
    if (isdigit(*p))
       {		/* string starts with decimal digit ...		*/
        sts = longToken(&p,&dsc_token,0,&kk);
        if (!p || *p!='-')
            day = 0;
        else
           {
            day = kk;
            p++;
            sts = longToken(&p,&dsc_token,0,&kk);
           }
       }
    else
       {		/* string starts with non-digit ...		*/
        day = 0;
        if (*p == '-')
            p++;
        if (!(isdigit(*p) || *p==':'))
            return(0);			/*---------------> return: err	*/
        sts = longToken(&p,&dsc_token,0,&kk);
       }
			/*-------------------- hr...--------------------*/
    hr = sts ? kk : 0;
    if (p && *p==':')  p++;

			/*------------------- min...--------------------*/
    sts = longToken(&p,&dsc_token,0,&kk);
    min = sts ? kk : 0;
    if (p && *p==':')  p++;

			/*------------------- sec...--------------------*/
    sts = longToken(&p,&dsc_token,0,&kk);
    sec = sts ? kk : 0;
    if (p && *p=='.')  p++;

			/*------------------ hund...--------------------*/
    sts = longToken(&p,&dsc_token,0,&kk);
    hund = sts ? kk : 0;
    if (sts)
        sprintf(hundString,".%.3s",token);
    else
        hundString[0] = '\0';

			/*------------------------------------------------
			 * Check for reasonable values ...
			 *-----------------------------------------------*/
    if (day<0 || hr<0 || min<0 || sec<0 || hund<0)
        return(0);			/*----------------> return: err	*/
    if (day>9999 || hr>23 || min>59 || sec>59)
        return(0);			/*----------------> return: err	*/
    if (p)
       return(0);			/*----------------> return: err	*/

    sprintf(token,"%d-%d:%d:%d%s",day,hr,min,sec,hundString);
    k = strlen(token);
    if (k > dsc->dscW_length)
        k = dsc->dscW_length;
    strncpy(dsc->dscA_pointer,token,k);
    if (k < dsc->dscW_length)
        dsc->dscA_pointer[k] = '\0';
    if (utknlen)
        *utknlen = k;
    if (uval)
        printf("\ndeltatimeToken: *SORRY* can't return value yet\n\n");

    return(1);
   }



	/****************************************************************
	 * equalsAscToken:
	 ****************************************************************/
int   equalsAscToken(
    char  **s		/* <m> addr from which to start search		*/
   ,struct descriptor  *dsc		/* <w> return token here	*/
   ,long  *utknLen	/* <w:opt> length of token (2-byte word)	*/
   ,char  optAlph[]	/* <r:opt> User-defined alph extensions		*/
   )
   {
    int   sts;
    long  tknLen;
    char  *p;

    p = nonblank(*s);
    if (!p || *p != '=')
       {
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
    p++;			/* Bump p past '='			*/
    sts = ascToken(&p,dsc,&tknLen,optAlph);
    if (sts & 1)
        *s = p;			/* Return new ptr value to caller	*/

    if (utknLen)
        *utknLen = tknLen;

    return(sts);
   }



	/****************************************************************
	 * equalsIntToken:
	 ****************************************************************/
int   equalsLongToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    long  *uval			/* <w:opt> return long value here	*/
   )
   {
    int   sts;
    long  val;
    long  tknLen;
    char  *p;

    p = nonblank(*s);
    if (!p || *p != '=')
       {
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
    p++;			/* Bump p past '='			*/
    sts = longToken(&p,dsc_token,&tknLen,&val);
    if (sts & 1)
       {
        *s = p;			/* Return new ptr value to caller	*/
        if (uval)
            *uval = val;	/* Return value to caller		*/
       }
    if (utknLen)
        *utknLen = tknLen;

    return(sts);
   }



	/****************************************************************
	 * equalsDoubleToken:
	 ****************************************************************/
int   equalsDoubleToken(
    char  **s,			/* <m> addr of ptr to char string	*/
    struct descriptor  *dsc_token,	/* <w> return token here	*/
    long  *utknLen,		/* <w:opt> token length <longword>	*/
    double *uval		/* <w:opt> return double value here	*/
   )
   {
    int   sts;
    double val;
    long  tknLen;
    char  *p;

    p = nonblank(*s);
    if (!p || *p != '=')
       {
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
    p++;			/* Bump p past '='			*/
    sts = doubleToken(&p,dsc_token,&tknLen,&val);
    if (sts & 1)
       {
        *s = p;			/* Return new ptr value to caller	*/
        if (uval)
            *uval = val;	/* Return value to caller		*/
       }
    if (utknLen)
        *utknLen = tknLen;

    return(sts);
   }



	/****************************************************************
	 * equalsAscFilename:
	 ****************************************************************/
int   equalsAscFilename(
    char  **s		/* <m> addr from which to start search		*/
   ,struct descriptor  *dsc		/* <w> return token here	*/
   ,long  *utknLen	/* <w:opt> length of token			*/
   )
   {
    int   sts;
    char  *p;

    p = nonblank(*s);
    if (!p || *p != '=')
       {
        if (utknLen)
            *utknLen = 0;
        return(0);
       }
    p++;			/* Bump p past '='			*/
    sts = ascFilename(&p,dsc,utknLen);
    if (sts & 1)
        *s = p;			/* Return new ptr value to caller	*/

    return(sts);
   }



	/***************************************************************
	 * main:
	 ***************************************************************/
/*#include        rmsdef
/*#include        ssdef
/*
/*main()
/*   {
/*    static char  token[128];
/*    static char  line[128];
/*    static $DESCRIPTOR(dsc_line,line);
/*    short  lineLength;
/*    static $DESCRIPTOR(dsc_token,token);
/*    static $DESCRIPTOR(dsc_prompt,"Enter line >  ");
/*    static int   flag;			/* for lib$get_foreign		*/
/*    short tknLen;
/*    int   sts;
/*    char  *p;
/*    char  *wc;
/*    int   val;
/*    static char wildcards[] = "*%#";
/*
/*    printf("Wildcards?  <N>  ");
/*    wc = yesno() ? wildcards : 0;
/*
/*    for ( ; ; )
/*       {
/*        sts = lib$get_foreign(&dsc_line,&dsc_prompt,&lineLength,&flag);
/*        if (sts != SS$_NORMAL)
/*           {
/*            if (sts != RMS$_EOF)
/*                dasmsg(sts,"Error from lib$get_foreign");
/*            exit(sts);
/*           }
/*        line[lineLength] = '\0';
/*        for (p=line ; ; )
/*           {
/*            if (intToken(&p,&dsc_token,&tknLen,&val))
/*                printf("   length=%-3d '%s'  integer=%d\n",tknLen,token,val);
/*            else if (nextToken(&p,&dsc_token,&tknLen,wc))
/*                printf("   length=%-3d '%s'\n",tknLen,token);
/*            else
/*                break;			/* out of loop			*/
/*           }
/*       }
/*   }						/*  */
#include	<ctype.h>
#include	<string.h>
#include        "dasutil.h"
#ifdef vms
#include        <stdlib.h>
#else
#include	<malloc.h>
#endif

/********************************************************************
*  WILDCARD_MATCH.C --
*
*  Function to compare an "input" string with a "comparator" string.
*  The input string may contain wildcard chars.  Comparison is not
*  case-sensitive, except for special-case "wc_match".
*
*  C usage:
*     char  *input,*compare_to;
*     char  wild_card='*', any_char='%', any_dig='#';
*               :
*     if (wildcard_match(input,compare_to [,wild_card, any_char, any_dig]))
*         do_something();
*
*  Description:
*  The INPUT string is compared to COMPARE_TO string.
*  The comparison is not case-sensitive.
*  Wild cards and "any char" characters are recognized in 'INPUT',
*   and are handled in any combinations.
*  The INPUT and COMPARE_TO arguments can be either the address
*   of character-string descriptors (for FORTRAN), or the address
*   of a null-terminated character string (for C).
*  The optional WILD_CARD, ANY_CHAR, and ANY_NUM characters must be passed
*   by value.  Passing a null effectively disables these features.
*
*
* History:
*  11-Feb-1993  TRG   Copied/converted from VAX.
*  24-Oct-1989  TRG   Add "any-digit" feature.
*  18-Mar-1987  TRG   Create.
*
*********************************************************************/

static char  pattern_wc = '*';		/* Matches any string		*/
static char  pattern_any = '%';		/* Matches any single char	*/
static char pattern_dig = '#';		/* Matches any single digit	*/
static char  wild_chars[4];

static strcmp_uc();
static strncmp_uc();



	/*****************************************************************
	 * wildcard_match:
	 *****************************************************************/
int   wildcard_match(
    char  *s,		/* <r> input pattern, may contain wildcards	*/
    char  *t,		/* <r> comparison string			*/
    char  wc,		/* <r:opt> wildcard char			*/
    char  any,		/* <r:opt> "any single char" character		*/
    char  anydig	/* <r:opt> "any single numerical digit" char	*/
   )
   {
    int   i;

		/*======================================================
		 * Set "wild-card" and "any-char" characters.
		 *  "wild_chars[]" is a string containing the characters.
		 *  It is used by the C-language strchr() function.
		 *=====================================================*/
    pattern_wc = wc ? wc : '*';			/* Set 'wild card' char	*/
    pattern_any = any ? any : '%';		/* 'Any-char' char	*/
    pattern_dig = anydig ? anydig : '#';	/* 'Any-digit' char	*/
    i = 0;
    if (isgraph(pattern_wc))
        wild_chars[i++] = pattern_wc;
    if (isgraph(pattern_any))
        wild_chars[i++] = pattern_any;
    if (isgraph(pattern_dig))
        wild_chars[i++] = pattern_dig;
    wild_chars[i] = '\0';

		/*=======================================================
		 * Perform comparison.
		 *======================================================*/
    i = wc_match(s,t);
    return(i);
   }




	/***************************************************************
	 * wc_match() :
	 * Recursive routine, checks for match between 2 strings.
	 ***************************************************************/
int   wc_match(
    char  *s,		/* <r> input pattern, may contain wildcards	*/
    char  *t		/* <r> comparison string			*/
   )
   {
    int   length;

		/*======================================================
		 * Check for "wild_card" character
		 *=====================================================*/
    if (*s == pattern_wc)		/* Wild-card match		*/
       {
        s++;
        if (!*s)			/* No more "input" characters?	*/
            return(1);
        for ( ; *t ; t++)
            if (wc_match(s,t))
                return(1);
        return(0);
       }

		/*=======================================================
		 * Else, check for "any_char" or "any_dig" character
		 *======================================================*/
    if (*s == pattern_any)			/* Any-character match	*/
        return(*t ? wc_match(s+1,t+1) : 0);
    if (*s == pattern_dig && isdigit(*t))	/* Any-digit match	*/
        return(*t ? wc_match(s+1,t+1) : 0);

		/*=======================================================
		 * Else, check for plain old pattern match, up as far as
		 *  the next "wild_char" character.
		 *======================================================*/
    length = strcspn(s,wild_chars);
    if (!length)
        return(!strcmp_uc(s,t));			/*  */
    if (!strncmp_uc(s,t,length))			/*  */
        return(wc_match(s+length,t+length));
    else
        return(0);			/*   <<< no match >>>	*/
   }



	/*****************************************************************
	 * strcmp_uc:    strncmp_uc:
	 *****************************************************************/
#define upcase(C)  (islower(C) ? C+'A'-'a' : C)

static strcmp_uc(s,t)
unsigned char  *s,*t;
   {
    for ( ; *s  ; s++,t++)
       {
        if (*s == *t)
            continue;
        if (upcase(*s) != upcase(*t))
            break;
       }
    return((int)*s - (int)*t);
   }

static strncmp_uc(s,t,n)
unsigned char  *s,*t;
int   n;
   {
    if (n < 1)
        return(0);

    for ( ; *s && n  ; s++,t++,n--)
       {
        if (*s == *t)
            continue;
        if (upcase(*s) != upcase(*t))
            break;
       }
    return(n ? (upcase(*s)-upcase(*t)) : 0);
   }






	/*****************************************************************
	 * main:
	 *****************************************************************/
/*main()
/*   {
/*    char   comparison[40];
/*    char   input[40];
/*
/*    char   *str,*getstr();
/*
/*    printf("Enter 'comparison' string >  ");
/*    if ((str=getstr()) == 0)  exit();
/*    sscanf(str,"%s",comparison);
/*
/*    for ( ; ; )
/*       {
/*        printf("Enter 'input' string >  ");
/*        if ((str=getstr()) == 0)  exit();
/*        sscanf(str,"%s",input);
/*        printf("\tStrings '%s' and '%s' are %s\n",
/*            input,comparison,
/*            wildcard_match(input,comparison)?"matching":"different");
/*       }
/*   }								/*  */
#include        <stdio.h>
#include        <stdlib.h>
#include        "dasutil.h"
#ifdef vms
#include        <unixio.h>
#endif

/***********************************************************************
* yesno.c
*
* Utility routines yesno(), interactive(), etc.
*
* History:
*  19-Jan-1993  TRG   Create.  Copied from VAX.
*
************************************************************************/



	/****************************************************************
	 * yesno:
	 ****************************************************************/
int   yesno(
    int   v		/* Default response: 0=false  nonZero=true	*/
   )
   {
    int i,val ;
    char buf[10];		/* For terminal input			*/

    val = v;			/* Set "default" return value		*/
    for ( ; ; )
       {
        if (fgets(buf,sizeof(buf),stdin) == 0)  exit(0);
        i = buf[0];

        if (i=='N' || i=='n')
           {
            val = 0;  break;
           }
        else if (i=='Y' || i=='y')
           {
            val = 1;  break;
           }
        else if (i == '\n')
            break;

        printf("\t\t'y' or 'n' please  > ");
       }
    return(val);
   }



	/***************************************************************
	 * interactive:
	 ***************************************************************/
int   interactive()
   {
    return(isatty(fileno(stdin)));
   }
