public fun RFXVConfig__store(as_is _nid, optional _method)
{
	private _N_HEAD      =   0;
	private _N_COMMENT   =   1;
	private _N_VVMC01_INI = 13;
	private _N_VVMC01_PPC = 14;
	
	WRITE(*, "V CONFIG STORE 1");

	_status = MdsConnect("150.178.34.151:8100");
	if(_status != 0)
	{
		_handle = mdsvalue('OPCConnect("OPC.SimaticNET", 500)');
		
		if( _handle > 0)
		{
/*
			_vvmc01_ini = mdsvalue('OPCGet("\\\\SR:\\\\V1_ISO_Fetch\\\\VVMC1_INI")');
*/
			_vvmc01_ini = mdsvalue('OPCGet('//_handle//', "\\\\SR:\\\\V1_ISO_Fetch\\\\VVMC1_INI")');
			if( mdsvalue( "OPCerror("//_handle//")") ) 
			{
				DevLogErr(_nid, "Could not read OPC item VVMC1_INI "//mdsvalue( 'OPCerrormessage('//_handle//')' ) );
			} 
			else
			{
			
				WRITE(*, "_N_VVMC01_INI = "//_vvmc01_ini);
				DevPut(_nid, _N_VVMC01_INI, _vvmc01_ini);
			}			
/*			
			_vvmc01_ppc = mdsvalue('OPCGet("\\\\SR:\\\\V1_ISO_Fetch\\\\VVMC1_PPC")');
*/
			_vvmc01_ppc = mdsvalue('OPCGet('//_handle//', "\\\\SR:\\\\V1_ISO_Fetch\\\\VVMC1_PPC")');
			if( mdsvalue( "OPCerror("//_handle//")") )
			{
				DevLogErr(_nid, "Could not read OPC item VVMC1_INI "//mdsvalue( 'OPCerrormessage('//_handle//')' ) );
			} 
			else
			{
				WRITE(*, "_N_VVMC01_PPC = "//_vvmc01_ppc);
				DevPut(_nid, _N_VVMC01_PPC, _vvmc01_ppc);
			}
			
			mdsvalue('OPCDisconnect('//_handle//')');
			
			MdsDisconnect();
		}
		else
		{
			DevLogErr(_nid, "Could not open connection to OPC server OPC.SimaticNET on ntOpc server Workstation");
			abort();		
		}
	}

	return ( _status );
}



 