public fun PCAT_voltage(in _sig1, in _sig2)
{
   _config = \RFX::POLOIDAL:PC_CONFIG;

   _control = \RFX::POLOIDAL:PC_CONTROL;

	if(_config == 'SERIES')
	{
		return ( make_signal(_sig1 + _sig2,,dim_of(_sig1)));
        }

	if(_config == 'PARALLEL')
	{
		return ( _sig1 );
        }


	if(_config == 'INDIPENDENT')
	{
		return ( _sig1 );
	}

    return (0);
}



