#include        "tclsysdef.h"
#include        <ctype.h>
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_DELETE_NODE.C --
*
* TclDeleteNode:  Delete a node
*
* History:
*  15-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


#ifdef vms
#define TreeDeleteNodeExecute  TREE$DELETE_NODE_EXECUTE

int   TREE$DELETE_NODE_EXECUTE();
#endif

typedef struct _nidlist
   {
    int   nid;
    struct _nidlist  *next;
   } NidList;


static NidList *nid_list = 0;

	/***************************************************************
	 * InitTouchNodes:
	 ***************************************************************/
static void InitTouchNodes()
   {
    int nid = 0;
    while (TreeDeleteNodeGetNid(&nid) & 1)
       {
        NidList *new = malloc(sizeof(NidList));
        new->nid = nid;
        new->next = nid_list;
        nid_list = new;
       }
   }

	/***************************************************************
	 * TouchNodes:
	 ***************************************************************/
static void TouchNodes()
   {
    while (nid_list)
       {
        NidList *first = nid_list;
        TclNodeTouched(first->nid,delete);
        nid_list = first->next;
        free(first);
       }
   }



	/*****************************************************************
	 * TclDeleteNode:
	 * Delete a node
	 ****************************************************************/
int TclDeleteNode()
   {
    int   count = 0;
    int   nids = 0;
    int   reset = 1;
    int   nid;
    int   usageMask = -1;
    char  *nodename;
    char  *pathnam;
    void  *ctx;
    static DYNAMIC_DESCRIPTOR(dsc_nodnam);
    static DYNAMIC_DESCRIPTOR(dsc_out);

    while (cli_get_value("NODENAME",&dsc_nodnam) & 1)
       {
        ctx = 0;
        nodename = dsc_nodnam.dscA_pointer;
        while (TreeFindNodeWild(nodename,&nid,&ctx,usageMask) & 1)
           {
            nids++;
            TreeDeleteNodeInitialize(nid,&count,reset);
            reset = 0;
           }
        TreeFindNodeEnd(&ctx);
       }
    if (!count)
       {
        TclTextOut("No nodes found.");
        return 1;
       }
    if ((nids != count) && (cli_present("CONFIRM") & 1))
       {
        char  ans;
        static char  message[] = "Additional descendent and/or device\
 nodes will be deleted";
        static DYNAMIC_DESCRIPTOR(dsc_answer);
        static char  prompt[] = "Respond D-delete,L-list,Q-quit: ";

        TclTextOut(message);
        ans = 0;
        while (dsc_answer.dscW_length ? (ans!='D' && ans!='Q') : 1)
           {
            str_free1_dx(&dsc_answer);
            mdsdcl_get_input(prompt,&dsc_answer);
            ans = dsc_answer.dscW_length ?
                    toupper(*dsc_answer.dscA_pointer) : 0;
            if (ans == 'L')
               {
                nid = 0;
                while (TreeDeleteNodeGetNid(&nid) & 1)
                   {
                    pathnam = TreeGetPath(nid);
                    TclTextOut(pathnam);
                    TreeFree(pathnam);
                   }
               }
           }
        if (ans == 'Q')
            return 1;
       }
    if (cli_present("LOG") & 1)
       {
        nid = 0;
        while (TreeDeleteNodeGetNid(&nid) & 1)
           {
            pathnam = TreeGetPath(nid);
            str_concat(&dsc_out,"TCL-I-DELETE, deleted node ",pathnam);
            TclTextOut(dsc_out.dscA_pointer);
            TreeFree(pathnam);
            str_free1_dx(&dsc_out);
           }
       }
    InitTouchNodes();
    TreeDeleteNodeExecute();
    TouchNodes();
    return 1;
   }
