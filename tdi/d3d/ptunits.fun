FUN PUBLIC PTUNITS(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_pointname=_pointname//"          ";
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;

	_units="    ";  /* string length 4 */
	_stat=libMdsD3D->mdsptheadunits_(_shot, _pointname, ref(_error), REF(_units));

	if (_error == 0) {
	  return(_units);
	} else {
	  return("");
	}
	
}
