// SD.h -- sense data
#ifndef __SD_H
#define __SD_H

// for Linux on x86
typedef struct
{
  __u8 code : 7;
  __u8 valid : 1;

  __u8 segment_no;

  __u16 sense_key : 4;
  __u16 zero1 : 12;

  __u8 word_count_defect[3];

  __u8 additional_sense_length;

  struct
  {
    __u8 bdmd : 1; // branch demand present
    __u8 dsne : 1; // serial demand stack not empty
    __u8 bdsq : 1; // branch demand sequencer set
    __u8 snex : 1; // command not executed on serial highway
    __u8 crto : 1; // crate overflowed (SCS set)
    __u8 to : 1;   // time-out n parallel or serial highway
    __u8 no_x : 1; // module returned X = 0
    __u8 no_q : 1; // module returned Q = 0
  } main_status_reg;

  struct
  {
    // 'high' byte
    __u8 cret : 1;   // command type message detected at receiver
    __u8 timos : 1;  // serial time-out
    __u8 rpe : 1;    // parity error in reply
    __u8 hdrrec : 1; // truncated command header received
    __u8 cmdfor : 1; // corrupt command format
    __u8 rnre1 : 1;  // reply not recognized (length = 1)
    __u8 rnrg1 : 1;  // reply not recognized (length > 1)
    __u8 snex : 1;   // serial no execution

    // 'lo' byte
    __u8 fill : 2;
    __u8 hngd : 1; // hung demand message received
    __u8 spare : 1;
    __u8 sync : 1;  // receiver has lost byte sync (bit mode only)
    __u8 losyn : 1; // sync lost while awaiting reply (bit mode only)
    __u8 rerr : 1;  // error bit set in reply
    __u8 derr : 1;  // delayed error bit set in reply
  } serial_status_reg;

  __u8 zero2;

  __u8 additional_sense_code;

  __u8 zero3;

  __u8 slot_high_bit : 1;
  __u8 crate : 7;

  __u8 address : 4;
  __u8 slot : 4;
} SenseData;

#endif
