public fun RFXVIConfig__store(as_is _nid, optional _method)
{
        private _N_HEAD      =   0;
        private _N_COMMENT   =   1;
	private _N_VVMC01_INI = 13;
	private _N_VVMC01_PPC = 14;
	
	WRITE(*, "VI CONFIG STORE");

	_status = MdsConnect("150.178.34.151:8100");
	if(_status != 0)
	{
		_error = mdsvalue('OPCConnect("OPC.SimaticNET", 500)');
		if( ! _error )
		{
			_vvmc01_ini = mdsvalue('OPCGet("\\\\SR:\\\\V1_ISO_Fetch\\\\VVMC1_INI")');
			if( mdsvalue( 'OPCerror()' ) )
			{
				DevLogErr(_nid, "Could not read OPC item VVMC1_INI "//mdsvalue( 'OPCerrormessage()' ) );
			} 
			else
			{
			
				DevPut(_nid, _N_VVMC01_INI, _vvmc01_ini);
				WRITE(*, "_N_VVMC01_INI = "//_vvmc01_ini);
			}			
			
			_vvmc01_ppc = mdsvalue('OPCGet("\\\\SR:\\\\V1_ISO_Fetch\\\\VVMC1_PPC")');
			if( mdsvalue( 'OPCerror()' ) )
			{
				DevLogErr(_nid, "Could not read OPC item VVMC1_INI "//mdsvalue( 'OPCerrormessage()' ) );
			} 
			else
			{
				DevPut(_nid, _N_VVMC01_PPC, _vvmc01_ppc);
				WRITE(*, "_N_VVMC01_PPC = "//_vvmc01_ppc);
			}
			
			mdsvalue('OPCDisconnect()');
		}
		else
		{
			DevLogErr(_nid, "Could not open connection to OPC server OPC.SimaticNET on ntOpc server Workstation");
			abort();		
		}
	}

	return ( _status );
}



 