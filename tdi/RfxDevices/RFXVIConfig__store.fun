public fun RFXVIConfig__store(as_is _nid, optional _method)
{
    private _N_HEAD      =      0;
    private _N_COMMENT   =      1;
	private _N_VVMC01_INI = 13;
	private _N_VVMC01_PPC = 14;

	OPCConnect(, 500)
	_vvmc01_ini = OPCGet('VVMC1_INI');
	_vvmc01_ppc = OPCGet('VVMC1_PPC');
	OPCDisconnect();

	DevPut(_nid, _N_VVMC01_INI, _vvmc01_ini);
	DevPut(_nid, _N_VVMC01_PPC, _vvmc01_ppc);
	return (1);
}



 