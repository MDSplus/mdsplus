#include        <ctype.h>
#include        <stdlib.h>
#include        <string.h>
#include        <stdio.h>
#include        "clisysdef.h"

/********************************************************************
* CLI_UTIL.C --
*
* Various utility routines ...
*
* History:
*  22-Oct-1997  TRG  Create.
*
*********************************************************************/


static int   cmdstringLen;
static char  *cliNonblank_save;	/* last ptr from cliNonblank	*/


	/****************************************************************
	 * cliNonblank:
	 * Return next non-blank char.
	 ****************************************************************/
char  *cliNonblank(		/* Return: addr in cmd line		*/
    char  *p			/* <r> current position in cmd line	*/
   )
   {
    p = nonblank(p);
    cliNonblank_save = p;
    return(p);			/*----------------------------> return	*/
   }



	/***************************************************************
	 * cli_error:
	 ***************************************************************/
int   cli_error(		/* Display only				*/
    int   sts			/* <r> status value			*/
   ,char  msg[]			/* <r> message to display		*/
   )
   {
    int   k;
    char  fmt[20];
    char  *p;
    struct descriptor  *dsc;

    if (msg && msg[0])
        fprintf(stderr,"\n*ERR* %s\n",msg);

    dsc = cli_addr_cmdline_dsc();
    if (p = dsc->dscA_pointer)
       {
        strtrim(p,0);
        fprintf(stderr,"cmdline = '%s'\n",p);
        k = cliNonblank_save - p;
        if (k>=0 && k<dsc->dscW_length)
           {
            sprintf(fmt,"%%%ds^^^^\n",k+11);
            fprintf(stderr,fmt,"");
           }
       }
    fprintf(stderr,"\n");
    return(sts);
   }



	/***************************************************************
	 * cliToken:
	 * Get token, as described by CLI flags
	 ***************************************************************/
int   cliToken(
    char  **pp			/* <m> addr of ptr to char string	*/
   ,struct descriptor *dsc_ret	/* <w> return token here		*/
   ,int   flags			/* <r> cli flags, describing data type	*/
   )
   {
    int   k;
    int   sts;
    int   typeflag;
    char  c;
    char  *p,*p2;
    char  util[1024];
    static char  legalMdsChars[] = "0123456789._-:;*?%\\";
    static struct descriptor  dscUtil = {
                        sizeof(util)-1,DSC_K_DTYPE_T,DSC_K_CLASS_S,0};

    typeflag = flags & 0x0FFF;
    p = *pp = nonblank(*pp);
    if (!p)
        return(0);

    dscUtil.dscA_pointer = util;	/* set addr in dscUtil descrip	*/

    if (!typeflag ||
      typeflag==VAL_M_QUOTED_STRING || typeflag==VAL_M_USER_DEFINED_TYPE)
        sts = ascToken(&p,&dscUtil,0,legalMdsChars);
    else if (typeflag == VAL_M_FILE)
        sts = ascFilename(&p,&dscUtil,0);
    else if (typeflag == VAL_M_NUMBER)
       {
        if (isdigit(*p) || (*p=='-' && isdigit(*(p+1))))
           {
            sts = longToken(&p,&dscUtil,0,0);
           }
        else if (*p == '%')
           {
            c = toupper(*(p+1));
            p2 = p + 2;		/* first char of putative number	*/
            if (c == 'X')
               {
                for ( ; isxdigit(*p2) ; p2++)
                    ;
               }
            else if (c == 'O')
               {			/* octal number ...		*/
                for ( ; isdigit(*p2) && *p2<='7' ; p2++)
                    ;
               }
            k = p2 - p;			/* number of chars scanned	*/
            if (k == 2)
                sts = 0;		/*..invalid number		*/
            else
               {
                if (k > dscUtil.dscW_length)
                    k = dscUtil.dscW_length;
                strncpy(dscUtil.dscA_pointer,p,k);
                if (k < dscUtil.dscW_length)
                    dscUtil.dscA_pointer[k] = '\0';
                l2un(dscUtil.dscA_pointer,0,k);
                p = nonblank(p2);
                sts = CLI_STS_NORMAL;
               }
           }
       }
    else if (typeflag == VAL_M_DELTATIME)
       {
        sts = deltatimeToken(&p,&dscUtil,0,0);
       }
    else if (typeflag == VAL_M_REST_OF_LINE)
       {
        k = strlen(p);
        if (k > dscUtil.dscW_length)
            k = dscUtil.dscW_length;
        strncpy(dscUtil.dscA_pointer,p,k);
        if (k < dscUtil.dscW_length)
            dscUtil.dscA_pointer[k] = '\0';
        p = 0;
        sts = CLI_STS_NORMAL;
       }
    else
       {
        printf("\n*ERR* cliToken can't handle typeflag %04X\n\n",
            typeflag);
        exit(0);
       }

			/*================================================
			 * Return to caller ...
			 *===============================================*/
    str_copy_dx(dsc_ret,&dscUtil);
    if (sts & 1)
        *pp = nonblank(p);
    return(sts);
   }



	/*************************************************************
	 * make_lookup_keyword:
	 *************************************************************/
struct cmd_struct  *make_lookup_keyword( /* Return: addr of cmd_struct	*/
    struct cduKeyword  keywordList[]	/* <r> keyword list		*/
   )
   {
    int   i;
    int   icnt;
    struct cmd_struct  *cmd;
    struct cduKeyword  *key;

		/*=====================================================
		 * Check size of keywordList[] ...
		 *====================================================*/
    for (icnt=0,key=keywordList ; key->keyA_name ; icnt++,key++)
        ;

    cmd = malloc((icnt+1)*sizeof(struct cmd_struct));
    if (!cmd)
       {
        fprintf(stderr,"*EXIT* Out of space!\n");
        exit(0);
       }
    clear_buffer(cmd,(icnt+1)*sizeof(struct cmd_struct));
    cmd[0].cmdA_string = (void *)(icnt+1);

		/*======================================================
		 * Initialize cmd[] table ...
		 *=====================================================*/
    for (i=1,key=keywordList ; i<=icnt ; i++,key++)
       {
        cmd[i].cmdL_id = i;
        cmd[i].cmdA_string = key->keyA_name;
       }
    return(cmd);
   }
