Public Fun LinFit(in _knots_x, in _knots_y, in _xin)
{
  _num_knots = size(_knots_x);
  _num_x = size(_xin);
  _yout = zero(size(_xin), 0.0);
  MitDevicesIO->LinFit(_num_knots, _knots_x, _knots_y, _num_x,_xin, ref(_yout));
  return (make_signal(_yout, *, _xin));
}
