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

#include <dcl.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <usagedef.h>

/**********************************************************************
 * TCL_DO_METHOD.C --
 *
 * TclDoMethod:  Do something.
 *
 * History:
 *  05-Mar-1998  TRG  Create.  Ported from original MDSplus code.
 *
 ************************************************************************/

extern int TdiIntrinsic();
extern void TdiGetLong();
/****************************************************************
 * TclDoMethod:
 ****************************************************************/
EXPORT int TclDoMethod(void *ctx, char **error,
                       char **output __attribute__((unused)))
{
  int i;
  int argc;
  int status;
  unsigned char do_it;
  int nid;
  unsigned short boolVal;
  struct descriptor_s bool_dsc = {sizeof(boolVal), DTYPE_W, CLASS_S,
                                  (char *)&boolVal};
  struct descriptor nid_dsc = {4, DTYPE_NID, CLASS_S, (char *)&nid};
  char *arg = 0;
  char *if_clause = 0;
  char *method = 0;
  char *object = 0;
  struct descriptor method_dsc = {0, DTYPE_T, CLASS_S, 0};

  cli_get_value(ctx, "OBJECT", &object);
  status = TreeFindNode(object, &nid);
  if (STATUS_OK)
  {
    do_it = (TreeIsOn(nid) | cli_present(ctx, "OVERRIDE")) & 1;
    if (IS_OK(cli_present(ctx, "IF")))
    {
      struct descriptor if_clause_dsc = {0, DTYPE_T, CLASS_S, 0};
      struct descriptor *args[] = {&if_clause_dsc};
      cli_get_value(ctx, "IF", &if_clause);
      if_clause_dsc.length = strlen(if_clause);
      if_clause_dsc.pointer = if_clause;
      status = TdiIntrinsic(OPC_EXECUTE, 1, &args, &bool_dsc);
      free(if_clause);
      if (STATUS_OK)
        do_it = do_it && boolVal;
      else
        do_it = 0;
    }
    if (do_it)
    {
      cli_get_value(ctx, "METHOD", &method);
      method_dsc.length = strlen(method);
      method_dsc.pointer = method;
      argc = 0;
      mdsdsc_xd_t xdarg[255];
      mdsdsc_t *arglist[255];
      EMPTYXD(xd);
      if (IS_OK(cli_present(ctx, "ARGUMENT")))
      {
        while (argc < 255 && IS_OK(cli_get_value(ctx, "ARGUMENT", &arg)))
        {
          struct descriptor arg_dsc = {strlen(arg), DTYPE_T, CLASS_S, arg};
          struct descriptor *arg_dsc_ptr = &arg_dsc;
          xdarg[argc] = xd; // empty_xd
          status = TdiIntrinsic(OPC_COMPILE, 1, &arg_dsc_ptr, &xdarg[argc]);
          free(arg);
          if (STATUS_OK)
          {
            arglist[argc] = xdarg[argc].pointer;
            argc++;
          }
          else
            break;
        }
      }
      if (STATUS_OK)
      {
        status = TreeDoMethodA(&nid_dsc, &method_dsc, argc, arglist, &xd);
        if (STATUS_OK)
        {
          if (xd.pointer)
          {
            TdiGetLong(&xd, &status);
            MdsFree1Dx(&xd, NULL);
          }
          else
            status = SsSUCCESS;
        }
      }
      free(method);
      for (i = 0; i < argc; i++)
        MdsFree1Dx(&xdarg[i], NULL);
    }
  }
  free(object);
  if (STATUS_NOT_OK)
  {
    char *msg = MdsGetMsg(status);
    *error = malloc(strlen(msg) + 100);
    sprintf(*error,
            "Error executing device method\n"
            "Error message was: %s\n",
            msg);
  }
  return status;
}
