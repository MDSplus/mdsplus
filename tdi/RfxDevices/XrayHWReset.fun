public fun XrayHWReset(in _rack, in _chan_id)
{
    write(*, "XRAY HW Reset ");

    _errors_id   = zero([78],0);
       
    Tomo->RESET_SW_SXR(val(_rack), _chan_id, ref(_errors_id)); 

    return (_errors_id);
}