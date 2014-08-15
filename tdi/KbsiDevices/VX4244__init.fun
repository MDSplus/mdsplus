public fun VX4244__init(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _N_SAMPLE_DELAY = 6;
    private _N_INPUT_RANGE = 19;
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    
   for(_i=0; _i<4; _i++)
   {
     for(_j=0; _j<4; _j++)
     {
        _range = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE +_j*6 +_i*39)),(DevLogErr(_nid, "Missing # channel select."); abort();));
        if (_range<>7)
        {
          _delay = real(if_error(data(DevNodeRef(_nid, _N_SAMPLE_DELAY +_i*39)),(DevLogErr(_nid, "Missing # Sample delay."); abort();)),53BU);
          if_error(Tkvx4244_32->tkvx4244_initMeasurementCycle(VAL(_id),_delay, VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(0),VAL(0)),
		(DevLogErr(_nid, "Error as initializing Group "//text(_i,1)); abort();));
          break;
        }
     }
   }
    TreeClose();
   return (1);
}
