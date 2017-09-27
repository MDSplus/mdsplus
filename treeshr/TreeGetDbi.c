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
#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <dbidef.h>

extern void **TreeCtx();
static inline int minInt(int a, int b) { return a < b ? a : b;}

int TreeGetDbi(struct dbi_itm *itmlst)
{
  return _TreeGetDbi(*TreeCtx(), itmlst);
}

#define set_retlen(length) if ((unsigned int)lst->buffer_length < length) { status = TreeBUFFEROVF; break; } else retlen=length
#define CheckOpen(db) if (!db || !db->open) {status=TreeNOT_OPEN;break;}
#define set_ret_char(val) memset(lst->pointer, 0, (size_t)lst->buffer_length); *((char *)lst->pointer) = val

int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst)
{
  PINO_DATABASE *db = (PINO_DATABASE *) dbid;
  struct dbi_itm *lst = itmlst;
  int status = TreeNORMAL;
  while ((lst->code != 0) && (status & 1)) {
    char *string = NULL;
    unsigned short retlen = 0;
    switch (lst->code) {
    case DbiNAME:

      CheckOpen(db);
      string = strcpy(malloc(strlen(db->main_treenam) + 1), db->main_treenam);
      break;

    case DbiSHOTID:

      CheckOpen(db);
      set_retlen(sizeof(db->shotid));
      *(int *)lst->pointer = db->shotid;
      break;

    case DbiMODIFIED:

      CheckOpen(db);
      set_ret_char(db->modified);
      break;

    case DbiOPEN_FOR_EDIT:

      CheckOpen(db);
      set_ret_char(db->open_for_edit);
      break;

    case DbiOPEN_READONLY:

      CheckOpen(db);
      set_ret_char(db->open_readonly);
      break;

    case DbiINDEX:

      {
	int idx;
	for (idx = 0, db = (PINO_DATABASE *) dbid; db && (idx < (*(int *)(lst->pointer) & 0xff));
	     idx++, db = db->next) ;
	break;
      }

    case DbiNUMBER_OPENED:

      {
	int count;
	PINO_DATABASE *db_tmp;
	for (count = 0, db_tmp = (PINO_DATABASE *) dbid; db_tmp ? db_tmp->open : 0;
	     count++, db_tmp = db_tmp->next) ;
	memset(lst->pointer, 0, (size_t)lst->buffer_length);
	memcpy(lst->pointer, &count, (size_t)minInt(lst->buffer_length, sizeof(int)));
	if (lst->return_length_address)
	  *lst->return_length_address = minInt(lst->buffer_length, sizeof(int));
	break;
      }

    case DbiMAX_OPEN:

      {
	int count = db->stack_size;
	memset(lst->pointer, 0, (size_t)lst->buffer_length);
	memcpy(lst->pointer, &count, (size_t)minInt(lst->buffer_length, sizeof(int)));
	if (lst->return_length_address)
	  *lst->return_length_address = minInt(lst->buffer_length, sizeof(int));
	break;
      }

    case DbiDEFAULT:

      CheckOpen(db);
      {
	int nid;
	_TreeGetDefaultNid(db, &nid);
	string = _TreeGetPath(db, nid);
      }
      break;

    case DbiVERSIONS_IN_MODEL:
      CheckOpen(db);
      {
	int value = db->tree_info->header->versions_in_model;
	memset(lst->pointer, 0, (size_t)lst->buffer_length);
	memcpy(lst->pointer, &value, (size_t)minInt(lst->buffer_length, sizeof(int)));
	if (lst->return_length_address)
	  *lst->return_length_address = minInt(lst->buffer_length, sizeof(int));
	break;
      }

    case DbiVERSIONS_IN_PULSE:
      CheckOpen(db);
      {
	int value = db->tree_info->header->versions_in_pulse;
	memset(lst->pointer, 0, (size_t)lst->buffer_length);
	memcpy(lst->pointer, &value, (size_t)minInt(lst->buffer_length, sizeof(int)));
	if (lst->return_length_address)
	  *lst->return_length_address = minInt(lst->buffer_length, sizeof(int));
	break;
      }

    default:
      status = TreeILLEGAL_ITEM;

    }
    if (string) {
      if (lst->buffer_length && lst->pointer) {
	retlen = (unsigned short)minInt((int)strlen(string), lst->buffer_length);
	strncpy((char *)lst->pointer, string, (size_t)retlen);
	if (retlen < lst->buffer_length)
	  ((char *)lst->pointer)[retlen] = '\0';
	free(string);
      } else {
	lst->pointer = (unsigned char *)string;
	retlen = (unsigned short)strlen(string);
      }
    }
    if (lst->return_length_address)
      *lst->return_length_address = retlen;
    lst++;
  }
  return status;
}
