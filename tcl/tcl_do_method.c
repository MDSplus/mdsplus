#include        "tclsysdef.h"
#include        <libroutines.h>
#include        <mds_stdarg.h>
#include        <mdsshr.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_DO_METHOD.C --
*
* TclDoMethod:  Do something.
*
* History:
*  05-Mar-1998  TRG  Create.  Ported from original MDSplus code.
*
************************************************************************/


#ifdef vms
#define TdiExecute  TDI$EXECUTE
#define TdiCompile  TDI$COMPILE
#define TreeDoMethod  TREE$DO_METHOD
extern int   TREE$DO_METHOD();
#endif


extern int   TdiExecute();
extern int   TdiCompile();



	/****************************************************************
	 * TclDoMethod:
	 ****************************************************************/
int TclDoMethod()
   {
    int   i;
    int   argc;
    int   sts;
    unsigned char  do_it;
    struct descriptor_xd xdarg[255];
    static int   nid;
    static unsigned short boolVal;
    static struct descriptor_s bool_dsc =
                    {sizeof(boolVal), DTYPE_W, CLASS_S, (char *)&boolVal};
    static struct descriptor nid_dsc = {4,DTYPE_NID,CLASS_S,(char *)&nid};
    static struct descriptor_xd empty_xd = {0,DTYPE_DSC,CLASS_XD,0,0};
    static DYNAMIC_DESCRIPTOR(arg);
    static DYNAMIC_DESCRIPTOR(if_clause);
    static DYNAMIC_DESCRIPTOR(method);
    static DYNAMIC_DESCRIPTOR(object);
    static void  *arglist[256] = {(void *)2,&nid_dsc,&method};

    cli_get_value("OBJECT",&object);
    sts = TreeFindNode(object.dscA_pointer,&nid);
    if (sts & 1)
       {
        do_it = (TreeIsOn(nid) | cli_present("OVERRIDE")) & 1;
        if (cli_present("IF") & 1)
           {
            cli_get_value("IF",&if_clause);
            sts = TdiExecute(&if_clause,&bool_dsc MDS_END_ARG);
            if (sts & 1)
                do_it = do_it && boolVal;
            else
                do_it = 0;
           }
        if (do_it)
           {
            cli_get_value("METHOD",&method);
            argc = 0;
            if (cli_present("ARGUMENT") & 1)
               {
                while (cli_get_value("ARGUMENT",&arg) & 1)
                   {
                    xdarg[argc] = empty_xd;
                    sts = TdiCompile(&arg,&xdarg[argc] MDS_END_ARG);
                    if (sts & 1)
                       {
                        arglist[argc + 3] = xdarg[argc].dscA_pointer;
                        argc++;
                       }
                    else
                        break;
                   }
               }
            if (sts & 1)
               {
                arglist[0] = (void *)(argc + 2);
                sts = LibCallg(arglist,TreeDoMethod);
               }
            str_free1_dx(&arg);
            str_free1_dx(&method);
            for (i = 0; i < argc; i++)
                MdsFree1Dx(&xdarg[i],NULL);
           }
       }
    str_free1_dx(&object);
#ifdef vms
    if (!(sts & 1)) lib$signal(sts,0);
#endif
    return sts;
   }
