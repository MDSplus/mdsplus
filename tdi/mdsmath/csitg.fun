/*	CSITG.FUN
	Integrate a cubic spline polynomial.
	_a is beginning of interval
	_b is end of interval
	_cs is cubic spline signal from CSINT or CSAKM
*/
FUN PUBLIC CSITG(IN _a, IN _b, IN _cs) {
	_a = FS_FLOAT(DATA(_a));
	_b = FS_FLOAT(DATA(_b));
	_break = FS_FLOAT(DATA(DIM_OF(_cs,1)));
	_coeff = FS_FLOAT(DATA(_cs));
	_nintv = SIZE(_break) - 1;
	_na = SIZE(_a);
	_nb = SIZE(_b);
	_y = _a;
	MdsMath->csitgv_(_na, _a, _nb, _b, _nintv, _break, _coeff, REF(_y));
	RETURN (MAKE_SIGNAL(_y,,.5*(_a+_b)));
}
