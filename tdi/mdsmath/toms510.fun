FUN PUBLIC TOMS510(IN _s,OPTIONAL _e) {
	_xdata = FLOAT(DIM_OF(_s),kind(0.0));
	_ydata = FLOAT(DATA(_s), kind(0.0));
        _w = _xdata;
	_n = SIZE(_ydata);
        _x = RAMP(_n,1.);
        _y = _x;
        _k = 0;
        _error = present(_e) ? _e : .02;
        _error = _error * abs(maxval(_ydata) - minval(_ydata));
	MdsMath->toms510_(REF(_xdata),REF(_ydata),REF(_error),REF(_n), REF(_x), REF(_y), REF(_w), REF(_k),REF(6));
        _y = _y[0 : _k];
        _x = _x[0 : _k];
	RETURN(MAKE_SIGNAL(_y,*,_x));
}

