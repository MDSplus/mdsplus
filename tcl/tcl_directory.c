#include 	<config.h>
#include        "tclsysdef.h"
#include        <ncidef.h>
#include        <usagedef.h>
#include        <string.h>

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

extern char *MdsDtypeString();
extern char *MdsClassString();
#ifdef vms
extern int sys$asctim();
#else
extern int LibSysAscTim();
#endif

static int doFull(int nid, unsigned char nodeUsage, int version);
	/****************************************************************
	 * MdsOwner:
	 ****************************************************************/
static char *MdsOwner(		/* Return: ptr to "user" string         */
		       unsigned int owner	/* <r> owner id                         */
    )
{
  static char ownerString[20];

  sprintf(ownerString, "[%o,%o]", owner >> 16, owner & 0xFFFF);
  return (ownerString);
}

	/***************************************************************
	 * MdsDatime:
	 ***************************************************************/
static char *MdsDatime(		/* Return: ptr to date+time string      */
			int time[]	/* <r> date/time to display: quadword       */
    )
{
  int flags = 0;
  int sts;
  short len;
  static char datime[24];
  static DESCRIPTOR(dsc_datime, datime);

#ifdef vms
  sts = sys$asctim(&len, &dsc_datime, time, flags);
#else
  sts = LibSysAscTim(&len, &dsc_datime, time, flags);
#endif
  datime[len] = '\0';
  return (datime);
}

	/****************************************************************
	 * TclDirectory:
	 * Perform directory function
	 ****************************************************************/
int TclDirectory(void *ctx)
{
  unsigned int usage;
  char *tagnam;
  char textLine[128];
  static char fmtTotal[] = "Total of %d node%s.";
  static char fmtGrandTotal[] = "Grand total of %d node%s.";
  char *nodnam=0;
  char *pathnam;

  int nid;
  int status;
  void *ctx1 = 0;
  void *ctx2 = 0;
  int found = 0;
  int grand_found = 0;
  int first_tag;
  int full;
  int nodnamLen;
  char *outline=memset(malloc(1),0,1);

  int retlen;
  int last_parent_nid = -1;
  int version;
  static int parent_nid;
  static char nodnamC[12 + 1];
  static int relationship;
  int previous_relationship;
  unsigned char nodeUsage;
  NCI_ITM general_info_list[] = {
    {4, NciPARENT, &parent_nid, 0}
    , {12, NciNODE_NAME, nodnamC, &nodnamLen}
    , {4, NciPARENT_RELATIONSHIP, &relationship, 0}
    , {1, NciUSAGE, &nodeUsage, 0}
    , {0, NciEND_OF_LIST, 0, 0}
  };
  int usageMask = -1;

  parent_nid = 0;

  full = cli_present(ctx, "FULL") & 1;
  if (cli_present(ctx, "USAGE") & 1) {
    char *usageStr=0;
    usageMask = 0;
    while (cli_get_value(ctx, "USAGE", &usageStr) & 1) {
      printf("Deal with usage mask\n");
      usageMask = usageMask | (1 << usage);
    }
  }
  while (cli_get_value(ctx, "NODE", &nodnam) & 1) {
    while ((status = TreeFindNodeWild(nodnam, &nid, &ctx1, usageMask)) & 1) {
	grand_found++;
	status = TreeGetNci(nid, general_info_list);
	nodnamC[nodnamLen] = '\0';
	if (parent_nid != last_parent_nid) {
	  if (found) {
	    if (!full && (strlen(outline) > 0)) {
	      TclTextOut(outline);
	      outline[0]=0;
	    }
	    TclTextOut("  ");
	    sprintf(textLine, fmtTotal, found, (found > 1) ? "s" : "");
	    TclTextOut(textLine);
	  }
	  TclTextOut("  ");
	  pathnam = TreeGetPath(parent_nid);
	  TclTextOut(pathnam);
	  TreeFree(pathnam);	/* free the string      */
	  TclTextOut("  ");
	  found = 0;
	  last_parent_nid = parent_nid;
	  previous_relationship = relationship;
	}
	found++;
	if (full) {
	  if (previous_relationship != relationship) {
	    TclTextOut("  ");
	    previous_relationship = relationship;
	  }
	  outline=realloc(outline, strlen(outline)+strlen(nodnamC)+10);
	  strcat(outline," ");
	  if (relationship != NciK_IS_CHILD)
	    strcat(outline,":");
	  strcat(outline,nodnamC);
	  ctx2 = 0;
	  first_tag = 1;
	  while ((tagnam = TreeFindNodeTags(nid, &ctx2))) {
	    outline=realloc(outline,strlen(outline)+strlen(tagnam)+20);
	    if (first_tag)
	      strcat(outline," tags: \\");
	    else
	      strcat(outline,",\\");
	    strcat(outline,tagnam);
	    TreeFree(tagnam);
	    first_tag = 0;
	  }
	  TclTextOut(outline);
	  outline[0]=0;
	  version = 0;
	  while (doFull(nid, nodeUsage, version++) & 1) ;
	} else {
	  if (previous_relationship != relationship) {
	    TclTextOut(outline);
	    outline[0]=0;
	    TclTextOut("  ");
	    previous_relationship = relationship;
	  }
	  outline=realloc(outline,strlen(outline)+strlen(nodnamC)+10);
	  strcat(outline," ");
	  if (relationship != NciK_IS_CHILD)
	    strcat(outline,":");
	  strcat(outline,nodnamC);
	  if (strlen(outline) > 60) {
	    TclTextOut(outline);
	    outline[0]=0;
	  }
	}
    }
    TreeFindNodeEnd(&ctx1);
  }
  if (found) {
    if (!full) {
      if (strlen(outline) > 0) {
	TclTextOut(outline);
	outline[0]=0;
      }
    }
    TclTextOut("  ");
    sprintf(textLine, fmtTotal, found, (found > 1) ? "s" : "");
    TclTextOut(textLine);
  }
  if (grand_found) {
    if (found != grand_found) {
      TclTextOut("  ");
      sprintf(textLine, fmtGrandTotal, grand_found, (grand_found > 1) ? "s" : "");
      TclTextOut(textLine);
    }
  }
  free(outline);
  return ((status == TreeNMN) || (status == TreeNNF)) ? 1 : status;
}

static int doFull(int nid, unsigned char nodeUsage, int version)
{
  static char fmtConglom2[] = "      Original element name: %s%s";
  char *pathnam;
  int time[2];
  char partC[64 + 1];
  int retlen;
  int head_nid;
  int partlen;
  char *outline=memset(malloc(1),0,1);
  NCI_ITM cong_list[] = {
    {4, NciCONGLOMERATE_NIDS, &head_nid, &retlen}
    , {64, NciORIGINAL_PART_NAME, partC, &partlen}
    , {0, NciEND_OF_LIST, 0, 0}
  };
  char *reference;
  static char fmtStates[] = "      Status: %s,parent is %s, usage %s%s%s%s";
  static char fmtTime[] = "      Data inserted: %s    Owner: %s";
  static char fmtDescriptor[] = "      Dtype: %-20s  Class: %-18s  Length: %d bytes";
  static char fmtConglom1[] = "      Model element: %d";
  int k;
  char *p;
  char textLine[128];
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
    "unknown"
  };
#define MAX_USAGES   (sizeof(usages)/sizeof(usages[0]))
  int nciFlags;
  unsigned int owner;
  char class;
  char dtype;
  int dataLen;
  unsigned short conglomerate_elt;
  int vers;
  NCI_ITM full_list[] = {
    {4, NciVERSION, &vers, 0}
    , {4, NciGET_FLAGS, &nciFlags, 0}
    , {8, NciTIME_INSERTED, time, 0}
    , {4, NciOWNER_ID, &owner, 0}
    , {1, NciCLASS, &class, 0}
    , {1, NciDTYPE, &dtype, 0}
    , {4, NciLENGTH, &dataLen, 0}
    , {2, NciCONGLOMERATE_ELT, &conglomerate_elt, 0}
    , {0, NciEND_OF_LIST, 0, 0}
  };
  int status;
  vers = version;
  status = TreeGetNci(nid, full_list);
  if (status & 1) {
    if (version == 0) {
      k = (nodeUsage < MAX_USAGES) ? nodeUsage : (MAX_USAGES - 1);
      p = usages[k];
      sprintf(textLine, fmtStates,
	      (nciFlags & NciM_STATE) ? "off" : "on",
	      (nciFlags & NciM_PARENT_STATE) ? "off" : "on",
	      p,
	      (nciFlags & NciM_WRITE_ONCE) ?
	      (dataLen ? ",readonly" : ",write-once") : "",
	      (nciFlags & NciM_ESSENTIAL) ? ",essential" : "",
	      (nciFlags & NciM_CACHED) ? ",cached" : "");
      TclTextOut(textLine);

      if (nciFlags & NciM_NO_WRITE_MODEL)
	TclTextOut("      not writeable in model");
      if (nciFlags & NciM_NO_WRITE_SHOT)
	TclTextOut("      not writeable in pulse file");
      if (nodeUsage == TreeUSAGE_SUBTREE) {
	sprintf(textLine,
		"      subtree %sincluded in pulse file.",
		(nciFlags & NciM_INCLUDE_IN_PULSE) ? "" : "NOT");
	TclTextOut(textLine);
      }

      if (nciFlags & NciM_COMPRESSIBLE) {
	outline=strcpy(realloc(outline,strlen("compressible")+10),"compressible");
	if (nciFlags & (NciM_COMPRESS_ON_PUT | NciM_DO_NOT_COMPRESS))
	  strcat(outline,",");
      }
      if (nciFlags & NciM_COMPRESS_ON_PUT) {
	outline=strcat(realloc(outline,strlen(outline)+strlen("compress on put")+10),"compress on put");
	if (nciFlags & NciM_DO_NOT_COMPRESS)
	  strcat(outline,",");
      }
      if (nciFlags & NciM_DO_NOT_COMPRESS)
	outline=strcat(realloc(outline,strlen(outline)+strlen("do not compress")+10),"do not compress");
      if (strlen(outline)>0) {
	char *noutline=strcpy(malloc(strlen(outline)+10), "      ");
	strcat(noutline,outline);
	TclTextOut(noutline);
	free(noutline);
	outline[0]=0;
      }

      switch (nciFlags & (NciM_PATH_REFERENCE | NciM_NID_REFERENCE)) {
      case NciM_PATH_REFERENCE:
	reference = "      contains node references (paths only)";
	break;
      case NciM_NID_REFERENCE:
	reference = "      contains node references (node ids only)";
	break;
      case NciM_PATH_REFERENCE | NciM_NID_REFERENCE:
	reference = "      contains node references (paths and node ids)";
	break;
      default:
	reference = 0;
	break;
      }
      if (reference) {
	TclTextOut(reference);
      }
    } else
      TclTextOut("");
    sprintf(textLine, fmtTime, MdsDatime(time), MdsOwner(owner));
    TclTextOut(textLine);
    if (dataLen) {
      sprintf(textLine, fmtDescriptor,
	      MdsDtypeString((int)dtype), MdsClassString((int)class), dataLen);
      TclTextOut(textLine);
    } else
      TclTextOut("      There is no data stored for this node");
    if (conglomerate_elt != 0) {
      sprintf(textLine, fmtConglom1, conglomerate_elt);
      TclTextOut(textLine);
      partlen = 0;
      TreeGetNci(nid, cong_list);
      if (partlen) {
	pathnam = TreeGetPath(head_nid);
	partC[partlen] = 0;
	sprintf(textLine, fmtConglom2, pathnam, partC);
	TclTextOut(textLine);
	TreeFree(pathnam);
      }
    }
  }
  free(outline);
  return status;
}

int TclLs(void *ctx) {
  char *cmd = 0;
  cli_get_value(ctx, "command_line", &cmd);
  printf("TclLs called with command line: '%s'\n",cmd);
  free(cmd);
  return 1;
}
