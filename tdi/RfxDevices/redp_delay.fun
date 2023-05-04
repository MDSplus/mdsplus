public fun redp_delay(as_is _redp, in _trig_delay, in _base_lev_delay)
{
    _TRIG_REC_OFFS = 37;
    _DECIMATION_OFFS = 2;
    _EV_SAMPLES_OFFS = 11;
    _base_nid = getnci(_redp, 'nid_number');
    _is_trig_nid = _base_nid + _TRIG_REC_OFFS;
    _decimation_nid = _base_nid + _DECIMATION_OFFS;
    _ev_samples_nid = _base_nid + _EV_SAMPLES_OFFS;
    _trig_rec = data(getnci(_is_trig_nid, 'record'));
    _ev_samples = data(getnci(_ev_samples_nid, 'record'));
    _decimation = data(getnci(_decimation_nid, 'record'));
    _period = 125E-9 / _decimation;
    _lev_delay = _base_lev_delay + _ev_samples * _period;
    _num_trigs = size(_trig_rec);
    _lev_delays = zero(_num_trigs) + _lev_delay;
    _trig_delays = zero(_num_trigs) + _trig_delay;
    return(int(conditional(_trig_delays, _lev_delays, _trig_rec)));
}

