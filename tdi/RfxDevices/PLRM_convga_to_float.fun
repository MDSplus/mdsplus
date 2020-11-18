public fun PLRM_convga_to_float(in _ga)
{


	write(*, '********************* PLRM_convga_to_float *********************');


	_f = extract(5, 6, _ga);

/*	_int = extract(0,4,_s);
	write(* , "_int: ", _int);
	_dec = extract(4,2,_s);
	write(* , "_dec: ", _dec);

	_s = _int // "." // _dec;
	
	write(* , "_s: ", _s);
*/
	

	_f = data(compile(_f));




	_f = .01 * _f;

	write(* , "_f: ", _f);	

	return(_f);
}
