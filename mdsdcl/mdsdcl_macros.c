#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <lib$routines.h>
#include        <ssdef.h>
#include        <time.h>
#else
#include        <sys/time.h>
#endif

/**********************************************************************
* MDSDCL_MACROS.C --
*
* Macro-related routines ...
*   mdsdcl_define:  Define a macro
*   mdsdcl_do_macro:  Execute a macro
*   mdsdcl_show_macro:  Display any or all macros
*
* History:
*  07-Jan-1998  TRG  mdsdcl_do_macro: new function.
*  30-Dec-1997  TRG  Create.
*
***********************************************************************/


extern struct _mdsdcl_ctrl  MDSDCL_COMMON;
static struct _mdsdcl_ctrl *ctrl = &MDSDCL_COMMON;

#define INCR           32	/* allocate in chunks			*/
#define MIN_LINES      4	/* minimum num lines allocated		*/


	/***************************************************************
	 * find_macro:
	 * Find the specified _mdsdcl_macro struct ...
	 ***************************************************************/
static struct _mdsdcl_macro  *find_macro( /* Return: addr of struct	*/
    char  name[]			/* <r> macro name, upper case	*/
   )
   {
    int   i,k;
    struct _mdsdcl_macro  *m;

    m = ctrl->macro.list;
    for (i=0 ; i<ctrl->macro.numMacros ; i++)
        if (!strcmp(name,m[i].name))  break;

    return((i<ctrl->macro.numMacros) ? m+i : 0);
   }



	/*****************************************************************
	 * expand_macro_list:
	 * Add room for more macro definitions ...
	 *****************************************************************/
static void  expand_macro_list()	/* Return: void			*/
   {
    int   i,k;
    int   nbytes;

    nbytes = ctrl->macro.maxMacros * sizeof(struct _mdsdcl_macro) + INCR;
    if (ctrl->macro.list)
        ctrl->macro.list = realloc(ctrl->macro.list,nbytes);
    else
        ctrl->macro.list = malloc(nbytes);

    ctrl->macro.maxMacros = nbytes/sizeof(struct _mdsdcl_macro);
    return;
   }



	/*****************************************************************
	 * get_macro:
	 * Like find_macro, but create new struct if necessary ...
	 *****************************************************************/
static struct _mdsdcl_macro  *get_macro( /* Return: addr of struct	*/
    char  name[]			/* <r> macro name, upper case	*/
   )
   {
    int   i,k;
    int   nbytes;
    struct _mdsdcl_macro  *m;

    if (m = find_macro(name))
        return(m);			/*--------------------> return	*/

    if (ctrl->macro.numMacros >= ctrl->macro.maxMacros)
        expand_macro_list();

		/*=======================================================
		 * Add new macro to macro.list[]
		 *======================================================*/
    m = ctrl->macro.list + ctrl->macro.numMacros++;
    m->name = STRING_ALLOC(name);
    m->lines = (char **)malloc(INCR);
    m->maxLines = INCR / sizeof(char *);
    m->numLines = 0;

    return(m);
   }



	/***************************************************************
	 * extend_macro:
	 * Add more lines to a macro ...
	 ***************************************************************/
static void  extend_macro(	/* Return:  void			*/
    struct _mdsdcl_macro  *m	/* <m> macro to be extended		*/
   )
   {
    int   i,k;
    int   nbytes;

    if (m->numLines != m->maxLines)
        fprintf(stderr,"\n*WARN* from extend_macro()\n\n");

    nbytes = m->maxLines * sizeof(char *) + INCR;
    m->lines = (char **)realloc(m->lines,nbytes);
    m->maxLines = nbytes / sizeof(char *);
    return;
   }



	/***************************************************************
	 * end_macro:
	 * Return TRUE if line consists of words "END MACRO"
	 ***************************************************************/
static int   end_macro(		/* Return: TRUE if "END MACRO"		*/
    char  originalLine[]	/* <r> line to check			*/
   )
   {
    char  *p;
    char  line[40];

    p = nonblank(originalLine);
    l2un(line,p,sizeof(line));
    strntrim(line,0,sizeof(line));
    return(!strcmp(line,"END MACRO") ? 1 : 0);
   }



	/****************************************************************
	 * mdsdcl_define:
	 * Define an mds macro ...
	 ****************************************************************/
int   mdsdcl_define()			/* Return: status		*/
   {
    int   i,k;
    int   sts;
    char  name[32];
    char  *p;
    struct _mdsdcl_macro  *m;
    static DYNAMIC_DESCRIPTOR(dsc_name);
    static DYNAMIC_DESCRIPTOR(dsc_cmd);
    static char  fmt1[] = "Macro '%s' is open -- cannot be editted";

    sts = cli_get_value("P1",&dsc_name);
    if (~sts & 1)
        return(sts);

    k = dsc_name.dscW_length;
    if (k >= sizeof(name))
        k = sizeof(name) - 1;
    l2un(name,dsc_name.dscA_pointer,k);
    name[k] = '\0';

		/*=======================================================
		 * Get addr of macro struct.  Delete any existing lines ...
		 *======================================================*/
    m = get_macro(name);
    if (m->isOpen)
        return(MdsMsg(MDSDCL_STS_ERROR,fmt1,m->name));

    for (i=0 ; i<m->numLines ; i++)
        free(m->lines[i]);
    m->numLines = 0;

		/*======================================================
		 * Read new macro from input ...
		 *=====================================================*/
    for ( ; mdsdcl_get_input_nosymbols("DEFMAC> ",&dsc_cmd) & 1 ; )
       {
        p = nonblank(dsc_cmd.dscA_pointer);
        if (!p || end_macro(p))
            break;

        if (m->numLines >= m->maxLines)
            extend_macro(m);

        m->lines[m->numLines++] = STRING_ALLOC(dsc_cmd.dscA_pointer);
       }
    return(1);
   }



	/*****************************************************************
	 * display_macro:
	 *****************************************************************/
static void  display_macro(		/* Return: void			*/
    struct _mdsdcl_macro  *m		/* <r> ptr to macro struct	*/
   )
   {
    int  i,k;
    char **p;

    printf("\n%s\n",m->name);
    p = m->lines;
    for (i=0 ; i<m->numLines ; i++)
        printf("  %s\n",p[i]);
    return;
   }



	/*****************************************************************
	 * mdsdcl_show_macro:
	 *****************************************************************/
int   mdsdcl_show_macro()		/* Return: status		*/
   {
    int   i,k;
    int   full;
    int   sts;
    struct _mdsdcl_macro  *m;
    static DYNAMIC_DESCRIPTOR(dsc_name);

    sts = cli_get_value("MACRO",&dsc_name);
    if (sts & 1)
       {
        l2u(dsc_name.dscA_pointer,0);
        m = find_macro(dsc_name.dscA_pointer);
        if (!m)
           {
            fprintf(stderr,"  --> macro '%s' not defined\n",
                dsc_name.dscA_pointer);
            return(1);
           }
        display_macro(m);
        return(1);
       }
		/*=======================================================
		 * List all defined macros ...
		 *======================================================*/
    full = cli_present("FULL") & 1;
    m = ctrl->macro.list;
    if (!m)
       {
        fprintf(stderr,"  --> No macros defined\n\n");
        return(1);
       }
    for (i=0 ; i<ctrl->macro.numMacros ; i++,m++)
       {
        if (full)
            display_macro(m);
        else
            printf("  %s\n",m->name);
       }
    return(1);
   }



	/****************************************************************
	 * mdsdcl_do_macro:
	 ****************************************************************/
int   mdsdcl_do_macro()		/* Return: status			*/
   {
    int   i,k;
    int   icnt;
    int   irepeat;
    int   sts;
    char  prmName[4];
    struct _mdsdcl_io  *io;
    struct _mdsdcl_macro  *macro;
    static DYNAMIC_DESCRIPTOR(dsc_util);
    static char  fmt0[] = "Qualifier not supported:  use '@'";
    static char  fmt1[] = "No such command: '%s'";
    static char  fmt2[] = "Macro '%s' is already in use";
    static char  fmt3[] = "Exceeded MAX_DEPTH for indirect files & macros";

    sts = cli_get_value("NAME",&dsc_util);
    if (~sts & 1)
        return(MdsMsg(MDSDCL_STS_ERROR,"Error getting NAME"));

    if (cli_present("INTERACTIVE") & 1)
        return(MdsMsg(MDSDCL_STS_ERROR,fmt0,0));

    l2u(dsc_util.dscA_pointer,0);		/* convert to uc	*/
    macro = find_macro(dsc_util.dscA_pointer);
    if (!macro)
       {
        MdsMsg(0,"No such command");
        return(MDSDCL_STS_ERROR);
       }

    if (macro->isOpen)
        return(MdsMsg(MDSDCL_STS_ERROR,fmt2,dsc_util.dscA_pointer));

		/*=======================================================
		 * Get repeat count ...
		 *======================================================*/
    sts = cli_get_value("REPEAT",&dsc_util);
    if (sts & 1)
       {
        if (sscanf(dsc_util.dscA_pointer,"%d",&irepeat) != 1)
            irepeat = 1;
       }
    else
        irepeat = 1;

		/*========================================================
		 * Create new ioLevel[] in ctrl struct ...
		 *=======================================================*/
    if (ctrl->depth >= MAX_DEPTH)
        return(MdsMsg(MDSDCL_STS_INDIRECT_ERROR,fmt3,0));

    for (icnt=0 ; icnt<irepeat ; icnt++)
       {
        macro->isOpen = 1;
        io = ctrl->ioLevel + ++ctrl->depth;
        io->fp = 0;
        io->ioMacro = macro;
        io->lineno = 0;

        for (i=0 ; i<8 ; i++)
           {
            sprintf(prmName,"P%d",i+1);
            sts = cli_get_value(prmName,io->ioParameter+i);
            if (~sts & 1)
                break;
           }
        for ( ; i<8 ; i++)
            str_free1_dx(io->ioParameter+i);

        do {
            sts = mdsdcl_do_command(0);
           }  while (sts & 1);

        if (sts == MDSDCL_STS_INDIRECT_EOF)
            continue;
        else
            break;
       }

    return((sts==MDSDCL_STS_INDIRECT_EOF) ? 1 : sts);
   }
