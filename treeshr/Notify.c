#include <stdio.h>
#include <treeshr_hooks.h>
extern char* TreeGetPath(int);
int Notify(TreeshrHookType htype, char *tree, int shot, int nid) {
  char *name;
  char def_name[50];
  char *na="N/A";
  char *path=na;
  sprintf(def_name,"Hook number %d", htype);
  name=def_name;
  switch(htype) {
  case OpenTree:
    name="OpenTree";
    break;
  case OpenTreeEdit:
    name="OpenTreeEdit";
    break;
  case RetrieveTree:
    return 0;
  case WriteTree:
    name="WriteTree";
    break;
  case CloseTree:
    name="CloseTree";
    break;
  case OpenNCIFileWrite:
    return 1;
  case OpenDataFileWrite:
    return 1;
  case GetData:
    name="GetData";
    path=TreeGetPath(nid);
    break;
  case GetNci:
    name="GetNci";
    path=TreeGetPath(nid);
    break;
  case PutData:
    name="PutData";
    path=TreeGetPath(nid);
    break;
  case PutNci:
    name="PutNci";
    path=TreeGetPath(nid);
    break;
  }
  printf("%s hook called for tree=%s, shot=%d, node=%s\n",name,tree,shot,path);
  if (path != na && path != (char *)0)
    free(path);
  return 1;
}
