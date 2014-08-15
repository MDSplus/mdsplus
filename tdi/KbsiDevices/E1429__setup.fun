public fun E1429__setup(as_is _nid, optional _method)
{
   private _K_CONG_NODES = 34;
   private _N_ID = 2;
   private _N_SAMPLE_PERIOD = 4;
   private _N_SAMPLE_DELAY = 5;
   private _N_SAMPLE_SOURCE = 6;
   private _N_SAMPLE_NUM = 7;
   private _N_SAMPLE_NUMPRE = 8;
   private _N_TRIG_SOURCE = 10;
   private _N_TRIG_SLOPE = 11;
   private _N_INPUT_RANGE = 18;
   private _N_INPUT_PORT = 19;

   _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));

   private _num = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM)),(DevLogErr(_nid, "Missing # of samples"); abort();));
   private _numpre = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUMPRE)),(DevLogErr(_nid, "Missing # of pre trig."); abort();));

/* Input */
   for(_i=0; _i<2; _i++) {
      _port = if_error(data(DevNodeRef(_nid, _N_INPUT_PORT + _i*6)),(DevLogErr(_nid, "Missing port"); abort();));
      _range = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE + _i*6)),(DevLogErr(_nid, "Missing range"); abort();));
      if_error(hpe1429_32->hpe1429_configure (VAL(_id), VAL(_i+1), VAL(_port), _range, VAL(_num), VAL(_numpre), VAL(1)),
         (DevLogErr(_nid, "Error in Input setup"); abort();));
   }

/* Arming */
   private _source = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)),(DevLogErr(_nid, "Missing arming source"); abort();));
   private _slope = if_error(data(DevNodeRef(_nid, _N_TRIG_SLOPE)),(DevLogErr(_nid, "Missing arming slope"); abort();));
   if_error(hpe1429_32->hpe1429_armStarEvent (VAL(_id), VAL(1), VAL(_source), VAL(_slope)),
	(DevLogErr(_nid, "Error in arming setup"); abort();));

   private _delay = real(if_error(data(DevNodeRef(_nid, _N_SAMPLE_DELAY)),(DevLogErr(_nid, "Missing delay time"); abort();)),53UB);
   if_error(hpe1429_32->hpe1429_armStarDel (VAL(_id), _delay),
	(DevLogErr(_nid, "Error in delay setup"); abort();));

/* Sample */
   _source = if_error(data(DevNodeRef(_nid, _N_SAMPLE_SOURCE)),(DevLogErr(_nid, "Missing sampling source"); abort();));
   private _period = real(if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD)),(DevLogErr(_nid, "Missing sampling time"); abort();)),53UB);
   if_error(hpe1429_32->hpe1429_trigger (VAL(_id), VAL(_source), _period),
	(DevLogErr(_nid, "Error in trigger setup"); abort();));

    TreeClose();
   return (1);
}