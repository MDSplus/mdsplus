
public fun BoloHWInit(in _chan_id, in _gain_id, in _filter_id, in _ref_phase_id, in _carrier_freq)
{

     write(*, "BOLO HW init");
/*
	 write(*, "CHAN id   = ", _chan_id);
	 write(*, "Gain      = ", _gain_id);
	 write(*, "Filter    = ", _filter_id);
	 write(*, "Ref ph    = ", _ref_phase_id);
	 write(*, "Car Freq  = ", _carrier_freq);
	 write(*, "reduction = ", _reduction);
*/

    _errors_id = zero([48],0);

    Tomo->SETUP_BOLO( val(48), _chan_id, _gain_id, _filter_id, _ref_phase_id, _carrier_freq, ref(_errors_id));

    write(*, "BOLO HW init End ");

    return (_errors_id);
}