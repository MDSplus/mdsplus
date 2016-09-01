/*	CSSMOOTH.FUN
	Do smoothing using rectangular (boxcar) interval.
	_x is evaluation points
	_dx is full width of interval
	_cs is result of CSAKM or CSINT

	Ken Klare, LANL P-4 (c)1991
*/
FUN PUBLIC CSSMOOTH(IN _x, IN _dx, IN _cs) {
	_y = CSITG(_x - _dx * .5, _x + _dx * .5, _cs);
	RETURN (MAKE_SIGNAL(_y/_dx, , _x));
}
