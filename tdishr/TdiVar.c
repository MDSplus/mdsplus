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
                FUN [PRIVATE|PUBLIC] ident ([OPTIONAL][AS_IS|IN|INOUT|OUT] ident...) stmt
                var (var...)                    invocation
        Inquires:
                ALLOCATED (var)                 test defined
                PRESENT (var)                   test in argument list or defined
        Fetches (these are the "var"):
                [PRIVATE|PUBLIC] ident          by data type / special function
                VAR ("text expression")         fetch by expression
        Stores:
                var = expression                assignment
                var += expression               binary operator and assignment
                VAR ("text", expr)              fetch/store by text expression
        Changes:
                ++ [PRIVATE|PUBLIC] var         pre-increment
                -- [PRIVATE|PUBLIC] var         pre-decrement
                [PRIVATE|PUBLIC] var ++         post-increment
                [PRIVATE|PUBLIC] var --         post-decrement
                DEALLOCATE (var...)             free data field
        Utilities:
                TdiFindIdent(search, var, [key], [node_ptr], [block_ptr])
                TdiGetIdent(var, data)  fetch variable
                TdiPutIdent(var, data)  store variable
                RESET_PRIVATE() RESET_PUBLIC()  release variables
                SHOW_PRIVATE([text],...)
                SHOW_PUBLIC([text],...)         display variables

        FUN arguments declared:
                AS_IS overrides evaluation.
                not OPTIONAL must be present and not null.
                IN are evaluated and must not be changed.
                INOUT or OUT must be variables.
                OUT are initially undefined and may not be evaluated until defined.
        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
        NEED subscripted assignment.
*/
#define DEF_FREED

#include <mdsplus/mdsconfig.h>
#include "STATICdef.h"
#define DEF_FREEXD
#define DEF_FREED
#include "tdithreadsafe.h"
#include "tdirefstandard.h"
#include "tdishrp.h"
#include <libroutines.h>
#include <strroutines.h>
#include <stdio.h>

#define LibVM_EXTEND_AREA 32
#define LibVM_TAIL_LARGE 128
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <string.h>
#include <mdsshr_messages.h>

// #define DEBUG

extern unsigned short OpcEquals, OpcEqualsFirst;
extern unsigned short OpcFun;
extern unsigned short OpcIn, OpcInOut, OpcOptional, OpcOut, OpcAsIs;
extern unsigned short OpcPostDec, OpcPostInc, OpcPreDec, OpcPreInc;
extern unsigned short OpcPrivate, OpcPublic, OpcVar;

extern int TdiFaultHandler();
extern int TdiData();
extern int TdiDeallocate();
extern int TdiDecompile();
extern int TdiEvaluate();
extern int TdiPutLong();
extern int TdiEquals();
extern int TdiSubtract();
extern int TdiAdd();

	/*******************************************
        This uses the balanced binary tree routines.
        LOOKUP_TREE INSERT_TREE TRAVERSE_TREE
        Full descriptors cost memory, save time.
        *******************************************/
typedef struct link {
  struct link *left, *right;	/* binary tree links */
  unsigned short reserved;	/* tree flags */
  struct descriptor_xd xd;	/* data descriptor */
  struct descriptor name_dsc;	/* name descriptor */
  unsigned char name[1];	/* unchanging name */
} node_type;
typedef struct {
  node_type *head;		/* changing binary-tree head */
  void *head_zone;		/* created on first call */
  void *data_zone;		/* created on first call */
  int _public;			/* is public */
} block_type;
typedef struct {
  struct descriptor match;
  block_type *block_ptr;
  int count;
} user_type;

STATIC_CONSTANT struct descriptor_d EMPTY_D = { 0, DTYPE_T, CLASS_D, 0 };
STATIC_CONSTANT struct descriptor EMPTDY_S = { 0, DTYPE_T, CLASS_S, 0 };
STATIC_CONSTANT struct descriptor_xd NULL_XD = { 0, 0, 0, 0, 0 };

STATIC_CONSTANT unsigned char true = 1;
STATIC_CONSTANT struct descriptor true_dsc = { sizeof(true), DTYPE_BU, CLASS_S, (char *)&true };


static pthread_mutex_t public_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_PUBLIC pthread_mutex_lock(&public_lock)
//;fprintf(stderr,"public locked by %ld\n",*(long int*)pthread_self())
#define UNLOCK_PUBLIC_PUSH pthread_cleanup_push((void*)pthread_mutex_unlock,&public_lock)
#define LOCK_PUBLIC_PUSH LOCK_PUBLIC;UNLOCK_PUBLIC_PUSH
#define UNLOCK_PUBLIC pthread_cleanup_pop(1)
//;fprintf(stderr,"public unlocked by %ld\n",*(long int*)pthread_self())
#define UNLOCK_PUBLIC_IF(cond) pthread_cleanup_pop(cond)
//;if (cond) fprintf(stderr,"public unlocked by %ld\n",*(long int*)pthread_self())
#define UNLOCK_IF_PUBLIC(ptr) UNLOCK_PUBLIC_IF(ptr==&_public)
STATIC_THREADSAFE block_type _public = { 0, 0, 0, 1 };

STATIC_CONSTANT DESCRIPTOR(star, "*");
STATIC_CONSTANT DESCRIPTOR(percent, "%");
struct descriptor *Tdi3Narg(){
  return &TdiGetThreadStatic()->TdiVar_new_narg_d;
}

/*--------------------------------------------------------------
        Put a byte to output.
*/
int TdiPutLogical(unsigned char data, struct descriptor_xd *out_ptr){
  INIT_STATUS;
  unsigned short len = (unsigned short)sizeof(unsigned char);
  unsigned char dtype = (unsigned char)DTYPE_BU;
  if (!out_ptr)
    return TdiNULL_PTR;
  status = MdsGet1DxS(&len, &dtype, out_ptr);
  if STATUS_OK
    *(unsigned char *)out_ptr->pointer->pointer = data;
  return status;
}

/*--------------------------------------------------------------
        Comparison routine.
*/
STATIC_ROUTINE int compare(struct descriptor *key_ptr, node_type * node_ptr, block_type * block_ptr __attribute__ ((unused))){
  return StrCaseBlindCompare(key_ptr, &node_ptr->name_dsc);
}

/*--------------------------------------------------------------
        Allocation routine, does not set data field.
        ASSUMED called only once per node.
*/
STATIC_ROUTINE int allocate(struct descriptor *key_ptr,
			    node_type ** node_ptr_ptr, block_type * block_ptr){
  INIT_STATUS;
  unsigned int len = sizeof(struct link) - 1 + key_ptr->length;

	/*******************************************************************
        Must clear memory unless allocate is called only once for each node.
        *******************************************************************/
  if (block_ptr->data_zone == 0) {
    LibCreateVmZone(&block_ptr->head_zone);
    LibCreateVmZone(&block_ptr->data_zone);
  }
  status = LibGetVm(&len, (void *) node_ptr_ptr, &block_ptr->head_zone);
  if STATUS_OK {
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
#define _private (*(block_type*)&TdiThreadStatic_p->TdiVar_private)
STATIC_ROUTINE int TdiFindIdent(int search,
                                struct descriptor_r *ident_ptr,
                                struct descriptor *key_ptr,
                                node_type ** node_ptr_ptr, block_type ** block_ptr_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  struct descriptor key_dsc;
  struct descriptor_d name_dsc;
  node_type *node_ptr;
  int code;

  if (ident_ptr == 0)
    status = TdiNULL_PTR;
  else
    switch (ident_ptr->class) {
    case CLASS_S:
    case CLASS_D:
      switch (ident_ptr->dtype) {
	/********************************************
	The choice of default public/private is here.
	********************************************/
      case DTYPE_T:
      case DTYPE_IDENT: {
        key_dsc = *(struct descriptor *)ident_ptr;
        key_dsc.dtype = DTYPE_T;
        key_dsc.class = CLASS_S;
        if (search & 1) {
          if (search & 4) {
            status = LibLookupTree((void *)&_private.head, (void *)&key_dsc, compare, (void **)&node_ptr);
            if STATUS_OK {
              if (node_ptr->xd.class != 0) {
                if(block_ptr_ptr) *block_ptr_ptr = &_private;
                break;
              } else {
                status = TdiUNKNOWN_VAR;
              }
            }
          } else {
            if(block_ptr_ptr) *block_ptr_ptr = &_private;
            break;
          }
        }
        if (search & 2) {
          if (search & 4) {
            LOCK_PUBLIC_PUSH;
            status = LibLookupTree((void *)&_public.head, (void *)&key_dsc, compare, (void **)&node_ptr);
            if STATUS_OK {
              if (node_ptr->xd.class != 0) {
                if (block_ptr_ptr) *block_ptr_ptr = &_public;
              } else {
                status = TdiUNKNOWN_VAR;
              }
            }

            UNLOCK_PUBLIC_IF(!(block_ptr_ptr && *block_ptr_ptr==&_public));
          } else if (block_ptr_ptr) {
            LOCK_PUBLIC;
	    *block_ptr_ptr = &_public;
          }
        }
        break;}
      default:
        status = TdiINVDTYDSC;
        break;
       }
      break;
    case CLASS_R:
      if (ident_ptr->dtype != DTYPE_FUNCTION) {
        status = TdiINVCLADTY;
        break;
      }
      code = *(unsigned short *)ident_ptr->pointer;
      if (ident_ptr->ndesc < 1) {
        status = TdiMISS_ARG;
        break;
      }
                /***************************************
                Could have (public _a+=6)++ as argument.
                So strip ++ and += and use public.
                ***************************************/
      if (code == OpcPublic)
        status = TdiFindIdent(search & ~1,
                         (struct descriptor_r *)ident_ptr->dscptrs[0],
                         &key_dsc, &node_ptr, block_ptr_ptr);
      else if (code == OpcPrivate)
        status = TdiFindIdent(search & ~2,
                         (struct descriptor_r *)ident_ptr->dscptrs[0],
                         &key_dsc, &node_ptr, block_ptr_ptr);
      else if (code == OpcEquals || code == OpcEqualsFirst
               || code == OpcPostDec || code == OpcPreDec
               || code == OpcPostInc || code == OpcPreInc) {
        struct descriptor_xd tmp = EMPTY_XD;
        status = TdiEvaluate(ident_ptr, &tmp MDS_END_ARG);
        if STATUS_OK
          status = TdiFindIdent(search,
                           (struct descriptor_r *)ident_ptr->dscptrs[0],
			   &key_dsc, &node_ptr, block_ptr_ptr);
      } else if (code == OpcVar) {
        name_dsc = EMPTY_D;
        status = TdiData(ident_ptr->dscptrs[0], &name_dsc MDS_END_ARG);
        if STATUS_OK
          status = TdiFindIdent(search, (struct descriptor_r *)&name_dsc,
                           &key_dsc, &node_ptr, block_ptr_ptr);
        StrFree1Dx((struct descriptor_d *)&name_dsc);
      } else
        status = TdiINV_OPC;
      break;
    default:
      status = TdiINVCLADSC;
      break;
    }
  if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  if (STATUS_OK || status == TdiUNKNOWN_VAR) {
    if (key_ptr)       *key_ptr       = key_dsc;
    if (node_ptr_ptr)  *node_ptr_ptr  = node_ptr;
  } else {
    if (node_ptr_ptr)  *node_ptr_ptr  = NULL;
    if (block_ptr_ptr) *block_ptr_ptr = NULL;
  }
  return status;
}

int TdiFindSymbol(char* name){
  struct descriptor_s ident_ptr = { strlen(name), DTYPE_T, CLASS_S, name };
  node_type *node_ptr;
  block_type *block_ptr = NULL;
  int status;
  UNLOCK_PUBLIC_PUSH;
  status = TdiFindIdent(7, (struct descriptor_r *)&ident_ptr, 0, &node_ptr, &block_ptr);
  UNLOCK_IF_PUBLIC(block_ptr);
  return STATUS_OK ? (block_ptr==&_public ? 2 : 1) : (status==TdiUNKNOWN_VAR ? 0 : -1);
}

/*--------------------------------------------------------------
        Get active variable for VAR and EVALUATE. Copies to MDSSHR space.
*/
int TdiGetIdent(struct descriptor *ident_ptr, struct descriptor_xd *data_ptr){
  node_type *node_ptr;
  block_type *block_ptr = NULL;
  INIT_STATUS;
  UNLOCK_PUBLIC_PUSH;
  status = TdiFindIdent(7, (struct descriptor_r *)ident_ptr, 0, &node_ptr, &block_ptr);
#ifdef DEBUG
  char string[64];
  memcpy(string,ident_ptr->pointer,ident_ptr->length);string[ident_ptr->length] = '\0';
  fprintf(stderr,"GET: %s %s     block=%p\n",block_ptr==&_public ? "public" : STATUS_OK ? "private" : "new", string, block_ptr);
#endif
  if STATUS_OK
    status = MdsCopyDxXd(node_ptr->xd.pointer, data_ptr);
  else
    MdsFree1Dx(data_ptr, NULL);
  UNLOCK_IF_PUBLIC(block_ptr);
  return status;
}

/*--------------------------------------------------------------
        Keep an XD-DSC of whatever.
        Assumes we are given XD or class-0.
*/
int TdiPutIdent(struct descriptor_r *ident_ptr, struct descriptor_xd *data_ptr){
  struct descriptor key_dsc = EMPTDY_S;
  node_type *node_ptr;
  block_type *block_ptr = NULL;
  struct descriptor_d upstr = { 0, DTYPE_T, CLASS_D, 0 };
  INIT_STATUS;
  STATIC_CONSTANT int zero = 0;
  if (ident_ptr->dtype == DTYPE_DSC)
    return TdiPutIdent((struct descriptor_r *)ident_ptr->pointer, data_ptr);
	/************************************
        Find where we should place the stuff.
        ************************************/
  UNLOCK_PUBLIC_PUSH;
  status = TdiFindIdent(3, ident_ptr, &key_dsc, 0, &block_ptr);
  StrUpcase((struct descriptor *)&upstr, &key_dsc);
#ifdef DEBUG
  char string[64];
  memcpy(string,key_dsc.pointer,key_dsc.length);string[key_dsc.length] = '\0';
  fprintf(stderr,"PUT: %s %s     block=%p\n",block_ptr==&_public ? "public" : STATUS_OK ? "private" : "new", string, block_ptr);
#endif
  if STATUS_OK
    status =
      LibInsertTree((void **)&block_ptr->head, &upstr, &zero, compare, allocate, (void *)&node_ptr, block_ptr);
  StrFree1Dx(&upstr);
  if STATUS_OK {
    if (node_ptr->xd.class == 0)
      node_ptr->xd = EMPTY_XD;
    if (data_ptr)
      status =
	  MdsCopyDxXdZ(data_ptr->pointer, &node_ptr->xd,
		       &block_ptr->data_zone, NULL, NULL, NULL, NULL);
    else {
      MdsFree1Dx(&node_ptr->xd, &block_ptr->data_zone);
      node_ptr->xd = NULL_XD;
    }
  }
  UNLOCK_IF_PUBLIC(block_ptr);
  return status;
}

/***************************************************************
        Display/free wildcarded variables.
        IDENT names are not evaluated.
        PUBLIC or PRIVATE(ident or text) overrides block_ptr.
*/
STATIC_ROUTINE int wild_loop(int (*doit) (),struct descriptor *arg,user_type* user_p) {
  int status;
  UNLOCK_PUBLIC_PUSH;
  status = TdiFindIdent(3, (struct descriptor_r *)arg, &user_p->match, 0, &user_p->block_ptr);
  if STATUS_OK
    status = StrUpcase(&user_p->match, &user_p->match);
  if STATUS_OK {
    if (StrPosition(&user_p->match, (struct descriptor *)&star, 0)
      || StrPosition(&user_p->match, (struct descriptor *)&percent, 0)) {
      status = LibTraverseTree((void **)&user_p->block_ptr->head, doit, user_p);
    } else {
      node_type *node_ptr;
      status = LibLookupTree((void **)&user_p->block_ptr->head, (void *)&user_p->match, compare, (void **)&node_ptr);
      if STATUS_OK
        status = (*doit) (node_ptr, user_p);
      else if (status == LibKEYNOTFOU)
        status = MDSplusSUCCESS;
    }
  }
  UNLOCK_IF_PUBLIC(user_p->block_ptr);
  return status;
}
STATIC_ROUTINE int wild(int (*doit) (),
			int narg,
			struct descriptor *list[],
			block_type * block_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  user_type user = { {0, DTYPE_T, CLASS_D, 0}, 0, 0 };
  user.block_ptr = block_ptr;
  if (narg == 0 || list[0] == 0)
    status = LibTraverseTree((void **)&block_ptr->head, doit, &user);
  else {
    int j;
    for (j = 0; STATUS_OK && j < narg; ++j)
      status = wild_loop(doit,list[j],&user);
  }
  if (user.match.class == CLASS_D)
    StrFree1Dx((struct descriptor_d *)&user.match);
  if STATUS_OK
    status = TdiPutLong(&user.count, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Release variables.
*/
STATIC_ROUTINE int free_one(node_type * node_ptr, user_type * user_ptr)
{
  block_type *block_ptr = user_ptr->block_ptr;
  INIT_STATUS;

	/***************************************************
        Deallocated variables are not displayed or re-freed.
        ***************************************************/
  if (node_ptr->xd.class == 0)
    return 1;
  if (user_ptr->match.length == 0)
    status = MDSplusSUCCESS;
  else
    status = StrMatchWild(&node_ptr->name_dsc, &user_ptr->match);
  if STATUS_OK {
    if (node_ptr->xd.l_length)
      status = LibFreeVm(&node_ptr->xd.l_length, (void *)&node_ptr->xd.pointer, &block_ptr->data_zone);
    node_ptr->xd = NULL_XD;
    user_ptr->count++;
  } else
    status = MDSplusSUCCESS;
  return status;
}

/*--------------------------------------------------------------
        Release all variables under a header and the headers too.
        Used to release the FUN variables.
*/
STATIC_ROUTINE int free_all(node_type ** pnode)
{
  INIT_STATUS, stat2;
  GET_TDITHREADSTATIC_P;
  unsigned int len;
  if ((*pnode)->xd.l_length)
    status = LibFreeVm(&(*pnode)->xd.l_length, (void *)&(*pnode)->xd.pointer, &_private.data_zone);
  if ((*pnode)->left) {
    stat2 = free_all(&((*pnode)->left));
    if STATUS_OK
      status = stat2;
  }
  if ((*pnode)->right) {
    stat2 = free_all(&((*pnode)->right));
    if STATUS_OK
      status = stat2;
  }
  len = sizeof(struct link) - 1 + (*pnode)->name_dsc.length;
  stat2 = LibFreeVm(&len, (void *)pnode, &_private.head_zone);
  *pnode = 0;
  if STATUS_OK
    status = stat2;
  return status;
}

/*--------------------------------------------------------------
        Release variables.
*/
int Tdi1Deallocate(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  if (narg == 0 && _private.head) {
    status = free_all(&_private.head);
    _private.head = NULL;
    return status;
  }
  return wild((int (*)())free_one, narg, list, &_private, out_ptr);
}

/*--------------------------------------------------------------
        Check for allocated variable, private only by default.
*/
int Tdi1Allocated(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor key_dsc = EMPTDY_S;
  node_type *node_ptr;
  block_type *block_ptr;
  int found;
  UNLOCK_PUBLIC_PUSH;
  status = TdiFindIdent(3, (struct descriptor_r *)list[0], &key_dsc, 0, &block_ptr);
  if STATUS_OK
    status = LibLookupTree((void **)&block_ptr->head, (void *)&key_dsc, compare, (void **)&node_ptr);
  UNLOCK_IF_PUBLIC(block_ptr);
  found = STATUS_OK;
  if (found)
    found = node_ptr->xd.class != 0;
  else if (status == LibKEYNOTFOU || status == TdiUNKNOWN_VAR)
    status = MDSplusSUCCESS;
  if STATUS_OK
    status = TdiPutLogical((unsigned char)found, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Check for argument present.
*/
int Tdi1Present(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor key_dsc = EMPTDY_S;
  node_type *node_ptr;
  block_type *block_ptr;
  int found;
  UNLOCK_PUBLIC_PUSH;
  status = TdiFindIdent(3, (struct descriptor_r *)list[0], &key_dsc, 0, &block_ptr);
  if STATUS_OK
    status = LibLookupTree((void **)&block_ptr->head, (void *)&key_dsc, compare, (void **)&node_ptr);
  UNLOCK_IF_PUBLIC(block_ptr);
  found = STATUS_OK;
  if (found) ;
  else if (status == LibKEYNOTFOU || status == TdiUNKNOWN_VAR)
    status = MDSplusSUCCESS;
  if STATUS_OK
    status = TdiPutLogical((unsigned char)found, out_ptr);
  return status;
}

/***************************************************************
        Execute a function. Set up the arguments.
*/
extern int TdiCompile();
int compile_fun(struct descriptor *entry)
{
  static DESCRIPTOR(dnul, "\0");
  static DESCRIPTOR(dfun, ".fun\0");
  static DESCRIPTOR(def_path, "MDS_PATH:");
  int status;
  struct descriptor_d file = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_xd tmp = EMPTY_XD;
  FREEXD_ON_EXIT(&tmp);
  FREED_ON_EXIT(&file);
  status = StrConcat((struct descriptor *)&file, (struct descriptor *)&def_path, entry, &dfun MDS_END_ARG);
#ifdef DEBUG
  char string[64];
  memcpy(string,file.pointer,file.length);string[file.length] = '\0';
  fprintf(stderr,"compile: %s\n",string);
#endif
  if STATUS_OK {
    void *ctx = 0;
    struct descriptor dcs = { 0, DTYPE_T, CLASS_S, 0 };
    LibFindFileRecurseCaseBlind((struct descriptor *)&file, (struct descriptor *)&file, &ctx);
    LibFindFileEnd(&ctx);
    StrAppend(&file, (struct descriptor *)&dnul);
    FILE *unit = fopen(file.pointer, "rb");
    if (unit) {
	long flen;
	fseek(unit, 0, SEEK_END);
	flen = ftell(unit);
	flen = (flen > 0xffff) ? 0xffff : flen;
	fseek(unit, 0, SEEK_SET);
	dcs.pointer = (char *)malloc(flen);
	dcs.length = (unsigned short)flen;
	fread(dcs.pointer, (size_t) flen, 1, unit);
	fclose(unit);
	status = TdiCompile(&dcs, &tmp MDS_END_ARG);
	free(dcs.pointer);
    } else
	status = TdiUNKNOWN_VAR;
  }
  FREED_NOW(&file);
  if STATUS_OK {
    struct descriptor_function *pfun = (struct descriptor_function *)tmp.pointer;
    if (pfun->dtype == DTYPE_FUNCTION) {
      unsigned short code = *(unsigned short *)pfun->pointer;
      if (code == OpcFun) {
	struct descriptor_function *pfun2 = (struct descriptor_function *)pfun->arguments[0];
	if (pfun2->dtype == DTYPE_FUNCTION) {
	  code = *(unsigned short *)pfun2->pointer;
	  if (code == OpcPrivate || code == OpcPublic)
	    pfun2 = (struct descriptor_function *)pfun2->arguments[0];
	}
	StrUpcase((struct descriptor *)pfun2, (struct descriptor *)pfun2);
	if (StrCompare(entry, (struct descriptor *)pfun2) == 0)
	  status = TdiEvaluate(&tmp, &tmp MDS_END_ARG);
      }
    }
  }
  FREEXD_NOW(&tmp);
  return status;
}

extern char* findModule();
extern int TdiExtPython2();
int TdiDoFun(struct descriptor *ident_ptr,
	     int nactual, struct descriptor_r *actual_arg_ptr[], struct descriptor_xd *out_ptr)
{
  node_type *node_ptr;
	/******************************************
        Get name of function to do. Check its type.
        ******************************************/
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  int status, ispython;
  char *filename, *dirspec;
  FREE_ON_EXIT(filename);
  FREE_ON_EXIT(dirspec);
  // look up method: check cached, check python, or load
  pthread_mutex_lock(&lock);
  pthread_cleanup_push((void*)pthread_mutex_unlock,&lock);
  dirspec = NULL; filename = NULL; ispython = B_FALSE;
  status = TdiFindIdent(7, (struct descriptor_r *)ident_ptr, 0, &node_ptr, 0);
  if (status==TdiUNKNOWN_VAR) {
    // check if method is python
    dirspec = findModule(ident_ptr,&filename);
    if (dirspec)
      ispython = B_TRUE;
    else { // not a python method, load tdi fun
      status = compile_fun(ident_ptr);
      if STATUS_OK
        status = TdiFindIdent(7, (struct descriptor_r *)ident_ptr, 0, &node_ptr, 0);
    }
  }
  pthread_cleanup_pop(1);
  // method is loaded or python at this point
  if (ispython) // execute python method if special flag is set
    status = TdiExtPython2(dirspec, filename, nactual, (struct descriptor **)actual_arg_ptr, out_ptr);
  FREE_NOW(dirspec);
  FREE_NOW(filename);
  if (ispython || STATUS_NOT_OK)
    return status;
  struct descriptor_r *formal_ptr = 0, *formal_arg_ptr, *actual_ptr;
  int code, opt, j, nformal = 0;
  if ((formal_ptr = (struct descriptor_r *)node_ptr->xd.pointer) == 0
     || formal_ptr->dtype != DTYPE_FUNCTION
     || *(unsigned short *)formal_ptr->pointer != OpcFun)
    return TdiUNKNOWN_VAR;
  if ((nformal = formal_ptr->ndesc - 2) < nactual)
    return TdiEXTRA_ARG;

  GET_TDITHREADSTATIC_P;
  struct descriptor_xd tmp = EMPTY_XD;
  node_type *old_head, *new_head = 0;
  int *new_narg = &TdiThreadStatic_p->TdiVar_new_narg;
  int old_narg = *new_narg;
	/**************************************
	 Now copy input arguments into new head.
	 Pick up some keywords from prototype.
	 Only OPTIONAL arguments may be omitted.
	 OUT arguments are not evaluated.
	 **************************************/
  for (j = 0; j < nformal && STATUS_OK; ++j) {
    formal_arg_ptr = (struct descriptor_r *)formal_ptr->dscptrs[j + 2];
    if (formal_arg_ptr == 0) {
      status = TdiNULL_PTR;
      break;
    }
    if (formal_arg_ptr->dtype == DTYPE_FUNCTION && formal_arg_ptr->ndesc == 1) {
      code = *(unsigned short *)formal_arg_ptr->pointer;
      opt = (code == OpcOptional);
      if (opt) {
	formal_arg_ptr = (struct descriptor_r *)formal_arg_ptr->dscptrs[0];
	if (formal_arg_ptr == 0) {
	  status = TdiNULL_PTR;
	  break;
	}
	if (formal_arg_ptr->dtype != DTYPE_FUNCTION || formal_arg_ptr->ndesc != 1)
	  code = 0;
	else
	  code = *(unsigned short *)formal_arg_ptr->pointer;
      }
    } else
      code = opt = 0;
		/****************************
                [OPTIONAL] AS_IS IN INOUT OUT only.
                All other words are bad here.
                ****************************/
    if (code == OpcIn || code == OpcInOut || code == OpcOut) {
      formal_arg_ptr = (struct descriptor_r *)formal_arg_ptr->dscptrs[0];
      if (formal_arg_ptr->dtype != DTYPE_T) {
	status = TdiINVDTYDSC;
	break;
      }
    } else if (code == OpcAsIs)
      formal_arg_ptr = (struct descriptor_r *)formal_arg_ptr->dscptrs[0];
    else if (formal_arg_ptr->dtype != DTYPE_IDENT && (formal_arg_ptr->dtype != DTYPE_T || !opt)) {
      status = TdiINVDTYDSC;
      break;
    }
		/******************************
                OUT is present, but undefined.
                Eval others with old vari list.
                ******************************/
    if (j >= nactual || (actual_ptr = actual_arg_ptr[j]) == 0) {
      if (!opt)
	status = TdiMISS_ARG;
    } else if (code == OpcOut) {
      old_head = _private.head;
      _private.head = new_head;
      status = TdiPutIdent(formal_arg_ptr, 0);
      new_head = _private.head;
      _private.head = old_head;
    } else {
      if (code == OpcAsIs) {
	status = MdsCopyDxXd((struct descriptor *)actual_ptr, &tmp);
      } else if (actual_ptr->dtype == DTYPE_IDENT) {
	status = TdiGetIdent((struct descriptor *)actual_ptr, &tmp);
	if (opt && status == TdiUNKNOWN_VAR)
	  status = MDSplusSUCCESS;
      } else {
	status = TdiEvaluate(actual_ptr, &tmp MDS_END_ARG);
	if (!opt && STATUS_OK && tmp.pointer == 0)
	  status = TdiMISS_ARG;
      }
			/**************************
                        Do this with new variables.
                        **************************/
      old_head = _private.head;
      _private.head = new_head;
      if STATUS_OK
	status = TdiPutIdent(formal_arg_ptr, &tmp);
      new_head = _private.head;
      _private.head = old_head;
    }
  }
	/**************************
        Do this with new variables.
        **************************/
  old_head = _private.head;
  _private.head = new_head;
  *new_narg = nactual;
  if STATUS_OK {
    status = TdiEvaluate(formal_ptr->dscptrs[1], out_ptr MDS_END_ARG);
    if (status == TdiRETURN)
      status = MDSplusSUCCESS;
  }
	/*****************************
        Send outputs back up the line.
        *****************************/
  for (j = 0; j < nactual && STATUS_OK; ++j) {
    formal_arg_ptr = (struct descriptor_r *)formal_ptr->dscptrs[j + 2];
    if (actual_arg_ptr[j]
	&& formal_arg_ptr->dtype == DTYPE_FUNCTION && formal_arg_ptr->ndesc == 1) {
      if ((code = *(unsigned short *)formal_arg_ptr->pointer) == OpcOptional) {
	formal_arg_ptr = (struct descriptor_r *)formal_arg_ptr->dscptrs[0];
	if (formal_arg_ptr->dtype != DTYPE_FUNCTION || formal_arg_ptr->ndesc != 1)
	  code = 0;
	else
	  code = *(unsigned short *)formal_arg_ptr->pointer;
      }
      if (code == OpcInOut || code == OpcOut) {
	status =
	    TdiFindIdent(7, (struct descriptor_r *)formal_arg_ptr->dscptrs[0], 0, &node_ptr, 0);
				/**************************
                                Do this with old variables.
                                **************************/
	new_head = _private.head;
	_private.head = old_head;
	if STATUS_OK
	  status = TdiPutIdent(actual_arg_ptr[j], &node_ptr->xd);
	old_head = _private.head;
	_private.head = new_head;
      }
    }
  }
  TdiDeallocate(&tmp MDS_END_ARG);
  MdsFree1Dx(&tmp, NULL);
  _private.head = old_head;
  *new_narg = old_narg;
  return status;
}

/***************************************************************
        Replace variable. Assumed called by INTRINSIC so that out_ptr is XD-DSC.
*/
int Tdi1Equals(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiEvaluate(list[1], out_ptr MDS_END_ARG);
	/************************************
        Place the data or clear the variable.
        ************************************/
  if STATUS_OK
    status = TdiPutIdent((struct descriptor_r *)list[0], out_ptr);
  else
    TdiPutIdent((struct descriptor_r *)list[0], 0);
  return status;
}

/*--------------------------------------------------------------
        Store in first argument of binary opertor.
*/
int Tdi1EqualsFirst(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  if (list[0]->dtype != DTYPE_FUNCTION || ((struct descriptor_function *)list[0])->ndesc != 2)
    status = TdiINVDTYDSC;
  else
    status = TdiEquals(((struct descriptor_r *)list[0])->dscptrs[0], list[0], out_ptr MDS_END_ARG);
  return status;
}

/*--------------------------------------------------------------
        Decrement a variable before use.
*/
int Tdi1PreDec(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiSubtract(list[0], &true_dsc, out_ptr MDS_END_ARG);
  if STATUS_OK
    status = TdiPutIdent((struct descriptor_r *)list[0], out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Increment a variable before use.
*/
int Tdi1PreInc(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiAdd(list[0], &true_dsc, out_ptr MDS_END_ARG);
  if STATUS_OK
    status = TdiPutIdent((struct descriptor_r *)list[0], out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Decrement a variable after use.
*/
int Tdi1PostDec(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;

  status = TdiGetIdent(list[0], out_ptr);
  if STATUS_OK
    status = TdiSubtract(out_ptr->pointer, &true_dsc, &tmp MDS_END_ARG);
  if STATUS_OK
    status = TdiPutIdent((struct descriptor_r *)list[0], &tmp);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Increment a variable after use.
*/
int Tdi1PostInc(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  status = TdiGetIdent(list[0], out_ptr);
  if STATUS_OK
    status = TdiAdd(out_ptr->pointer, &true_dsc, &tmp MDS_END_ARG);
  if STATUS_OK
    status = TdiPutIdent((struct descriptor_r *)list[0], &tmp);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/***************************************************************
        Find by identifier.
        PRIVATE and PUBLIC must have text argument, not expression.
        They are keywords, not standard functions. NEED we change this?
*/
int Tdi1Private(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  node_type *node_ptr;
  status = LibLookupTree((void **)&_private.head, (void *)list[0], compare, (void **)&node_ptr);
  if STATUS_OK
    status = MdsCopyDxXd(node_ptr->xd.pointer, out_ptr);
  else if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  return status;
}

/*--------------------------------------------------------------
        Find by identifier.
        PRIVATE and PUBLIC must have text argument, not expression.
*/
int Tdi1Public(int opcode __attribute__ ((unused)), int narg __attribute__((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status;
  LOCK_PUBLIC_PUSH;
  node_type *node_ptr;
  status = LibLookupTree((void **)&_public.head, (void *)list[0], compare, (void **)&node_ptr);
  if STATUS_OK
    status = MdsCopyDxXd(node_ptr->xd.pointer, out_ptr);
  else if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  UNLOCK_PUBLIC;
  return status;
}

/*--------------------------------------------------------------
        Find by text expression.
*/
int Tdi1Var(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  INIT_AND_FREEXD_ON_EXIT(tmp);
  status = TdiData(list[0], &tmp MDS_END_ARG);
  if STATUS_OK {
    if (narg < 2 || list[1] == 0)
      status = TdiGetIdent(tmp.pointer, out_ptr);
    else
      status = TdiEquals(tmp.pointer, list[1], out_ptr MDS_END_ARG);
  }
  FREEXD_NOW(tmp);
  return status;
}

/***************************************************************
        Define a function by reconstruction.
*/
int Tdi1Fun(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  DESCRIPTOR_FUNCTION(hold, 0, 255);
  int j;
  unsigned short opcode_s = (unsigned short)opcode;
  hold.pointer = (unsigned char *)&opcode_s;
  hold.ndesc = (unsigned char)narg;
  for (j = narg; --j >= 0;)
    if ((hold.arguments[j] = list[j]) == 0)
      return TdiNULL_PTR;
  status = MdsCopyDxXd((struct descriptor *)&hold, out_ptr);
  if STATUS_OK
    status = TdiPutIdent((struct descriptor_r *)list[0], out_ptr);
  return status;
}

/***************************************************************
        Release the private variables.
        This to be used by functions.
*/
int Tdi1ResetPrivate()
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
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
  _public.head = 0;
  TdiResetGetRecord();
  if (_public.data_zone)
    status = LibResetVmZone(&_public.data_zone);
  else if (_public.head_zone)
    status = LibResetVmZone(&_public.head_zone);
  else status = MDSplusSUCCESS;
  UNLOCK_PUBLIC;
  return status;
}

/***************************************************************
        Display a variable.
*/
STATIC_ROUTINE int show_one(node_type * node_ptr, user_type * user_ptr)
{
  INIT_STATUS;
  struct descriptor_d tmp = EMPTY_D;
  struct descriptor_r *rptr = (struct descriptor_r *)node_ptr->xd.pointer;

  if (node_ptr->xd.class == 0)
    return 1;
  if (user_ptr->match.length == 0)
    status = MDSplusSUCCESS;
  else
    status = StrMatchWild(&node_ptr->name_dsc, &user_ptr->match);
  if STATUS_OK {
    if (rptr)
      status = TdiDecompile(rptr, &tmp MDS_END_ARG);
    if STATUS_OK {
      if (rptr && rptr->dtype == DTYPE_FUNCTION && *(unsigned short *)rptr->pointer == OpcFun)
	printf("%.*s\n", tmp.length, tmp.pointer);
      else if (tmp.length > 0)
	printf("%s %.*s\t= %.*s\n",
	       user_ptr->block_ptr->_public ? "Public" : "Private",
	       node_ptr->name_dsc.length, node_ptr->name_dsc.pointer, tmp.length, tmp.pointer);
      else
	printf("%s %.*s\t=\n",
	       user_ptr->block_ptr->_public ? "Public" : "Private",
	       node_ptr->name_dsc.length, node_ptr->name_dsc.pointer);
      user_ptr->count++;
    } else
      printf("ERROR on %.*s\n", node_ptr->name_dsc.length, node_ptr->name_dsc.pointer);
    StrFree1Dx(&tmp);
  } else
    status = MDSplusSUCCESS;
  return status;
}

/*--------------------------------------------------------------
        Display private variables.
*/
int Tdi1ShowPrivate(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr){
  GET_TDITHREADSTATIC_P;
  return wild((int (*)())show_one, narg, list, &_private, out_ptr);
}

/*--------------------------------------------------------------
        Display public variables.
*/
int Tdi1ShowPublic(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status;
  LOCK_PUBLIC_PUSH;
  status = wild((int (*)())show_one, narg, list, &_public, out_ptr);
  UNLOCK_PUBLIC;
  return status;
}

extern EXPORT int TdiSaveContext(void *ptr[6]){
  GET_TDITHREADSTATIC_P;
  ptr[0] = (void *)_private.head;
  ptr[1] = _private.head_zone;
  ptr[2] = _private.data_zone;
  LOCK_PUBLIC_PUSH;
  ptr[3] = (void *)_public.head;
  ptr[4] = _public.head_zone;
  ptr[5] = _public.data_zone;
  UNLOCK_PUBLIC;
  return 1;
}

extern EXPORT int TdiDeleteContext(void *ptr[6])
{
  if (ptr[1])
    LibDeleteVmZone(&ptr[1]);
  if (ptr[2])
    LibDeleteVmZone(&ptr[2]);
  if (ptr[4] || ptr[5]) {
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
extern EXPORT int TdiRestoreContext(void *ptr[6]){
  GET_TDITHREADSTATIC_P;
  _private.head = (node_type *) ptr[0];
  _private.head_zone = ptr[1];
  _private.data_zone = ptr[2];
  LOCK_PUBLIC_PUSH;
  _public.head = ptr[3];
  _public.head_zone = (node_type *) ptr[4];
  _public.data_zone = ptr[5];
  UNLOCK_PUBLIC;
  return 1;
}
