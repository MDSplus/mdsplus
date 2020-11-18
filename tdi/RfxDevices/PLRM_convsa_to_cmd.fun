public fun PLRM_convsa_to_cmd(in _prefix, in _ga, in _sa)
{
	private _CIFRE = 5;

	private _PRECISIONE = 100.; /* 2 cifre dopo la virgola */




	write(*, '********************* PLRM_convsa_to_cmd *********************');


	_a = _ga - 360. * floor(_ga / 360.);
	write(*, "_a: ", _a);

	_b = _sa - 360. * floor(_sa / 360.);
	write(*, "_b: ", _b);



	_g = _b - _a;


	write(*, "_g: ", _g);

	if(_g > 0.)
	{
		_ra2 = -360. + _g;
	}
	else if(_g < 0.)
	{

		_ra2 = 360. + _g;
	}
	else
	{
		_ra2 = _g;
	}


	write(*, "_ra2: ", _ra2);


	if (abs(_g) <= 180.)
		_ra = _g;
	else
		_ra = _ra2;




	_ra = _PRECISIONE  * (  _ra + ((_ra >= 0)?.005:-.005));
	_ra = int(_ra);

	_sign = (_ra >= 0) ? "+" : "-";
	_ra =  trim(adjustl(text(abs(_ra))));




	_l = len(_ra);

	for(_i = _CIFRE; _i > _l; _i--)
	{
		_ra = "0" // _ra;
	}

	_ra = _prefix // _sign // _ra;

	write(*, "_ra: ", _ra);








	

	return(_ra);
}
