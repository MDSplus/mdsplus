/*	DERIV.FUN
	Evaluate derivative of a signal by first fitting a cubic spline.
        Based upon the routines CSAKM and CSDER in the Numerical Recipes
        book and the IMSL library.

	_s is a 1-D signal (i.e. s(t) for example)

	Robert Granetz, MIT PFC  1993
*/
FUN PUBLIC DERIV(IN _s) {
	RETURN (CSDER(DIM_OF(_s), CSAKM(_s)));
}
