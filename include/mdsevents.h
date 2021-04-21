#ifndef IPCS_MSGPTR
#include <msgs.h>
#endif

struct _id
{
  struct linkage link;
  long status;
  char *name;
  void (*astadr)();
  void *astprm;
  void *data;
  unsigned int dlen;
  MSGPTR receipt;
  unsigned short tag;
  unsigned short timertag;
  unsigned short msgtag;
};

typedef struct _id *idptr;
extern idptr find_mdsevent_id();
extern idptr find_mdsevent_idtag();
extern idptr create_mdsevent_id();

#define MDSEVENTD "MDSEVENTD"
