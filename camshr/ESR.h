// ESR.h  -- error status register
#ifndef __ESR_H
#define __ESR_H

// for Linux on x86
typedef struct
{
  __u32 noq : 1;
  __u32 nox : 1;
  __u32 ste : 1;
  __u32 adnr : 1;
  __u32 tpe : 1;
  __u32 lpe : 1;
  __u32 n_gt_23 : 1;
  __u32 err : 1;
  __u32 no_sync : 1;
  __u32 tmo : 1;
  __u32 derr : 1;
  __u32 should_be_zero : 5;
  __u32 error_code : 4;
  __u32 should_be_0 : 2;
  __u32 no_halt : 1;
  __u32 read : 1;
  __u32 ad : 1;
  __u32 word_size : 2;
  __u32 qmd : 2;
  __u32 tm : 2;
  __u32 cm : 1;

#if 0
  __u8 noq:1;
  __u8 nox:1;
  __u8 ste:1;
  __u8 adnr:1;
  __u8 tpe:1;
  __u8 lpe:1;
  __u8 n_gt_23:1;
  __u8 err:1;
  __u8 no_sync:1;
  __u8 tmo:1;
  __u8 derr:1;
  __u8 should_be_zero:5;
  __u8 error_code:4;
  __u8 should_be_0:2;
  __u8 no_halt:1;
  __u8 read:1;
  __u8 ad:1;
  __u8 word_size:2;
  __u8 qmd:2;
  __u8 tm:2;
  __u8 cm:1;
#endif
} ErrorStatusRegister;

#endif
