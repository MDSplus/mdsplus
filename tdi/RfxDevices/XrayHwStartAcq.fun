public fun XrayHwStartAcq(in _num_mod, in _reduction)
{
    write(*, "XRAY Start Acquisition");
	
    for( _m = 1; _m <= _num_mod; _m++)
    {
		Tomo->vt10StartAcq(_m, val(0xf), val(_reduction)); 
    }

}