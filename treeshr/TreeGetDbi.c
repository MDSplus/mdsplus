#include <treeshr.h>
#include "treeshrp.h"
#include <dbidef.h>
#include <stdlib.h>
#include <string.h>

extern void *DBID;
#define min(a,b) (((a) < (b)) ? (a) : (b))
int TreeGetDbi(struct dbi_itm *itmlst) {return _TreeGetDbi(DBID,itmlst);}
#define set_retlen(length) if (lst->buffer_length < length) { status = TreeBUFFEROVF; break; } else retlen=length
#define check_open(db) if (!db) {status=TreeNOT_OPEN;break;}


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

      check_open(db);
      string = strcpy(malloc(strlen(db->main_treenam)+1),db->main_treenam);
      break;

     case DbiSHOTID:

      check_open(db);
      set_retlen(sizeof(db->shotid));
      *(int *) lst->pointer = db->shotid;
      break;

     case DbiMODIFIED:

      check_open(db);
      {
        memset(lst->pointer,0,lst->buffer_length);
        *(char *)lst->buffer_length = (char)db->modified;
	if (lst->return_length_address)
	  *lst->return_length_address = min(lst->buffer_length, sizeof(char));
      }
      break;

     case DbiOPEN_FOR_EDIT:

      check_open(db);
      {
        memset(lst->pointer,0,lst->buffer_length);
        *(char *)lst->buffer_length = (char)db->open_for_edit;
	if (lst->return_length_address)
	  *lst->return_length_address = min(lst->buffer_length, sizeof(char));
      }
      break;

     case DbiOPEN_READONLY:

      check_open(db);
      {
        memset(lst->pointer,0,lst->buffer_length);
        *(char *)lst->buffer_length = (char)db->open_readonly;
	if (lst->return_length_address)
	  *lst->return_length_address = min(lst->buffer_length, sizeof(char));
      }
      break;

     case DbiINDEX:

      {
	int       idx;
	for (idx = 0, db = (PINO_DATABASE *)dbid; db && (idx < *((unsigned char *) (lst->pointer))); idx++, db = db->next);
	if (!(db ? db->open : 0))
	  status = TreeNOT_OPEN;
	break;
      }

     case DbiNUMBER_OPENED:

      {
	int       count;
	PINO_DATABASE *db_tmp;
	for (count = 0, db_tmp = (PINO_DATABASE *)dbid; db ? db->open : 0; count++, db = db->next);
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

      check_open(db);
      {
	NID       nid;
	node_to_nid(db, db->default_node, (&nid));
	string = _TreeGetPath(db, *(int *)&nid);
      }
      break;

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
          lst->pointer[retlen] = '\0';
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
