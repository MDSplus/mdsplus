#include <stdlib.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
extern int TdiExecute();
extern int ReadInt();
int GetSupportedDevices(char ***devnames, char ***imagenames, int *number)
{
  static EMPTYXD(dev_list);
  static DESCRIPTOR(expr, "_devs = MdsDevices()");
  int status;
  status = TdiExecute(&expr, &dev_list MDS_END_ARG);
  if (status&1) {
    int i;
    struct descriptor_a *a_ptr = (struct descriptor_a *)dev_list.pointer;
    int num=ReadInt("size(_devs)/2" MDS_END_ARG);
    char **dnames;
    char **inames;
    dnames = (char **)malloc(num * sizeof(char **));
    inames = (char **)malloc(num * sizeof(char **));
    for (i=0; i<num; i++) {
      dnames[i] = (char *)a_ptr->pointer+(i*2*a_ptr->length);
      inames[i] = (char *)a_ptr->pointer+(i*2+1)*a_ptr->length;
    }
    *devnames = dnames;
    *imagenames = inames;
    *number = num;
  }
}


