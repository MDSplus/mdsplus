/*	CSDER.FUN
	Evaluate the derivative of a cubic spline polynomial.
	_x is a vector of evaluation points
	_cs is cubic spline signal from CSINT or CSAKM

	Robert Granetz, MIT PFC  1993
*/
FUN PUBLIC CSDER(IN _x, IN _cs) {
	_x = FS_FLOAT(DATA(_x));
	_break = FS_FLOAT(DATA(DIM_OF(_cs,1)));
	_coeff = FS_FLOAT(DATA(_cs));
	_nintv = SIZE(_break) - 1;
	_nx = SIZE(_x);
	_order= 1;
	_y = _x;
	MdsMath->csderv_(_order, _nx, _x, _nintv, _break, _coeff, REF(_y));
	RETURN (make_signal(_y,,_x));
}
