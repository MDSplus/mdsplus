#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>

/**********************************************************************
* TCL_DIRECTORY.C --
*
* TclDirectory:  Perform directory function.
*
* History:
*  24-Feb-1998  TRG  Add TreeFree() after calls to TreeFindNodeTags().
*  20-Jan-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/


extern char  *MdsDtypeString();
extern char  *MdsClassString();
#ifdef vms
extern int  sys$asctim();
#else
extern int LibSysAscTim();
#endif



	/****************************************************************
	 * MdsOwner:
	 ****************************************************************/
static char  *MdsOwner(		/* Return: ptr to "user" string		*/
    unsigned int   owner	/* <r> owner id				*/
   )
   {
    static char  ownerString[20];

    sprintf(ownerString,"[%o,%o]",owner>>16,owner&0xFFFF);
    return(ownerString);
   }



	/***************************************************************
	 * MdsDatime:
	 ***************************************************************/
static char  *MdsDatime(	/* Return: ptr to date+time string	*/
    int   time[]	/* <r> date/time to display: quadword	*/
   )
   {
    int   flags = 0;
    int   sts;
    short len;
    static char  datime[21];
    static DESCRIPTOR(dsc_datime,datime);

#ifdef vms
    sts = sys$asctim(&len,&dsc_datime,time,flags);
#else
    sts = LibSysAscTim(&len,&dsc_datime,time,flags);
#endif
    datime[len] = '\0';
    return(datime);
   }



	/****************************************************************
	 * TclDirectory:
	 * Perform directory function
	 ****************************************************************/
int   TclDirectory()
   {
    int   k;
    unsigned int   usage;
    char  *p;
    char  *pathnam;
    char  *reference;
    char  *tagnam;
    char  textLine[128];
    static char  fmtTotal[] = "Total of %d node%s.";
    static char  fmtGrandTotal[] = "Grand total of %d node%s.";
    static char fmtStates[] = "      Status: %s,parent is %s, usage %s%s%s";
    static char  fmtTime[] = "      Data inserted: %s    Owner: %s";
    static char  fmtDescriptor[] = "      Dtype: %-20s  Class: %-18s  Length: %d bytes";
    static char  fmtConglom1[] = "      Model element: %d";
    static char  fmtConglom2[] = "      Original element name: %s%s";
    static DYNAMIC_DESCRIPTOR(dsc_nodnam);
    static DYNAMIC_DESCRIPTOR(dsc_nodeList);
    static DYNAMIC_DESCRIPTOR(dsc_outline);

    static char partC[64+1];
    int nid;
    int status;
    void  *ctx = 0;
    void  *ctx2 = 0;
    int   found = 0;
    int   grand_found = 0;
    int   first_tag;
    int   full;
    static int   nciFlags;
    static int   nodnamLen;
    static int   retlen;
    static int   time[2];
    static unsigned int   owner;
    static char class;
    static char dtype;
    static int   dataLen;
    static unsigned short conglomerate_elt;
    static int   head_nid;
    static int   partlen;
    static NCI_ITM full_list[] = {
          {4,NciGET_FLAGS,&nciFlags,0}
         ,{8,NciTIME_INSERTED,time,0}
         ,{4,NciOWNER_ID,&owner,0}
         ,{1,NciCLASS,&class,0}
         ,{1,NciDTYPE,&dtype,0}
         ,{4,NciLENGTH,&dataLen,0}
         ,{2,NciCONGLOMERATE_ELT,&conglomerate_elt,0}
         ,{0,NciEND_OF_LIST,0,0}
         };

    static NCI_ITM cong_list[] = {
          {4,NciCONGLOMERATE_NIDS,&head_nid,&retlen}
         ,{64,NciORIGINAL_PART_NAME,partC,&partlen}
         ,{0,NciEND_OF_LIST,0,0}
         };
    int last_parent_nid = -1;
    static int parent_nid;
    static char nodnamC[12+1];
    static int relationship;
    int previous_relationship;
    static unsigned char nodeUsage;
    static NCI_ITM general_info_list[] = {
          {4,NciPARENT,&parent_nid,0}
         ,{12,NciNODE_NAME,nodnamC,&nodnamLen}
         ,{4,NciPARENT_RELATIONSHIP,&relationship,0}
         ,{1,NciUSAGE,&nodeUsage,0}
         ,{0,NciEND_OF_LIST,0,0}
         };
    static int elmnt;
    static DYNAMIC_DESCRIPTOR(dsc_allUsage);
    static DYNAMIC_DESCRIPTOR(dsc_usageStr);
    int usageMask = -1;
    static char *usages[] = { "any",
                            "structure",
                            "action",
                            "device",
                            "dispatch",
                            "numeric",
                            "signal",
                            "task",
                            "text",
                            "window",
                            "axis",
                            "subtree",
                            "compound data",
                            "unknown"};
#define MAX_USAGES   (sizeof(usages)/sizeof(usages[0]))


    parent_nid = 0;

    full = cli_present("FULL") & 1;
    if (cli_present("USAGE") & 1)
       {
        usageMask = 0;
        str_free1_dx(&dsc_allUsage);
        while (cli_get_value("USAGE",&dsc_usageStr) & 1)
           {
            str_concat(&dsc_allUsage,&dsc_allUsage,&dsc_usageStr,",",0);

            str_concat(&dsc_usageStr,"USAGE.",&dsc_usageStr,0);
            if (cli_get_value(&dsc_usageStr,&dsc_usageStr) & 1)
               {
                sscanf(dsc_usageStr.dscA_pointer,"%d",&usage);
                usageMask = usageMask | (1 << usage);
               }
            else
                MdsMsg(0,"Error getting usage id#",0);
           }
       }
    str_free1_dx(&dsc_outline);
    while (cli_get_value("NODE",&dsc_nodeList) & 1)
       {
        l2u(dsc_nodeList.dscA_pointer,0);
        for (elmnt=0; str_element(&dsc_nodnam,elmnt,',',&dsc_nodeList) & 1; elmnt++)
           {
            while ((status = TreeFindNodeWild(dsc_nodnam.dscA_pointer,&nid,&ctx,usageMask)) & 1)
               {
                grand_found++;
                status = TreeGetNci(nid,general_info_list);
                nodnamC[nodnamLen] = '\0';
                if (parent_nid != last_parent_nid)
                   {
                    if (found)
                       {
                        if (!full && dsc_outline.dscW_length)
			{
                            TclTextOut(dsc_outline.dscA_pointer);
                            str_free1_dx(&dsc_outline);
                        }
                        TclTextOut("  ");
                        sprintf(textLine,fmtTotal,found,(found>1)?"s":"");
                        TclTextOut(textLine);
                       }
                    TclTextOut("  ");
                    pathnam = TreeGetPath(parent_nid);
                    TclTextOut(pathnam);
                    TreeFree(pathnam);		/* free the string	*/
                    TclTextOut("  ");
                    found = 0;
                    last_parent_nid = parent_nid;
                    previous_relationship = relationship;
                   }
                found++;
                if (full)
                   {
                    if (previous_relationship != relationship)
                       {
                        TclTextOut("  ");
                        previous_relationship = relationship;
                       }
                    if (relationship == NciK_IS_CHILD)
                        str_concat(&dsc_outline,"  ",nodnamC,0);
                    else
                        str_concat(&dsc_outline," :",nodnamC,0);

                    ctx2 = 0;
                    first_tag = 1;
                    while (tagnam = TreeFindNodeTags(nid,&ctx2))
                       {
                        str_concat(&dsc_outline,&dsc_outline,
                                (first_tag?" tags: \\":",\\"),tagnam,0);
                        TreeFree(tagnam);
                        first_tag = 0;
                       }
                    TclTextOut(dsc_outline.dscA_pointer);
                    str_free1_dx(&dsc_outline);

                    status = TreeGetNci(nid,full_list);
                    if (status & 1)
                       {
                        k = (nodeUsage<MAX_USAGES) ?
                                         nodeUsage : (MAX_USAGES-1);
                        p = usages[k];
                        sprintf(textLine,fmtStates,
                            (nciFlags & NciM_STATE) ? "off" : "on",
                            (nciFlags & NciM_PARENT_STATE) ? "off" : "on",
                            p,
                            (nciFlags & NciM_WRITE_ONCE) ?
                                (dataLen?",readonly":",write-once") : "",
                            (nciFlags & NciM_ESSENTIAL) ? ",essential":"");
                        TclTextOut(textLine);

                        if (nciFlags & NciM_NO_WRITE_MODEL)
                            TclTextOut("      not writeable in model");
                        if (nciFlags & NciM_NO_WRITE_SHOT)
                            TclTextOut("      not writeable in pulse file");
                        if (nodeUsage == TreeUSAGE_SUBTREE)
                           {
                            sprintf(textLine,
                               "      subtree %sincluded in pulse file.",
                               (nciFlags & NciM_INCLUDE_IN_PULSE)?"":"NOT");
                            TclTextOut(textLine);
                           }

                        if (nciFlags & NciM_COMPRESSIBLE)
                           {
                            str_copy_dx(&dsc_outline,"compressible");
                            if (nciFlags & (NciM_COMPRESS_ON_PUT | NciM_DO_NOT_COMPRESS))
                                 str_append(&dsc_outline,",");
                           }
                        if (nciFlags & NciM_COMPRESS_ON_PUT)
                           {
                            str_append(&dsc_outline,"compress on put");
                            if (nciFlags & NciM_DO_NOT_COMPRESS)
                                str_append(&dsc_outline,",");
                           }
                        if (nciFlags & NciM_DO_NOT_COMPRESS)
                            str_append(&dsc_outline,"do not compress");
                        if (dsc_outline.dscW_length)
                           {
                            str_prefix(&dsc_outline,"      ");
                            TclTextOut(dsc_outline.dscA_pointer);
                            str_free1_dx(&dsc_outline);
                           }

                        switch (nciFlags & (NciM_PATH_REFERENCE | NciM_NID_REFERENCE))
                           {
                            case NciM_PATH_REFERENCE:
                                reference = " (paths only)";  break;
                            case NciM_NID_REFERENCE:
                                reference = " (node ids only)";  break;
                            case NciM_PATH_REFERENCE | NciM_NID_REFERENCE:
                                reference = " (paths and node ids)";  break;
                            default:
                                reference = 0;  break;
                           }
                        if (reference)
                           {
                            str_concat(&dsc_outline,
                              "      contains node references",reference,0);
                            TclTextOut(dsc_outline.dscA_pointer);
                           }

                        sprintf(textLine,fmtTime,
                            MdsDatime(time),MdsOwner(owner));
                        TclTextOut(textLine);
                        if (dataLen)
                           {
                            sprintf(textLine,fmtDescriptor,
                                MdsDtypeString((int)dtype),
                                MdsClassString((int)class),dataLen);
                            TclTextOut(textLine);
                           }
                        else
                            TclTextOut("      There is no data stored for this node");
                        if (conglomerate_elt != 0)
                           {
                            sprintf(textLine,fmtConglom1,conglomerate_elt);
                            TclTextOut(textLine);

                            TreeGetNci(nid,cong_list);
                            if (partlen)
                               {
                                pathnam = TreeGetPath(head_nid);
                                partC[partlen] = 0;
                                sprintf(textLine,fmtConglom2,pathnam,partC);
                                TclTextOut(textLine);
                                TreeFree(pathnam);
                               }
                           }
                       }
#ifdef vms
                    else
                      lib$signal(status,0);
#endif
                   }
                else
                   {
                    if (previous_relationship != relationship)
                       {
                        TclTextOut(dsc_outline.dscA_pointer);
                        str_free1_dx(&dsc_outline);
                        TclTextOut("  ");
                        previous_relationship = relationship;
                       }
                    if (relationship == NciK_IS_CHILD)
                        str_concat(&dsc_outline,
                            &dsc_outline,"  ",nodnamC,0);
                    else
                        str_concat(&dsc_outline,
                            &dsc_outline," :",nodnamC,0);
                    if (dsc_outline.dscW_length > 60)
                       {
                        TclTextOut(dsc_outline.dscA_pointer);
                        str_free1_dx(&dsc_outline);
                       }
                   }
               }
            TreeFindNodeEnd(&ctx);
           }
       }
    if (found)
       {
        if (!full)
           {
            if (dsc_outline.dscW_length)
	    {
              TclTextOut(dsc_outline.dscA_pointer);
              str_free1_dx(&dsc_outline);
            }
           }
        TclTextOut("  ");
        sprintf(textLine,fmtTotal,found,(found>1)?"s":"");
        TclTextOut(textLine);
       }
    if (grand_found)
       {
        if (found != grand_found)
           {
            TclTextOut("  ");
            sprintf(textLine,fmtGrandTotal,grand_found,
                    (grand_found>1)?"s":"");
            TclTextOut(textLine);
           }
       }
#ifdef vms
    else
        lib$signal(status,0);
#endif

    return ((status == TreeNMN) || (status == TreeNNF)) ? 1 : status;
   }
