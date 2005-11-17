public fun SxcHwStartAcq(in _rack, in _chan_id, in _errors, in _reduction, in _trig_mode)
{
    write(*, "SXC Start Acquisition");

    Tomo->VmeStartAcq(val(_rack), val(20), _chan_id, ref(_errors), val(_reduction), val(_trig_mode) ); 
    
    return ( _errors );

}