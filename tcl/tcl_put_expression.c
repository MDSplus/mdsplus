#include        "tclsysdef.h"

/***********************************************************************
* TCL_PUT_EXPRESSION.C --
*
* Put an INTEGER (?) expression into a node.
*
* History:
*  04-Jun-1998  TRG  Create.  Ported from original MDSplus code.
*
************************************************************************/

extern int TdiCompile();



	/***************************************************************
	 * TclPutExpression:
	 * Put an INTEGER (?) expression into a node.
	 ***************************************************************/
int TclPutExpression()
   {
    static DYNAMIC_DESCRIPTOR(dsc_nodnam);
    static DYNAMIC_DESCRIPTOR(dsc_ascValue);
    static int val;
    static struct descriptor_xd value_xd = {0,DTYPE_DSC,CLASS_XD,0,0};
    int sts;
    int nid;
    int ctx = 0;

    cli_get_value("NODE",&dsc_nodnam);
    sts = TreeFindNode(dsc_nodnam.dscA_pointer,&nid);
    if (sts & 1)
       {
        if (cli_present("EXTENDED") & 1)
           {
            static DYNAMIC_DESCRIPTOR(val_part);
            static DYNAMIC_DESCRIPTOR(dsc_eof);
            int use_lf = cli_present("LF") & 1;
            int symbols = cli_present("SYMBOLS") & 1;

            str_free1_dx(&dsc_ascValue);
            str_free1_dx(&dsc_eof);
            cli_get_value("EOF",&dsc_eof);
            while ((mdsdcl_get_input_nosymbols("PUT> ",&val_part) & 1))
               {
                if (dsc_eof.dscA_pointer && val_part.dscA_pointer)
                   {
                    if (!strcmp(dsc_eof.dscA_pointer,val_part.dscA_pointer))
                        break;
                   }
                else if (!val_part.dscW_length)
                    break;
                if (use_lf) {
                    static DESCRIPTOR(lf,"\n");
                    StrConcat(&dsc_ascValue,&dsc_ascValue,&val_part,&lf,0);
                }
                else
                    StrAppend(&dsc_ascValue,&val_part);
               }
            str_free1_dx(&val_part);
           }
        else
            cli_get_value("VALUE",&dsc_ascValue);
        sts = TdiCompile(&dsc_ascValue,&value_xd MDS_END_ARG);
        if (sts & 1)
           {
            if (!value_xd.dscL_l_length) value_xd.dscB_dtype = DTYPE_DSC;
            sts = TreePutRecord(nid,(struct descriptor *)&value_xd,0);
           }
       }
    str_free1_dx(&dsc_ascValue);
    MdsFree1Dx(&value_xd,NULL);
    if (~sts & 1)
        MdsMsg(sts,"TclPutExpression: error processing command");
    return sts;
   }
