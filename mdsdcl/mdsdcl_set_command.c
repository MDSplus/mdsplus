#include        <stdio.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <ssdef.h>
#include        <stdarg.h>
#include        <lib$routines.h>
#else
#include        <stdlib.h>
#include        <sys/stat.h>
#endif

/***********************************************************************
* MDSDCL_SET_COMMAND.C --
*
* Include new command table in mdsdcl.
*
* History:
*  13-Feb-1998  TRG  Allow user to specify only the facility (e.g. "tcl"),
*                     rather than the entire name "tcl_commands".
*  16-Jan-1998  TRG  Create.  Ported from orginal mds code.
*
************************************************************************/


#ifdef vms
#define LibFindImageSymbol  lib$find_image_symbol
#else
int  LibFindImageSymbol();
#endif


	/****************************************************************
	 * mdsdcl_set_command:
	 ****************************************************************/
int   mdsdcl_set_command(		/* Return: status		*/
    struct _mdsdcl_ctrl  *ctrl		/* <m> the control structure	*/
   )
   {
    int   i,k;
    int   sts;
    void  *hndl;
    void  *newTable;
    static DYNAMIC_DESCRIPTOR(dsc_table);

		/*------------------------------------------------------
		 * Get tablename and find its address in shared library ...
		 *-----------------------------------------------------*/
    sts = cli_get_value("TABLE",&dsc_table);
    if (sts & 1)
       {
        sts = LibFindImageSymbol(&dsc_table,&dsc_table,&newTable);
        if ((~sts & 1) && (dsc_table.dscW_length < 10))
           {
            str_concat(&dsc_table,&dsc_table,"_commands",0);
            sts = LibFindImageSymbol(&dsc_table,&dsc_table,&newTable);
           }
        if (~sts & 1)
            return(MdsMsg(sts,"Failed to open table %s",
                dsc_table.dscA_pointer));

		/*------------------------------------------------------
		 *... add newTable address to "tbladr[]" list
		 *-----------------------------------------------------*/
        for (i=0 ; i<ctrl->tables ; i++)
            if (newTable == ctrl->tbladr[i])  break;
        if (i == ctrl->tables)
           {
            if (ctrl->tables >= MAX_TABLES)
               {
                fprintf(stderr,"set_command: *WARN* Max_tables exceeded\n");
                return(0);
               }
            ctrl->tbladr[ctrl->tables++] = newTable;
           }
       }

		/*------------------------------------------------------
		 * Check for other qualifiers ...
		 *-----------------------------------------------------*/
    if (cli_present("HELPLIB") & 1)
        cli_get_value("HELPLIB",&ctrl->helplib);

    if (cli_present("PROMPT") & 1)
        cli_get_value("PROMPT",&ctrl->prompt);

    if (cli_present("DEF_FILE") & 1)
        cli_get_value("DEF_FILE",&ctrl->def_file);

    return(1);
   }
