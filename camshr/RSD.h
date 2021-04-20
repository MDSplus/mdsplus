// RSD.h -- request sense data
#ifndef __RSD_H
#define __RSD_H
#pragma pack(1)
// for Linux on x86
// NB! order of members is different from original
//      to get data to pack into same size memory, ie
//      40 bytes
typedef struct
{
  __u8 error_code : 7;
  __u8 valid : 1;

  __u8 segment_number;

  __u8 sense_key : 4;
  __u8 rsvd : 1;
  __u8 ili : 1;
  __u8 eom : 1;
  __u8 fm : 1;

  __u32 information_long;

  __u8 sense_length;

  __u32 command_specific_information;

  __u8 sense_code;

  __u8 sense_qual;

  __u32 reserved;

  __u32 stat;

  union {
    __u16 stacsr;
    struct
    {
      __u8 noq : 1;
      __u8 nox : 1;
      __u8 done : 1;
      __u8 lam_pending : 1;
      __u8 : 1;
      __u8 qrpt_timeout : 1;
      __u8 : 1;
      __u8 abort : 1;
      __u8 xmt_fifo_empty : 1;
      __u8 xmt_fifo_full : 1;
      __u8 rcv_fifo_empty : 1;
      __u8 rcv_fifo_full : 1;
      __u8 high_byte_first : 1;
      __u8 scsi_id : 3;
    } csr;
  } u1;

  union {
    __u32 staesr;
    ErrorStatusRegister esr;
  } u2;

  __u32 staccs;

  __u32 stasum;

  __u32 stacnt;
} RequestSenseData;
#pragma pack(4)
#endif
