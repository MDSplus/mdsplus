#ifndef _TREESHR_HOOKS_H
#define _TREESHR_HOOKS_H

typedef enum
{
  OpenTree,
  OpenTreeEdit,
  RetrieveTree,
  WriteTree,
  CloseTree,
  OpenNCIFileWrite,
  OpenDataFileWrite,
  GetData,
  GetNci,
  PutData,
  PutNci
} TreeshrHookType;

#endif
