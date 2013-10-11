#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <dbidef.h>

static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";

extern void **TreeCtx();
#ifndef HAVE_VXWORKS_H
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
int TreeGetDbi(struct dbi_itm *itmlst) {return _TreeGetDbi(*TreeCtx(),itmlst);}
#define set_retlen(length) if ((unsigned int)lst->buffer_length < length) { status = TreeBUFFEROVF; break; } else retlen=length
#define CheckOpen(db) if (!db || !db->open) {status=TreeNOT_OPEN;break;}
#define set_ret_char(val) memset(lst->pointer, 0, lst->buffer_length); *((char *)lst->pointer) = val

int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst)
{
  PINO_DATABASE *db = (PINO_DATABASE *)dbid;
  struct dbi_itm *lst = itmlst;
  int       status = TreeNORMAL;
  while (lst->code && (status & 1))
  {
    char *string = NULL;
    unsigned short retlen = 0;
    switch (lst->code)
    {
     case DbiNAME:

      CheckOpen(db);
      string = strcpy(malloc(strlen(db->main_treenam)+1),db->main_treenam);
      break;

     case DbiSHOTID:

      CheckOpen(db);
      set_retlen(sizeof(db->shotid));
      *(int *) lst->pointer = db->shotid;
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
	int       idx;
	for (idx = 0, db = (PINO_DATABASE *)dbid; db && (idx < (*(int *) (lst->pointer) & 0xff)); idx++, db = db->next);
	break;
      }

     case DbiNUMBER_OPENED:

      {
	int       count;
	PINO_DATABASE *db_tmp;
	for (count = 0, db_tmp = (PINO_DATABASE *)dbid; db ? db->open : 0; count++, db = db->next)
	  ;
        memset(lst->pointer,0,lst->buffer_length);
        memcpy(lst->pointer,&count,min(lst->buffer_length,sizeof(int)));
	if (lst->return_length_address)
	  *lst->return_length_address = min(lst->buffer_length, sizeof(int));
	break;
      }

     case DbiMAX_OPEN:

      {
	int count = db->stack_size;
        memset(lst->pointer,0,lst->buffer_length);
        memcpy(lst->pointer,&count,min(lst->buffer_length,sizeof(int)));
	if (lst->return_length_address)
	  *lst->return_length_address = min(lst->buffer_length, sizeof(int));
	break;
      }

     case DbiDEFAULT:

      CheckOpen(db);
      {
	int nid;
        _TreeGetDefaultNid(db,&nid);
	string = _TreeGetPath(db, nid);
      }
      break;

     case DbiVERSIONS_IN_MODEL:
       CheckOpen(db);
       {
         int value=db->tree_info->header->versions_in_model;
         memset(lst->pointer,0,lst->buffer_length);
         memcpy(lst->pointer,&value,min(lst->buffer_length,sizeof(int)));
         if (lst->return_length_address)
           *lst->return_length_address = min(lst->buffer_length, sizeof(int));
         break;
       }

     case DbiVERSIONS_IN_PULSE:
       CheckOpen(db);
       {
         int value=db->tree_info->header->versions_in_pulse;
         memset(lst->pointer,0,lst->buffer_length);
         memcpy(lst->pointer,&value,min(lst->buffer_length,sizeof(int)));
         if (lst->return_length_address)
           *lst->return_length_address = min(lst->buffer_length, sizeof(int));
         break;
       }


     default:
      status = TreeILLEGAL_ITEM;

    }
    if (string)
    {
      if (lst->buffer_length && lst->pointer)
      {
	    retlen = min((unsigned short)strlen(string), lst->buffer_length);
        strncpy((char *)lst->pointer,string,retlen);
        if (retlen < lst->buffer_length)
          ((char *)lst->pointer)[retlen] = '\0';
        free(string);
      }
      else
      {
        lst->pointer = (unsigned char *)string;
        retlen = strlen(string);
      }
    }
    if (lst->return_length_address)
      *lst->return_length_address = retlen;
    lst++;
  }
  return status;
}
