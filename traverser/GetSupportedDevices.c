#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <tdishr.h>

int GetSupportedDevices(char ***devnames, int *number)
{
  EMPTYXD(dev_list);
  static DESCRIPTOR(expr, "_devs = MdsDevices()");
  int status;
  *number=0;
  *devnames=0;
  status = TdiExecute((struct descriptor *)&expr, &dev_list MDS_END_ARG);
  if (status & 1) {
    int i;
    struct descriptor_a *a_ptr = (struct descriptor_a *)dev_list.pointer;
    *number = a_ptr->arsize/a_ptr->length/2;
    *devnames = (char **)malloc(*number * sizeof(char **));
    for (i = 0; i < *number; i++) {
      char *devname;
      (*devnames)[i] = devname = strndup((const char *)a_ptr->pointer + (i * 2 * a_ptr->length),(size_t)a_ptr->length);
      while(strlen(devname) > 0 && isspace(devname[strlen(devname)-1]))
	devname[strlen(devname)-1]='\0';
    }
  }
  MdsFree1Dx(&dev_list,0);
  return status;
}
