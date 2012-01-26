fun public mdsvalue(         in  _p1,optional in  _p2,optional in  _p3,optional in  _p4,optional in  _p5,
		    optional in  _p6,optional in  _p7,optional in  _p8,optional in  _p9,optional in _p10,
		    optional in _p11,optional in _p12,optional in _p13,optional in _p14,optional in _p15,
		    optional in _p16,optional in _p17,optional in _p18,optional in _p19,optional in _p20)
/*	mdsvalue('string',optional in _p2, optional in _p2...)

	mdsvalue on remote connection

call:   mdsvalue('2+3 * $,',_p1);
	_p1		= First parameter must be a string '2+3' or '$'
	_p2 - _p20     	= optional parameters for $ replacement

	BPDuval: [CRPP, EPFL, Switzerland]	March 2000
*/
{
/* Build start of string */
   if(mdscurrent() == 'local') {
      return(execute(_p1,_p2,_p3,_p4,_p5,_p6,_p7,_p8,_p9,_p10,_p11,_p12,_p13,_p14,_p15,_p16,_p17,_p18,_p10,_p20));
   } else  {
      _oo = "build_call(24,'TdiShrExt','rMdsValue',descr(_p1";
/* loop through verifying parameters */
      for(_i=2;_i<=20; _i++) {
	 _ss = '_p'//trim(adjustl(_i));
	 _str = '!present('//_ss//')';
/*      write(*,_str);*/
	 if(execute(_str))
	   break;
	 _oo=_oo//'),descr('//_ss;
/*      write(*,_oo);*/
      }
      _oo = _oo//"),val(0ul))";
/* execute the constructed command */
      return(execute(_oo));
   }
}
