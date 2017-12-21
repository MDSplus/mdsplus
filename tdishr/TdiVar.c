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
#include <mdsplus/mdsconfig.h>
#include "STATICdef.h"
#define DEF_FREEXD
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

STATIC_THREADSAFE pthread_mutex_t lock;
STATIC_THREADSAFE int lock_initialized = 0;
STATIC_THREADSAFE block_type _public = { 0, 0, 0, 1 };

STATIC_CONSTANT DESCRIPTOR(star, "*");
STATIC_CONSTANT DESCRIPTOR(percent, "%");
struct descriptor *Tdi3Narg(){
  return &TdiGetThreadStatic()->TdiVar_new_narg_d;
}

/*--------------------------------------------------------------
        Put a byte to output.
*/
int TdiPutLogical(unsigned char data, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_CONSTANT unsigned short len = (unsigned short)sizeof(unsigned char);
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_BU;
  if (out_ptr == 0)
    return 0;
  status = MdsGet1DxS(&len, &dtype, out_ptr);
  if STATUS_OK
    *(unsigned char *)out_ptr->pointer->pointer = data;
  return status;
}

/*--------------------------------------------------------------
        Comparison routine.
*/
STATIC_ROUTINE int compare(struct descriptor *key_ptr, node_type * node_ptr, block_type * block_ptr __attribute__ ((unused)))
{
  return StrCaseBlindCompare(key_ptr, &node_ptr->name_dsc);
}

/*--------------------------------------------------------------
        Allocation routine, does not set data field.
        ASSUMED called only once per node.
*/
STATIC_ROUTINE int allocate(struct descriptor *key_ptr,
			    node_type ** node_ptr_ptr, block_type * block_ptr)
{
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
        search: 1=public 2=private 4=check that it exists.
*/
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
  block_type *block_ptr;
  int code;
  block_type *private = (block_type *) & TdiThreadStatic_p->TdiVar_private;

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
      case DTYPE_IDENT:
	block_ptr = private;
	key_dsc = *(struct descriptor *)ident_ptr;
	key_dsc.dtype = DTYPE_T;
	key_dsc.class = CLASS_S;
	if (search & 4) {
	  if (search & 1) {
	    status = LibLookupTree((void *)&private->head, (void *)&key_dsc, compare, (void **)&node_ptr);
	    if STATUS_OK {
	      if (node_ptr->xd.class != 0) {
		break;
	      } else {
		status = TdiUNKNOWN_VAR;
	      }
	    }
	  }
	  if (search & 2) {
	    status = LibLookupTree((void *)&_public.head, (void *)&key_dsc, compare, (void **)&node_ptr);
	    if STATUS_OK {
	      if (node_ptr->xd.class != 0) {
		block_ptr = &_public;
	      } else {
		status = TdiUNKNOWN_VAR;
	      }
	    }
	  }
	} else if (search & 1) ;
	else if (search & 2)
	  block_ptr = &_public;
	break;
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
	status =
	    TdiFindIdent(search & ~1,
			 (struct descriptor_r *)ident_ptr->dscptrs[0],
			 &key_dsc, &node_ptr, &block_ptr);
      else if (code == OpcPrivate)
	status =
	    TdiFindIdent(search & ~2,
			 (struct descriptor_r *)ident_ptr->dscptrs[0],
			 &key_dsc, &node_ptr, &block_ptr);
      else if (code == OpcEquals || code == OpcEqualsFirst
	       || code == OpcPostDec || code == OpcPreDec
	       || code == OpcPostInc || code == OpcPreInc) {
	struct descriptor_xd tmp = EMPTY_XD;
	status = TdiEvaluate(ident_ptr, &tmp MDS_END_ARG);
	if STATUS_OK
	  status =
	      TdiFindIdent(search,
			   (struct descriptor_r *)ident_ptr->dscptrs[0], &key_dsc, &node_ptr,
			   &block_ptr);
      } else if (code == OpcVar) {
	name_dsc = EMPTY_D;
	status = TdiData(ident_ptr->dscptrs[0], &name_dsc MDS_END_ARG);
	if STATUS_OK
	  status =
	      TdiFindIdent(search, (struct descriptor_r *)&name_dsc,
			   &key_dsc, &node_ptr, &block_ptr);
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
    if (block_ptr_ptr) *block_ptr_ptr = block_ptr;
  } else {
    if (node_ptr_ptr)  *node_ptr_ptr  = NULL;
    if (block_ptr_ptr) *block_ptr_ptr = NULL;
  }
  return status;
}

/*--------------------------------------------------------------
        Get active variable for VAR and EVALUATE. Copies to MDSSHR space.
*/
int TdiGetIdent(struct descriptor *ident_ptr, struct descriptor_xd *data_ptr)
{
  node_type *node_ptr;
  INIT_STATUS;

  LockTdiMutex(&lock, &lock_initialized);
  status = TdiFindIdent(7, (struct descriptor_r *)ident_ptr, 0, &node_ptr, 0);
  if STATUS_OK
    status = MdsCopyDxXd(node_ptr->xd.pointer, data_ptr);
  else
    MdsFree1Dx(data_ptr, NULL);
  UnlockTdiMutex(&lock);
  return status;
}

/*--------------------------------------------------------------
        Keep an XD-DSC of whatever.
        Assumes we are given XD or class-0.
*/
int TdiPutIdent(struct descriptor_r *ident_ptr, struct descriptor_xd *data_ptr)
{
  struct descriptor key_dsc = EMPTDY_S;
  node_type *node_ptr;
  block_type *block_ptr;
  struct descriptor_d upstr = { 0, DTYPE_T, CLASS_D, 0 };
  INIT_STATUS;
  STATIC_CONSTANT int zero = 0;
  if (ident_ptr->dtype == DTYPE_DSC)
    return TdiPutIdent((struct descriptor_r *)ident_ptr->pointer, data_ptr);
  LockTdiMutex(&lock, &lock_initialized);

	/************************************
        Find where we should place the stuff.
        ************************************/
  status = TdiFindIdent(3, ident_ptr, &key_dsc, 0, &block_ptr);
  StrUpcase((struct descriptor *)&upstr, &key_dsc);
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
  UnlockTdiMutex(&lock);
  return status;
}

/***************************************************************
        Display/free wildcarded variables.
        IDENT names are not evaluated.
        PUBLIC or PRIVATE(ident or text) overrides block_ptr.
*/
STATIC_ROUTINE int wild(int (*doit) (),
			int narg,
			struct descriptor *list[],
			block_type * block_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  node_type *node_ptr;
  user_type user = { {0, DTYPE_T, CLASS_D, 0}, 0, 0 };
  int j;
  LockTdiMutex(&lock, &lock_initialized);
  user.block_ptr = block_ptr;
  if (narg == 0 || list[0] == 0)
    status = LibTraverseTree((void **)&block_ptr->head, doit, &user);
  else
    for (j = 0; STATUS_OK && j < narg; ++j) {
      status = TdiFindIdent(3, (struct descriptor_r *)list[j], &user.match, 0, &user.block_ptr);
      if STATUS_OK
	status = StrUpcase(&user.match, &user.match);
      if STATUS_OK {
	if (StrPosition(&user.match, (struct descriptor *)&star, 0)
	    || StrPosition(&user.match, (struct descriptor *)&percent, 0)) {
	  status = LibTraverseTree((void **)&user.block_ptr->head, doit, &user);
	} else {
	  status = LibLookupTree((void **)&user.block_ptr->head, (void *)&user.match, compare, (void **)&node_ptr);
	  if STATUS_OK
	    status = (*doit) (node_ptr, &user);
	  else if (status == LibKEYNOTFOU)
	    status = MDSplusSUCCESS;
	}
      }
    }
  if (user.match.class == CLASS_D)
    StrFree1Dx((struct descriptor_d *)&user.match);
  if STATUS_OK
    status = TdiPutLong(&user.count, out_ptr);
  UnlockTdiMutex(&lock);
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
  block_type *private = (block_type *) &TdiThreadStatic_p->TdiVar_private;

  if ((*pnode)->xd.l_length)
    status = LibFreeVm(&(*pnode)->xd.l_length, (void *)&(*pnode)->xd.pointer, &private->data_zone);
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
  stat2 = LibFreeVm(&len, (void *)pnode, &private->head_zone);
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
  block_type *private = (block_type *) &TdiThreadStatic_p->TdiVar_private;
  if (narg == 0 && private->head) {
    status = free_all(&private->head);
    private->head = 0;
    return status;
  }
  return wild((int (*)())free_one, narg, list, private, out_ptr);
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

  LockTdiMutex(&lock, &lock_initialized);
  status = TdiFindIdent(3, (struct descriptor_r *)list[0], &key_dsc, 0, &block_ptr);
  if STATUS_OK
    status = LibLookupTree((void **)&block_ptr->head, (void *)&key_dsc, compare, (void **)&node_ptr);
  found = STATUS_OK;
  if (found)
    found = node_ptr->xd.class != 0;
  else if (status == LibKEYNOTFOU || status == TdiUNKNOWN_VAR)
    status = MDSplusSUCCESS;
  if STATUS_OK
    status = TdiPutLogical((unsigned char)found, out_ptr);
  UnlockTdiMutex(&lock);
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
  LockTdiMutex(&lock, &lock_initialized);
  status = TdiFindIdent(3, (struct descriptor_r *)list[0], &key_dsc, 0, &block_ptr);
  if STATUS_OK
    status = LibLookupTree((void **)&block_ptr->head, (void *)&key_dsc, compare, (void **)&node_ptr);
  found = STATUS_OK;
  if (found) ;
  else if (status == LibKEYNOTFOU || status == TdiUNKNOWN_VAR)
    status = MDSplusSUCCESS;
  if STATUS_OK
    status = TdiPutLogical((unsigned char)found, out_ptr);
  UnlockTdiMutex(&lock);
  return status;
}

/***************************************************************
        Execute a function. Set up the arguments.
*/

int TdiDoFun(struct descriptor *ident_ptr,
	     int nactual, struct descriptor_r *actual_arg_ptr[], struct descriptor_xd *out_ptr)
{
  GET_TDITHREADSTATIC_P;
  node_type *node_ptr;
  struct descriptor_r *formal_ptr = 0, *formal_arg_ptr, *actual_ptr;
  struct descriptor_xd tmp = EMPTY_XD;
  node_type *old_head, *new_head = 0;
  int code, opt, j, nformal = 0, status;
  int *new_narg = &TdiThreadStatic_p->TdiVar_new_narg;
  int old_narg = *new_narg;
  block_type *private = (block_type *) &TdiThreadStatic_p->TdiVar_private;

	/******************************************
        Get name of function to do. Check its type.
        ******************************************/

  status = TdiFindIdent(7, (struct descriptor_r *)ident_ptr, 0, &node_ptr, 0);
  if (STATUS_NOT_OK) ;
  else if ((formal_ptr = (struct descriptor_r *)node_ptr->xd.pointer) == 0
	   || formal_ptr->dtype != DTYPE_FUNCTION
	   || *(unsigned short *)formal_ptr->pointer != OpcFun)
    status = TdiUNKNOWN_VAR;
  else if ((nformal = formal_ptr->ndesc - 2) < nactual)
    status = TdiEXTRA_ARG;
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
      old_head = private->head;
      private->head = new_head;
      status = TdiPutIdent(formal_arg_ptr, 0);
      new_head = private->head;
      private->head = old_head;
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
      old_head = private->head;
      private->head = new_head;
      if STATUS_OK
	status = TdiPutIdent(formal_arg_ptr, &tmp);
      new_head = private->head;
      private->head = old_head;
    }
  }
	/**************************
        Do this with new variables.
        **************************/
  old_head = private->head;
  private->head = new_head;
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
	new_head = private->head;
	private->head = old_head;
	if STATUS_OK
	  status = TdiPutIdent(actual_arg_ptr[j], &node_ptr->xd);
	old_head = private->head;
	private->head = new_head;
      }
    }
  }
  TdiDeallocate(&tmp MDS_END_ARG);
  MdsFree1Dx(&tmp, NULL);
  private->head = old_head;
  *new_narg = old_narg;
  if (status == TdiUNKNOWN_VAR)
    status = TdiExtPython(ident_ptr, nactual, (struct descriptor **)actual_arg_ptr, out_ptr);
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
  block_type *private = (block_type *) &TdiThreadStatic_p->TdiVar_private;

  status = LibLookupTree((void **)&private->head, (void *)list[0], compare, (void **)&node_ptr);
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
  INIT_STATUS;
  node_type *node_ptr;
  LockTdiMutex(&lock, &lock_initialized);

  status = LibLookupTree((void **)&_public.head, (void *)list[0], compare, (void **)&node_ptr);
  if STATUS_OK
    status = MdsCopyDxXd(node_ptr->xd.pointer, out_ptr);
  else if (status == LibKEYNOTFOU)
    status = TdiUNKNOWN_VAR;
  UnlockTdiMutex(&lock);
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
  block_type *private = (block_type *) &TdiThreadStatic_p->TdiVar_private;

  private->head = 0;
  if (private->data_zone)
    status = LibResetVmZone(&private->data_zone);
  if (private->head_zone)
    status = LibResetVmZone(&private->head_zone);
  return status;
}

/*--------------------------------------------------------------
        Release the public variables.
*/
int Tdi1ResetPublic()
{
  INIT_STATUS;

  LockTdiMutex(&lock, &lock_initialized);
  _public.head = 0;
  TdiResetGetRecord();
  if (_public.data_zone)
    status = LibResetVmZone(&_public.data_zone);
  if (_public.head_zone)
    status = LibResetVmZone(&_public.head_zone);
  UnlockTdiMutex(&lock);
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
  block_type *_private = (block_type *) &TdiThreadStatic_p->TdiVar_private;
  return wild((int (*)())show_one, narg, list, _private, out_ptr);
}

/*--------------------------------------------------------------
        Display public variables.
*/
int Tdi1ShowPublic(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  LockTdiMutex(&lock, &lock_initialized);
  status = wild((int (*)())show_one, narg, list, &_public, out_ptr);
  UnlockTdiMutex(&lock);
  return status;
}

extern EXPORT int TdiSaveContext(void *ptr[6]){
  GET_TDITHREADSTATIC_P;
  block_type *_private;
  LockTdiMutex(&lock, &lock_initialized);
  _private = (block_type *) &TdiThreadStatic_p->TdiVar_private;
  ptr[0] = (void *)_private->head;
  ptr[1] = _private->head_zone;
  ptr[2] = _private->data_zone;
  ptr[3] = (void *)_public.head;
  ptr[4] = _public.head_zone;
  ptr[5] = _public.data_zone;
  UnlockTdiMutex(&lock);
  return 1;
}

extern EXPORT int TdiDeleteContext(void *ptr[6])
{
  if (ptr[1])
    LibDeleteVmZone(&ptr[1]);
  if (ptr[2])
    LibDeleteVmZone(&ptr[2]);
  if (ptr[4])
    LibDeleteVmZone(&ptr[4]);
  if (ptr[5])
    LibDeleteVmZone(&ptr[5]);
  return 1;
}

/*-------------------------------------------------------------
        Restore variable context
*/
extern EXPORT int TdiRestoreContext(void *ptr[6]){
  GET_TDITHREADSTATIC_P;
  block_type *_private;
  LockTdiMutex(&lock, &lock_initialized);
  _private = (block_type *) &TdiThreadStatic_p->TdiVar_private;
  _private->head = (node_type *) ptr[0];
  _private->head_zone = ptr[1];
  _private->data_zone = ptr[2];
  _public.head = ptr[3];
  _public.head_zone = (node_type *) ptr[4];
  _public.data_zone = ptr[5];
  UnlockTdiMutex(&lock);
  return 1;
}
