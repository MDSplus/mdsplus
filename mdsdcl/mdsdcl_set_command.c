#include        <stdio.h>
#include        "mdsdcl.h"
#ifdef vms
#include        <ssdef.h>
#include        <stdarg.h>
#include        <lib$routines.h>
#else
#include        <dlfcn.h>
#include        <stdlib.h>
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



#ifdef unix
	/****************************************************************
	 * dlopen_table:
	 * Find "table" shared object ...
	 ****************************************************************/
static void  *dlopen_table(	/* Return: shared-object handle		*/
    char  *tableName		/* <r> table name			*/
   )
   {
    int   i,k;
    char  *p;
    char  utilString[80];
    void  *hndl;
    FILE  *fp;
    static DYNAMIC_DESCRIPTOR(dsc_filename);

		/*=======================================================
		 * First, check for environment variable (upper case)
		 *======================================================*/
    l2u(utilString,tableName);
    if (p = getenv(utilString))
       {
        strcpy(utilString,p);
        hndl = dlopen(utilString,RTLD_LAZY);
        if (hndl)
            return(hndl);		/*--------------------> return	*/
       }

		/*======================================================
		 * Next, try local directory ...
		 *=====================================================*/
    sprintf(utilString,"ls -r ./%s.so*  2> /dev/null",tableName);
    if (fp = popen(utilString,"r"))
       {
        hndl = 0;
        if (p = fgets(utilString,sizeof(utilString),fp))
           {
            if (ascFilename(&p,&dsc_filename,0))
                hndl = dlopen(dsc_filename.dscA_pointer,RTLD_LAZY);
            str_free1_dx(&dsc_filename);
           }
        pclose(fp);			/* close the pipe		*/
        if (hndl)
            return(hndl);		/*--------------------> return	*/
       }

		/*=======================================================
		 * Finally, try $MDSPLUS/shlib ...
		 *======================================================*/
    p = getenv("MDSPLUS");
    if (!p)
        p = "$MDSPLUS";
    sprintf(utilString,"ls -r %s/shlib/%s.so*  2> /dev/null",
        p,tableName);
    if (fp = popen(utilString,"r"))
       {
        hndl = 0;
        if (p = fgets(utilString,sizeof(utilString),fp))
           {
            if (ascFilename(&p,&dsc_filename,0))
                hndl = dlopen(dsc_filename.dscA_pointer,RTLD_LAZY);
            str_free1_dx(&dsc_filename);
           }
        pclose(fp);			/* close the pipe		*/
        if (hndl)
            return(hndl);		/*--------------------> return	*/
       }

    return(0);				/*--------------------> return	*/
   }
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
#ifdef vms
        sts = lib$find_image_symbol(&dsc_table,&dsc_table,&newTable);
        if (~sts & 1)
           {			/* Maybe try appending "_commands" ?	*/
            if (dsc_table.dscW_length < 10)
               {
                str_concat(&dsc_table,&dsc_table,"_commands",0);
                sts = lib$find_image_symbol(&dsc_table,&dsc_table,
                                &newTable);
               }
            if (~sts & 1)
               {
                dasmsg(sts,"set_command: *ERR* from find_image_symbol");
                return(sts);
               }
           }
#else
        hndl = dlopen_table(dsc_table.dscA_pointer);
        if (!hndl)
           {			/* Maybe try appending "_commands" ?	*/
            if (dsc_table.dscW_length < 10)
               {
                str_concat(&dsc_table,&dsc_table,"_commands",0);
                hndl = dlopen_table(dsc_table.dscA_pointer);
               }
            if (!hndl)
               {
			/*-----------------------------------------------
			 * No luck.  Print error message ...
			 *----------------------------------------------*/
                mdsMsg(0,"Failed to open table '%s'",
                    dsc_table.dscA_pointer);
                fprintf(stderr,"\n");
                return(MDSDCL_STS_ERROR);
               }
           }

        newTable = dlsym(hndl,dsc_table.dscA_pointer);
        if (!newTable)
           {
            fprintf(stderr,"*ERR* table %s not found\n",
                dsc_table.dscA_pointer);
            return(MDSDCL_STS_ERROR);
           }
#endif

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
