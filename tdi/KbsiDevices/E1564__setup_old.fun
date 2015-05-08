public fun E1564__setup(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 44;
    private _N_ID = 2;
    private _N_SAMPLE_PERIOD = 4;
    private _N_SAMPLE_SOURCE = 5;
    private _N_SAMPLE_NUM = 6;
    private _N_SAMPLE_NUMPRE = 7;
    private _N_TRIG_SOURCE = 9;
    private _N_TRIG_LEVEL = 10;
    private _N_INPUT_RANGE = 16;
    private _N_INPUT_FILTER = 17;

    _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));

/* Sample */
   private _num = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM)),(DevLogErr(_nid, "Missing # of samples"); abort();));
   private _numpre = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUMPRE)),(DevLogErr(_nid, "Missing # of pre trig."); abort();));
   private _source = if_error(data(DevNodeRef(_nid, _N_SAMPLE_SOURCE)),(DevLogErr(_nid, "Missing sampling source"); abort();));
   private _period = real(if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD)),(DevLogErr(_nid, "Missing sampling time"); abort();)),53UB);
   if_error(hpe1564_32->hpe1564_sample (VAL(_id), VAL(_num), VAL(_numpre), VAL(_source), _period),
	(DevLogErr(_nid, "Error in Sample setup"); abort();));

/* Range */
   private _r0 = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE)),(DevLogErr(_nid, "Missing range of ch 0"); abort();));
   private _r1 = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE + 6)),(DevLogErr(_nid, "Missing range of ch 1"); abort();));
   private _r2 = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE + 12)),(DevLogErr(_nid, "Missing range of ch 2"); abort();));
   private _r3 = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE + 18)),(DevLogErr(_nid, "Missing range of ch 3"); abort();));
   private _f0 = if_error(data(DevNodeRef(_nid, _N_INPUT_FILTER)),(DevLogErr(_nid, "Missing filter freq. of ch 0"); abort();));
   private _f1 = if_error(data(DevNodeRef(_nid, _N_INPUT_FILTER + 6)),(DevLogErr(_nid, "Missing filter freq. of ch 1"); abort();));
   private _f2 = if_error(data(DevNodeRef(_nid, _N_INPUT_FILTER + 12)),(DevLogErr(_nid, "Missing filter freq. of ch 2"); abort();));
   private _f3 = if_error(data(DevNodeRef(_nid, _N_INPUT_FILTER + 18)),(DevLogErr(_nid, "Missing filter freq. of ch 3"); abort();));
   if_error(hpe1564_32->hpe1564_input (VAL(_id), _r0, _f0, _r1, _f1, _r2, _f2, _r3, _f3),
	(DevLogErr(_nid, "Error in Input setup"); abort();));

/* Trigger */
   _source = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)),(DevLogErr(_nid, "Missing trigger source"); abort();));
   private _level = real(if_error(data(DevNodeRef(_nid, _N_TRIG_LEVEL)),(DevLogErr(_nid, "Missing trigger level"); abort();)),53UB);
   if_error(hpe1564_32->hpe1564_trigEvent (VAL(_id), VAL(1), VAL(_source), _level),
	(DevLogErr(_nid, "Error in Input setup"); abort();));
    return (1);
}