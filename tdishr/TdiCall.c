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
/*      TdiCall.C
        Call a routine in an image.
        A CALL has pointers to
        0       descriptor of image logical name (default SYS$SHARE:.EXE)
        1       descriptor of routine name.
        2...    descriptors of (ndesc-2) arguments.
        You must check number of arguments.
        Limit is 255 arguments.
                result = image->entry(in1, ...)

        Or the result type may be given.
                result = image->entry:type(in1, ...)
        where type is F for floating, B for byte, etc.,
        or DSC for a descriptor of any kind supported.
        A pointer to XD and D descriptor will free that descriptor.
        The type is given by the value at the record pointer.

        Ken Klare, LANL P-4     (c)1990,1992
*/
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsplus.h>
#include <mdsshr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr_messages.h>

extern int TdiConcat();
extern int TdiData();
extern int TdiEvaluate();
extern int TdiFaultHandler();
extern int TdiFindImageSymbol();
extern int TdiGetLong();
extern int tdi_put_ident();

#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
_Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")
#endif

    static inline int interlude(dtype_t rtype, mdsdsc_t **newdsc,
                                int (*routine)(), void **result, int *max)
{
  switch (rtype)
  {
  case DTYPE_MISSING:
  {
    LibCallg(newdsc, routine);
    break;
  }
  case DTYPE_C:
  case DTYPE_T:
  case DTYPE_POINTER:
  case DTYPE_DSC:
  {
    void *(*called_p)() = (void *(*)())LibCallg;
    void **result_p = (void *)result;
    *max = sizeof(void *);
    *result_p = called_p(newdsc, routine);
    break;
  }
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FC:
  case DTYPE_FSC:
  case DTYPE_Q:
  case DTYPE_QU:
  { // 8 bytes
    int64_t (*called_q)() = (int64_t(*)())LibCallg;
    int64_t *result_q = (int64_t *)result;
    *max = sizeof(int64_t);
    *result_q = called_q(newdsc, routine);
    break;
  }
  // case DTYPE_F:
  // case DTYPE_FS:
  default:
  { // 4 bytes
    int32_t (*called_int)() = (int32_t(*)())LibCallg;
    int32_t *result_int = (int32_t *)result;
    *max = sizeof(int32_t);
    *result_int = called_int(newdsc, routine);
  }
  }
  return 1;
}

int get_routine(int narg, mdsdsc_t *list[], int (**proutine)())
{
  mdsdsc_xd_t image = EMPTY_XD, entry = EMPTY_XD;
  if (narg > 255 + 2)
    return TdiNDIM_OVER;
  int status = TdiData(list[0], &image MDS_END_ARG);
  if (STATUS_OK)
    status = TdiData(list[1], &entry MDS_END_ARG);
  if (STATUS_OK)
    status = TdiFindImageSymbol(image.pointer, entry.pointer, proutine);
  if (STATUS_NOT_OK)
    printf("%s\n", LibFindImageSymbolErrString());
  MdsFree1Dx(&entry, NULL);
  MdsFree1Dx(&image, NULL);
  return status;
}

typedef struct cleanup_tdi_call
{
  int count;
  mdsdsc_xd_t xds[255];
} cleanup_tdi_call_t;

static void cleanup_tdi_call(void *arg)
{
  cleanup_tdi_call_t *c = (cleanup_tdi_call_t *)arg;
  int i;
  for (i = 0; i < c->count; ++i)
  {
    MdsFree1Dx(&c->xds[i], NULL);
  }
}
#define CUR_POS (c->count - 1)
#define CUR_XD c->xds[CUR_POS]
#define NEW_XD()                     \
  do                                 \
  {                                  \
    c->xds[(c->count++)] = EMPTY_XD; \
  } while (0)

int tdi_call(dtype_t rtype, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr, cleanup_tdi_call_t *c)
{
  int (*routine)();
  int status = get_routine(narg, list, &routine);
  RETURN_IF_STATUS_NOT_OK;
  mds_function_t *pfun;
  int j, max = 0;
  char result[8] = {0}; // we need up to 8 bytes
  unsigned short code;
  mdsdsc_t dx = {0, rtype == DTYPE_C ? DTYPE_T : rtype, CLASS_S, result};
  unsigned char origin[255];
  mdsdsc_t *newdsc[256] = {0};
  *(int *)&newdsc[0] = narg - 2;
  for (j = 2; j < narg && STATUS_OK; ++j)
  {
    for (pfun = (struct descriptor_function *)list[j];
         pfun && pfun->dtype == DTYPE_DSC;)
      pfun = (struct descriptor_function *)pfun->pointer;
    if (pfun && pfun->dtype == DTYPE_FUNCTION)
    {
      code = *(unsigned short *)pfun->pointer;
      if (code == OPC_DESCR)
      {
        NEW_XD();
        origin[CUR_POS] = (unsigned char)j;
        status = TdiData(pfun->arguments[0], &CUR_XD MDS_END_ARG);
        newdsc[j - 1] = (mdsdsc_t *)CUR_XD.pointer;
      }
      else if (code == OPC_REF)
      {
        NEW_XD();
        origin[CUR_POS] = (unsigned char)j;
        status = TdiData(pfun->arguments[0], &CUR_XD MDS_END_ARG);
        if (CUR_XD.pointer)
        {
          if (CUR_XD.pointer->dtype == DTYPE_T)
          {
            DESCRIPTOR(zero, "\0");
            TdiConcat(&CUR_XD, &zero, &CUR_XD MDS_END_ARG);
          }
          newdsc[j - 1] = (mdsdsc_t *)CUR_XD.pointer->pointer;
        }
      }
      else if (code == OPC_VAL)
      {
        int ans;
        EMPTYXD(xd);
        status = TdiData(pfun->arguments[0], &xd MDS_END_ARG);
        if (STATUS_OK && xd.pointer && xd.pointer->dtype == DTYPE_POINTER)
        {
          *(void **)&newdsc[j - 1] = *(void **)xd.pointer->pointer;
        }
        else
        {
          status = TdiGetLong(pfun->arguments[0], &ans);
          *(long *)&newdsc[j - 1] = ans;
        }
        MdsFree1Dx(&xd, 0);
      }
      else if (code == OPC_XD)
      {
        NEW_XD();
        origin[CUR_POS] = (unsigned char)j;
        status = TdiEvaluate(pfun->arguments[0], newdsc[j - 1] = (mdsdsc_t *)&CUR_XD MDS_END_ARG);
      }
      else
        goto fort;
    }
    /********************************************
    Default is DESCR for text and REF for others.
    ********************************************/
    else
    {
    fort:
      NEW_XD();
      origin[CUR_POS] = (unsigned char)j;
      if (list[j])
        status = TdiData(list[j], &CUR_XD MDS_END_ARG);
      newdsc[j - 1] = CUR_XD.pointer;
      if (newdsc[j - 1])
      {
        if (newdsc[j - 1]->dtype != DTYPE_T)
          newdsc[j - 1] = (mdsdsc_t *)newdsc[j - 1]->pointer;
        else
        {
          DESCRIPTOR(zero_dsc, "\0");
          TdiConcat(&CUR_XD, &zero_dsc, &CUR_XD MDS_END_ARG);
          newdsc[j - 1] = (mdsdsc_t *)CUR_XD.pointer->pointer;
        }
      }
    }
  }
  if (STATUS_OK)
    status = interlude(rtype, newdsc, routine, (void **)result, &max);
  if (!out_ptr)
    goto skip;
  if (STATUS_OK)
    switch (rtype)
    {
    case DTYPE_MISSING:
      break;
    case DTYPE_DSC:
      dx.pointer = *(char **)result;
      if (*(void **)result)
        switch ((*(mdsdsc_t **)result)->class)
        {
        case CLASS_XD:
          MdsFree1Dx(out_ptr, NULL);
          *out_ptr = **(mdsdsc_xd_t **)result;
          **(mdsdsc_xd_t **)result = EMPTY_XD;
          goto skip;
        case CLASS_D:
          MdsFree1Dx(out_ptr, NULL);
          *(mdsdsc_t *)out_ptr = **(mdsdsc_t **)result;
          (*(mdsdsc_t **)result)->length = 0;
          (*(mdsdsc_t **)result)->pointer = NULL;
          goto skip;
        default:
          break;
        }
      break;
    case DTYPE_C: /* like T but with free */
    case DTYPE_T:
    case DTYPE_PATH:
    case DTYPE_EVENT:
      if (!*(char **)result)
        goto skip;
      dx.length = (length_t)strlen(dx.pointer = *(char **)result);
      break;
    case DTYPE_NID:
      dx.length = sizeof(int);
      break;
    case DTYPE_POINTER:
      dx.length = sizeof(void *);
      break;
    default:
    {
      if ((int)rtype < TdiCAT_MAX)
      {
        if ((dx.length = TdiREF_CAT[(int)rtype].length) > max)
          status = TdiTOO_BIG;
      }
      else
        status = TdiINVDTYDSC;
    }
    }
  if (STATUS_OK)
    status = MdsCopyDxXd(&dx, out_ptr);
  if (rtype == DTYPE_C)
    free(*(char **)result); // free result
skip:
  for (j = 0; j <= CUR_POS; ++j)
  {
    for (pfun = (mds_function_t *)list[origin[j]];
         pfun && pfun->dtype == DTYPE_DSC;)
      pfun = (mds_function_t *)pfun->pointer;
    if (pfun && pfun->dtype == DTYPE_FUNCTION)
    {
      code = *(length_t *)pfun->pointer;
      if (code == OPC_DESCR || code == OPC_REF || code == OPC_XD)
        pfun = (mds_function_t *)pfun->arguments[0];
      if (pfun && pfun->dtype == DTYPE_IDENT)
        tdi_put_ident(pfun, &c->xds[j]);
    }
  }
  return status;
}

int TdiCall(dtype_t rtype, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  int status;
  cleanup_tdi_call_t c;
  c.count = 0;
  pthread_cleanup_push(cleanup_tdi_call, (void *)&c);
  status = tdi_call(rtype, narg, list, out_ptr, &c);
  pthread_cleanup_pop(1);
  return status;
}