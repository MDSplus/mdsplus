public fun TSEdgeCCDGrid(in _x0, in _y0, in _l, in _n, in _grad)
{

  _deg = _grad * 2 * $pi/360.;

  _a = [[ 0 : _n * 2 ]/2, _n];
  _b =  abs(_a - _n) ;
  _c = [ 0 , replicate([_n, _n , 0 , 0], 0, _n/2), (mod(_n, 2) ? [_n, _n, 0 ]: _n )];
  _d = abs(_c - _n);


  _i = [_a, _d];
  _j = [_c, _b];


  _x = _x0 - _l * sin(_deg) * _j + _l * cos(_deg) * _i;
  _y = _y0 + _l * cos(_deg) * _j + _l * sin(_deg) * _i;

  	

  return ( make_signal( set_range( size( _x ), _x) ,, set_range( size( _y ), _y)) );


}
