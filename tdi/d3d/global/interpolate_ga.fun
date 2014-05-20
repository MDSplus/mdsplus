/* interpolate_ga(in _x, in _xarr, in _yarr)
   Function interpolate computes the y value corresponding to the given x,
   by interpolation along _xarr and _yarr 

   Rewrote definition for _i from the original version to improve performance.
   20080829 SMF
*/

public fun interpolate_ga(in _x, in _xarr, in _yarr)
{
    if(!(size(_xarr) == size(_yarr)))
    {
	write(*, 'Different size for x and y arrays');
	return (0);
    }
    _n = size(_xarr);
    if(_x <= _xarr[0]) return (_yarr[0]);
    if(_x >= _xarr[_n - 1]) return (_yarr[_n - 1]);
    _i = size(pack(_xarr,_xarr<=_x));
    return (_yarr[_i - 1] + (_x - _xarr[_i - 1]) * (_yarr[_i] - _yarr [_i - 1])/(_xarr[_i] - _xarr[_i - 1]));

}

