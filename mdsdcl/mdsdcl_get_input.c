#include        <ctype.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#endif

/**********************************************************************
* MDSDCL_GET_INPUT.C --
*
* Basic input routine for MDSDCL ...
*
* History:
*  22-Dec-1997  TRG  Create.
*
*************************************************************************/


extern struct _mdsdcl_ctrl  MDSDCL_COMMON;
static struct _mdsdcl_ctrl *ctrl = &MDSDCL_COMMON;



	/*****************************************************************
	 * closeIndirectLevel:
	 *****************************************************************/
static int   closeIndirectLevel()
   {
    int   i;
    struct _mdsdcl_io  *io;
    struct _mdsdcl_macro  *macro;

    if (!ctrl->depth)
        return(CLI_STS_EOF);

    io = ctrl->ioLevel + ctrl->depth--;

    if (io->fp)
       {		/* It's a file:  close it ...			*/
        fclose(io->fp);
        io->fp = 0;
       }
    else
       {		/*..else, it's a macro ...			*/
        macro = io->ioMacro;
        macro->isOpen = 0;
        io->ioMacro = 0;
       }

    for (i=0 ; i<8 ; i++)
       str_free1_dx(io->ioParameter + i);

    return(MDSDCL_STS_INDIRECT_EOF);
   }



	/*****************************************************************
	 * openIndirectLevel:
	 *****************************************************************/
static int   openIndirectLevel(		/* Return: status		*/
    char  *p				/* <r> addr in cmdline		*/
   )
   {
    int   i,k;
    char  *p2;
    FILE  *fp;
    struct _mdsdcl_io  *io;
    static DYNAMIC_DESCRIPTOR(dsc_filename);

    if (ctrl->depth >= MAX_DEPTH)
       {
        return(MdsMsg(MDSDCL_STS_INDIRECT_ERROR,
            "Exceeded MAX_DEPTH for indirect files"));
       }

    if (!ascFilename(&p,&dsc_filename,0))
        return(MdsMsg(MDSDCL_STS_INDIRECT_ERROR,
            "Illegal filename: %s",p?p:""));

    fp = fopen(dsc_filename.dscA_pointer,"r");
    if (!fp)
       {
        if (nonblank(ctrl->def_file.dscA_pointer))
           {
            p2 = strchr(ctrl->def_file.dscA_pointer,'*');
            if (p2)
               {
                k = p2 - ctrl->def_file.dscA_pointer;
                str_replace(&dsc_filename,&ctrl->def_file,k,k+1,
                    &dsc_filename);
                fp = fopen(dsc_filename.dscA_pointer,"r");
               }
           }
        if (!fp)
           {
            p2 = dsc_filename.dscA_pointer;
            return(MdsMsg(MDSDCL_STS_INDIRECT_ERROR,
                "No such filename: %s",p2?p2:""));
           }
       }
		/*=====================================================
		 * We've opened the indirect file.  Now fill in params.
		 *====================================================*/
    io = ctrl->ioLevel + ++ctrl->depth;
    io->fp = fp;
    for (i=0 ; i<8 && ascToken(&p,io->ioParameter+i,0,0) ; i++)
        ;
    for ( ; i<8 ; i++)
        str_free1_dx(io->ioParameter+i);

    return(1);
   }



	/*****************************************************************
	 * readInputLine:
	 ****************************************************************/
static int   readInputLine(	/* Return: status			*/
    char  *prompt		/* <r> prompt string			*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   )
   {
    int   k;
    int   sts;
    char  *p;
    FILE  *fp;
    struct _mdsdcl_io  *io;
    struct _mdsdcl_macro  *macro;
    static char  line[1024];

    io = ctrl->ioLevel + ctrl->depth;
    fp = io->fp;

    if (fp)
       {			/* If it's not a macro			*/
        p = fgets_with_edit(line,sizeof(line),fp,prompt);
        if (!p)
           {
            sts = closeIndirectLevel();
            return(sts);		/*--------------------> return	*/
           }
       }
    else
       {			/*..else, get next line from macro ...	*/
        macro = io->ioMacro;
        if (!macro)
           {
#ifdef vms
            lib$signal(SS$_DEBUG);
#endif
            return(MdsMsg(MDSDCL_STS_INDIRECT_ERROR,
                        "Error getting next macro line",0));
           }
        if (io->lineno == macro->numLines)
           {				/* We're at end of macro	*/
            sts = closeIndirectLevel();
            return(sts);		/*--------------------> return	*/
           }
        p = macro->lines[io->lineno++];
       }

    k = strlen(p);
    if (p[k-1] == '\n')
        p[k-1] = '\0';

    str_copy_dx(dsc_cmd,p);
    return(1);
   }



	/*****************************************************************
	 * get_cmdstring:
	 * Get full string with no comments or continuation-characters
	 ****************************************************************/
static int   get_cmdstring(	/* Return: status			*/
    void  *uprompt		/* <r:opt> user-specified prompt	*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   )
   {
    int   i,k;
    int   sts;
    char  *p,*p2;
    char  quote_char;
    struct descriptor  *dsc;
    struct  {
        char  continuation[1];		/* an underscore		*/
        char  prompt[63];		/* prompt string		*/
       }  prompt;
    static DYNAMIC_DESCRIPTOR(dsc_continueLine);

		/*------------------------------------------------------
		 * Prepare the prompt string ...
		 *-----------------------------------------------------*/
    prompt.continuation[0] = '_';
    if (!uprompt)
        strcpy(prompt.prompt,"Command: ");
    else
       {
        if (is_cdescr(uprompt))
           {
            dsc = uprompt;
            p = dsc->dscA_pointer;
            k = dsc->dscW_length;
           }
        else
           {
            p = uprompt;
            k = strlen(p);
           }
        if (k >= sizeof(prompt))
            k = sizeof(prompt.prompt) - 1;
        strncpy(prompt.prompt,p,k);
        prompt.prompt[k] = '\0';
       }

    sts = readInputLine(prompt.prompt,dsc_cmd);
    if (~sts & 1)
       {
        str_free1_dx(dsc_cmd);
        return(sts);			/*--------------------> return	*/
       }

    p = nonblank(dsc_cmd->dscA_pointer);
    if (!p)
        return(1);				/*---> return blank line */

    for ( ; *p ; p++)
       {
        if (IS_COMMENT_CHARACTER(*p))
           {
            for ( ; isspace(*(p-1)) ; p--)
                ;			/* remove white space ...	*/
            *p = '\0';
            break;			/* out of loop			*/
           }

        if (*p=='"' || *p=='\'')
           {		/*=============== Quoted string ===============*/
            quote_char = *p;		/* either single or double quote */

			/*-----------------------------------------------
			 * Search for matching 'quote_char' ...
			 *----------------------------------------------*/
            for (p++ ; *p ; p++)
               {
                if (*p != quote_char)
                    continue;
				/* found a quote_char ...		*/
                if (*(p+1) == quote_char)
                   {		/*..2 successive quote_chars ...	*/
                    p++;
                    continue;
                   }
                break;		/* *p is matching quote_char!		*/
               }
            if (*p)
                continue;	/* *p is matching quote_char		*/
            break;		/* end of line reached ... out of loop	*/
           }

        if (IS_CONTINUATION_CHARACTER(*p))
           {		/*=========== Continuation character ===========*/
            p2 = nonblank(p+1);
            if (p2 && !IS_COMMENT_CHARACTER(*p2))
                continue;	/* not really a "continuation" ...	*/
		/*--------------------------------------------------------
		 * *p is a continuation character.
		 * -- back up within cmdstring[] to preceding non-blank.
		 * -- read continuation of cmdstring[]
		 *-------------------------------------------------------*/
            for ( ; p!=dsc_cmd->dscA_pointer && isspace(*(p-1)) ; p--)
                ;
            *p = ' ';		/* tack on a single blank	*/
            *(p+1) = '\0';
            sts = readInputLine(prompt.continuation,&dsc_continueLine);
            if (~sts & 1)
                break;			/* EOF:  out of loop		*/
            i = p - dsc_cmd->dscA_pointer;
            str_append(dsc_cmd,&dsc_continueLine);
            p = dsc_cmd->dscA_pointer + i;
            str_free1_dx(&dsc_continueLine);
           }
       }
    return(1);
   }



	/****************************************************************
	 * displayCmdline:
	 ****************************************************************/
static void  displayCmdline(
    char  *cmdline		/* <r> the command line			*/
   )
   {
    int   i;

    if (!nonblank(cmdline))
        return;
    for (i=0 ; i<=ctrl->depth ; i++)
        printf("==");
    printf("> %s\n",cmdline);
    return;
   }



	/*****************************************************************
	 * really_get_input:
	 *****************************************************************/
static int   really_get_input(		/* Return: status		*/
    void  *uprompt			/* <r> dsc or c-string		*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   ,int   *len				/* <w:opt> length of command line*/
   ,int   flag				/* <r> 1 for "no-substitute"	*/
   )
   {
    int   sts;
    char  *p;
    struct _mdsdcl_io  *io;

		/*--------------------------------------------------------
		 * Get full string with no comments or continuation-chars.
		 *-------------------------------------------------------*/
    str_free1_dx(dsc_cmd);		/* Clear dsc_cmd		*/

    sts = get_cmdstring(uprompt,dsc_cmd);
    if (~sts & 1)
        return(sts);

		/*--------------------------------------------------------
		 * Unless otherwise directed:
		 *  1) insert symbols into line
		 *  2) if it's an indirect cmd procedure, open file and
		 *     take input from the file
		 *------------------------------------------------------*/
    if (!flag)
       {
        io = ctrl->ioLevel + ctrl->depth;
        mdsdcl_insert_symbols(dsc_cmd,io->ioParameter);

        p = nonblank(dsc_cmd->dscA_pointer);
        if (p && *p=='@')
           {
            if (ctrl->verify)
                displayCmdline(p);
            p++;			/* bump past the '@'		*/
            sts = openIndirectLevel(p);
            if (!sts & 1)
                return(sts);
            return(really_get_input(uprompt,dsc_cmd,len,flag));
           }
       }

    return(1);
   }



	/**************************************************************
	 * mdsdcl_get_input:
	 **************************************************************/
int   mdsdcl_get_input(			/* Return: status		*/
    void  *uprompt			/* <r> dsc or c-string		*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   )
   {
    int   sts;

    sts = really_get_input(uprompt,dsc_cmd,0,0);
    if (ctrl->verify)
        displayCmdline(dsc_cmd->dscA_pointer);
    return(sts);
   }



	/***************************************************************
	 * mdsdcl_get_input_nosymbols:
	 **************************************************************/
int   mdsdcl_get_input_nosymbols(	/* Return: status		*/
    void  *uprompt			/* <r> dsc or c-string		*/
   ,struct descriptor  *dsc_cmd		/* <w> new command line		*/
   )
   {
    return(really_get_input(uprompt,dsc_cmd,0,1));
   }



	/****************************************************************
	 * mdsdcl_close_indirect_all:
	 * Close all ioLevels except the base level
	 ****************************************************************/
int   mdsdcl_close_indirect_all()
   {
    for ( ; ctrl->depth ; )
        closeIndirectLevel();

    return(1);
   }
