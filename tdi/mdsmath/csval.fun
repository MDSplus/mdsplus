/*	CSVAL.FUN
	Evaluate a cubic spline polynomial.
	_x is evaluation points
	_cs is cubic spline signal from CSINT or CSAKM

	Ken Klare, LANL P-4 (c)1991
*/
FUN PUBLIC CSVAL(IN _x, IN _cs) {
	_x = (DATA(_x));
	_break = (DATA(DIM_OF(_cs,1)));
	_coeff = (DATA(_cs));
	_nintv = SIZE(_break) - 1;
	_nx = SIZE(_x);
	_y = _x;
	MdsMath->csvalv_(_nx, _x, _nintv, _break, _coeff, REF(_y));
	RETURN (make_signal(_y,,_x));
}
