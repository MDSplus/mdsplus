#ifndef _TDITHREADSTATIC_H
#define _TDITHREADSTATIC_H

#include "../mdsshr/mdsthreadstatic.h"
#include "tdirefzone.h"

#define TDI_STACK_SIZE 3

//#define TDITHREADSTATIC_VAR  TDITHREADSTATIC_VAR
//#define TDITHREADSTATIC_TYPE TdiThreadStatic
#define TDITHREADSTATIC_ARG TDITHREADSTATIC_TYPE *const TDITHREADSTATIC_VAR
#define TDITHREADSTATIC(MTS) TDITHREADSTATIC_ARG = TdiGetThreadStatic(MTS)
#define TDITHREADSTATIC_INIT TDITHREADSTATIC(NULL)

typedef union {
  int16_t cnt[4];
  int64_t all;
} balance_t;
typedef struct
{
  void *scanner;
  int stack_idx;
  struct stack
  {
    void *buffer;
    TdiRefZone_t refzone;   // TdiCompile
    int compile_rec;        // TdiCompile
    int getdata_rec;        // TdiGetData
    int indent;             // TdiDecompile, TdiDecompileR
    uint16_t decompile_max; // TdiDecompile
    balance_t balance;
  } stack[TDI_STACK_SIZE];
  struct
  {
    void *head;
    void *head_zone;
    void *data_zone;
    int public;
  } var_private;            // TdiVar
  mdsdsc_t var_new_narg_d;  // TdiVar
  int var_new_narg;         // TdiVar
  int var_rec;              // TdiVar
  mdsdsc_t *range_ptrs[3];  // TdiCull, TdiDtypeRange, TdiItoX, TdiSubscript
  mdsdsc_xd_t *self_ptr;    // TdiGetArgs, TdiGetData, TdiSubscript
  mdsdsc_d_t intrinsic_msg; // TdiIntrinsic
  int intrinsic_rec;        // TdiIntrinsic
  int intrinsic_stat;       // TdiIntrinsic
  int use_get_record_fun;   // TdiGetData, TdiVar
} TDITHREADSTATIC_TYPE;

#define TDI_SCANNER (TDITHREADSTATIC_VAR->scanner)
#define TDI_STACK_IDX (TDITHREADSTATIC_VAR->stack_idx)
#define TDI_STACK (TDITHREADSTATIC_VAR->stack[TDI_STACK_IDX])
#define TDI_VAR_PRIVATE (TDITHREADSTATIC_VAR->var_private)
#define TDI_INTRINSIC_MSG (TDITHREADSTATIC_VAR->intrinsic_msg)
#define TDI_INTRINSIC_STAT (TDITHREADSTATIC_VAR->intrinsic_stat)
#define TDI_INTRINSIC_REC (TDITHREADSTATIC_VAR->intrinsic_rec)
#define TDI_RANGE_PTRS (TDITHREADSTATIC_VAR->range_ptrs)
#define TDI_SELF_PTR (TDITHREADSTATIC_VAR->self_ptr)
#define TDI_VAR_NEW_NARG (TDITHREADSTATIC_VAR->var_new_narg)
#define TDI_VAR_NEW_NARG_D (TDITHREADSTATIC_VAR->var_new_narg_d)
#define TDI_VAR_REC (TDITHREADSTATIC_VAR->var_rec)
#define TDI_USE_GET_RECORD_FUN (TDITHREADSTATIC_VAR->use_get_record_fun)

#define TDI_DECOMPILE_MAX (TDI_STACK.decompile_max)
#define TDI_COMPILE_REC (TDI_STACK.compile_rec)
#define TDI_GETDATA_REC (TDI_STACK.getdata_rec)
#define TDI_INDENT (TDI_STACK.indent)
#define TDI_REFZONE (TDI_STACK.refzone)
#define TDI_BUFFER (TDI_STACK.buffer)
#define TDI_BALANCE_R (TDI_STACK.balance.cnt[0])
#define TDI_BALANCE_S (TDI_STACK.balance.cnt[1])
#define TDI_BALANCE_C (TDI_STACK.balance.cnt[2])
#define TDI_BALANCE (TDI_STACK.balance.all)

extern DEFINE_GETTHREADSTATIC(TDITHREADSTATIC_TYPE, TdiGetThreadStatic);
#endif // ifndef _TDITHREADSTATIC_H
