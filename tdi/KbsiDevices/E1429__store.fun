public fun E1429__store(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _N_SAMPLE_PERIOD = 4;
    private _N_SAMPLE_DELAY = 5;
    private _N_SAMPLE_NUM = 7;
    private _N_SAMPLE_NUMPRE = 8;
    private _N_TRIG_T0 = 13;
    private _N_FOO_0 = 20;

/* Get id to communicate with a digitizer */
    private _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
    private _num = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM)),(DevLogErr(_nid, "Missing # of samples"); abort();));
    private _delay = if_error(data(DevNodeRef(_nid, _N_SAMPLE_DELAY)),(DevLogErr(_nid, "Missing Delay"), 0.));
    private _npre = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUMPRE)),(DevLogErr(_nid, "Missing # of pretrig."); abort();));
    private _dt = if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD)),(DevLogErr(_nid, "Missing sampling period"); abort();));
    private _t0 = if_error(data(DevNodeRef(_nid, _N_TRIG_T0)),(DevLogErr(_nid, "Missing T0"), 0.));
    _t0 = _t0 + _delay - _dt * _npre;
    private _dim = make_dim(make_window(0, _num - 1, _t0), make_slope(make_with_units(_dt,'s')));

/* Read data from a digitizer */
    private _port = 0w;
    private _range = 0d0;
    private _num = 0;
    private _preTrig = 0;
    private _numArm = 0;

/* Raw data -> signal */
    for(_i=0; _i<2; _i++) {
       IF_ERROR(hpe1429_32->hpe1429_configure_Q (VAL(_id), VAL(_i+1), REF(_port), REF(_range), REF(_num), REF(_preTrig), REF(_numArm)),
          (DevLogErr(_nid, "Error while reading digitizer configuration"); abort();));
       _line = array(_num, 0w);
       IF_ERROR(hpe1429_32->hpe1429_fetc_Q (VAL(_id), VAL(_i+1), VAL(_num), REF(_line)),
          (DevLogErr(_nid, "Error while fetching data"); abort();));
       _signal = compile('build_signal(build_with_units((`_range / 2047.) * $VALUE,"V"), (`_line/16), (`_dim))');
       TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0 + 6 * _i), XD(_signal), VAL(0));
    }

    TreeClose();
    return (1);
}

