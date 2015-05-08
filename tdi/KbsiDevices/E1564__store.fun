public fun E1564__store(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _N_SAMPLE_PERIOD = 4;
    private _N_SAMPLE_NUM = 6;
    private _N_SAMPLE_NUMPRE = 7;
    private _N_TRIG_T0 = 11;
    private _N_FOO_0 = 18;

/* Get id to communicate with a digitizer */
    private _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
    private _num = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM)),(DevLogErr(_nid, "Missing # of samples"); abort();));
    private _npre = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUMPRE)),(DevLogErr(_nid, "Missing # of pretrig."); abort();));
    private _dt = if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD)),(DevLogErr(_nid, "Missing sampling period"); abort();));
    private _t0 = if_error(data(DevNodeRef(_nid, _N_TRIG_T0)),(DevLogErr(_nid, "Missing T0"), 0.));
    _t0 -= _dt * _npre;
    private _dim = make_dim(make_window(0, _num - 1, _t0), make_slope(make_with_units(_dt,'s')));

/* Read data from a digitizer */
    private _r0 = 0d0;
    private _r1 = 0d0;
    private _r2 = 0d0;
    private _r3 = 0d0;
    private _f = 0d0;
    IF_ERROR(hpe1564_32->hpe1564_input_Q (VAL(_id), REF(_r0), REF(_f), REF(_r1), REF(_f), REF(_r2), REF(_f), REF(_r3), REF(_f)),
      (DevLogErr(_nid, "Error while reading digitizer configuration"); abort();));
    _range=[_r0, _r1, _r2, _r3];
    _line = zero(_num * 4, 0w);
    IF_ERROR(hpe1564_32->hpe1564_fetchAll_Q (VAL(_id), VAL(_num * 2), REF(_line), REF(_num)),
      (DevLogErr(_nid, "Error while fetching data"); abort();));

/* Raw data -> signal */
    for(_i=0; _i<4; _i++) {
        _signal = compile('build_signal(build_with_units((`_range[_i]/32768.0)*$VALUE,"V"), (`_line[_i:4*_num-1:4]), (`_dim))');
        TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0 + 6*_i), XD(_signal), VAL(0));
    }
    TreeClose();
    return (1);
}

