#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>

#include <mdsshr.h>
#include "j412_gen.h"
#include "devroutines.h"

static int one = 1;
#define min(a,b) ((a) < (b)) ? (a) : (b)
#define max(a,b) ((a) < (b)) ? (b) : (a)
#define return_on_error(f) if (!((status = f) & 1)) goto on_error;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(in_struct->name, a, f, d, 16, 0), &one, &one))
#define stop(f,a,n,d)  return_on_error(DevCamChk(CamStopw(in_struct->name, a, f, n, d, 24, 0), &one, &one))

EXPORT int j412___init(struct descriptor *nid_d_ptr __attribute__ ((unused)), InInitStruct * in_struct)
{
  struct descriptor_xd xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int status;
  status = TdiLong((struct descriptor *)in_struct->set_points, &xd MDS_END_ARG);
  if (status & 1)
    status = TdiData((struct descriptor *)&xd, &xd MDS_END_ARG);
  if (status & 1) {
    struct descriptor_a *a_ptr = (struct descriptor_a *)xd.pointer;
    int num = a_ptr->arsize / a_ptr->length;
    int *buff = (int *)a_ptr->pointer;
    int i;

    num = max(min(1024, num), 0);

    if (num) {
      status = status & 1;
      for (i = 1; (i < num) && status; i++)
	status = buff[i] > buff[i - 1];

      if (status) {
	pio(24, 0, 0);		/* disable module */
	pio(16, 1, &in_struct->num_cycles);	/* set recycle mode */
	pio(16, 2, 0);
	stop(16, 0, num, buff);
	pio(26, 0, 0);		/* enable module */
      } else
	status = J412$_NOT_SORTED;
    } else
      status = J412$_NO_DATA;
 on_error:
    MdsFree1Dx(&xd, 0);
  }
  return status;
}
