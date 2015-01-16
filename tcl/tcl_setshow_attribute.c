#include        "tclsysdef.h"
#include        <mdsshr.h>
#include <strroutines.h>
#include <alloca.h>
#include <string.h>
extern int TdiDecompile();
extern int TdiSortVal();

/**********************************************************************
* TCL_SHOW_ATTRIBUTE.C --
*
* TclShowAttribute:  Display node attributes.
*
*
************************************************************************/

	/***************************************************************
	 * TclShowAttribute:
	 ***************************************************************/
int TclShowAttribute(void *ctx)
{
  int status;
  int nid;
  EMPTYXD(xd);
  char *node=0;
  char *attr=0;
  struct descriptor dsc_string = {0, DTYPE_T, CLASS_D, 0};
  cli_get_value(ctx, "NODE", &node);
  status = TreeFindNode(node, &nid);
  if (status & 1) {
    status = cli_get_value(ctx, "NAME", &attr);
    if (status & 1) {
      status = TreeGetXNci(nid, attr, &xd);
      if (status & 1) {
	status = TdiDecompile(&xd, &dsc_string MDS_END_ARG);
	if (status & 1) {
	  char *p = strncpy(alloca(dsc_string.length + 1),
			    dsc_string.pointer,
			    dsc_string.length);
	  p[dsc_string.length] = '\0';
	  TclTextOut(p);
	}
	StrFree1Dx(&dsc_string);
	MdsFree1Dx(&xd, 0);
      }
    } else {
      if (TreeGetXNci(nid, "attributenames", &xd) & 1) {
	TdiSortVal(&xd, &xd MDS_END_ARG);
	if (xd.pointer && xd.pointer->class == CLASS_A) {
	  typedef ARRAY(char) ARRAY_DSC;
	  ARRAY_DSC *array = (ARRAY_DSC *) xd.pointer;
	  char *name = array->pointer;
	  TclTextOut("Defined attributes for this node:");
	  for (name = array->pointer; name < array->pointer + array->arsize;
	       name += array->length) {
	    char *out = alloca(array->length + 6);
	    sprintf(out, "    %.*s", array->length, name);
	    TclTextOut(out);
	  }
	} else {
	  TclTextOut("No attributes defined for this node");
	}
	MdsFree1Dx(&xd, 0);
      } else {
	TclTextOut("No attributes defined for this node");
      }
      status = 1;
    }
  }
  if (node)
    free(node);
  if (attr)
    free(attr);
  if (!(status & 1)) {
    MdsMsg(status, 0);
  }
  return status;
}

extern int TdiCompile();

int TclSetAttribute(void *ctx)
{
  char *nodnam = 0;
  char *attname = 0;
  char *ascValue = 0;
  struct descriptor dsc_ascValue = {0, DTYPE_T, CLASS_S, 0};
  static int val;
  static struct descriptor_xd value_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int sts;
  int nid;

  cli_get_value(ctx, "NODE", &nodnam);
  cli_get_value(ctx, "NAME", &attname);
  sts = TreeFindNode(nodnam, &nid);
  if (sts & 1) {
    if (cli_present(ctx, "EXTENDED") & 1) {
      printf("deal with extended stuff\n");
      /*
      static DYNAMIC_DESCRIPTOR(val_part);
      static DYNAMIC_DESCRIPTOR(dsc_eof);
      int use_lf = cli_present("LF") & 1;
      int symbols = cli_present("SYMBOLS") & 1;

      str_free1_dx(&dsc_ascValue);
      str_free1_dx(&dsc_eof);
      cli_get_value("EOF", &dsc_eof);
      while ((mdsdcl_get_input_nosymbols("ATT> ", &val_part) & 1)) {
	if (dsc_eof.dscA_pointer && val_part.dscA_pointer) {
	  if (!strcmp(dsc_eof.dscA_pointer, val_part.dscA_pointer))
	    break;
	} else if (!val_part.dscW_length)
	  break;
	if (use_lf)
	  str_concat(&dsc_ascValue, &dsc_ascValue, &val_part, "\n", 0);
	else
	  str_append(&dsc_ascValue, &val_part);
      }
      str_free1_dx(&val_part);
      */
    } else
      cli_get_value(ctx, "VALUE", &ascValue);
    dsc_ascValue.length = strlen(ascValue);
    dsc_ascValue.pointer = ascValue;
    sts = TdiCompile(&dsc_ascValue, &value_xd MDS_END_ARG);
    if (sts & 1) {
      if (!value_xd.l_length)
	value_xd.dtype = DTYPE_DSC;
      sts = TreeSetXNci(nid, attname, (struct descriptor *)&value_xd);
    }
  }
  if (nodnam)
    free(nodnam);
  if (attname)
    free(attname);
  if (ascValue)
    free(ascValue);
  MdsFree1Dx(&value_xd, NULL);
  if (~sts & 1)
    MdsMsg(sts, "TclSetAttribute: error processing command");
  return sts;
}
