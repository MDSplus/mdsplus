#include	<config.h>
#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_SET_NODE.C --
*
* TclSetNode:  Set node in mdsPlus tree.
*
* History:
*  15-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/



	/****************************************************************
	 * TclSetNode:
	 ****************************************************************/
int TclSetNode()
   {
    int   nid;
    int   status;
    int   log;
    int   usageMask;
    void  *ctx = 0;
    char  *nodename;
    static DYNAMIC_DESCRIPTOR(dsc_nodename);
    static DYNAMIC_DESCRIPTOR(dsc_status);
    cli_get_value("NODENAME",&dsc_nodename);
    cli_get_value("STATUS",&dsc_status);
    nodename = dsc_nodename.dscA_pointer;
    l2u(nodename,0);
    log = cli_present("LOG") & 1;

    usageMask = -1;
    while ((status = TreeFindNodeWild(nodename,&nid,&ctx,usageMask)) & 1)
       {
	 if (dsc_status.dscA_pointer)
	 {
           int statval = atoi(dsc_status.dscA_pointer);
           NCI_ITM setnci[] = {{sizeof(int), NciSTATUS, 0, 0},{0,NciEND_OF_LIST,0,0}};
           setnci[0].pointer = (unsigned char *)&statval;
           TreeSetNci(nid,setnci);
         }
	 switch (cli_present("SUBTREE"))
           {
            case CLI_STS_PRESENT:
              status = TreeSetSubtree(nid);
              break;
            case CLI_STS_NEGATED:
              status = TreeSetNoSubtree(nid);
              break;
           }
        if (!(status & 1)) goto error;
        if (cli_present("ON") & 1)
           {
            status = TreeTurnOn(nid);
            if (status & 1)
                TclNodeTouched(nid,on_off);
            else
                TclErrorOut(status);
           }
        else if (cli_present("OFF") & 1)
           {
            status = TreeTurnOff(nid);
            if (status & 1)
                TclNodeTouched(nid,on_off);
            else
                TclErrorOut(status);
           }
        if (!(status & 1)) goto error;
           {
            int set_flags;
            int clear_flags;
            DYNAMIC_DESCRIPTOR(dsc_path);
            NCI_ITM get_itmlst[] =  { {0,NciPATH,(unsigned char *) &dsc_path,0}, {0,NciEND_OF_LIST}};
            NCI_ITM set_itmlst[] =  { {0,NciSET_FLAGS,(unsigned char *) &set_flags,0}, {0,NciEND_OF_LIST}};
            NCI_ITM clear_itmlst[] =  { {0,NciCLEAR_FLAGS,(unsigned char *) &clear_flags,0}, {0,NciEND_OF_LIST}};
            set_flags = 0;
            clear_flags = 0;
            switch (cli_present("WRITE_ONCE"))
               {
                case CLI_STS_PRESENT:
                  set_flags |= NciM_WRITE_ONCE;
                  break;
                case CLI_STS_NEGATED:
                  clear_flags |= NciM_WRITE_ONCE;
                  break;
               }
            switch (cli_present("CACHED"))
               {
                case CLI_STS_PRESENT:
                  set_flags |= NciM_CACHED;
                  break;
                case CLI_STS_NEGATED:
                  clear_flags |= NciM_CACHED;
                  break;
               }
            switch (cli_present("COMPRESS_ON_PUT"))
               {
                case CLI_STS_PRESENT:
                  set_flags |= NciM_COMPRESS_ON_PUT;
                  break;
                case CLI_STS_NEGATED:
                  clear_flags |= NciM_COMPRESS_ON_PUT;
                  break;
               }
            switch (cli_present("COMPRESS_SEGMENTS"))
               {
                  case CLI_STS_PRESENT:
                    set_flags |= NciM_COMPRESS_SEGMENTS;
                    break;
                  case CLI_STS_NEGATED:
                    clear_flags |= NciM_COMPRESS_SEGMENTS;
                    break;
               }
            switch (cli_present("DO_NOT_COMPRESS"))
               {
                case CLI_STS_PRESENT:
                  set_flags |= NciM_DO_NOT_COMPRESS;
                  break;
                case CLI_STS_NEGATED:
                  clear_flags |= NciM_DO_NOT_COMPRESS;
                  break;
               }
            switch (cli_present("SHOT_WRITE"))
               {
                case CLI_STS_PRESENT:
                  clear_flags |= NciM_NO_WRITE_SHOT;
                  break;
                case CLI_STS_NEGATED:
                  set_flags |= NciM_NO_WRITE_SHOT;
                  break;
               }
            switch (cli_present("MODEL_WRITE"))
               {
                case CLI_STS_PRESENT:
                  clear_flags |= NciM_NO_WRITE_MODEL;
                  break;
                case CLI_STS_NEGATED:
                  set_flags |= NciM_NO_WRITE_MODEL;
                  break;
               }
            switch (cli_present("INCLUDED"))
               {
                case CLI_STS_PRESENT:
                  set_flags |= NciM_INCLUDE_IN_PULSE;
                  break;
                case CLI_STS_NEGATED:
                  clear_flags |= NciM_INCLUDE_IN_PULSE;
                  break;
               }
            switch (cli_present("ESSENTIAL"))
               {
                case CLI_STS_PRESENT:
                  set_flags |= NciM_ESSENTIAL;
                  break;
                case CLI_STS_NEGATED:
                  clear_flags |= NciM_ESSENTIAL;
                  break;
               }
            if (set_flags) status = TreeSetNci(nid,set_itmlst);
            if (clear_flags) status = TreeSetNci(nid,clear_itmlst);
            if (status & 1)
               {
                if (log)
                   {
                    TreeGetNci(nid,get_itmlst);
                    str_concat(&dsc_path,"Node: ",&dsc_path," modified",0);
                    TclTextOut(dsc_path.dscA_pointer);
                    str_free1_dx(&dsc_path);
                   }
               }
            else
                goto error;
           }
       }
    TreeFindNodeEnd(&ctx);
    if (status == TreeNNF) goto error;
    return 1;
error:
#ifdef vms
    lib$signal(status,0);
#else
    MdsMsg(status,0);
#endif
    return status;
   }
