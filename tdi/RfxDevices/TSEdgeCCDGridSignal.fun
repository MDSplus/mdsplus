public fun TSEdgeCCDGridSignal(in _data, in _x0, in _y0, in _l, in _n)
{

  _deg = 0;
/*
  _deg = _grad * 2 * $pi/360.;


  _a = [[ 0 : _n * 2 ]/2, _n];
  _b =  abs(_a - _n) ;
  _c = [ 0 , replicate([_n, _n , 0 , 0], 0, _n/2), (mod(_n, 2) ? [_n, _n, 0 ]: _n )];
  _d = abs(_c - _n);


  _i = [_a, _d];
  _j = [_c, _b];

  _data = ramp([20,20]);
*/
  _out = [];

  for( _i = 0; _i < _n; _i++)
  {
	for( _j = 0; _j < _n; _j++)
	{
		_x1 = _x0 - _l * sin(_deg) * _j + _l * cos(_deg) * _i;
		_y1 = _y0 + _l * cos(_deg) * _j + _l * sin(_deg) * _i;

		_r =  _x1 -  sin(_deg) * [0.._l] + cos(_deg) * [0.._l];
		_c =  _y1 +  cos(_deg) * [0.._l] + sin(_deg) * [0.._l];

/*  	
		write(*, _c);
		write(*, _r);
	
		write(*, _data[_c, _r]);

		write(*, " MEAN ", sum( _data[_c, _r] * 1. ) );
*/
		_out = [ _out, sum( _data[_c, _r] * 1. )];
		
	}
  }

  write(*, _out);
  return ( make_signal( _out ,, [0.._n * _n] ) );

}
