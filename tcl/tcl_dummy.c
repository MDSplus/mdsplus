#include        <stdio.h>

#define dummy(a) \
    int a(){printf("Routine %s not implemented yet\n",#a);return 0;}

dummy(TclLink)
dummy(TreeMarkIncludes)
dummy(TclCommand)
dummy(TclRundownTree)
dummy(TclSetEvent)
dummy(TclWfevent)
