public fun smooth1d(in _sig, in _width)
{
  _in = fs_float(data(_sig));
  _size = size(_in);
  _out = array(_size,0.0);
  libMdsMath->boxsmooth(REF(_size),REF(_in),REF(long(_width)),ref(_out));
  return(make_signal(_out,*,dim_of(_sig)));
}
