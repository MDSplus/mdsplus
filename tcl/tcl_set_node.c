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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <dcl.h>
#include <mdsdcl_messages.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <_ncidef.h>
#include <strroutines.h>
#include <treeshr.h>
#include <usagedef.h>

#include "tcl_p.h"

DEFINE_COMPRESSION_METHODS

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
EXPORT int TclSetNode(void *ctx, char **error, char **output)
{
  int nid;
  int status = 1;
  int log;
  int usageMask;
  void *ctx1 = 0;
  char *nodename = 0;
  char *statusStr = 0;
  unsigned char compression_method=0;

  cli_get_value(ctx, "NODENAME", &nodename);
  cli_get_value(ctx, "STATUS", &statusStr);
  log = cli_present(ctx, "LOG") & 1;

  if (cli_present(ctx, "COMPRESSION_METHOD"))
  {
    char *compression_method_str = 0;
    if(cli_get_value(ctx, "COMPRESSION_METHOD", &compression_method_str) & 1)
    {
      char *p = compression_method_str;
      unsigned int i;
      for ( ; *p; ++p) *p = tolower(*p);
      for (i=0; i < NUM_COMPRESSION_METHODS; i++)
      {
        if(strcmp(compression_method_str, compression_methods[i].name) == 0)
        {
            compression_method=i;
            break;
        }
      }
      if(i >= NUM_COMPRESSION_METHODS)
      {
        *error = malloc(strlen(nodename) + strlen(compression_method_str) + 100);
        sprintf(*error,
        "Error: Problem setting compression method for node %s\n"
        "\t%s not a valid compression method\n",
        nodename, compression_method_str);
          
      }
      free(compression_method_str);
    }
  }


  usageMask = -1;
  while ((status = TreeFindNodeWild(nodename, &nid, &ctx1, usageMask)) & 1)
  {
    if (statusStr)
    {
      int statval = strtol(statusStr, NULL, 0);
      NCI_ITM setnci[] = {{sizeof(int), NciSTATUS, 0, 0},
                          {0, NciEND_OF_LIST, 0, 0}};
      setnci[0].pointer = (unsigned char *)&statval;
      TreeSetNci(nid, setnci);
    }
    if (cli_present(ctx, "COMPRESSION_METHOD"))
    {
      NCI_ITM setnci[] = {{sizeof(compression_method), NciCOMPRESSION_METHOD, 0, 0},
                          {0, NciEND_OF_LIST, 0, 0}};
      setnci[0].pointer = (unsigned char *)&compression_method;
      TreeSetNci(nid, setnci);
    }
 
    switch (cli_present(ctx, "SUBTREE"))
    {
    case MdsdclPRESENT:
      status = TreeSetSubtree(nid);
      if (STATUS_NOT_OK)
      {
        char *msg = MdsGetMsg(status);
        *error = malloc(strlen(nodename) + strlen(msg) + 100);
        sprintf(*error,
                "Error: Problem setting node %s to subtree\n"
                "Error mesage was: %s\n",
                nodename, msg);
        goto error;
      }
      break;
    case MdsdclNEGATED:
      status = TreeSetNoSubtree(nid);
      if (STATUS_NOT_OK)
      {
        char *msg = MdsGetMsg(status);
        *error = malloc(strlen(nodename) + strlen(msg) + 100);
        sprintf(*error,
                "Error: Problem setting node %s to nosubtree\n"
                "Error mesage was: %s\n",
                nodename, msg);
        goto error;
      }
      break;
    }
    if (STATUS_NOT_OK)
      goto error;
    if (cli_present(ctx, "ON") & 1)
    {
      status = TreeTurnOn(nid);
      if (STATUS_OK)
        TclNodeTouched(nid, on_off);
      else
      {
        char *msg = MdsGetMsg(status);
        *error = malloc(strlen(nodename) + strlen(msg) + 100);
        sprintf(*error,
                "Error: Problem turning node %s on\n"
                "Error mesage was: %s\n",
                nodename, msg);
        goto error;
      }
    }
    else if (cli_present(ctx, "OFF") & 1)
    {
      status = TreeTurnOff(nid);
      if (STATUS_OK)
        TclNodeTouched(nid, on_off);
      else
      {
        char *msg = MdsGetMsg(status);
        *error = malloc(strlen(nodename) + strlen(msg) + 100);
        sprintf(*error,
                "Error: Problem turning node %s off\n"
                "Error mesage was: %s\n",
                nodename, msg);
        goto error;
      }
    }
    {
      int set_flags;
      int clear_flags;
      struct descriptor_d dsc_path = {0, DTYPE_T, CLASS_D, 0};
      NCI_ITM set_itmlst[] = {{0, NciSET_FLAGS, (unsigned char *)&set_flags, 0},
                              {0, NciEND_OF_LIST, 0, 0}};
      NCI_ITM clear_itmlst[] = {
          {0, NciCLEAR_FLAGS, (unsigned char *)&clear_flags, 0},
          {0, NciEND_OF_LIST, 0, 0}};
      set_flags = 0;
      clear_flags = 0;
      switch (cli_present(ctx, "WRITE_ONCE"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_WRITE_ONCE;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_WRITE_ONCE;
        break;
      }
      switch (cli_present(ctx, "CACHED"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_CACHED;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_CACHED;
        break;
      }
      switch (cli_present(ctx, "COMPRESS_ON_PUT"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_COMPRESS_ON_PUT;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_COMPRESS_ON_PUT;
        break;
      }
      switch (cli_present(ctx, "COMPRESS_SEGMENTS"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_COMPRESS_SEGMENTS;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_COMPRESS_SEGMENTS;
        break;
      }
      switch (cli_present(ctx, "DO_NOT_COMPRESS"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_DO_NOT_COMPRESS;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_DO_NOT_COMPRESS;
        break;
      }
      switch (cli_present(ctx, "SHOT_WRITE"))
      {
      case MdsdclPRESENT:
        clear_flags |= NciM_NO_WRITE_SHOT;
        break;
      case MdsdclNEGATED:
        set_flags |= NciM_NO_WRITE_SHOT;
        break;
      }
      switch (cli_present(ctx, "MODEL_WRITE"))
      {
      case MdsdclPRESENT:
        clear_flags |= NciM_NO_WRITE_MODEL;
        break;
      case MdsdclNEGATED:
        set_flags |= NciM_NO_WRITE_MODEL;
        break;
      }
      switch (cli_present(ctx, "INCLUDED"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_INCLUDE_IN_PULSE;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_INCLUDE_IN_PULSE;
        break;
      }
      switch (cli_present(ctx, "ESSENTIAL"))
      {
      case MdsdclPRESENT:
        set_flags |= NciM_ESSENTIAL;
        break;
      case MdsdclNEGATED:
        clear_flags |= NciM_ESSENTIAL;
        break;
      }
      if (set_flags)
        status = TreeSetNci(nid, set_itmlst);
      if (clear_flags)
        status = TreeSetNci(nid, clear_itmlst);
      if (STATUS_OK)
      {
        if (log)
        {
          char *nout;
          if (*output)
          {
            *output = realloc(*output, strlen(*output) + dsc_path.length + 100);
            nout = *output + strlen(*output);
          }
          else
          {
            *output = malloc(dsc_path.length + 100);
            nout = *output;
          }
          sprintf(nout, "Node: %.*s modified\n", dsc_path.length,
                  dsc_path.pointer);
        }
        StrFree1Dx(&dsc_path);
      }
      else
      {
        char *msg = MdsGetMsg(status);
        *error = malloc(strlen(msg) + dsc_path.length + 100);
        sprintf(*error,
                "Error problem modifying node %.*s\n"
                "Error message was: %s\n",
                dsc_path.length, dsc_path.pointer, msg);
        StrFree1Dx(&dsc_path);
        goto error;
      }
    }
  }
error:
  TreeFindNodeEnd(&ctx1);
  if (status == TreeNMN)
    status = 1;
  free(nodename);
  return status;
}
