/*	SLANTED_BASELINE.FUN
	
	Input:   _s is a 1-D signal (i.e. s(t) for example)
              Optional:
                 _window is the number of points to average together
                    at either end of the signal to compute the 
                    baseline. Default 10.
                  
	Output:  The signal _s, with a slanted baseline subtracted out

	Robert Granetz, MIT PFC  1995
*/
FUN PUBLIC SLANTED_BASELINE(IN _s, OPTIONAL IN _window) {
	IF (NOT PRESENT(_window)) _window=10;
	_sig=data(_s);
	_npts=size(_sig);
	_i=lbound(_sig,0);
	_b1=mean(_sig[_i : _i+_window-1]);
	_i=ubound(_sig,0);
	_b2=mean(_sig[_i-_window+1 : _i]);
	_baseline=ramp([_npts],0.)/_npts*(_b2-_b1) + _b1;
	_sig=(_sig-_baseline);
	RETURN (MAKE_SIGNAL(_sig,*,dim_of(_s)));
}
