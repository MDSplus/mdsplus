public fun a14_adjust(in _trig, in _clock)
{
  _w = nint(build_dim(build_window(-1,0,_trig),_clock)*1E6);
  if (size(_w) > 1)
    return((nint(_trig*1E6) - _w[-1]) >= (_w[0] - _w[-1])/2);
  else
    return(0);
}
