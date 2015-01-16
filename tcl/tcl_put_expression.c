#include        "tclsysdef.h"
#include <strroutines.h>
#include <string.h>

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
int TclPutExpression(void *ctx)
{
  char *nodnam = 0;
  char *ascValue = 0;
  struct descriptor dsc_ascValue = {0, DTYPE_T, CLASS_S, 0};
  static int val;
  struct descriptor_xd value_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int sts;
  int nid;

  cli_get_value(ctx, "NODE", &nodnam);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1) {
    if (cli_present(ctx, "EXTENDED") & 1) {
      char *val_part=0;
      char *eof=0;
      int use_lf = cli_present(ctx, "LF") & 1;
      int symbols = cli_present(ctx, "SYMBOLS") & 1;
      cli_get_value(ctx, "EOF", &eof);
      printf("deal with extended input\n");
      /*
      while ((mdsdcl_get_input_nosymbols("PUT> ", &val_part) & 1)) {
	if (eof && val_part) {
	  if (!strcmp(eof, val_part))
	    break;
	} else if (val_part==NULL)
	  break;
	if (use_lf) {
	  static DESCRIPTOR(lf, "\n");
	  StrConcat(&dsc_ascValue, &dsc_ascValue, &val_part, &lf, 0);
	} else
	  StrAppend(&dsc_ascValue, &val_part);
      }
      str_free1_dx(&val_part);
      */
    } else
      cli_get_value(ctx, "VALUE", &ascValue);
    dsc_ascValue.length=strlen(ascValue);
    dsc_ascValue.pointer=ascValue;
    sts = TdiCompile(&dsc_ascValue, &value_xd MDS_END_ARG);
    if (sts & 1) {
      if (!value_xd.l_length)
	value_xd.dtype = DTYPE_DSC;
      sts = TreePutRecord(nid, (struct descriptor *)&value_xd, 0);
    }
  }
  if (ascValue)
    free(ascValue);
  MdsFree1Dx(&value_xd, NULL);
  if (~sts & 1)
    MdsMsg(sts, "TclPutExpression: error processing command");
  return sts;
}
