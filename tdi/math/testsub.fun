FUN PUBLIC TESTSUB(IN _s) {
	_xdata = DIM_OF(_s);
	_n = SIZE(_xdata);
	_stat = MdsMath->testsub(_n, REF(_xdata));
	RETURN(_xdata);
}
