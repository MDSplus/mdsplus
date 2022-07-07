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
#include <stdlib.h>
#include <string.h>

#include <dbidef.h>
#include <dcl.h>
#include <mdsshr.h>
#include <treeshr.h>

#include "tcl_p.h"

/**********************************************************************
 * TCL_SHOW_DB.C --
 *
 * Show MDSplus database information.
 *
 * History:
 *  04-May-1998  TRG  Create.  Ported from original mds code.
 *
 ************************************************************************/

/***************************************************************
 * TclShowDb:
 ***************************************************************/
EXPORT int TclShowDB(void *ctx __attribute__((unused)),
                     char **error __attribute__((unused)), char **output)
{
  int sts;
  int open;
  DBI_ITM itm1[] = {
    {sizeof(open), DbiNUMBER_OPENED, &open, 0}, {0}
  };
  sts = TreeGetDbi(itm1);
  if (sts & 1)
  {
    int idx;
    *output = strdup("");
    for (idx = 0; idx < open; idx++)
    {
      int shotid;
      int tree_version;
      char modified;
      char edit;
      char *outstr = 0;
      char * dbi_name = 0;
      char * dbi_default = 0;
      size_t outstr_length = 0;
      DBI_ITM itm2[] = {
        {sizeof(idx), DbiINDEX, &idx, 0},
        {0, DbiNAME, 0, 0},
        {sizeof(shotid), DbiSHOTID, &shotid, 0},
        {0, DbiDEFAULT, 0, 0},
        {sizeof(edit), DbiOPEN_FOR_EDIT, &edit, 0},
        {sizeof(modified), DbiMODIFIED, &modified, 0},
        {sizeof(tree_version), DbiTREE_VERSION, &tree_version, 0},
        {0}
      };
      sts = TreeGetDbi(itm2);
      dbi_name = itm2[1].pointer;
      dbi_default = itm2[3].pointer;
      outstr_length = strlen(dbi_name) + strlen(dbi_default) + 100;
      outstr = (char *)malloc(outstr_length);
      snprintf(outstr, outstr_length,
        "%03d  %-12s  shot: %d [%s] %s%s version: %d\n",
        idx,
        dbi_name,
        shotid,
        dbi_default,
        edit ? "open for edit" : " ",
        modified ? ",modified" : " ",
        tree_version
      );
      free(dbi_name);
      free(dbi_default);
      tclAppend(output, outstr);
      free(outstr);
    }
    tclAppend(output, "\n");
  }
  return sts;
}
