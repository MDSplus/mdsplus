public fun XrayHWInit(in _rack, in _chan_id, in _gain_id, in _filter_id, in _trans_id, in _detector_id, in _bias_id)
{

    write(*, "XRAY HW init ");
    
    _errors_id   = zero([78],0);
   
 
    write(*, "Channels id ", _chan_id);
    write(*, "gain id ", _gain_id);
    write(*, "filter id ", _filter_id);
    write(*, "trans id ", _trans_id);
    write(*, "detector id ", _detector_id);
    write(*, "bias id ", _bias_id);
 

    Tomo->SETUP_SXR(val(_rack),  _chan_id, _gain_id, _filter_id, _trans_id, _detector_id, _bias_id, ref(_errors_id) );

    return (_errors_id);

}