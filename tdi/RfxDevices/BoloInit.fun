public fun BoloHWInit(in _gain, in _filter, in _reduction)
{

    _chan_id = [0..48];
    _gain_id = [byte(_gain)];
    _filter_id = [byte(_filter)];
    _ref_phase_id = [4];
    _carrier_freq = [0];
    _errors_id = zero(48,0);

    for(_i = 0; _i < 47; _i++)
    {
	_gain_id = [_gain_id, byte(_gain)]	
    	_filter_id = [_filter_id, byte(_filter)];
    	_ref_phase_id = [_ref_phase_id , 4];
    	_carrier_freq = [_carrier_freq, 0];
    }

    libTomo->SETUP_BOLO( val(48), _chan_id, _gain_id, _filter_id, _ref_phase_id, _carrier_freq, _errors_id);

    for(_i = 0; _i < 48; _i++)
	write(*, "Canale ", _i, " INIT err = ", _errors_id[_i]);

    libTomo->ZERO_SW(val(48), _chan_id, _errors_id); 

    for(_i = 0; _i < 48; _i++)
	write(*, "Canale ", _i, " ZERO SW err = ", _errors_id[_i]);

    for( _m = 1; _m < 13; _m++)
    {
	write(*,"Init module ", _m);
        libTomo->vt10Init(val(_m));
	libTomo->vt10StartAcq(val(0xf), val(_reduction)); 
    }
}