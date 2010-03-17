public fun smooth1d(as_is _sig, in _width)
{
  _in = fs_float(data(_sig));
  _size = size(_in);
  if (_size <= 0) return(*);
  _out = array(_size,0.0);
  libMdsMath->boxsmooth(REF(_size),REF(_in),REF(long(_width)),ref(_out));
  return(make_signal(_out,*,dim_of(_sig)));
}
