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
/*      Tdi1Var.C
        Find a variable by name and other goodies.
        Function definition and execution:
                FUN [PRIVATE|PUBLIC] ident ([OPTIONAL][AS_IS|IN|INOUT|OUT]
   ident...) stmt var (var...)                    invocation Inquires: ALLOCATED
   (var)                 test defined PRESENT (var)                   test in
   argument list or defined Fetches (these are the "var"): [PRIVATE|PUBLIC]
   ident          by data type / special function VAR ("text expression") fetch
   by expression Stores: var = expression                assignment var +=
   expression               binary operator and assignment VAR ("text", expr)
   fetch/store by text expression Changes:
                ++ [PRIVATE|PUBLIC] var         pre-increment
                -- [PRIVATE|PUBLIC] var         pre-decrement
                [PRIVATE|PUBLIC] var ++         post-increment
                [PRIVATE|PUBLIC] var --         post-decrement
                DEALLOCATE (var...)             free data field
        Utilities:
                find_ident(search, var, [key], [node_ptr], [block_ptr])
                tdi_get_ident(var, data)  fetch variable
                tdi_put_ident(var, data)  store variable
                RESET_PRIVATE() RESET_PUBLIC()  release variables
                SHOW_PRIVATE([text],...)
                SHOW_PUBLIC([text],...)         display variables

        FUN arguments declared:
                AS_IS overrides evaluation.
                not OPTIONAL must be present and not null.
                IN are evaluated and must not be changed.
                INOUT or OUT must be variables.
                OUT are initially undefined and may not be evaluated until
   defined. Ken Klare, LANL P-4     (c)1989,1990,1991,1992 NEED subscripted
   assignment.
*/
#include "tdirefstandard.h"
#include "tdithreadstatic.h"
#include <libroutines.h>
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <strroutines.h>

#define LibVM_EXTEND_AREA 32
#define LibVM_TAIL_LARGE 128
#include <ctype.h>
#include <inttypes.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr_messages.h>

// #define DEBUG
#include <mdsmsg.h>

extern int TdiFaultHandler();
extern int TdiData();
extern int TdiDeallocate();
extern int TdiDecompile();
extern int TdiEvaluate();
extern int TdiPutLong();
extern int TdiEquals();
extern int TdiSubtract();
extern int TdiAdd();
extern int TdiResetGetRecord();
/*******************************************
This uses the balanced binary tree routines.
LOOKUP_TREE INSERT_TREE TRAVERSE_TREE
Full descriptors cost memory, save time.
*******************************************/
typedef struct link
{
  struct link *left, *right; /* binary tree links */
  unsigned short reserved;   /* tree flags */
  mdsdsc_xd_t xd;            /* data descriptor */
  mdsdsc_t name_dsc;         /* name descriptor */
  unsigned char name[1];     /* unchanging name */
} node_type;
typedef struct
{
  node_type *head; /* changing binary-tree head */
  void *head_zone; /* created on first call */
  void *data_zone; /* created on first call */
  int _public;     /* is public */
} block_type;
typedef struct
{
  mdsdsc_t match;
  block_type *block_ptr;
  int count;
} user_type;

const mdsdsc_d_t EMPTY_D = {0, DTYPE_T, CLASS_D, 0};
const mdsdsc_t EMPTDY_S = {0, DTYPE_T, CLASS_S, 0};
const mdsdsc_xd_t NULL_XD = {0, 0, 0, 0, 0};

const unsigned char true = 1;
const mdsdsc_t true_dsc = {sizeof(true), DTYPE_BU, CLASS_S, (char *)&true};

static pthread_mutex_t public_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_PUBLIC pthread_mutex_lock(&public_lock)
//;fprintf(stderr,"public locked by %ld\n",*(long int*)pthread_self())
#define UNLOCK_PUBLIC_PUSH \
  pthread_cleanup_push((void *)pthread_mutex_unlock, &public_lock)
#define LOCK_PUBLIC_PUSH \
  LOCK_PUBLIC;           \
  UNLOCK_PUBLIC_PUSH
#define UNLOCK_PUBLIC pthread_cleanup_pop(1)
//;fprintf(stderr,"public unlocked by %ld\n",*(long int*)pthread_self())
#define UNLOCK_PUBLIC_IF(cond) pthread_cleanup_pop(cond)
//;if (cond) fprintf(stderr,"public unlocked by %ld\n",*(long
//int*)pthread_self())
#define UNLOCK_IF_PUBLIC(ptr) UNLOCK_PUBLIC_IF(ptr == &_public)
static block_type _public = {0, 0, 0, 1};

const DESCRIPTOR(star, "*");
const DESCRIPTOR(percent, "%");
mdsdsc_t *Tdi3Narg()
{
  TDITHREADSTATIC_INIT;
  return &TDI_VAR_NEW_NARG_D;
}

/*--------------------------------------------------------------
        Put a byte to output.
*/
int tdi_put_logical(uint8_t data, mdsdsc_xd_t *const out_ptr)
{
  INIT_STATUS;
  length_t len = (length_t)sizeof(uint8_t);
  dtype_t dtype = DTYPE_BU;
  if (!out_ptr)
    return TdiNULL_PTR;
  status = MdsGet1DxS(&len, &dtype, out_ptr);
  if (STATUS_OK)
    *(uint8_t *)out_ptr->pointer->pointer = data;
  return status;
}

/*--------------------------------------------------------------
        Comparison routine.
*/
static int compare(const mdsdsc_t *const key_ptr,
                   const node_type *const node_ptr,
                   const block_type *const block_ptr __attribute__((unused)))
{
  return StrCaseBlindCompare(key_ptr, &node_ptr->name_dsc);
}

/*--------------------------------------------------------------
        Allocation routine, does not set data field.
        ASSUMED called only once per node.
*/
static int allocate(const mdsdsc_t *const key_ptr,
                    node_type **const node_ptr_ptr,
                    block_type *const block_ptr)
{
  INIT_STATUS;
  unsigned int len = sizeof(struct link) - 1 + key_ptr->length;

  /*******************************************************************
  Must clear memory unless allocate is called only once for each node.
  *******************************************************************/
  if (block_ptr->data_zone == 0)
  {
    LibCreateVmZone(&block_ptr->head_zone);
    LibCreateVmZone(&block_ptr->data_zone);
  }
  status = LibGetVm(&len, (void *)node_ptr_ptr, &block_ptr->head_zone);
  if (STATUS_OK)
  {
    node_type *node_ptr = *node_ptr_ptr;
    node_ptr->xd = EMPTY_XD;
    node_ptr->name_dsc = *key_ptr;
    node_ptr->name_dsc.pointer = (char *)&node_ptr->name[0];
    memcpy(node_ptr->name, key_ptr->pointer, key_ptr->length);
  }
  return status;
}

/*--------------------------------------------------------------
        Find active variable.
        Look in private and public lists.
        search: 1=private 2=public 4=check that it exists.
        if block_ptr is returned and public public_lock is acquired
*/
#define _private (*(block_type *)&TDI_VAR_PRIVATE)
static int find_ident(const int search, const mdsdsc_r_t *const ident_ptr,
                      mdsdsc_t *const key_ptr, node_type **const node_ptr_ptr,
                      block_type **const block_ptr_ptr, TDITHREADSTATIC_ARG)
{
  INIT_STATUS;
  mdsdsc_t key_dsc;
  mdsdsc_d_t name_dsc;
  node_type *node_ptr;
  int code;

  if (ident_ptr == 0)
    status = TdiNULL_PTR;
  else
    switch (ident_ptr->class)
    {
    case CLASS_S:
    case CLASS_D:
      switch (ident_ptr->dtype)
      {
        /********************************************
        The choice of default public/private is here.
        ********************************************/
      case DTYPE_T:
      case DTYPE_IDENT:
      {
        key_dsc = *(mdsdsc_t *)ident_ptr;
        key_dsc.dtype = DTYPE_T;
        key_dsc.class = CLASS_S;
        if (search & 1)
        {
          if (search & 4)
          {
            status = LibLookupTree((void *)&_private.head, (void *)&key_dsc,
                                   compare, (void **)&node_ptr);
            if (STATUS_OK)
            {
              if (node_ptr->xd.class != 0)
              {
                if (block_ptr_ptr)
                  *block_ptr_ptr = &_private;
                break;
              }
              else
              {
                status = TdiUNKNOWN_VAR;
              }
            }
          }
          else
          {
            if (block_ptr_ptr)
              *block_ptr_ptr = &_private;
            break;
          }
        }
        if (search & 2)
        {
          if (search & 4)
          {
            LOCK_PUBLIC_PUSH;
            status = LibLookupTree((void *)&_public.head, (void *)&key_dsc,
                                   compare, (void **)&node_ptr);
            if (STATUS_OK)
            {
              if (node_ptr->xd.class != 0)
              {
                if (block_ptr_ptr)
                  *block_ptr_ptr = &_public;
              }
              else
              {
                status = TdiUNKNOWN_VAR;
              }
            }

            UNLOCK_PUBLIC_IF(!(block_ptr_ptr && *block_ptr_ptr == &_public));
          }
          else if (block_ptr_ptr)
          {
            LOCK_PUBLIC;
            *block_ptr_ptr = &_public;
          }
        }
        break;
      }
      default:
        status = TdiINVDTYDSC;
        break;
      }
      break;
    case CLASS_R:
      if (ident_ptr->dtype != DTYPE_FUNCTION)
      {
        status = TdiINVCLADTY;
        break;
      }
      code = *(unsigned short *)ident_ptr->pointer;
      if (ident_ptr->ndesc < 1)
      {
        status = TdiMISS_ARG;
        break;
      }
      /***************************************
      Could have (public _a+=6)++ as argument.
      So strip ++ and += and use public.
      ***************************************/
      if (code == OPC_PUBLIC)
        status =
            find_ident(search & ~1, (mdsdsc_r_t *)ident_ptr->dscptrs[0],
                       &key_dsc, &node_ptr, block_ptr_ptr, TDITHREADSTATIC_VAR);
      else if (code == OPC_PRIVATE)
        status =
            find_ident(search & ~2, (mdsdsc_r_t *)ident_ptr->dscptrs[0],
                       &key_dsc, &node_ptr, block_ptr_ptr, TDITHREADSTATIC_VAR);
      else if (code == OPC_EQUALS || code == OPC_EQUALS_FIRST ||
               code == OPC_POST_DEC || code == OPC_PRE_DEC ||
               code == OPC_POST_INC || code == OPC_PRE_INC)
      {
        INIT_AND_FREEXD_ON_EXIT(tmp);
        status = TdiEvaluate(ident_ptr, &tmp MDS_END_ARG);
        if (STATUS_OK)
          status =
              find_ident(search, (mdsdsc_r_t *)ident_ptr->dscptrs[0], &key_dsc,
                         &node_ptr, block_ptr_ptr, TDITHREADSTATIC_VAR);
        FREEXD_NOW(tmp);
      }
      else if (code == OPC_VAR)
      {
        name_dsc = EMPTY_D;
        status = TdiData(ident_ptr->dscptrs[0], &name_dsc MDS_END_ARG);
        if (STATUS_OK)
          status = find_ident(search, (mdsdsc_r_t *)&name_dsc, &key_dsc,
                              &node_ptr, block_ptr_ptr, TDITHREADSTATIC_VAR);
        StrFree1Dx((mdsdsc_d_t *)&name_dsc);
      }
      else
        status = TdiINV_OPC;
      break;
    default:
      status = TdiINVCLADSC;
      break;
    }
  if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  if (STATUS_OK || status == TdiUNKNOWN_VAR)
  {
    if (key_ptr)
      *key_ptr = key_dsc;
    if (node_ptr_ptr)
      *node_ptr_ptr = node_ptr;
  }
  else
  {
    if (node_ptr_ptr)
      *node_ptr_ptr = NULL;
    if (block_ptr_ptr)
      *block_ptr_ptr = NULL;
  }
  return status;
}

/*--------------------------------------------------------------
        Get active variable for VAR and EVALUATE. Copies to MDSSHR space.
*/
static int get_ident(const mdsdsc_t *const ident_ptr,
                     mdsdsc_xd_t *const data_ptr, TDITHREADSTATIC_ARG)
{
  node_type *node_ptr;
  block_type *block_ptr = NULL;
  INIT_STATUS;
  UNLOCK_PUBLIC_PUSH;
  status = find_ident(7, (mdsdsc_r_t *)ident_ptr, 0, &node_ptr, &block_ptr,
                      TDITHREADSTATIC_VAR);
#ifdef DEBUG
  char string[64];
  memcpy(string, ident_ptr->pointer, ident_ptr->length);
  string[ident_ptr->length] = '\0';
  fprintf(stderr, "GET: %s %s     block=%p\n",
          block_ptr == &_public ? "public" : STATUS_OK ? "private" : "new",
          string, block_ptr);
#endif
  if (STATUS_OK)
    status = MdsCopyDxXd(node_ptr->xd.pointer, data_ptr);
  else
    MdsFree1Dx(data_ptr, NULL);
  UNLOCK_IF_PUBLIC(block_ptr);
  return status;
}
int tdi_get_ident(const mdsdsc_t *const ident_ptr,
                  mdsdsc_xd_t *const data_ptr)
{
  TDITHREADSTATIC_INIT;
  return get_ident(ident_ptr, data_ptr, TDITHREADSTATIC_VAR);
}

/*--------------------------------------------------------------
        Keep an XD-DSC of whatever.
        Assumes we are given XD or class-0.
*/
static inline int put_ident_inner(const mdsdsc_r_t *const ident_ptr,
                                  mdsdsc_xd_t *const data_ptr,
                                  TDITHREADSTATIC_ARG)
{
  INIT_STATUS;
  block_type *block_ptr = NULL;
  UNLOCK_PUBLIC_PUSH;
  mdsdsc_t key_dsc = EMPTDY_S;
  node_type *node_ptr;
  mdsdsc_d_t upstr = {0, DTYPE_T, CLASS_D, 0};
  /************************************
  Find where we should place the stuff.
  ************************************/
  status =
      find_ident(3, ident_ptr, &key_dsc, 0, &block_ptr, TDITHREADSTATIC_VAR);
  StrUpcase((mdsdsc_t *)&upstr, &key_dsc);
#ifdef DEBUG
  char string[64];
  memcpy(string, key_dsc.pointer, key_dsc.length);
  string[key_dsc.length] = '\0';
  fprintf(stderr, "PUT: %s %s     block=%p\n",
          block_ptr == &_public ? "public" : STATUS_OK ? "private" : "new",
          string, block_ptr);
#endif
  if (STATUS_OK)
  {
    int zero = 0;
    status = LibInsertTree((void **)&block_ptr->head, &upstr, &zero, compare,
                           allocate, (void *)&node_ptr, block_ptr);
  }
  StrFree1Dx(&upstr);
  if (STATUS_OK)
  {
    if (node_ptr->xd.class == 0)
      node_ptr->xd = EMPTY_XD;
    if (data_ptr)
      status = MdsCopyDxXdZ(data_ptr->pointer, &node_ptr->xd,
                            &block_ptr->data_zone, NULL, NULL, NULL, NULL);
    else
    {
      MdsFree1Dx(&node_ptr->xd, &block_ptr->data_zone);
      node_ptr->xd = NULL_XD;
    }
  }
  UNLOCK_IF_PUBLIC(block_ptr);
  return status;
}
static int put_ident(const mdsdsc_r_t *ident_ptr, mdsdsc_xd_t *const data_ptr,
                     TDITHREADSTATIC_ARG)
{
  while (ident_ptr->dtype == DTYPE_DSC)
    ident_ptr = (mdsdsc_r_t *)ident_ptr->pointer;
  return put_ident_inner(ident_ptr, data_ptr, TDITHREADSTATIC_VAR);
}
int tdi_put_ident(const mdsdsc_r_t *ident_ptr, mdsdsc_xd_t *const data_ptr)
{
  TDITHREADSTATIC_INIT;
  while (ident_ptr->dtype == DTYPE_DSC)
    ident_ptr = (mdsdsc_r_t *)ident_ptr->pointer;
  return put_ident_inner(ident_ptr, data_ptr, TDITHREADSTATIC_VAR);
}

/***************************************************************
        Display/free wildcarded variables.
        IDENT names are not evaluated.
        PUBLIC or PRIVATE(ident or text) overrides block_ptr.
*/
static int wild_loop(int (*const doit)(), const mdsdsc_t *const arg,
                     user_type *const user_p, TDITHREADSTATIC_ARG)
{
  int status;
  UNLOCK_PUBLIC_PUSH;
  status = find_ident(3, (mdsdsc_r_t *)arg, &user_p->match, 0,
                      &user_p->block_ptr, TDITHREADSTATIC_VAR);
  if (STATUS_OK)
    status = StrUpcase(&user_p->match, &user_p->match);
  if (STATUS_OK)
  {
    if (StrPosition(&user_p->match, (mdsdsc_t *)&star, 0) ||
        StrPosition(&user_p->match, (mdsdsc_t *)&percent, 0))
    {
      status =
          LibTraverseTree((void **)&user_p->block_ptr->head, doit, user_p);
    }
    else
    {
      node_type *node_ptr;
      status =
          LibLookupTree((void **)&user_p->block_ptr->head,
                        (void *)&user_p->match, compare, (void **)&node_ptr);
      if (STATUS_OK)
        status = (*doit)(node_ptr, user_p);
      else if (status == LibKEYNOTFOU)
        status = MDSplusSUCCESS;
    }
  }
  UNLOCK_IF_PUBLIC(user_p->block_ptr);
  return status;
}
static int wild(int (*const doit)(), const int narg, mdsdsc_t *const list[],
                block_type *const block_ptr, mdsdsc_xd_t *const out_ptr,
                TDITHREADSTATIC_ARG)
{
  INIT_STATUS;
  user_type user = {{0, DTYPE_T, CLASS_D, 0}, 0, 0};
  user.block_ptr = block_ptr;
  if (narg == 0 || list[0] == 0)
    status = LibTraverseTree((void **)&block_ptr->head, doit, &user);
  else
  {
    int j;
    for (j = 0; STATUS_OK && j < narg; ++j)
      status = wild_loop(doit, list[j], &user, TDITHREADSTATIC_VAR);
  }
  if (user.match.class == CLASS_D)
    StrFree1Dx((mdsdsc_d_t *)&user.match);
  if (STATUS_OK)
    status = TdiPutLong(&user.count, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Release variables.
*/
static int free_one(node_type *const node_ptr, user_type *const user_ptr)
{
  block_type *block_ptr = user_ptr->block_ptr;
  INIT_STATUS;

  /***************************************************
  Deallocated variables are not displayed or re-freed.
  ***************************************************/
  if (node_ptr->xd.class == 0)
    return MDSplusSUCCESS;
  if (user_ptr->match.length == 0)
    status = MDSplusSUCCESS;
  else
    status = StrMatchWild(&node_ptr->name_dsc, &user_ptr->match);
  if (STATUS_OK)
  {
    if (node_ptr->xd.l_length)
      status =
          LibFreeVm(&node_ptr->xd.l_length, (void *)&node_ptr->xd.pointer,
                    &block_ptr->data_zone);
    node_ptr->xd = NULL_XD;
    user_ptr->count++;
  }
  else
    status = MDSplusSUCCESS;
  return status;
}

/*--------------------------------------------------------------
        Release all variables under a header and the headers too.
        Used to release the FUN variables.
*/
static int free_all(node_type **const pnode, TDITHREADSTATIC_ARG)
{
  INIT_STATUS, stat2;
  unsigned int len;
  if ((*pnode)->xd.l_length)
    status = LibFreeVm(&(*pnode)->xd.l_length, (void *)&(*pnode)->xd.pointer,
                       &_private.data_zone);
  if ((*pnode)->left)
  {
    stat2 = free_all(&((*pnode)->left), TDITHREADSTATIC_VAR);
    if (STATUS_OK)
      status = stat2;
  }
  if ((*pnode)->right)
  {
    stat2 = free_all(&((*pnode)->right), TDITHREADSTATIC_VAR);
    if (STATUS_OK)
      status = stat2;
  }
  len = sizeof(struct link) - 1 + (*pnode)->name_dsc.length;
  stat2 = LibFreeVm(&len, (void *)pnode, &_private.head_zone);
  *pnode = 0;
  if (STATUS_OK)
    status = stat2;
  return status;
}

/*--------------------------------------------------------------
        Release variables.
*/
int Tdi1Deallocate(opcode_t opcode __attribute__((unused)), int narg,
                   mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  if (narg == 0 && _private.head)
  {
    status = free_all(&_private.head, TDITHREADSTATIC_VAR);
    _private.head = NULL;
    return status;
  }
  return wild((int (*)())free_one, narg, list, &_private, out_ptr,
              TDITHREADSTATIC_VAR); // runs on public and private, or clears
                                    // private if no args
}

/*--------------------------------------------------------------
        Check for allocated variable, private only by default.
*/
int Tdi1Allocated(opcode_t opcode __attribute__((unused)),
                  int narg __attribute__((unused)), mdsdsc_t *list[],
                  mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  mdsdsc_t key_dsc = EMPTDY_S;
  node_type *node_ptr;
  block_type *block_ptr;
  unsigned char found;
  UNLOCK_PUBLIC_PUSH;
  status = find_ident(3, (mdsdsc_r_t *)list[0], &key_dsc, 0, &block_ptr,
                      TDITHREADSTATIC_VAR);
  if (STATUS_OK)
    status = LibLookupTree((void **)&block_ptr->head, (void *)&key_dsc, compare,
                           (void **)&node_ptr);
  found = STATUS_OK && (node_ptr->xd.class != 0);
  UNLOCK_IF_PUBLIC(block_ptr);
  if ((status == LibKEYNOTFOU) || (status == TdiUNKNOWN_VAR))
    status = MDSplusSUCCESS;
  if (STATUS_OK)
    status = tdi_put_logical(found, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Check for argument present.
*/
int Tdi1Present(opcode_t opcode __attribute__((unused)),
                int narg __attribute__((unused)), mdsdsc_t *list[],
                mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  mdsdsc_t key_dsc = EMPTDY_S;
  node_type *node_ptr;
  block_type *block_ptr;
  int found;
  UNLOCK_PUBLIC_PUSH;
  status = find_ident(3, (mdsdsc_r_t *)list[0], &key_dsc, 0, &block_ptr,
                      TDITHREADSTATIC_VAR);
  if (STATUS_OK)
    status = LibLookupTree((void **)&block_ptr->head, (void *)&key_dsc, compare,
                           (void **)&node_ptr);
  UNLOCK_IF_PUBLIC(block_ptr);
  found = STATUS_OK;
  if (found)
    ;
  else if (status == LibKEYNOTFOU || status == TdiUNKNOWN_VAR)
    status = MDSplusSUCCESS;
  if (STATUS_OK)
    status = tdi_put_logical((unsigned char)found, out_ptr);
  return status;
}

/***************************************************************
        Execute a function. Set up the arguments.
*/
typedef enum
{
  EXT_NONE,
  EXT_FUN,
  EXT_PY
} ext_t;

static inline ext_t matchext(const mdsdsc_d_t *const file)
{
  if (file->length < 4)
    return EXT_NONE;
  char *p = file->pointer + file->length - 4;
  char ext[5] = {p[0], toupper(p[1]), toupper(p[2]), toupper(p[3]), '\0'};
  if (strcmp(ext, ".FUN") == 0)
    return EXT_FUN;
  if (strcmp(ext + 1, ".PY") == 0)
    return EXT_PY;
  return EXT_NONE;
}
static inline int findfile_fun(const mdsdsc_t *const entry,
                               char **const funfile, char **const pyfile)
{
  int status;
  INIT_AND_FREED_ON_EXIT(bufd, DTYPE_T);
  ext_t isext = EXT_NONE;
  {
    const char ext[] = ".*";
    const char env[] = "MDS_PATH:";
    const size_t extlen = sizeof(ext) - 1, envlen = sizeof(env) - 1;
    bufd.length = envlen + entry->length + extlen;
    char *tmp = bufd.pointer = malloc(bufd.length);
    memcpy(tmp, env, envlen);
    tmp += envlen; // adds "<env>:"
    memcpy(tmp, entry->pointer, entry->length);
    tmp += entry->length;     // adds "<entry>"
    memcpy(tmp, ext, extlen); // adds "<ext>\0"
  }
  void *ctx = 0;
  do
  {
    status =
        LibFindFileRecurseCaseBlind((mdsdsc_t *)&bufd, (mdsdsc_t *)&bufd, &ctx);
  } while (STATUS_OK && (isext = matchext(&bufd)) == EXT_NONE);
  LibFindFileEnd(&ctx);
  if (STATUS_OK)
  {
    char *file = memcpy(malloc(bufd.length + 1), bufd.pointer, bufd.length);
    file[bufd.length] = '\0';
    if (isext == EXT_PY)
    {
      *pyfile = file;
      isext = EXT_FUN;
      bufd.pointer = realloc(bufd.pointer, ++bufd.length);
      memcpy(bufd.pointer + bufd.length - 4, ".FUN", 4);
    }
    else
    {
      *funfile = file;
      isext = EXT_PY;
      bufd.pointer = realloc(bufd.pointer, --bufd.length);
      memcpy(bufd.pointer + bufd.length - 3, ".PY", 3);
    }
    if (IS_OK(
            LibFindFileCaseBlind((mdsdsc_t *)&bufd, (mdsdsc_t *)&bufd, &ctx)))
    {
      file = memcpy(malloc(bufd.length + 1), bufd.pointer, bufd.length);
      file[bufd.length] = '\0';
      if (isext == EXT_PY)
        *pyfile = file;
      else
        *funfile = file;
    }
    LibFindFileEnd(&ctx);
  }
  FREED_NOW(bufd);
  return status;
}

extern int TdiCompile();
static int compile_fun(const mdsdsc_t *const entry, const char *const file)
{
  if (!file)
    return TdiUNKNOWN_VAR;
  int status;
  INIT_AND_FREEXD_ON_EXIT(tmp);
  MDSDBG("compile: %s\n", file);
  FILE *unit = fopen(file, "rb");
  if (unit)
  {
    fseek(unit, 0, SEEK_END);
    size_t flen = ftell(unit);
    if (flen > 0xffff)
    {
      fclose(unit);
      status = TdiSTRTOOLON;
    }
    else
    {
      fseek(unit, 0, SEEK_SET);
      mdsdsc_t dcs = {flen, DTYPE_T, CLASS_D, (char *)malloc(flen)};
      FREED_ON_EXIT(&dcs);
      size_t readlen = fread(dcs.pointer, 1, flen, unit);
      if (readlen < flen)
        perror("Error reading tdi function\n");
      fclose(unit);
      status = TdiCompile(&dcs, &tmp MDS_END_ARG);
      FREED_NOW(&dcs);
    }
  }
  else
    status = TdiUNKNOWN_VAR;
  if (STATUS_OK)
  {
    mds_function_t *pfun = (mds_function_t *)tmp.pointer;
    if (pfun->dtype == DTYPE_FUNCTION)
    {
      unsigned short code = *(unsigned short *)pfun->pointer;
      if (code == OPC_FUN)
      {
        mds_function_t *pfun2 = (mds_function_t *)pfun->arguments[0];
        if (pfun2->dtype == DTYPE_FUNCTION)
        {
          code = *(unsigned short *)pfun2->pointer;
          if (code == OPC_PRIVATE || code == OPC_PUBLIC)
            pfun2 = (mds_function_t *)pfun2->arguments[0];
        }
        StrUpcase((mdsdsc_t *)pfun2, (mdsdsc_t *)pfun2);
        if (StrCompare(entry, (mdsdsc_t *)pfun2) == 0)
          status = TdiEvaluate(&tmp, &tmp MDS_END_ARG);
      }
    }
  }
  FREEXD_NOW(&tmp);
  return status;
}

extern int tdi_load_python_fun(const char *const filepath,
                               char **const funname);
extern int tdi_call_python_fun(const char *const filename, const int nargs,
                               const mdsdsc_r_t *const *const args,
                               mdsdsc_xd_t *const out_ptr);
static int find_fun(const mdsdsc_t *const ident_ptr, node_type **const node_ptr,
                    TDITHREADSTATIC_ARG)
{
  int status = find_ident(7, (mdsdsc_r_t *)ident_ptr, 0, node_ptr, 0,
                          TDITHREADSTATIC_VAR);
  if (status == TdiUNKNOWN_VAR)
  {
    INIT_AND_FREE_ON_EXIT(char *, pyfile);
    INIT_AND_FREE_ON_EXIT(char *, funfile);
    // check if we can find method as either .py or .fun
    status = findfile_fun(ident_ptr, &funfile, &pyfile);
    if (pyfile)
    {
      char *funname;
      status = tdi_load_python_fun(pyfile, &funname);
      if (STATUS_OK)
      {
        mdsdsc_t function = {strlen(funname), DTYPE_T, CLASS_S, funname};
        mdsdsc_xd_t tmp = EMPTY_XD;
        status = MdsCopyDxXd((mdsdsc_t *)&function, &tmp);
        free(funname);
        if (STATUS_OK)
        {
          status =
              put_ident((mdsdsc_r_t *)ident_ptr, &tmp, TDITHREADSTATIC_VAR);
          MdsFree1Dx(&tmp, NULL);
        }
      }
      if (STATUS_NOT_OK)
        // unable to load python method try tdi alternative
        status = compile_fun(ident_ptr, funfile);
    }
    else // not a python method, load tdi fun
      status = compile_fun(ident_ptr, funfile);
    FREE_NOW(funfile);
    FREE_NOW(pyfile);
    if (STATUS_OK)
      status = find_ident(7, (mdsdsc_r_t *)ident_ptr, 0, node_ptr, 0,
                          TDITHREADSTATIC_VAR);
  }
  return status;
}

static pthread_mutex_t lock;
static void unlock(TDITHREADSTATIC_ARG)
{
  TDI_VAR_REC = FALSE;
  pthread_mutex_unlock(&lock);
}
int TdiDoFun(const mdsdsc_t *const ident_ptr, const int nactual,
             const mdsdsc_r_t *const actual_arg_ptr[],
             mdsdsc_xd_t *const out_ptr)
{
  TDITHREADSTATIC_INIT;
  node_type *node_ptr;
  /******************************************
  Get name of function to do. Check its type.
  ******************************************/
  int status;
  // look up method: check cached, check python, or load
  if (TDI_VAR_REC)
    status = find_fun(ident_ptr, &node_ptr, TDITHREADSTATIC_VAR);
  else
  {
    pthread_mutex_lock(&lock);
    TDI_VAR_REC = TRUE;
    pthread_cleanup_push((void *)unlock, (void *)TDITHREADSTATIC_VAR);
    status = find_fun(ident_ptr, &node_ptr, TDITHREADSTATIC_VAR);
    pthread_cleanup_pop(1);
  }
  if (STATUS_NOT_OK)
    return status;
  const mdsdsc_r_t *formal_ptr = 0, *formal_arg_ptr, *actual_ptr;
  if ((formal_ptr = (mdsdsc_r_t *)node_ptr->xd.pointer) == 0)
    return TdiUNKNOWN_VAR;
  if (formal_ptr->dtype == DTYPE_T)
  {
    char *funname = malloc(formal_ptr->length + 1);
    FREE_ON_EXIT(funname);
    memcpy(funname, formal_ptr->pointer, formal_ptr->length);
    funname[formal_ptr->length] = '\0';
    status = tdi_call_python_fun(funname, nactual, actual_arg_ptr, out_ptr);
    FREE_NOW(funname);
    return status;
  }
  if (formal_ptr->dtype != DTYPE_FUNCTION ||
      *(unsigned short *)formal_ptr->pointer != OPC_FUN)
    return TdiUNKNOWN_VAR;
  int code, opt, j, nformal = 0;
  if ((nformal = formal_ptr->ndesc - 2) < nactual)
    return TdiEXTRA_ARG;
  mdsdsc_xd_t tmp = EMPTY_XD;
  node_type *old_head, *new_head = 0;
  int old_narg = TDI_VAR_NEW_NARG;
  /**************************************
   Now copy input arguments into new head.
   Pick up some keywords from prototype.
   Only OPTIONAL arguments may be omitted.
   OUT arguments are not evaluated.
   **************************************/
  for (j = 0; j < nformal && STATUS_OK; ++j)
  {
    formal_arg_ptr = (mdsdsc_r_t *)formal_ptr->dscptrs[j + 2];
    if (formal_arg_ptr == 0)
    {
      status = TdiNULL_PTR;
      break;
    }
    if (formal_arg_ptr->dtype == DTYPE_FUNCTION && formal_arg_ptr->ndesc == 1)
    {
      code = *(unsigned short *)formal_arg_ptr->pointer;
      opt = (code == OPC_OPTIONAL);
      if (opt)
      {
        formal_arg_ptr = (mdsdsc_r_t *)formal_arg_ptr->dscptrs[0];
        if (formal_arg_ptr == 0)
        {
          status = TdiNULL_PTR;
          break;
        }
        if (formal_arg_ptr->dtype != DTYPE_FUNCTION ||
            formal_arg_ptr->ndesc != 1)
          code = 0;
        else
          code = *(unsigned short *)formal_arg_ptr->pointer;
      }
    }
    else
      code = opt = 0;
    /****************************
    [OPTIONAL] AS_IS IN INOUT OUT only.
    All other words are bad here.
    ****************************/
    if (code == OPC_IN || code == OPC_INOUT || code == OPC_OUT)
    {
      formal_arg_ptr = (mdsdsc_r_t *)formal_arg_ptr->dscptrs[0];
      if (formal_arg_ptr->dtype != DTYPE_T)
      {
        status = TdiINVDTYDSC;
        break;
      }
    }
    else if (code == OPC_AS_IS)
      formal_arg_ptr = (mdsdsc_r_t *)formal_arg_ptr->dscptrs[0];
    else if (formal_arg_ptr->dtype != DTYPE_IDENT &&
             (formal_arg_ptr->dtype != DTYPE_T || !opt))
    {
      status = TdiINVDTYDSC;
      break;
    }
    /******************************
    OUT is present, but undefined.
    Eval others with old vari list.
    ******************************/
    if (j >= nactual || (actual_ptr = actual_arg_ptr[j]) == 0)
    {
      if (!opt)
        status = TdiMISS_ARG;
    }
    else if (code == OPC_OUT)
    {
      old_head = _private.head;
      _private.head = new_head;
      status = put_ident(formal_arg_ptr, 0, TDITHREADSTATIC_VAR);
      new_head = _private.head;
      _private.head = old_head;
    }
    else
    {
      if (code == OPC_AS_IS)
      {
        status = MdsCopyDxXd((mdsdsc_t *)actual_ptr, &tmp);
      }
      else if (actual_ptr->dtype == DTYPE_IDENT)
      {
        status = get_ident((mdsdsc_t *)actual_ptr, &tmp, TDITHREADSTATIC_VAR);
        if (opt && status == TdiUNKNOWN_VAR)
          status = MDSplusSUCCESS;
      }
      else
      {
        status = TdiEvaluate(actual_ptr, &tmp MDS_END_ARG);
        if (!opt && STATUS_OK && tmp.pointer == 0)
          status = TdiMISS_ARG;
      }
      /**************************
      Do this with new variables.
      **************************/
      old_head = _private.head;
      _private.head = new_head;
      if (STATUS_OK)
        status = put_ident(formal_arg_ptr, &tmp, TDITHREADSTATIC_VAR);
      new_head = _private.head;
      _private.head = old_head;
    }
  }
  /**************************
  Do this with new variables.
  **************************/
  old_head = _private.head;
  _private.head = new_head;
  TDI_VAR_NEW_NARG = nactual;
  if (STATUS_OK)
  {
    status = TdiEvaluate(formal_ptr->dscptrs[1], out_ptr MDS_END_ARG);
    if (status == TdiRETURN)
      status = MDSplusSUCCESS;
  }
  /*****************************
  Send outputs back up the line.
  *****************************/
  for (j = 0; j < nactual && STATUS_OK; ++j)
  {
    formal_arg_ptr = (mdsdsc_r_t *)formal_ptr->dscptrs[j + 2];
    if (actual_arg_ptr[j] && formal_arg_ptr->dtype == DTYPE_FUNCTION &&
        formal_arg_ptr->ndesc == 1)
    {
      if ((code = *(unsigned short *)formal_arg_ptr->pointer) == OPC_OPTIONAL)
      {
        formal_arg_ptr = (mdsdsc_r_t *)formal_arg_ptr->dscptrs[0];
        if (formal_arg_ptr->dtype != DTYPE_FUNCTION ||
            formal_arg_ptr->ndesc != 1)
          code = 0;
        else
          code = *(unsigned short *)formal_arg_ptr->pointer;
      }
      if (code == OPC_INOUT || code == OPC_OUT)
      {
        status = find_ident(7, (mdsdsc_r_t *)formal_arg_ptr->dscptrs[0], 0,
                            &node_ptr, 0, TDITHREADSTATIC_VAR);
        /**************************
        Do this with old variables.
        **************************/
        new_head = _private.head;
        _private.head = old_head;
        if (STATUS_OK)
          status =
              put_ident(actual_arg_ptr[j], &node_ptr->xd, TDITHREADSTATIC_VAR);
        old_head = _private.head;
        _private.head = new_head;
      }
    }
  }
  TdiDeallocate(&tmp MDS_END_ARG);
  MdsFree1Dx(&tmp, NULL);
  _private.head = old_head;
  TDI_VAR_NEW_NARG = old_narg;
  return status;
}

/***************************************************************
        Replace variable. Assumed called by INTRINSIC so that out_ptr is XD-DSC.
*/
int Tdi1Equals(opcode_t opcode __attribute__((unused)),
               int narg __attribute__((unused)), mdsdsc_t *list[],
               mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  status = TdiEvaluate(list[1], out_ptr MDS_END_ARG);
  /************************************
  Place the data or clear the variable.
  ************************************/
  if (STATUS_OK)
    status = put_ident((mdsdsc_r_t *)list[0], out_ptr, TDITHREADSTATIC_VAR);
  else
    put_ident((mdsdsc_r_t *)list[0], NULL, TDITHREADSTATIC_VAR);
  return status;
}

/*--------------------------------------------------------------
        Store in first argument of binary opertor.
*/
int Tdi1EqualsFirst(opcode_t opcode __attribute__((unused)),
                    int narg __attribute__((unused)), mdsdsc_t *list[],
                    mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;

  if (list[0]->dtype != DTYPE_FUNCTION ||
      ((mds_function_t *)list[0])->ndesc != 2)
    status = TdiINVDTYDSC;
  else
    status = TdiEquals(((mdsdsc_r_t *)list[0])->dscptrs[0], list[0],
                       out_ptr MDS_END_ARG);
  return status;
}

/*--------------------------------------------------------------
        Decrement a variable before use.
*/
int Tdi1PreDec(opcode_t opcode __attribute__((unused)),
               int narg __attribute__((unused)), mdsdsc_t *list[],
               mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  status = TdiSubtract(list[0], &true_dsc, out_ptr MDS_END_ARG);
  if (STATUS_OK)
    status = tdi_put_ident((mdsdsc_r_t *)list[0], out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Increment a variable before use.
*/
int Tdi1PreInc(opcode_t opcode __attribute__((unused)),
               int narg __attribute__((unused)), mdsdsc_t *list[],
               mdsdsc_xd_t *out_ptr)
{
  int status = TdiAdd(list[0], &true_dsc, out_ptr MDS_END_ARG);
  if (STATUS_OK)
    status = tdi_put_ident((mdsdsc_r_t *)list[0], out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Decrement a variable after use.
*/
int Tdi1PostDec(opcode_t opcode __attribute__((unused)),
                int narg __attribute__((unused)), mdsdsc_t *list[],
                mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  EMPTYXD(tmp);
  int status = get_ident(list[0], out_ptr, TDITHREADSTATIC_VAR);
  if (STATUS_OK)
    status = TdiSubtract(out_ptr->pointer, &true_dsc, &tmp MDS_END_ARG);
  if (STATUS_OK)
    status = put_ident((mdsdsc_r_t *)list[0], &tmp, TDITHREADSTATIC_VAR);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Increment a variable after use.
*/
int Tdi1PostInc(opcode_t opcode __attribute__((unused)),
                int narg __attribute__((unused)), mdsdsc_t *list[],
                mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  EMPTYXD(tmp);
  int status = get_ident(list[0], out_ptr, TDITHREADSTATIC_VAR);
  if (STATUS_OK)
    status = TdiAdd(out_ptr->pointer, &true_dsc, &tmp MDS_END_ARG);
  if (STATUS_OK)
    status = put_ident((mdsdsc_r_t *)list[0], &tmp, TDITHREADSTATIC_VAR);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/***************************************************************
        Find by identifier.
        PRIVATE and PUBLIC must have text argument, not expression.
        They are keywords, not standard functions. NEED we change this?
*/
int Tdi1Private(opcode_t opcode __attribute__((unused)),
                int narg __attribute__((unused)), mdsdsc_t *list[],
                mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  node_type *node_ptr;
  int status = LibLookupTree((void **)&_private.head, (void *)list[0], compare,
                             (void **)&node_ptr);
  if (STATUS_OK)
    status = MdsCopyDxXd(node_ptr->xd.pointer, out_ptr);
  else if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  return status;
}

/*--------------------------------------------------------------
        Find by identifier.
        PRIVATE and PUBLIC must have text argument, not expression.
*/
int Tdi1Public(opcode_t opcode __attribute__((unused)),
               int narg __attribute__((unused)), mdsdsc_t *list[],
               mdsdsc_xd_t *out_ptr)
{
  int status;
  LOCK_PUBLIC_PUSH;
  node_type *node_ptr;
  status = LibLookupTree((void **)&_public.head, (void *)list[0], compare,
                         (void **)&node_ptr);
  if (STATUS_OK)
    status = MdsCopyDxXd(node_ptr->xd.pointer, out_ptr);
  else if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  UNLOCK_PUBLIC;
  return status;
}

/*--------------------------------------------------------------
        Find by text expression.
*/
int Tdi1Var(opcode_t opcode __attribute__((unused)), int narg, mdsdsc_t *list[],
            mdsdsc_xd_t *out_ptr)
{
  int status;
  INIT_AND_FREEXD_ON_EXIT(tmp);
  status = TdiData(list[0], &tmp MDS_END_ARG);
  if (STATUS_OK)
  {
    if (narg < 2 || list[1] == 0)
      status = tdi_get_ident(tmp.pointer, out_ptr);
    else
      status = TdiEquals(tmp.pointer, list[1], out_ptr MDS_END_ARG);
  }
  FREEXD_NOW(tmp);
  return status;
}

/***************************************************************
        Define a function by reconstruction.
*/
int Tdi1Fun(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  DESCRIPTOR_FUNCTION(hold, 0, 255);
  int j;
  opcode_t opcode_s = opcode;
  hold.pointer = &opcode_s;
  hold.ndesc = (uint8_t)narg;
  for (j = narg; --j >= 0;)
    if ((hold.arguments[j] = list[j]) == 0)
      return TdiNULL_PTR;
  int status = MdsCopyDxXd((mdsdsc_t *)&hold, out_ptr);
  if (STATUS_OK)
    status = tdi_put_ident((mdsdsc_r_t *)list[0], out_ptr);
  return status;
}

/***************************************************************
        Release the private variables.
        This to be used by functions.
*/
int Tdi1ResetPrivate()
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  _private.head = 0;
  if (_private.data_zone)
    status = LibResetVmZone(&_private.data_zone);
  if (_private.head_zone)
    status = LibResetVmZone(&_private.head_zone);
  return status;
}

/*--------------------------------------------------------------
        Release the public variables.
*/
int Tdi1ResetPublic()
{
  int status;
  LOCK_PUBLIC_PUSH;
  status = MDSplusSUCCESS;
  _public.head = 0;
  if (_public.data_zone)
    status = LibResetVmZone(&_public.data_zone);
  if (_public.head_zone)
    status = LibResetVmZone(&_public.head_zone);
  UNLOCK_PUBLIC;
  return status;
}

/***************************************************************
        Display a variable.
*/
static int show_one(const node_type *const node_ptr,
                    user_type *const user_ptr)
{
  INIT_STATUS;
  mdsdsc_d_t tmp = EMPTY_D;
  mdsdsc_r_t *rptr = (mdsdsc_r_t *)node_ptr->xd.pointer;

  if (node_ptr->xd.class == 0)
    return 1;
  if (user_ptr->match.length == 0)
    status = MDSplusSUCCESS;
  else
    status = StrMatchWild(&node_ptr->name_dsc, &user_ptr->match);
  if (STATUS_OK)
  {
    if (rptr)
      status = TdiDecompile(rptr, &tmp MDS_END_ARG);
    if (STATUS_OK)
    {
      if (rptr && rptr->dtype == DTYPE_FUNCTION &&
          *(unsigned short *)rptr->pointer == OPC_FUN)
        printf("%.*s\n", tmp.length, tmp.pointer);
      else if (tmp.length > 0)
        printf("%s %.*s\t= %.*s\n",
               user_ptr->block_ptr->_public ? "Public" : "Private",
               node_ptr->name_dsc.length, node_ptr->name_dsc.pointer,
               tmp.length, tmp.pointer);
      else
        printf("%s %.*s\t=\n",
               user_ptr->block_ptr->_public ? "Public" : "Private",
               node_ptr->name_dsc.length, node_ptr->name_dsc.pointer);
      user_ptr->count++;
    }
    else
      printf("ERROR on %.*s\n", node_ptr->name_dsc.length,
             node_ptr->name_dsc.pointer);
    StrFree1Dx(&tmp);
  }
  else
    status = MDSplusSUCCESS;
  return status;
}

/*--------------------------------------------------------------
        Display private variables.
*/
int Tdi1ShowPrivate(opcode_t opcode __attribute__((unused)), int narg,
                    mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  MDSDBG("TdiShowPrivate: %" PRIxPTR "\n", (uintptr_t)(void *)_private.head);
  return wild((int (*)())show_one, narg, list, &_private, out_ptr,
              TDITHREADSTATIC_VAR);
}

/*--------------------------------------------------------------
        Display public variables.
*/
int Tdi1ShowPublic(opcode_t opcode __attribute__((unused)), int narg,
                   mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  int status;
  LOCK_PUBLIC_PUSH;
  TDITHREADSTATIC_INIT;
  status = wild((int (*)())show_one, narg, list, &_public, out_ptr,
                TDITHREADSTATIC_VAR);
  UNLOCK_PUBLIC;
  return status;
}

extern EXPORT int TdiSaveContext(void *ptr[6])
{
  TDITHREADSTATIC_INIT;
  ptr[0] = (void *)_private.head;
  ptr[1] = _private.head_zone;
  ptr[2] = _private.data_zone;
  LOCK_PUBLIC_PUSH;
  ptr[3] = (void *)_public.head;
  ptr[4] = _public.head_zone;
  ptr[5] = _public.data_zone;
  UNLOCK_PUBLIC;
  MDSDBG("TdiSaveContext: %" PRIxPTR "\n", (uintptr_t)ptr[0]);
  return 1;
}

extern EXPORT int TdiDeleteContext(void *ptr[6])
{
  if (ptr[1])
    LibDeleteVmZone(&ptr[1]);
  if (ptr[2])
    LibDeleteVmZone(&ptr[2]);
  if (ptr[4] || ptr[5])
  {
    LOCK_PUBLIC_PUSH;
    if (ptr[4])
      LibDeleteVmZone(&ptr[4]);
    if (ptr[5])
      LibDeleteVmZone(&ptr[5]);
    UNLOCK_PUBLIC;
  }
  return 1;
}

/*-------------------------------------------------------------
        Restore variable context
*/
extern EXPORT int TdiRestoreContext(void *const ptr[6])
{
  TDITHREADSTATIC_INIT;
  MDSDBG("TdiRestoreContext: %" PRIxPTR "\n", (uintptr_t)ptr[0]);
  _private.head = (node_type *)ptr[0];
  _private.head_zone = ptr[1];
  _private.data_zone = ptr[2];
  LOCK_PUBLIC_PUSH;
  _public.head = ptr[3];
  _public.head_zone = (node_type *)ptr[4];
  _public.data_zone = ptr[5];
  UNLOCK_PUBLIC;
  return 1;
}
