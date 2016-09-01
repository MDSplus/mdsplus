/*	CSAKM.FUN
	Compute cubic splice by Not-A-Node method.
	Returns signal of coefficients and breakpoints.
	_s is signal with function values and axis points.

	Ken Klare, LANL P-4 (c)1991
*/
FUN PUBLIC CSAKM(IN _s) {
	_xdata = FS_FLOAT(DIM_OF(_s));
	_fdata = FS_FLOAT(DATA(_s));
	_n = SIZE(_fdata);
	if (_n < 5) abort();  /* will cause floating exception in MdsMath */
	IF (SIZE(_xdata) == 0) _xdata = RAMP(_n, 1.);
	_break = ARRAY(_n, 0.);
	_coeff = ARRAY([4,_n], 0.);
	MdsMath->csakm_(_n, REF(_xdata), REF(_fdata), REF(_break), REF(_coeff));
	RETURN(MAKE_SIGNAL(_coeff,,, _break));
}
