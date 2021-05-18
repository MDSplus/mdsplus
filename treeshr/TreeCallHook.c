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
#include "treeshrp.h"
#include <ctype.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr_hooks.h>

static int (*Notify)(TreeshrHookType, char *, int, int) = NULL;
static void load_Notify()
{
  LibFindImageSymbol_C("TreeShrHooks", "Notify", &Notify);
}

int TreeCallHook(TreeshrHookType htype, TREE_INFO *info, int nid)
{
  RUN_FUNCTION_ONCE(load_Notify);
  if (Notify)
    return (*Notify)(htype, info->treenam, info->shot, nid);
  return 1;
}

static char *_strcasestr(char *s1_in, char *s2_in)
{
  /* This is needed because strcasestr() is no available on all platforms */
  char *s1 = strdup(s1_in);
  char *s2 = strdup(s2_in);
  char *s;
  for (s = s1; *s; s++)
    *s = toupper((unsigned char)*s);
  for (s = s2; *s; s++)
    *s = toupper((unsigned char)*s);
  s = strstr(s1, s2);
  if (s)
    s = s1_in + (s - s1);
  free(s1);
  free(s2);
  return s;
}

static int isEnabled(char *hookName)
{
  char *enabledHooks = getenv("TreeHooks");
  if (enabledHooks == NULL)
    return 0;
  enabledHooks = strdup(enabledHooks);
  if ((_strcasestr(enabledHooks, "allhooks") != NULL) &&
      (_strcasestr(hookName, "full") == NULL))
    return 1;
  if ((_strcasestr(enabledHooks, "fullhooks") != NULL) &&
      (_strcasestr(hookName, "full") != NULL))
    return 1;

  char *p = enabledHooks;
  while (p)
  {
    p = _strcasestr(p, hookName);
    if (p)
    {
      if (p == enabledHooks || p[-1] == ',')
      {
        size_t len = strlen(hookName);
        if (p[len] == ',' || p[len] == '\0')
          return 1;
        else
          p = p + len;
      }
      else
        return 0;
    }
  }
  free(enabledHooks);
  return 0;
}

void TreeCallHookFun(char *hookType, char *hookName, ...)
{
  static int (*TdiExecute)() = NULL;
  int status;
  status = LibFindImageSymbol_C("TdiShr", "TdiExecute", &TdiExecute);
  if (!(isEnabled(hookName) && STATUS_OK))
    return;
  DESCRIPTOR(expression_d, "TreeShrHook($)");
  static DESCRIPTOR(hooktype_key_d, "type");
  DESCRIPTOR_FROM_CSTRING(hooktype_d, hookName);
  static DESCRIPTOR(tree_key_d, "tree");
  static DESCRIPTOR(shot_key_d, "shot");
  static DESCRIPTOR(nid_key_d, "nid");
  static DESCRIPTOR(data_key_d, "data");
  EMPTYXD(defaultAns);
  struct descriptor_xd *ans = &defaultAns;
  struct descriptor *dict = NULL;
  if (strcmp(hookType, "TreeHook") == 0)
  {
    va_list ap;
    va_start(ap, hookName);
    char *tree = va_arg(ap, char *);
    int shot = va_arg(ap, int);
    struct descriptor_xd *this_ans = va_arg(ap, struct descriptor_xd *);
    DESCRIPTOR_FROM_CSTRING(tree_d, tree);
    DESCRIPTOR_LONG(shot_d, &shot);
    va_end(ap);
    ans = this_ans ? this_ans : &defaultAns;
    struct descriptor *hook_dscs[] = {
        (struct descriptor *)&hooktype_key_d, (struct descriptor *)&hooktype_d,
        (struct descriptor *)&tree_key_d, (struct descriptor *)&tree_d,
        (struct descriptor *)&shot_key_d, (struct descriptor *)&shot_d};
    DESCRIPTOR_APD(hook_d, DTYPE_DICTIONARY, &hook_dscs, 6);
    dict = (struct descriptor *)&hook_d;
    (*TdiExecute)(&expression_d, dict, ans MDS_END_ARG);
  }
  else if (strcmp(hookType, "TreeNidHook") == 0)
  {
    va_list ap;
    va_start(ap, hookName);
    char *tree = va_arg(ap, char *);
    int shot = va_arg(ap, int);
    int nid = va_arg(ap, int);
    struct descriptor_xd *this_ans = va_arg(ap, struct descriptor_xd *);
    DESCRIPTOR_FROM_CSTRING(tree_d, tree);
    DESCRIPTOR_LONG(shot_d, &shot);
    DESCRIPTOR_NID(nid_d, &nid);
    va_end(ap);
    ans = this_ans ? this_ans : &defaultAns;
    struct descriptor *hook_dscs[] = {
        (struct descriptor *)&hooktype_key_d, (struct descriptor *)&hooktype_d,
        (struct descriptor *)&tree_key_d, (struct descriptor *)&tree_d,
        (struct descriptor *)&shot_key_d, (struct descriptor *)&shot_d,
        (struct descriptor *)&nid_key_d, (struct descriptor *)&nid_d};
    DESCRIPTOR_APD(hook_d, DTYPE_DICTIONARY, &hook_dscs, 8);
    dict = (struct descriptor *)&hook_d;
    (*TdiExecute)(&expression_d, dict, ans MDS_END_ARG);
  }
  else if (strcmp(hookType, "TreeNidDataHook") == 0)
  {
    va_list ap;
    va_start(ap, hookName);
    char *tree = va_arg(ap, char *);
    int shot = va_arg(ap, int);
    int nid = va_arg(ap, int);
    struct descriptor *data_d = va_arg(ap, struct descriptor *);
    struct descriptor_xd *this_ans = va_arg(ap, struct descriptor_xd *);
    DESCRIPTOR_FROM_CSTRING(tree_d, tree);
    DESCRIPTOR_LONG(shot_d, &shot);
    DESCRIPTOR_NID(nid_d, &nid);
    va_end(ap);
    ans = this_ans ? this_ans : &defaultAns;
    struct descriptor *hook_dscs[] = {
        (struct descriptor *)&hooktype_key_d, (struct descriptor *)&hooktype_d,
        (struct descriptor *)&tree_key_d, (struct descriptor *)&tree_d,
        (struct descriptor *)&shot_key_d, (struct descriptor *)&shot_d,
        (struct descriptor *)&nid_key_d, (struct descriptor *)&nid_d,
        (struct descriptor *)&data_key_d, data_d};
    DESCRIPTOR_APD(hook_d, DTYPE_DICTIONARY, &hook_dscs, 10);
    dict = (struct descriptor *)&hook_d;
    (*TdiExecute)(&expression_d, dict, ans MDS_END_ARG);
  }
  else
  {
    return;
  }
  MdsFree1Dx(&defaultAns, 0);
}
