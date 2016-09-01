/*	MULTITRACE.FUN
	A procedure to pack up several traces for use in DWScope.
	The Global parameter Shot should have something like:
	PUBLIC _shots=[681,682,683,684],0 (0=current will not be used.)
	The Y axis should be: MULTITRACE("expression")
	The expression can contain node names and that is why it is
	in quotes because it would become a NID and we want the PATH.
	Remember to double the backslash (\) in the quoted string.
	The string _shotname will have the list of shot numbers.

	Ken Klare, LANL P-4, (c)1992
*/
FUN PUBLIC MULTITRACE(IN _expr) {
	_n = SIZE(_shots);
	_s = USING((_z=$SHOTNAME,EXECUTE(_expr)),$DEFAULT,_shots[0]);
	_t = DIM_OF(_s);
	_d = [0.,DATA(_s),0.];
	FOR (_j = 1; _j < _n; ++_j) {
		_s = USING(((_z//=' '//$SHOTNAME),EXECUTE(_expr)[_t]),
			$DEFAULT, _shots[_j]);
		_d = [_d, 0., DATA(_s), 0.];
	}
	PUBLIC _shotname = _z;
	RETURN(make_signal(_d,,SPREAD([minval(_t),_t,maxval(_t)], 1, _n)));
}
