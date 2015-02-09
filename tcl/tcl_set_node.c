#include	<config.h>
#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>
#include        <alloca.h>
#include <dcl.h>
#include <string.h>
#include <strroutines.h>

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
int TclSetNode(void *ctx, char **error, char **output)
{
  int nid;
  int status = 1;
  int log;
  int usageMask;
  void *ctx1 = 0;
  char *nodename = 0;
  char *statusStr = 0;
  cli_get_value(ctx, "NODENAME", &nodename);
  cli_get_value(ctx, "STATUS", &statusStr);
  log = cli_present(ctx, "LOG") & 1;

  usageMask = -1;
  while ((status = TreeFindNodeWild(nodename, &nid, &ctx1, usageMask)) & 1) {
    if (statusStr) {
      int statval = atoi(statusStr);
      NCI_ITM setnci[] = { {sizeof(int), NciSTATUS, 0, 0}, {0, NciEND_OF_LIST, 0, 0} };
      setnci[0].pointer = (unsigned char *)&statval;
      TreeSetNci(nid, setnci);
    }
    switch (cli_present(ctx, "SUBTREE")) {
    case CLI_STS_PRESENT:
      status = TreeSetSubtree(nid);
      if (!(status & 1)) {
	char *msg = MdsGetMsg(status);
	*error = malloc(strlen(nodename) + strlen(msg) + 100);
	sprintf(*error, "Error: Problem setting node %s to subtree\n"
		"Error mesage was: %s\n", nodename, msg);
	goto error;
      }
      break;
    case CLI_STS_NEGATED:
      status = TreeSetNoSubtree(nid);
      if (!(status & 1)) {
	char *msg = MdsGetMsg(status);
	*error = malloc(strlen(nodename) + strlen(msg) + 100);
	sprintf(*error, "Error: Problem setting node %s to nosubtree\n"
		"Error mesage was: %s\n", nodename, msg);
	goto error;
      }
      break;
    }
    if (!(status & 1))
      goto error;
    if (cli_present(ctx, "ON") & 1) {
      status = TreeTurnOn(nid);
      if (status & 1)
	TclNodeTouched(nid, on_off);
      else {
	char *msg = MdsGetMsg(status);
	*error = malloc(strlen(nodename) + strlen(msg) + 100);
	sprintf(*error, "Error: Problem turning node %s on\n"
		"Error mesage was: %s\n", nodename, msg);
	goto error;
      }
    } else if (cli_present(ctx, "OFF") & 1) {
      status = TreeTurnOff(nid);
      if (status & 1)
	TclNodeTouched(nid, on_off);
      else {
	char *msg = MdsGetMsg(status);
	*error = malloc(strlen(nodename) + strlen(msg) + 100);
	sprintf(*error, "Error: Problem turning node %s off\n"
		"Error mesage was: %s\n", nodename, msg);
	goto error;
      }
    }
    {
      int set_flags;
      int clear_flags;
      struct descriptor dsc_path = { 0, DTYPE_T, CLASS_D, 0 };
      NCI_ITM get_itmlst[] = { {0, NciPATH, (unsigned char *)&dsc_path, 0}, {0, NciEND_OF_LIST} };
      NCI_ITM set_itmlst[] =
	  { {0, NciSET_FLAGS, (unsigned char *)&set_flags, 0}, {0, NciEND_OF_LIST} };
      NCI_ITM clear_itmlst[] =
	  { {0, NciCLEAR_FLAGS, (unsigned char *)&clear_flags, 0}, {0, NciEND_OF_LIST} };
      set_flags = 0;
      clear_flags = 0;
      switch (cli_present(ctx, "WRITE_ONCE")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_WRITE_ONCE;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_WRITE_ONCE;
	break;
      }
      switch (cli_present(ctx, "CACHED")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_CACHED;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_CACHED;
	break;
      }
      switch (cli_present(ctx, "COMPRESS_ON_PUT")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_COMPRESS_ON_PUT;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_COMPRESS_ON_PUT;
	break;
      }
      switch (cli_present(ctx, "COMPRESS_SEGMENTS")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_COMPRESS_SEGMENTS;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_COMPRESS_SEGMENTS;
	break;
      }
      switch (cli_present(ctx, "DO_NOT_COMPRESS")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_DO_NOT_COMPRESS;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_DO_NOT_COMPRESS;
	break;
      }
      switch (cli_present(ctx, "SHOT_WRITE")) {
      case CLI_STS_PRESENT:
	clear_flags |= NciM_NO_WRITE_SHOT;
	break;
      case CLI_STS_NEGATED:
	set_flags |= NciM_NO_WRITE_SHOT;
	break;
      }
      switch (cli_present(ctx, "MODEL_WRITE")) {
      case CLI_STS_PRESENT:
	clear_flags |= NciM_NO_WRITE_MODEL;
	break;
      case CLI_STS_NEGATED:
	set_flags |= NciM_NO_WRITE_MODEL;
	break;
      }
      switch (cli_present(ctx, "INCLUDED")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_INCLUDE_IN_PULSE;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_INCLUDE_IN_PULSE;
	break;
      }
      switch (cli_present(ctx, "ESSENTIAL")) {
      case CLI_STS_PRESENT:
	set_flags |= NciM_ESSENTIAL;
	break;
      case CLI_STS_NEGATED:
	clear_flags |= NciM_ESSENTIAL;
	break;
      }
      if (set_flags)
	status = TreeSetNci(nid, set_itmlst);
      if (clear_flags)
	status = TreeSetNci(nid, clear_itmlst);
      if (status & 1) {
	if (log) {
	  char *nout;
	  if (*output) {
	    *output = realloc(*output, strlen(*output) + dsc_path.length + 100);
	    nout = *output + strlen(*output);
	  } else {
	    *output = malloc(strlen(*output) + dsc_path.length + 100);
	    nout = *output;
	  }
	  sprintf(*output, "Node: %.#s modified\n", dsc_path.length, dsc_path.pointer);
	}
	StrFree1Dx(&dsc_path);
      } else {
	char *msg = MdsGetMsg(status);
	*error = malloc(strlen(msg) + dsc_path.length + 100);
	sprintf(*output, "Error problem modifying node %.#s\n"
		"Error message was: %s\n", dsc_path.length, dsc_path.pointer, msg);
	StrFree1Dx(&dsc_path);
	goto error;
      }
    }
  }
 error:
  TreeFindNodeEnd(&ctx1);
  if (status == TreeNMN)
    status = 1;
  if (nodename)
    free(nodename);
  return status;
}
