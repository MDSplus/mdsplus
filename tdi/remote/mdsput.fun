fun public mdsput(         in  _p1, in  _p2,optional in  _p3,optional in  _p4,optional in  _p5,
		    optional in  _p6,optional in  _p7,optional in  _p8,optional in  _p9,optional in _p10,
		    optional in _p11,optional in _p12,optional in _p13,optional in _p14,optional in _p15,
		    optional in _p16,optional in _p17,optional in _p18,optional in _p19,optional in _p20)
/*	mdsput('node','string',optional in _p3, optional in _p4...)

	mdsput on remote connection

call:   mdsput('mynode','2+3 * $',_p3);
	_p1		= First parameter must be a string, the nodename
	_p2		= Second parameter must be a string '2+3' or '$', the expression
	_p3 - _p20     	= optional parameters for $ replacement

	TWFredian: [MIT, USA]	August 2000
*/
{
	_oo = 'mdsvalue("TreePut($,$';
	_zz = ",_p1,_p2";
  for(_i=3;_i<=20; _i++) {
	  _ss = '_p'//trim(adjustl(_i));
	  _str = '!present('//_ss//')';
		if(execute(_str))
			break;
		_oo = _oo//',$';
		_zz = _zz//','//_ss;
	}
	_oo = _oo // ')"' // _zz // ')' ;
  return(execute(_oo));
}
