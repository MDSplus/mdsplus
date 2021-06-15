// module.h
//==================================
#ifndef __MODULE_H
#define __MODULE_H

// structure of CTS_DB_FILE
struct MODULE
{
  //--------------------------
  // logical CAMAC module name
  //--------------------------
  char Name[32]; // NB! no spaces allowed, but can use _'s
  char r1;

  //--------------------------
  // physical name
  //--------------------------
  unsigned char r2[2]; // 'GK'         <<-- fixed characters

  //--------------------------
  // scsi info
  //--------------------------
  char Adapter; // 'A'(=scsi0), 'B'(=scsi1), ...
  char Id;      // 0, 1, 2, ...

  //--------------------------
  // CAMAC info
  //--------------------------
  char Crate[2];       // 01, 02, ..., 99
  unsigned char r3[2]; // 'N:'         <<-- fixed characters
  char Slot[2];        // 1, 2, ..., 30

  unsigned char r4;

  //--------------------------
  // comment
  //--------------------------
  char Comment[41];
};
#define MODULE_ENTRY sizeof(struct MODULE)

// internal structure
typedef struct Module_
{
  char name[32]; // 2002.02.07
  int adapter;
  int id;
  int crate;
  int slot;
  char comment[41]; // 2002.02.07
} Module;

// linked list structure
struct t_mod
{
  char mod_name[32];  // module logical name
  struct t_mod *next; // pointer to next element
};

#endif
