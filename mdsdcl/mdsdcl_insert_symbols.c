#include        <ctype.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#else
#include        <sys/time.h>
#endif

/**********************************************************************
* MDSDCL_INSERT_SYMBOLS.C --
*
* Insert symbols from parameter list or environment into command line.
*
* History:
*  23-Dec-1997  TRG  Create.
*
************************************************************************/



	/****************************************************************
	 * insertSymbol:
	 ****************************************************************/
static char  *insertSymbol(		/* Return: addr in cmdline	*/
    struct descriptor  *dsc_cmdline	/* <m> the command line		*/
   ,int   offsetStart			/* <r> offset in cmdline to "'"	*/
   ,int   offsetToSymbol		/* <r> offset in cmdline to symbol*/
   ,struct descriptor  *dsc_prmList	/* <r> parameter list		*/
   )
   {
    int   i,k;
    int   offsetEnd;
    int   vlen;
    char  *p,*p2;
    char  symbol[32];
    static DYNAMIC_DESCRIPTOR(dsc_value);

    p = dsc_cmdline->dscA_pointer + offsetToSymbol;
    if (!isalpha(*p))
       {
        fprintf(stderr,"*WARN* 1st char of symbol must be alph\n\n");
        return(p);
       }

    for (p2=p+1 ; isalnum(*p2) ; p2++)
        ;
    k = p2 - p;
    if (k >= sizeof(symbol))
       {
        fprintf(stderr,"Symbol too long");
        return(p);
       }
    if (k)
        strncpy(symbol,p,k);
    symbol[k] = '\0';
    if (*p2 == '\'')
        p2++;

		/*=======================================================
		 * Is symbol a parameter ?
		 *======================================================*/
    if (k==2 && toupper(symbol[0])=='P' &&
            symbol[1]>='1' && symbol[1]<='8')
       {
        sscanf(symbol+1,"%d",&i);
        str_copy_dx(&dsc_value,dsc_prmList+(i-1));
       }
    else
       {
        p = getenv(symbol);
        str_copy_dx(&dsc_value,p?p:"");
       }

    offsetEnd = p2 - dsc_cmdline->dscA_pointer;
    str_replace(dsc_cmdline,dsc_cmdline,offsetStart,offsetEnd,&dsc_value);

		/*-------------------------------------------------------
		 * Return addr of last char of symbol ...
		 *------------------------------------------------------*/
    vlen = dsc_value.dscA_pointer ? strlen(dsc_value.dscA_pointer) : 0;
    return(dsc_cmdline->dscA_pointer + offsetStart + vlen - 1);
   }


	/****************************************************************
	 * mdsdcl_insert_symbols:
	 ****************************************************************/
void  mdsdcl_insert_symbols(		/* Returns: void		*/
    struct descriptor  *dsc_cmdline	/* <m> command line		*/
   ,struct descriptor  *dsc_prmList	/* <r> parameter list		*/
   )
   {
    int   i,k;
    int   doubleQuotes;
    int   offsetStart;
    char  *p,*p2;

    p = dsc_cmdline->dscA_pointer;
    if (!p)  return;

    doubleQuotes = 0;
    for ( ; *p ; p++)
       {
        if (*p == '"')
           {			/*========= Double-quote char =========*/
            if (!doubleQuotes)
                doubleQuotes = 1;
            else if (*(p+1) == '"')
                p++;
            else
                doubleQuotes = 0;
           }
        else if (*p == '\'')
           {			/*========= Single-quote char =========*/
            if (!doubleQuotes)
               {		/* normal unquoted string ...		*/
                if (*(p+1) == '\'')
                    strcpy(p,p+1);	/* replace by single quote	*/
                else
                   {			/*...else, replace symbol	*/
                    offsetStart = p - dsc_cmdline->dscA_pointer;
                    p = insertSymbol(dsc_cmdline,offsetStart,offsetStart+1,
                            dsc_prmList);
                   }
               }
            else
               {		/* inside quoted string ...		*/
                if (*(p+1) == '\'')
                   {			/* 2 single quotes denote symbol */
                    offsetStart = p - dsc_cmdline->dscA_pointer;
                    p = insertSymbol(dsc_cmdline,offsetStart,offsetStart+2,
                            dsc_prmList);
                   }
               }
           }
       }
    return;
   }
