public fun a14_clock_divide(in _clock, in _clock_divide) {
  if (_clock_divide == 1) return(_clock);
  _DTYPE_NID=192;
  _DTYPE_PATH=193;
  _DTYPE_RANGE=201;
  switch (kind(_clock)) {
    case(_DTYPE_NID) return(a14_clock_divide(getnci(_clock,"record"),_clock_divide)); break;
    case(_DTYPE_PATH) return(a14_clock_divide(getnci(_clock,"record"),_clock_divide)); break;
    case(_DTYPE_RANGE) return(make_range(begin_of(_clock),end_of(_clock),slope_of(_clock)*_clock_divide)); break;
    case DEFAULT _clock_out=data(_clock); return(_clock_out[make_range(0,size(_clock_out)-1,_clock_divide)]);
  }
}
