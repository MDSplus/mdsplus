#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>


#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <strroutines.h>
#include <dcl.h>

#include "tcl_p.h"

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
EXPORT int TclPutExpression(void *ctx, char **error, char **output __attribute__ ((unused)), char *(*getline)(), void *getlineinfo)
{
  char *nodnam = 0;
  char *ascValue = 0;
  struct descriptor dsc_ascValue = {0, DTYPE_T, CLASS_S, 0};
  struct descriptor_xd value_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int sts;
  int nid;

  cli_get_value(ctx, "NODE", &nodnam);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1) {
    if (cli_present(ctx, "EXTENDED") & 1) {
      char *eof=0;
      int use_lf = cli_present(ctx, "LF") & 1;
      char *line = 0;
      ascValue=strdup("");
      cli_get_value(ctx, "EOF", &eof);
      while ((line = (getline ? getline(getlineinfo) : readline("PUT> ")))
	     && (strlen(line)) > 0) {
	if (getline == NULL)
	  add_history(line);
	if ((eof && (strcasecmp(line,eof)==0)) || (strlen(line)==0)) {
	  free(line);
	  line=0;
	  break;
	}
	tclAppend(&ascValue, line);
	free(line);
	line=0;
	if (use_lf)
	  tclAppend(&ascValue, "\n");
      }
      if (line)
	free(line);
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
  if (nodnam)
    free(nodnam);
  MdsFree1Dx(&value_xd, NULL);
  if (~sts & 1) {
    char *msg = MdsGetMsg(sts);
    *error = malloc(strlen(msg)+100);
    sprintf(*error,"Error: Problem putting expression in node\n"
	    "Error message was: %s\n",msg);
  }
  return sts;
}
