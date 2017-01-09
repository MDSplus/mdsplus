/*      TdiCvtDxDx
        Convert to desired data type.

        status = TdiCvtDxDx(&in_dsc,&dtype,&out_dsc)

        Thomas W. Fredian, MIT-PFC      07-MAR-1989     Copyrighted
        Ken Klare, LANL CTR-7   (c)1989,1990
        NEED to handle array of descriptors.
*/
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <status.h>
#include <STATICdef.h>



extern int TdiConvert();
extern int TdiGetShape();

int TdiCvtDxDx(struct descriptor *in_ptr, unsigned char *dtype_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

	/*********************************
        No conversion needed, just memory.
        *********************************/
  if (in_ptr->dtype == *dtype_ptr)
    status = MdsCopyDxXd(in_ptr, out_ptr);

	/*******************
        Remove a descriptor.
        *******************/
  else if (in_ptr->dtype == DTYPE_DSC)
    status = TdiCvtDxDx((struct descriptor *)in_ptr->pointer, dtype_ptr, out_ptr);

	/******************************
        Make space and convert into it.
        ******************************/
  else {
    int mode = -1;
    struct descriptor_xd xs = { 0, DTYPE_DSC, CLASS_XS, 0, 0 };
    xs.pointer = in_ptr;
    status = TdiGetShape(1, &xs, 0, *dtype_ptr, &mode, out_ptr);
    if STATUS_OK
      status = TdiConvert(in_ptr, out_ptr->pointer);
  }
  return status;
}
