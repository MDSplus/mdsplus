public fun SxcHWZero(in _rack, in _chan_id)
{
    
    _errors_id   = zero([20],0);
       
    Tomo->ZERO_SW_SXC(val(_rack), _chan_id, ref(_errors_id)); 

    return (_errors_id);

}