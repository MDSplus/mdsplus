public fun XrayHWZero(in _rack, in _chan_id)
{

    write(*, "XRAY HW zero ");
    
    _errors_id   = zero([78],0);
       
    Tomo->ZERO_SW_SXR(val(_rack), _chan_id, ref(_errors_id)); 

    return (_errors_id);

}