FUN PUBLIC STEP_RESAMP2D(IN _s, OPTIONAL _start, OPTIONAL _end, IN _inc)
{
  /* Resample a 2-d signal using the 1st dimension */
  _x_orig = data(dim_of(_s));
  _n_orig = size(_x_orig);
  _y_orig = data(_s);
  _start = present(_start) ? _start : _x_orig[0];
  _end = present(_end) ? _end : _x_orig[_n_orig-1];
  _new_x = data(_start : _end : _inc);
  _n=size(_new_x);
  _new_y=*;
  for (_i=0;_i<_n;_i++)
  {
    if (_new_x[_i] < _x_orig[0])
      _new_y = [_new_y,_y_orig[0]];
    else if (_new_x[_i] > _x_orig[_n_orig-1])
      _new_y = [_new_y,_y_orig[_n_orig-1]];
    else
      _new_y = [_new_y,_y_orig[pack(0 : _n_orig,lastloc(_x_orig <= _new_x[_i]))[0]]];
  }
  return(make_signal(transpose(set_range(size(_new_y)/_n,_n,_new_y)),*,_new_x,dim_of(_s,1)));
}
