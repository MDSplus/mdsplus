public fun RfxTorBtFConfig(in _type, in _modeVal)
{

	_mode = execute("\\EDA3::CONTROL:TRIG1_CONTR");

    if ( _type == "TITLE" )
	{
	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  _modeVal)
		{
			return ( _modeVal );
		}
		else
		{
			return ( _modeVal//" OFF" );
		}
	}
	
    if ( _type == "SIGNAL" )
	{
		_zeroSig = make_signal([0.,0],,[-1., 1.]);
		
	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName == _modeVal)
		{
			switch( _modeVal )
			{
				case ( "Bt CONTROL")
				   _out = if_error( \EDA3::CONTROL.PARAMETERS:PAR243_VAL, _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				case ( "F CONTROL")
				   _out = if_error( \EDA3::CONTROL.PARAMETERS:PAR250_VAL, _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
			}
		}
		else
		{
			return ( _zeroSig );
		}
	}
}