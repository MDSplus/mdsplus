#include        <stdio.h>

#define dummy(a) \
    int a(){printf("Routine %s not implemented on this platform\n",#a);return 0;}

dummy(MDSDCL_ACTIVATE_IMAGE)
dummy(MDSDCL_ATTACH)
dummy(MDSDCL_CREATE_LIBRARY)
dummy(MDSDCL_NOOP)
dummy(MDSDCL_DEFINE_KEY)
dummy(MDSDCL_DELETE_KEY)
dummy(MDSDCL_FINISH)
dummy(MDSDCL_HELP)
dummy(MDSDCL_MODIFY)
dummy(MDSDCL_SAVE_MACRO)
dummy(MDSDCL_SET_KEY)
dummy(MDSDCL_SET_KEYBOARD_APPLICATION)
dummy(MDSDCL_SET_INTERRUPT)
dummy(MDSDCL_SET_LIBRARY)
dummy(MDSDCL_SHOW_KEY)
dummy(MDSDCL_SHOW_LIBRARY)


#ifdef vms
int   lib$establish()  {return(1);}
#endif
