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
#include <mdsdescrip.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <string.h>

static int CountRefs(struct descriptor *src);
static void FillRefs(struct descriptor *src, int *ans, int *idx);
static int GetNid(struct descriptor *dsc);

static EMPTYXD(src);
EXPORT int GetReferenceCount(int *nid)
{
  int cnt = 0;
  int status = TreeGetRecord(*nid, &src);
  if (status & 1) {
    cnt = CountRefs((struct descriptor *)&src);
  }
  return cnt;
}

EXPORT void GetReferences(int *nid __attribute__ ((unused)), int *ans)
{
  int idx = 0;
  FillRefs((struct descriptor *)&src, ans, &idx);
  MdsFree1Dx(&src, 0);
}

static void FillRefs(struct descriptor *src, int *ans, int *idx)
{
  for (; src->dtype == DTYPE_DSC; src = (struct descriptor *)src->pointer) ;
  if ((src->dtype == DTYPE_NID) || (src->dtype == DTYPE_PATH))
    if (src->class == CLASS_A) {
      struct descriptor_a *aptr = (struct descriptor_a *)src;
      unsigned int i;
      for (i = 0; i < aptr->arsize / aptr->length; i++) {
	ans[*idx] = GetNid(((struct descriptor *)aptr->pointer) + i);
	(*idx)++;
      }
    } else {
      ans[*idx] = GetNid(src);
      (*idx)++;
  } else if (src->class == CLASS_R) {
    struct descriptor_r *rptr = (struct descriptor_r *)src;
    int i;
    for (i = 0; i < rptr->ndesc; i++)
      FillRefs(rptr->dscptrs[i], ans, idx);
  }
}

static int CountRefs(struct descriptor *src)
{
  /* remove any leading dsc descriptors */
  for (; src->dtype == DTYPE_DSC; src = (struct descriptor *)src->pointer) ;
  if ((src->dtype == DTYPE_NID) || (src->dtype == DTYPE_PATH))
    if (src->class == CLASS_A) {
      struct descriptor_a *aptr = (struct descriptor_a *)src;
      return aptr->arsize / aptr->length;
    } else
      return 1;
  else if (src->class == CLASS_R) {
    struct descriptor_r *rptr = (struct descriptor_r *)src;
    int count = 0;
    int i;
    for (i = 0; i < rptr->ndesc; i++)
      count += CountRefs(rptr->dscptrs[i]);
    return count;
  } else
    return 0;
}

static int GetNid(struct descriptor *dsc)
{
  if (dsc->dtype == DTYPE_NID)
    return *(int *)dsc->pointer;
  else {
    int ans;
    char *tmp = strncpy(malloc(dsc->length + 1), dsc->pointer, dsc->length);
    int status;
    tmp[dsc->length] = 0;
    status = TreeFindNode(tmp, &ans);
    free(tmp);
    if ((status & 1) == 0)
      return -1;
    else
      return ans;
  }
}
