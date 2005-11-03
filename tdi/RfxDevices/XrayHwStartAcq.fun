public fun XrayHwStartAcq(in _rack, in _chan_id, in _errors, in _reduction, in _trig_mode)
{
    write(*, "XRAY Start Acquisition");
	
    Tomo->VmeStartAcq(val(_rack), val(78), _chan_id, ref(_errors), val(_reduction), val(_trig_mode) ); 

    return (_errors ); 

}