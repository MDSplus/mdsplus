public fun VX4244__store(as_is _nid, optional _method)
{
   private _N_ID = 2;
   private _N_SAMPLE_PERIOD = 5;     
   private _N_SAMPLE_DELAY = 6;      
   private _N_SAMPLE_NUM = 8;        
   private _N_TRIG_T0 = 14;          
   private _N_INPUT_RANGE = 19;      
   private _N_FOO_0 = 21;            
   

/* Get id to communicate with a digitizer */
   private _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
    
/* Read data from a digitizer */
   private _port = 0w;
   private _rangebuf= "                                                                                                                 ";

/* Raw data -> signal */
   IF_ERROR(Tkvx4244_32->tkvx4244_readInputVoltageSettings(VAL(_id), VAL(0),REF(_rangebuf)),(DevLogErr(_nid, "Error while readInputVoltageSettings"); abort();));
   _range=compile("["//extract(0,112,_rangebuf)//"]");
       
   for(_j=0; _j<4; _j++)
   {
      _dt = if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD +_j*39)),(DevLogErr(_nid, 'Missing #, Sampling period in GP'//text(_j,1)); abort();));
      _dt = 1.0/_dt;
      _num = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM +_j*39)),(DevLogErr(_nid, 'Missing #, Sample count in GP'//text(_j,1)); abort();));
      _response = array(_num, 0w);
      _t0 = if_error(data(DevNodeRef(_nid, _N_TRIG_T0 +_j*39)),(DevLogErr(_nid, 'Missing #, T0 in GP'//text(_j,1)), 0.));
      _delay = if_error(data(DevNodeRef(_nid, _N_SAMPLE_DELAY +_j*39)),(DevLogErr(_nid, 'Missing #, Delay in GP'//text(_j,1)), 0.));
      _t0 = _t0 + _delay;
      _dim = make_dim(make_window(0, _num - 1, _t0), make_range(*,*,_dt));
      for(_i=0; _i<4; _i++)
      {
/* Only Selected AI channels are store */
         _chonoff = if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE +_i*6 +_j*39)),(DevLogErr(_nid, 'Missing #, channel ON/OFF in GP'//text(_j,1)),7));
         if (_chonoff<>7) (IF_ERROR(Tkvx4244_32->tkvx4244_getBinaryData (VAL(_id), VAL(_num), VAL(0), VAL(_i+_j*4+1), REF(_response)),
	             (DevLogErr(_nid, 'Error while reading digitizer getBinaryData from input '//text(_i+_j*4,2)); abort();));
         _signal = compile('build_signal(build_with_units((`_range[_i+_j*4]/32768)*$VALUE,"V"), (`_response), (`_dim))');
         TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0 + 6 *_i + 39 *_j), XD(_signal), VAL(0)););
      }
    }
    TreeClose();
    return (1);
}
