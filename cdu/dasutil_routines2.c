#if defined(vms)
#include        <varargs.h>
#elif defined (_WIN32)
#include <io.h>
#define isatty _isatty
#endif

#include        "dasutil.h"
#include        <ctype.h>
#include        <stdio.h>
#include        <string.h>
#include        <time.h>

/***********************************************************************
* CDATE.C --
*
* return ptr to ASCII date string:  dd-mmm-yy
*
* History:
*  30-Mar-1988  TRG   Add optional "usrtime" parameter.
*                     qdatime(): New function.
*  25-Jul-1986  TRG   Add cdatime() function.
*  21-Dec-1984  TRG   Create.
*
************************************************************************/


static char  *month[] = {"Jan","Feb","Mar","Apr","May","Jun",
                        "Jul","Aug","Sep","Oct","Nov","Dec"};
static struct cmd_struct  monthList[13] = {
                0, (void *)(sizeof(monthList)/sizeof(monthList[0]))
               ,1,    "JANUARY"
               ,2,    "FEBRUARY"
               ,3,    "MARCH"
               ,4,    "APRIL"
               ,5,    "MAY"
               ,6,    "JUNE"
               ,7,    "JULY"
               ,8,    "AUGUST"
               ,9,    "SEPTEMBER"
               ,10,   "OCTOBER"
               ,11,   "NOVEMBER"
               ,12,   "DECEMBER"
               };



	/**************************************************************
	 * cdate:
	 **************************************************************/
char  *cdate(
    int   usrtime		/* <r> time in seconds			*/
   )
   {
    struct tm  *t;
    time_t  bintim;
    static char  datebuf[14];

    bintim = usrtime ? usrtime : time(0);
    t = localtime(&bintim);	/* Fill in "tm" structure		*/

    sprintf(datebuf,"%02d-%s-%02d",
        t->tm_mday,month[t->tm_mon],t->tm_year);
    return(datebuf);
   }



	/************************************************************
	 * cdatime:
	 ************************************************************/
char  *cdatime(
    int   usrtime		/* <r> time in seconds			*/
   )
   {
    int   year;
    struct tm  *t;
    time_t  bintim;
    static char  datimbuf[22];

    bintim = usrtime ? usrtime : time(0);
    t = localtime(&bintim);	/* Fill in "tm" structure		*/

    year = t->tm_year;
    if (year < 70)  year += 100;	/* Y2K	*/
    year += 1900;
    sprintf(datimbuf,"%02d-%s-%04d %02d:%02d:%02d",
        t->tm_mday,month[t->tm_mon],year,
        t->tm_hour,t->tm_min,t->tm_sec);
    return(datimbuf);
   }



#ifdef vms
	/************************************************************
	 * qdatime:
	 ************************************************************/
char  *qdatime(usrtime)
int   usrtime[2];		/* Quad-word, passed by reference	*/
   {
    int   numArgs;
    int   *q;			/* Ptr to quadword			*/
    static char  qdatimbuf[24];
    struct {
            short year;
            short month;
            short day;
            short hour;
            short min;
            short sec;
            short hundr;
           }  t;
    extern int   sys$numtim();

    va_count(numArgs);
    q = numArgs ? usrtime : 0;
    if (!q || (q[0] || q[1]))
       {
        sys$numtim(&t,q);

        sprintf(qdatimbuf,"%02d-%s-%04d %02d:%02d:%02d.%02d",
            t.day,month[t.month-1],t.year,t.hour,t.min,t.sec,t.hundr);
       }
    else
        strcpy(qdatimbuf,"   <None specified>    ");
    return(qdatimbuf);
   }
#endif



	/*****************************************************************
	 * now:
	 *****************************************************************/
char  *now()
   {
    return(cdatime(time(0)));
   }



	/**************************************************************
	 * asc2time:
	 * Convert string to "time" integer
	 **************************************************************/
int   asc2time(
    char  ascString[]		/* <r> DateString: dd-mmm-yy[yy] hh:mm:ss */
   ,time_t  *bintim		/* <w> time integer			*/
   )
   {
    int   k;
    char  *p,*p2;
    time_t  ktim;
    struct tm  tm;
    struct tm  *tmDefault;
    static char  token[12];
    static DESCRIPTOR(dsc_token,token);

		/*====================================================
		 * Get current time to fill in defaults ...
		 *===================================================*/
    time(&ktim);
    tmDefault = localtime(&ktim);
    clear_buffer(&tm,sizeof(tm));

		/*====================================================
		 * Parse ascii string ...
		 *===================================================*/
    if (!(p = nonblank(ascString)))
        return(0);
    if (!(p2 = strchr(p,'-')))
        return(0);
    if (p2 == p)
        tm.tm_mday = tmDefault->tm_mday;
    else
       {
        if (!longToken(&p,&dsc_token,0,&k))  return(0);
        if (*p && p!=p2)  return(0);
        tm.tm_mday = k;
       }

    p = nonblank(p2+1);			/*---- month ----*/
    p2 = p ? strchr(p,'-') : 0;
    if (!p || p2==p)
        tm.tm_mon = tmDefault->tm_mon;
    else
       {
        if (!(k = cmd_lookup(&p,monthList,0,NOMSG,0)))
            return(0);
        tm.tm_mon = k - 1;
        p = nonblank(p);
        if (p != p2)  return(0);
       }

    p = p ? p+1 : 0;			/*----- year ----*/
    if (!p || !*p || *p==' ' || *p==':')
        tm.tm_year = tmDefault->tm_year;
    else
       {
        if (!(longToken(&p,&dsc_token,0,&k)))  return(0);
        tm.tm_year = (k<1900) ? k : k-1900;
       }

		/*----------------------------------------------------
		 * <optional> hours:minutes:seconds
		 *---------------------------------------------------*/
    if (p && *p && (*p==' ' || *p==':'))
       {					/* hours		*/
        p = nonblank(p+1);
        if (p && isdigit(*p))
            {
             longToken(&p,&dsc_token,0,&k);
             tm.tm_hour = k;
            }
        else
            tm.tm_hour = tmDefault->tm_hour;

        if (p && *p==':')
           {					/* minutes		*/
            p = nonblank(p+1);
            if (p && isdigit(*p))
                {
                 longToken(&p,&dsc_token,0,&k);
                 tm.tm_min = k;
                }
            else
                tm.tm_min = tmDefault->tm_min;

            if (p && *p==':')
               {				/* seconds		*/
                p = nonblank(p+1);
                if (p && isdigit(*p))
                    {
                     longToken(&p,&dsc_token,0,&k);
                     tm.tm_sec = k;
                    }
                else
                    tm.tm_sec = tmDefault->tm_sec;
               }
           }
       }
    *bintim = mktime(&tm);
    return(1);
   }


	/***************************************************************
	 * main:
	 ***************************************************************/
/*main()
/*   {
/*    int   bintim;
/*    int   sts;
/*    static char  line[80];
/*    static DESCRIPTOR(dsc_line,line);
/*
/*    printf("cdate = %s\ncdatime = %s\n\n",cdate(0),cdatime(0));
/*    printf("now = %s\n",now());
/*
/*    for ( ; ; )
/*       {
/*        if (!getLine("Date/time",&dsc_line))
/*            break;
/*        sts = asc2time(line,&bintim);
/*        if (~sts & 1)
/*           {
/*            printf("\terror from asc2time\n\n");
/*            continue;
/*           }
/*        printf("bintim=%d  ctime=%s\n\n",bintim,ctime(&bintim));
/*       }
/*   }						/*  */
#include        "dasutil.h"
#include        <ctype.h>
#include        <errno.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#ifdef vms
#include        <ssdef.h>
#include        <unixio.h>
#include        <varargs.h>
#endif

/*********************************************************************
* GETNEXT.C --
*
* Routines for interactive i/o, with type-ahead.
*
* History:
*  09-Jun-1998  TRG  getString: Use str_copy_dx to set dsc_val string.
*                    getLine:   Use str_copy_dx to set dsc_val string.
*  20-May-1998  TRG  startNextToken: fgets becomes fgets_with_edit.
*  14-Oct-1996  TRG  getCmd: new function.
*  28-Feb-1995  TRG  getString_linePtr: new function.
*  15-Aug-1994  TRG  successReturn: Return ascToken in dsc_flag.
*  13-Aug-1994  TRG  Prompt may be C-string, as well as Descriptor.
*  12-Aug-1994  TRG  Added getString.
*  02-Aug-1994  TRG  Create.  Based on das$idl_tic.c.
*
*********************************************************************/


#define MAX_INDIRECT   3	/* Max number of indirect files		*/

struct inputStruct  {
        FILE  *fp;
        int   isatty;			/* T if input is tty		*/
        int   lineno;
        char  *linePtr;
        char  line[256];
       }  inputStruct[MAX_INDIRECT + 1];

union allTypes  {
        char  c;		/* TYP_BYTE				*/
        short i;		/* TYP_INT				*/
        int   l;		/* TYP_LONG				*/
        float f;		/* TYP_FLOAT				*/
        double d;		/* TYP_DOUBLE				*/
        char  *s;		/* TYP_STRING				*/
       };
#define TYP_BYTE     1
#define TYP_INT      2
#define TYP_LONG     3
#define TYP_FLOAT    4
#define TYP_DOUBLE   5
#define TYP_STRING   6


static int   inIdx;		/* Current level of input indirection	*/
static double dmin,dmax;	/* Data min, max			*/
static char  defStr[80];	/* "defaultVal" string, for prompt	*/
static char  token[80];
static DESCRIPTOR(dsc_token,token);
static DESCRIPTOR(dsc_cmdLine,inputStruct[0].line);
static struct descriptor  *dsc_flag;

static int   zero=0,one=1;

#ifdef vms
extern int   lib$signal();
extern int   lib$get_foreign();
#endif



	/***************************************************************
	 * __initialize:
	 ***************************************************************/
static void  __initialize()
   {
    if (inputStruct[0].fp)
        return;				/*---------------------> return	*/

#ifdef vms
    if (!dsc_cmdLine.dscA_pointer[0])
       {
        short len;

        lib$get_foreign(&dsc_cmdLine,0,&len);
        dsc_cmdLine.dscA_pointer[len] = '\0';
       }
#endif

    inputStruct[0].linePtr = dsc_cmdLine.dscA_pointer;

    inputStruct[0].fp = stdin;
    inputStruct[0].isatty = isatty(fileno(stdin));
    return;				/*---------------------> return	*/
   }



	/**************************************************************
	 * termio_init:
	 * Initialize cmdLine from argc,argv
	 **************************************************************/
void  termio_init(
    int   argc
   ,char  *argv[]
   )
   {
    int   i,k;
    int   maxlen;
    int   offset;
    char  *cmdLine;

    offset = 0;
    set_pgmname(argv[0]);
    cmdLine = dsc_cmdLine.dscA_pointer;
    maxlen = sizeof(inputStruct[0].line);

    for (i=1 ; i<argc ; i++)
       {
        k = strlen(argv[i]);
        if ((offset+k+1) >= maxlen)
           {
            dasmsg(0,"termio_init: *WARN* command line truncated");
            break;
           }
        sprintf(cmdLine+offset," %s",argv[i]);
        offset += k+1;
       }
    return;
   }



	/***************************************************************
	 * clearnext:
	 * Usage:  clearnext
	 *  Clear the type-ahead buffer
	 ***************************************************************/
void  clearnext(
    int   showFlag		/* <r> 1:show deleted line  0:no msg	*/
   )
   {
    if (showFlag && nonblank(inputStruct[inIdx].linePtr))
        printf("    Rest of line ignored: '%s'\n",
            inputStruct[inIdx].linePtr);
    inputStruct[inIdx].linePtr = 0;
    return;
   }



	/****************************************************************
	 * putNext:
	 * Add some text to type-ahead buffer ...
	 ****************************************************************/
void  putNext(
    void  *dsc_text		/* <r> string or descr: text to place	*/
   ,int   flag			/* <r> 0:atEnd 1:clear 2:atStart	*/
   )
   {
    int   k;
    int   numArgs;
    char  *p;
    char  temp[256];
    struct descriptor  *dsc;

    __initialize();

#ifdef vms
    va_count(numArgs);			/* get user's arg count		*/
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    if (is_cdescr(dsc_text))
       {
        dsc = dsc_text;
        p = dsc->dscA_pointer;
        k = dsc->dscW_length;
       }
    else
       {
        p = dsc_text;
        k = strlen(p);
       }

    switch((numArgs>1) ? flag : 0)
       {
        default:
            dasmsg(0,"putNext: *ERR* in usage");
#ifdef vms
            lib$signal(SS$_DEBUG);
#endif
            break;

        case 0:			/* add text at end ...			*/
            strntrim(temp,p,k);
            k = strlen(temp);
            if (inputStruct[inIdx].linePtr)
                strcat(inputStruct[inIdx].linePtr,temp);
            else
               {
                strcpy(inputStruct[inIdx].line,temp);
                inputStruct[inIdx].linePtr = inputStruct[inIdx].line;
               }
            break;

        case 1:			/* replace existing text with new text	*/
            strntrim(inputStruct[inIdx].line,p,k);
            inputStruct[inIdx].linePtr = inputStruct[inIdx].line;
            break;

        case 2:			/* new text at start of line		*/
            if (inputStruct[inIdx].linePtr)	/* old text in "temp"	*/
                strcpy(temp,inputStruct[inIdx].linePtr);
            else
                temp[0] = '\0';

            strntrim(inputStruct[inIdx].line,p,k);
            k = strlen(inputStruct[inIdx].line);
            inputStruct[inIdx].line[k] = ' ';
            strcpy(inputStruct[inIdx].line+k+1,temp);
            inputStruct[inIdx].linePtr = inputStruct[inIdx].line;
            break;
       }
    return;
   }



	/***************************************************************
	 * clearAll:
	 ***************************************************************/
static void  clearAll()
   {
    FILE  *fp;
    char  *linePtr;
    char  *p;
#ifdef vms
    char  filename[128];
#endif

    for ( ; inIdx ; inIdx--)
       {
        fp = inputStruct[inIdx].fp;
#ifdef vms
        p = fgetname(fp,filename);
#else
        p = "indirect file";
#endif
        printf("  --> closing %s,\tline %d\n",p?p:" ???unknown???",
            inputStruct[inIdx].lineno);
        fclose(fp);
       }

    if (linePtr = nonblank(inputStruct[inIdx].linePtr))
        printf("  Deleting remainder of input line '...%s'\n",linePtr);
    inputStruct[inIdx].linePtr = 0;

    return;
   }



	/*****************************************************************
	 * successReturn:
	 *****************************************************************/
static successReturn(
    char  *linePtr		/* <r> addr at end of scanned token	*/
   ,int   usingDefault		/* <r> flag				*/
   )
   {
    int   k;

    strcpy(token," ");
    if (linePtr && usingDefault)
       {
        if (*linePtr == ',')
            linePtr++;
        else
           {
            if (*linePtr == '\\')
                linePtr++;
            ascToken(&linePtr,&dsc_token,0,0);
           }
       }
    if (dsc_flag)
       {
        k = strlen(token);
        l2u(token,0);
        if (k < dsc_flag->dscW_length)
           {
            strcpy(dsc_flag->dscA_pointer,token);
            memset(dsc_flag->dscA_pointer+k,'\0',dsc_flag->dscW_length-k);
           }
        else
           {
            strncpy(dsc_flag->dscA_pointer,token,dsc_flag->dscW_length);
            if (k > dsc_flag->dscW_length)
                dasmsg(0,"successReturn: *WARN* token is truncated");
           }
       }

    inputStruct[inIdx].linePtr = linePtr;
    return(1);
   }



	/*****************************************************************
	 * openIndirectFile:
	 *****************************************************************/
static void  openIndirectFile(
    char  *linePtr		/* <r> current pos within cmd line	*/
   )
   {
    FILE  *fp;

    if (*linePtr != '@')
        exit(dasmsg(0,"openIndirectFile: bad param"));

    linePtr++;
    if (inIdx<MAX_INDIRECT && ascFilename(&linePtr,&dsc_token,0) &&
#ifdef vms
        (fp=fopen(token,"r","dna=.inp")))
#else
        (fp=fopen(token,"r")))
#endif
       {
        inputStruct[inIdx].linePtr = linePtr;	/* save old linePtr	*/
        inIdx++;
        inputStruct[inIdx].fp = fp;
        inputStruct[inIdx].isatty = isatty(fileno(fp));
        inputStruct[inIdx].lineno = 0;
        inputStruct[inIdx].linePtr = 0;
        return;			/*----------------------------> return	*/
       }
		/*========================================================
		 * Can't open indirect file, so close out *all* indirect
		 *  files, since sequence of input is bound to be messed up.
		 *=======================================================*/
    if (inIdx >= MAX_INDIRECT)
        printf("\n--> Max number of indirect files exceeded\n");
    else
        printf("\n--> Error opening indirect file %s\n",token);

    if (!inputStruct[inIdx].isatty)
        clearAll();		/* File input:  close all files ...	*/
    else
       {			/*..else tty input: clear rest of line	*/
        if (linePtr = nonblank(linePtr))
            printf("  Deleting remainder of input line '...%s'\n",linePtr);
        inputStruct[inIdx].linePtr = 0;
       }
    return;			/*----------------------------> return	*/
   }



	/*****************************************************************
	 * startNextToken:
	 *****************************************************************/
static char  *startNextToken(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   )
   {
    int   k;
    int   len;
    char  *p;
    char  promptString[256];
    char  *linePtr;
    struct inputStruct  *ip;
    struct descriptor  *dsc;

    ip = inputStruct + inIdx;
    linePtr = nonblank(ip->linePtr);
    for ( ; !linePtr ; linePtr=nonblank(linePtr))
       {
		/*--------------------------------------------------------
		 * Maybe write prompt.  Then, read next line
		 *-------------------------------------------------------*/
        if (dsc_prompt && ip->isatty)
           {			/*..only if in interactive mode		*/
            sprintf(promptString,"%s",inIdx ? "TT:" : "");
            len = strlen(promptString);
            if (is_cdescr(dsc_prompt))
               {
                dsc = dsc_prompt;
                p = dsc->dscA_pointer;
                k = dsc->dscW_length;
               }
            else
               {
                p = (char *)dsc_prompt;
                k = strlen(p);
               }
            if ((len+k) >= sizeof(promptString))
                k = sizeof(promptString) - len - 1;
            strncpy(promptString+len,p,k);
            len += k;
            promptString[len] = '\0';

            if (defStr[0])
               {
                k = strlen(defStr);
                if (((len+3)+k) >= sizeof(promptString))
                    k = sizeof(promptString) - (len+3) - 1;
                if (k > 0)
                   {
                    promptString[len++] = ' ';
                    promptString[len++] = '[';
                    strncpy(promptString+len,defStr,k);
                    len += k;
                    promptString[len++] = ']';
                   }
               }

            if ((len+3) >= sizeof(promptString))
                promptString[len] = '\0';
            else
                strcpy(promptString+len,":  ");
           }
		/*---------------------------------------------------------
		 * Read next line from current input ...
		 *--------------------------------------------------------*/
        if (fgets_with_edit(ip->line,sizeof(ip->line),ip->fp,promptString))
           {
            ip->lineno++;
            if (ip->isatty)
                printf("\n");
            strtrim(ip->line,0);
            linePtr = ip->line;
            continue;			/*----------------> back to top	*/
           }
		/*--------------------------------------------------------
		 * On EOF, close current file and continue reading
		 *  previous input file.
		 *-------------------------------------------------------*/
        if (ip->isatty)
            printf("\n");
        fclose(ip->fp);
        if (!inIdx)
            return(0);			/*----------------> EOF return	*/
        inIdx--;
        if (!inIdx && !dsc_prompt)
            return(0);			/*----> special no-input return	*/
        ip = inputStruct + inIdx;
        linePtr = ip->linePtr;
       }
		/*========================================================
		 * Non-blank input in buffer ...
		 *=======================================================*/
    if (*linePtr == '@')
       {
        openIndirectFile(linePtr);
        return(startNextToken(dsc_prompt));
       }
    ip->linePtr = linePtr;
    return(linePtr);				/*------------> return	*/
   }



	/**************************************************************
	 * outOfRange:
	 * Check for numeric value out-of-range
	 **************************************************************/
static int   outOfRange(dval)
double dval;
   {
    static char  fmt_OutOfRange[] =
          "--> GETNEXT: input is out-of-range.  Legal range is %.6g <= val";

			/*-----------------------------------------------
			 * Check for min and max range ...
			 *----------------------------------------------*/
    if (dmin==0.0 && dmax==0.0)
        ;				/* do nothing		*/
    else
       {				/*..else check limits	*/
        if ((float)dval<(float)dmin || (float)dval>(float)dmax)
           {
            printf(fmt_OutOfRange,dmin);
            if (dmax > dmin)
                printf(" <= %.6g",dmax);
            printf("\n");
            return(1);			/*--------------------> return	*/
           }
       }
    return(0);				/*--------------------> return	*/
   }



	/**************************************************************
	 * badInput:
	 **************************************************************/
static void  badInput(
    char  *txt		/* <r> Message text to print out [0 for none]	*/
   )
   {
    if (!inputStruct[inIdx].isatty)
        clearAll();		/* Close all files ...			*/
    else
       {
        if (txt)
            printf("%s\n",txt);
        clearnext(1);
       }
    return;
   }



	/**************************************************************
	 * getString_linePtr:
	 * getString_startOfLine:
	 **************************************************************/
char  *getString_linePtr()	/* Returns:  addr of next non-blank char */
   {
    __initialize();
    return(nonblank(inputStruct[inIdx].linePtr));
   }


char  *getString_startOfLine()	/* Returns:  addr of current line	*/
   {
    __initialize();
    return(nonblank(inputStruct[inIdx].line));
   }



	/**************************************************************
	 * bumpString_linePtr:
	 * Used to bump linePtr past next non-blank char.
	 * Typical usage: to bump past '-' in shot range "1234 - 1255".
	 **************************************************************/
void  bumpString_linePtr()
   {
    char  *p;

    __initialize();
    p = nonblank(inputStruct[inIdx].linePtr);
    inputStruct[inIdx].linePtr = p ? nonblank(p+1) : 0;
    return;
   }



	/***************************************************************
	 * getString:
	 ***************************************************************/
int   getString(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   ,char  alphExtensions[]		/* <r:opt> other legal 1st chars */
   )
   {
    int   k;
    int   numArgs;
    int   sts;
    char  *linePtr;
    char  *otherAlph;

    __initialize();

#ifdef vms
    va_count(numArgs);
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    if (numArgs < 2)
       {
        printf("  GETSTRING: insufficient arguments\n\t**EXIT**\n");
        exit(0);
       }
    otherAlph = (numArgs > 2) ? alphExtensions : 0;

    k = dsc_val->dscW_length;
    if (k > sizeof(defStr)-3)
        k = sizeof(defStr) - 3;
    defStr[0] = '\'';
    strntrim(defStr+1,dsc_val->dscA_pointer,k);
    strcat(defStr,"\'");
    dsc_flag = 0;

    for ( ; ; )
       {
        linePtr = startNextToken(dsc_prompt);
        if (!linePtr)
            return(0);			/*--------------> error return	*/
        if (*linePtr == ',')
            return(successReturn(linePtr,1));

        sts = ascToken(&linePtr,&dsc_token,0,otherAlph);
        if (~sts & 1)
           {
            badInput("  --> Bad input??  With getString???");
            continue;
           }

        str_copy_dx(dsc_val,&dsc_token);
        return(successReturn(linePtr,0));
       }
   }



	/****************************************************************
	 * getDouble:
	 ****************************************************************/
int   getDouble(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,double *val			/* <m> data value			*/
   ,double *valMin		/* <opt:r> lower data limit		*/
   ,double *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   )
   {
    int   incrDecrFlag;		/* Increment=1  Decrement=2		*/
    int   numArgs;
    int   sts;
    char  *linePtr;
    double dval;

    __initialize();

#ifdef vms
    va_count(numArgs);		/* Get user's arg cnt			*/
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    if (numArgs < 2)
       {
        printf("  GETDOUBLE: insufficient arguments.  Syntax is :\n");
        printf("   call GETDOUBLE ( prompt , val");
        printf(" [, valMin , valMax , flag ] )\n");
        exit(0);
       }

    sprintf(defStr,"%.6g",(float)*val);
    dmin = (numArgs>2 && valMin) ? *valMin : 0.0;
    dmax = (numArgs>3 && valMax) ? *valMax : dmin;
    dsc_flag = (numArgs>4) ? dsc_userFlag : 0;

    for ( ; ; )
       {
        linePtr = startNextToken(dsc_prompt);
        if (!linePtr)
            return(0);			/*---------------> error return	*/

		/*-------------------------------------------------------
		 * Check for increment/decrement operator ...
		 *------------------------------------------------------*/
        if (!strncmp(linePtr,"++",2))
            incrDecrFlag = 1;
        else if (!strncmp(linePtr,"--",2))
            incrDecrFlag = 2;
        else
            incrDecrFlag = 0;

        if (incrDecrFlag)
            linePtr += 2;

		/*--------------------------------------------------------
		 * Try to read characters as a number ...
		 *-------------------------------------------------------*/
        sts = doubleToken(&linePtr,&dsc_token,0,&dval);
        if (~sts & 1)
           {
			/*===============================================
			 * Couldn't read as a number: maybe accept default
			 *===============================================*/
            if (incrDecrFlag || (!dsc_flag && *linePtr!=','))
               {
                badInput("  --> Bad input:  rest of line ignored");
                continue;		/*----------------> back to top	*/
               }
            if (outOfRange(*val))
               {
                badInput(0);
                continue;		/*----------------> back to top	*/
               }
            return(successReturn(linePtr,1));	/*------------> return	*/
           }

		/*--------------------------------------------------------
		 *..Ok, got number from input.
		 *-------------------------------------------------------*/
        if (incrDecrFlag == 1)
            dval += *val;
        else if (incrDecrFlag == 2)
            dval = *val - dval;

        if (outOfRange(dval))
           {
            badInput(0);
            continue;			/*----------------> back to top	*/
           }

        *val = dval;
        return(successReturn(linePtr,0));	/*-------------> return	*/
       }
   }



	/****************************************************************
	 * getLong:
	 ****************************************************************/
int   getLong(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,int   *val			/* <m> data value			*/
   ,int   *valMin		/* <opt:r> lower data limit		*/
   ,int   *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   )
   {
    int   numArgs;
    int   sts;
    double v,vmin,vmax;

#ifdef vms
    va_count(numArgs);		/* Get user's arg cnt			*/
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    if (numArgs < 2)
       {
        printf("  GETLONG: insufficient arguments.  Syntax is :\n");
        printf("   call GETLONG ( prompt , val");
        printf("  [, valMin , valMax , flag ] )\n");
        exit(0);
       }

    v = (double)(*val);
    vmin = (numArgs>2 && valMin) ? (double)(*valMin) : -1.e+36;
    vmax = (numArgs>2 && valMax) ? (double)(*valMax) : 1.e+36;

    sts = getDouble(dsc_prompt,&v,&vmin,&vmax,(numArgs>4)?dsc_userFlag:0);
    if (sts & 1)
        *val = (int )v;
    else
        dasmsg(0,"getLong: err sts from getDouble");

    return(sts);
   }



	/****************************************************************
	 * getFloat:
	 ****************************************************************/
int   getFloat(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,float *val			/* <m> data value			*/
   ,float *valMin		/* <opt:r> lower data limit		*/
   ,float *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   )
   {
    int   numArgs;
    int   sts;
    double v,vmin,vmax;

#ifdef vms
    va_count(numArgs);		/* Get user's arg cnt			*/
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    if (numArgs < 2)
       {
        printf("  GETFLOAT: insufficient arguments.  Syntax is :\n");
        printf("   call GETFLOAT ( prompt , val");
        printf("  [, valMin , valMax , flag ] )\n");
        exit(0);
       }

    v = (double)(*val);
    vmin = (numArgs>2 && valMin) ? (double)(*valMin) : -1.e+36;
    vmax = (numArgs>2 && valMax) ? (double)(*valMax) : 1.e+36;

    sts = getDouble(dsc_prompt,&v,&vmin,&vmax,(numArgs>4)?dsc_userFlag:0);
    if (sts & 1)
        *val = (float)v;
    else
        dasmsg(0,"getFloat: err sts from getDouble");

    return(sts);
   }



	/**************************************************************
	 * getFilename:
	 **************************************************************/
int   getFilename(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   )
   {
    return(getString(dsc_prompt,dsc_val,"$[]_-.:;0123456789"));
   }



	/******************************************************************
	 * getEqualsLong:
	 ******************************************************************/
int   getEqualsLong(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,int   *val			/* <m> data value			*/
   ,int   *valMin		/* <opt:r> lower data limit		*/
   ,int   *valMax		/* <opt:r> upper data limit		*/
   ,struct descriptor  *dsc_userFlag	/* <opt:w> 1-char "flag"	*/
   )
   {
    int   numArgs;
    char  *p;
    void  *argMin,*argMax,*argFlag;

#ifdef vms
    va_count(numArgs);		/* Get user's arg cnt			*/
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    argMin = (numArgs > 2) ? valMin : 0;
    argMax = (numArgs > 3) ? valMax : 0;
    argFlag = (numArgs > 4) ? dsc_userFlag : 0;

    p = startNextToken(dsc_prompt);
    if (!p)
        return(0);			/*---------------> error return	*/
    if (*p == '=')
        inputStruct[inIdx].linePtr++;		/* bump lineptr past '=' */

    return(getLong(dsc_prompt,val,argMin,argMax,argFlag));
   }



	/***************************************************************
	 * getEqualsString:
	 ***************************************************************/
int   getEqualsString(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   ,char  alphExtensions[]		/* <r:opt> other legal 1st chars */
   )
   {
    int   numArgs;
    char  *otherAlph;
    char  *p;

    __initialize();

#ifdef vms
    va_count(numArgs);
#else
    numArgs = 99;			/* .. or trust to prototyping	*/
#endif
    if (numArgs < 2)
       {
        printf("  GETEQUALSSTRING: insufficient arguments\n\t**EXIT**\n");
        exit(0);
       }
    otherAlph = (numArgs > 2) ? alphExtensions : 0;

    p = startNextToken(dsc_prompt);
    if (!p)
        return(0);			/*---------------> error return	*/
    if (*p == '=')
        inputStruct[inIdx].linePtr++;		/* bump lineptr past '=' */

    return(getString(dsc_prompt,dsc_val,otherAlph));
   }



	/**************************************************************
	 * getEqualsFilename:
	 **************************************************************/
int   getEqualsFilename(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   )
   {
    char  *p;

    p = startNextToken(dsc_prompt);
    if (!p)
        return(0);			/*---------------> error return	*/
    if (*p == '=')
        inputStruct[inIdx].linePtr++;		/* bump lineptr past '=' */

    return(getFilename(dsc_prompt,dsc_val));
   }



	/************************************************************
	 * getYesno:
	 ************************************************************/
#define CMD_YES     1
#define CMD_NO      2

int   getYesno(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,int   ynDefault		/* <r> default:  1=Y  0=N		*/
   )
   {
    int   opt;
    int   sts;
    char  *linePtr;
    char  *p;
    void  *promptPtr;
    static struct cmd_struct  cmd_yesno[5] = {
                        0, (void *)(sizeof(cmd_yesno)/sizeof(cmd_yesno[0]))
                       ,CMD_YES,       "YES"
                       ,CMD_NO,        "NO"
                       ,CMD_YES,       "TRUE"
                       ,CMD_NO,        "FALSE"
                       };

		/*=======================================================
		 * Set "default" string
		 *======================================================*/
    strcpy(defStr,ynDefault?"Y":"N");

		/*======================================================
		 * Get user's reply
		 *=====================================================*/
    promptPtr = dsc_prompt;
    for ( ; ; promptPtr="  --> Y or N, please")
       {
        linePtr = startNextToken(promptPtr);
        if (!linePtr)
            exit(0);
        if (*linePtr == ',')
           {
            inputStruct[inIdx].linePtr = linePtr + 1;
            return(ynDefault);
           }

        sts = ascToken(&linePtr,&dsc_token,0,0);
        if (~sts & 1)
           {
            badInput("  --> Bad input??  With getYesno???");
            continue;
           }

        p = token;
        opt = cmd_lookup(&p,cmd_yesno,0,NOMSG,0);
        if (opt)
            break;

        clearAll();		/* Delete input line, close all files	*/
       }
    inputStruct[inIdx].linePtr = linePtr;
    return(opt==CMD_YES);
   }



	/****************************************************************
	 * getCmd:
	 ****************************************************************/
int   getCmd(
    void   *prompt		/* <r> Prompt string: char or dsc	*/
   ,struct cmd_struct  cmd[]	/* <r> List of possible commands	*/
   ,char  defaultCmd[]		/* <r,opt> Default command string	*/
   ,int   flags			/* <r> Flags: 1=NoMsg			*/
   )
   {
    int   k;
    char  *p;
    char  cmdString[80];
    static char  string[32];
    static DESCRIPTOR(dsc_string,string);

    strcpy(string,defaultCmd?defaultCmd:" ");
    if (!getString(prompt,&dsc_string,0))
        exit(0);
    strcpy(cmdString,string);
    if (string[0] == '/')
       {
        if (isalpha(*inputStruct[inIdx].linePtr))
           {
            getString(0,&dsc_string,0);
            strcpy(cmdString+1,string);
           }
       }
    p = cmdString;
    k = cmd_lookup(&p,cmd,0,flags,0);
    if (!k)
        clearnext(1);

    return(k);
   }



	/****************************************************************
	 * getLine:
	 ****************************************************************/
int   getLine(
    void  *dsc_prompt		/* <r> string or descr: prompt string	*/
   ,struct descriptor  *dsc_val		/* <m> string returned		*/
   )
   {
    char  *linePtr;
    static int   incr = 16;	/* mem increment for dynamic strings	*/

    __initialize();

    defStr[0] = '\0';		/* no default string			*/
    linePtr = startNextToken(dsc_prompt);
    if (!linePtr)
        return(0);			/*---------------> error return	*/

    str_copy_dx(dsc_val,linePtr);
    clearnext(0);			/* clear current line		*/
    return(1);
   }



	/**************************************************************
	 * main:
	 **************************************************************/
/*main()
/*   {
/*    int   sts;
/*    static double val;
/*    static double xmin= -100.0, xmax=100.0;
/*    static char  flag[4];
/*    static $DESCRIPTOR(dsc_flag,flag);
/*    static $DESCRIPTOR(dsc_prompt,"Enter value >  ");
/*
/*    for ( ; ; )
/*       {
/*        sts = getDouble(&dsc_prompt,&val,&xmin,&xmax,&dsc_flag);
/*        if (~sts & 1)
/*           {
/*            printf("--> error from getDouble\n\n");
/*            continue;
/*           }
/*        printf("    val=%.6g",val);
/*        if (flag[0] != ' ')
/*            printf("  flag='%s'",flag);
/*        printf("\n\n");
/*       }
/*   }							/*  */
