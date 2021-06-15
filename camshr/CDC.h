// CAMAC data command
#ifndef __CDC_H
#define __CDC_H

// for Linux on x86
typedef struct
{
  __u8 opcode;

  __u8 zero1;

  __u8 f : 5;
  __u8 bs : 1; // 1->24-bit   0->16-bit
  __u8 zero2 : 2;

  __u8 n : 5;
  __u8 m : 3;

  __u8 a : 4;
  __u8 sncx : 1; // skip non existant == 0
  __u8 scs : 1;  // single crate scan == 0
  __u8 dd : 1;   // disable SCSI detection & reconnection
  __u8 zero3 : 1;

  __u8 crate : 7;
  __u8 sp : 1; // serial or parallel

  __u8 transfer_len[3]; // reverse order, ie MSB, ..., LSB

  __u8 zero4;
} CamacDataCommand;

#endif
