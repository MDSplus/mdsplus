public fun MarteAbortRequestOPC(in _mask)
{

	_bitToSet = 1;
	_valueSet = [];
/*
	_value = byte_unsigned([0,0,0,0,0,0,0,0,0,0]);
*/

	_status = MdsConnect("150.178.34.151:8100");
	if(_status != 0)
	{
		_handle = mdsvalue('OPCConnect("OPC.SimaticNET", 500)');
		
		if( _handle > 0)
		{

			_value = mdsvalue('OPCGet('//_handle//', "\\\\S7:\\\\Supervisore\\\\aliases\\\\ENGDA_EVT_WARN")');
			if( mdsvalue( "OPCerror("//_handle//")") ) 
			{
				DevLogErr(_nid, "Could not read OPC item ENGDA_EVT_WARN "//mdsvalue( 'OPCerrormessage('//_handle//')' ) );
			} 
			else
			{
				WRITE(*, "ENGDA_EVT_WARN = "//_value);

				for( _i = 0, _k = 0; _i < 10; _i++,  _k += 8 )
				{
					if( _bitToSet >= _k && _bitToSet < _k + 8  )
						_valueSet = [ _valueSet, byte_unsigned( 1 << (_bitToSet - _i * 8) ) ];
					else
						_valueSet = [ _valueSet, byte_unsigned( 0 ) ];

				}
				_vset = _value | _valueSet;
	
				write(*, "VALUE ", _value);
				write(*, "to set ", _valueSet);
				write(*, "set ", _vset);


				_status = mdsvalue("opcPut('\\S7:\\Supervisore\\aliases\\ENGDA_EVT_WARN',$, 10)", _vset);		
				if( mdsvalue( "OPCerror("//_handle//")")  )
				{
					write(*, "Could not write OPC item ENGDA_EVT_WARN "//mdsvalue( 'OPCerrormessage('//_handle//')' ) );
				}
				else
				{
					write(*, "Could not write OPC item ENGDA_EVT_WARN "//mdsvalue( 'OPCerrormessage('//_handle//')' ) );
					write(*, "No error");
				}

			}			

			mdsvalue('OPCDisconnect('//_handle//')');
		}
		else
		{
			write(*, "error");
		}

		MdsDisconnect();

	}
	else
	{
		write(*, "error");
	}

	
}
