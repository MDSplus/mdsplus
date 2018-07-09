/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <mdsplus/mdsconfig.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <ncidef.h>
#include <usagedef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <treeshr.h>
#include <mdsdescrip.h>

#include "tcl_p.h"

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

static int doFull(char **output, int nid, unsigned char nodeUsage, int version);
	/****************************************************************
	 * MdsOwner:
	 ****************************************************************/
static char *MdsOwner(		/* Return: ptr to "user" string         */
		       unsigned int owner	/* <r> owner id                         */
    )
{
  static char ownerString[512];
  int gid = owner >> 16;
  int uid = owner & 0xFFFF;
  char *groupname=0;
  char *username=0;
#ifdef HAVE_GETGRGID
  struct group *g = getgrgid(gid);
  if (g) {
    groupname = alloca(strlen(g->gr_name)+3);
    sprintf(groupname,"(%s)",g->gr_name);
  }
#endif
#ifdef HAVE_GETPWUID
  struct passwd *p = getpwuid(uid);
  if (p) {
    username = alloca(strlen(p->pw_name)+3);
    sprintf(username,"(%s)",p->pw_name);
  }
#endif
  if (groupname==0)
    groupname="";
  if (username==0)
    username="";
  sprintf(ownerString, "gid=%d%s,uid=%d%s", gid, groupname, uid, username);
  return (ownerString);
}

	/***************************************************************
	 * MdsDatime:
	 ***************************************************************/
static char *MdsDatime(		/* Return: ptr to date+time string      */
			int time[]	/* <r> date/time to display: quadword       */
    )
{
  unsigned short len;
  static char datime[24];
  static DESCRIPTOR(dsc_datime, datime);

  if (time[0]==0 && time[1]== 0)
    return "                       ";

  LibSysAscTim(&len, (struct descriptor *)&dsc_datime, time);
  datime[len] = '\0';
  return (datime);
}

void tclAppend(char **output, char *str)
{
  if (*output)
    *output = strcat(realloc(*output, strlen(*output) + strlen(str) + 1), str);
  else
    *output = strdup(str);
}

	/****************************************************************
	 * TclDirectory:
	 * Perform directory function
	 ****************************************************************/
EXPORT int TclDirectory(void *ctx, char **error __attribute__ ((unused)), char **output)
{
  char *tagnam;
  char msg[128];
  char *nodnam = 0;
  char *pathnam;

  int nid;
  int status = 1;
  void *ctx1 = 0;
  void *ctx2 = 0;
  int found = 0;
  int grand_found = 0;
  int first_tag;
  int full;
  int nodnamLen;
  int last_parent_nid = -1;
  int version;
  int parent_nid;
  char nodnamC[12 + 1];
  int relationship;
  int previous_relationship = 0;
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
  if (*output == 0)
    *output = strdup("");
  full = cli_present(ctx, "FULL") & 1;
  if (cli_present(ctx, "USAGE") & 1) {
    char *usageStr = 0;
    usageMask = 0;
    while (cli_get_value(ctx, "USAGE", &usageStr) & 1) {
      if (usageStr) {
	char usage = tclUsageToNumber(usageStr, error);
	if (usage == -1) {
	  free(usageStr);
	  return 0;
	} else {
	  usageMask = usageMask | (1 << usage);
	  free(usageStr);
	}
      }
    }
  }
  while (cli_get_value(ctx, "NODE", &nodnam) & 1) {
    int listlen = 0;
    while ((status = TreeFindNodeWild(nodnam, &nid, &ctx1, usageMask)) & 1) {
      grand_found++;
      status = TreeGetNci(nid, general_info_list);
      nodnamC[nodnamLen] = '\0';
      if (parent_nid != last_parent_nid) {
	if (found) {
	  sprintf(msg, "\n\nTotal of %d node%s.\n", found, (found > 1) ? "s" : "");
	  listlen = 0;
	  tclAppend(output, msg);
	}
	found = 0;
	last_parent_nid = parent_nid;
	pathnam = TreeGetPath(parent_nid);
	if (pathnam) {
	  sprintf(msg, "\n%s\n\n", pathnam);
	  free(pathnam);
	  tclAppend(output, msg);
	}
	previous_relationship = relationship;
      }
      found++;
      if (full) {
	if (previous_relationship != relationship) {
	  tclAppend(output, "\n");
	  previous_relationship = relationship;
	}
	sprintf(msg, " %s%s", (relationship == NciK_IS_CHILD) ? " " : ":", nodnamC);
	tclAppend(output, msg);
	ctx2 = 0;
	first_tag = 1;
	while ((tagnam = TreeFindNodeTags(nid, &ctx2))) {
	  sprintf(msg, "%s\\%s", first_tag ? " tags: " : ",", tagnam);
	  tclAppend(output, msg);
	  free(tagnam);
	  first_tag = 0;
	}
	tclAppend(output, "\n");
	version = 0;
	while (doFull(output, nid, nodeUsage, version++) & 1) ;
      } else {
	if (previous_relationship != relationship) {
	  tclAppend(output, "\n\n");
	  listlen = 0;
	  previous_relationship = relationship;
	}
	sprintf(msg, " %s%s", (relationship == NciK_IS_CHILD) ? " " : ":", nodnamC);
	tclAppend(output, msg);
	listlen = listlen + strlen(msg);
	if (listlen > 60) {
	  tclAppend(output, "\n");
	  listlen = 0;
	}
      }
      mdsdclFlushOutput(*output);
    }
    TreeFindNodeEnd(&ctx1);
    if (nodnam)
      free(nodnam);
  }
  sprintf(msg, "%s\nTotal of %d node%s.\n", full ? "" : "\n", found,
	  ((found > 1) || (found == 0)) ? "s" : "");
  tclAppend(output, msg);
  if (grand_found) {
    if (found != grand_found) {
      sprintf(msg, "\n\nGrand total of %d node%s.\n", grand_found,
	      ((grand_found > 1) || (grand_found == 0)) ? "s" : "");
      tclAppend(output, msg);
    }
  }
  return ((status == TreeNMN) || (status == TreeNNF)) ? 1 : status;
}

static int doFull(char **output, int nid, unsigned char nodeUsage, int version)
{
  char *pathnam;
  int time[2];
  char partC[64 + 1];
  int retlen;
  int head_nid;
  int partlen;
  NCI_ITM cong_list[] = {
    {4, NciCONGLOMERATE_NIDS, &head_nid, &retlen}
    , {64, NciORIGINAL_PART_NAME, partC, &partlen}
    , {0, NciEND_OF_LIST, 0, 0}
  };
  char *reference;
  int k;
  char *p;
  char msg[128];
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
  uint32_t dataLen;
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
      sprintf(msg, "      Status: %s,parent is %s, usage %s%s%s%s\n",
	      (nciFlags & NciM_STATE) ? "off" : "on",
	      (nciFlags & NciM_PARENT_STATE) ? "off" : "on",
	      p,
	      (nciFlags & NciM_WRITE_ONCE) ?
	      (dataLen ? ",readonly" : ",write-once") : "",
	      (nciFlags & NciM_ESSENTIAL) ? ",essential" : "",
	      (nciFlags & NciM_CACHED) ? ",cached" : "");
      tclAppend(output, msg);
      if (nciFlags & NciM_NO_WRITE_MODEL)
	tclAppend(output, "      not writeable in model\n");
      if (nciFlags & NciM_NO_WRITE_SHOT)
	tclAppend(output, "      not writeable in pulse file\n");
      if (nodeUsage == TreeUSAGE_SUBTREE) {
	sprintf(msg,
		"      subtree %sincluded in pulse file.\n",
		(nciFlags & NciM_INCLUDE_IN_PULSE) ? "" : "NOT ");
	tclAppend(output, msg);
      }
      msg[0] = 0;
      if (nciFlags & NciM_COMPRESSIBLE) {
	strcat(msg, "compressible");
	strcat(msg, (nciFlags & (NciM_COMPRESS_ON_PUT | NciM_DO_NOT_COMPRESS | NciM_COMPRESS_SEGMENTS)) ? "," : "\n");
      }
      if (nciFlags & NciM_COMPRESS_ON_PUT) {
	strcat(msg, "compress on put");
	strcat(msg, (nciFlags & (NciM_DO_NOT_COMPRESS | NciM_COMPRESS_SEGMENTS )) ? "," : "\n");
      }
      if (nciFlags & NciM_DO_NOT_COMPRESS) {
	strcat(msg, "do not compress");
	strcat(msg, (nciFlags & NciM_COMPRESS_SEGMENTS ) ? "," : "\n");
      }
      if (nciFlags & NciM_COMPRESS_SEGMENTS)
	strcat(msg, "compress segments\n");
	
      if (strlen(msg) > 0) {
	tclAppend(output, "      ");
	tclAppend(output, msg);
      }

      switch (nciFlags & (NciM_PATH_REFERENCE | NciM_NID_REFERENCE)) {
      case NciM_PATH_REFERENCE:
	reference = "      contains node references (paths only)\n";
	break;
      case NciM_NID_REFERENCE:
	reference = "      contains node references (node ids only)\n";
	break;
      case NciM_PATH_REFERENCE | NciM_NID_REFERENCE:
	reference = "      contains node references (paths and node ids)\n";
	break;
      default:
	reference = 0;
	break;
      }
      if (reference) {
	tclAppend(output, reference);
      }
    }
    sprintf(msg, "      Data inserted: %s    Owner: %s\n", MdsDatime(time), MdsOwner(owner));
    tclAppend(output, msg);
    if (dataLen) {
      sprintf(msg, "      Dtype: %-20s  Class: %-18s  Length: %u bytes\n",
	      MdsDtypeString((int)dtype), MdsClassString((int)class), dataLen);
      tclAppend(output, msg);
    } else
      tclAppend(output, "      There is no data stored for this node\n");
    if (conglomerate_elt != 0) {
      sprintf(msg, "      Model element: %d\n", conglomerate_elt);
      tclAppend(output, msg);
      partlen = 0;
      TreeGetNci(nid, cong_list);
      if (partlen) {
	pathnam = TreeGetPath(head_nid);
	partC[partlen] = 0;
	sprintf(msg, "      Original element name: %s%s\n", pathnam, partC);
	tclAppend(output, msg);
	free(pathnam);
      }
    }
  }
  return status;
}

EXPORT int TclLs(void *ctx)
{
  char *cmd = 0;
  cli_get_value(ctx, "command_line", &cmd);
  printf("TclLs called with command line: '%s'\n", cmd);
  free(cmd);
  return 1;
}
