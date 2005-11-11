/* assumes NB tree is open */

FUN PUBLIC ENBI()
{
	_powersum = 0.0;
	_voltsum = 0.0;

	_nPower = GETNCI("\\TOP.*:PINJ_SCALAR","NID_NUMBER");
	_nVolts  = GETNCI("\\TOP.*:VOLTAGE","NID_NUMBER");

	for (_i=0; _i<SIZE(_nPower); _i++)
	  {
	    _len = GETNCI(_nPower[_i],"LENGTH");
	    if (_len > 0) 
	      {
	        write (*,_len);
	        _volt = GETNCI(_nVolts[_i],"RECORD");
	        _powersum = _powersum + GETNCI(_nPower[_i],"RECORD") * _volt;
		_voltsum = _voltsum + _volt;
	      }
	    write (*,_i);
          }

	if (_voltsum > 0.)
          { 
	    return (_powersum/_voltsum);
	  }
	else 
	  {
	    return (0.);
          }
}

