/*      Tdi3Square.C
        Interludes for some simple stuff.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <mdsdescrip.h>
#include <status.h>
#include <STATICdef.h>



extern int Tdi3Multiply();
extern int Tdi3Complex();
extern int Tdi3UnaryMinus();
extern int Tdi3Floor();

/*--------------------------------------------------------------
        Square a number by multiplying.
*/
int Tdi3Square(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  return Tdi3Multiply(in_ptr, in_ptr, out_ptr);
}

/*--------------------------------------------------------------
        F90 elemental, convert complex call to molded type.
*/
int Tdi3Cmplx(struct descriptor *x_ptr,
	      struct descriptor *y_ptr, struct descriptor *mold_ptr __attribute__ ((unused)), struct descriptor *out_ptr)
{
  return Tdi3Complex(x_ptr, y_ptr, out_ptr);
}

/*--------------------------------------------------------------
        F90 elemental, round to higher integral value. In CC it is CEIL.
*/
int Tdi3Ceiling(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  status = Tdi3UnaryMinus(in_ptr, out_ptr);
  if STATUS_OK
    status = Tdi3Floor(out_ptr, out_ptr);
  if STATUS_OK
    status = Tdi3UnaryMinus(out_ptr, out_ptr);
  return status;
}
