public fun VX4244__setup(as_is _nid, optional _method)
{
   private _K_CONG_NODES = 165;
   private _N_ID = 2;
   private _N_SAMPLE_PERIOD = 5;   /* sample rate    */
   private _N_SAMPLE_NUM = 8;        /* sample length count */
   private _N_TRIG_SOURCE = 11;     /* Default Trigger */
   private _N_INPUT_RANGE = 19;      
/*  AI channel select    Group 1  */
    
   _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
    
/* Voltage range of Group 0 */
   for(_i=0; _i<4; _i++)
      var('_range'//text(_i,1), if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE +_i*6)), (DevLogErr(_nid, 'Missing #, Range of input '//text(_i,1)//' in GP0'),7)));
   _range = [_range0, _range1, _range2, _range3];
   for(_i=0; _i<4; _i++)
      if_error(Tkvx4244_32->tkvx4244_confGpOneVoltageRange (VAL(_id), VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(_range[_i])),
	 (DevLogErr(_nid, 'Error in Range setting of input '//text(_i,1)//' in GP0'); abort();));

/* channel ON/OFF of Group 0 */
    if_error(Tkvx4244_32->tkvx4244_confGroupOneChannels (VAL(_id),VAL(_range0==7),VAL(_range1==7),VAL(_range2==7),VAL(_range3==7), VAL(0)),
       (DevLogErr(_nid, 'Error in channels ON/OFF in GP0'); abort();));
    if_error(Tkvx4244_32->tkvx4244_confGroupOneChannels (VAL(_id), VAL(_range0<>7),VAL(_range1<>7),VAL(_range2<>7),VAL(_range3<>7), VAL(1)),
       (DevLogErr(_nid, 'Error in channels ON/OFF in GP0'); abort();));

/* Voltage range of Group1 */
   for(_i=0; _i<4; _i++)
      var('_range'//text(_i,1), if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE +_i*6+39)), (DevLogErr(_nid, 'Missing #, Range of input '//text(_i,1)//' in GP1'),7)));
   _range = [_range0, _range1, _range2, _range3];
    for(_i=0; _i<4; _i++)
      if_error(Tkvx4244_32->tkvx4244_confGpTwoVoltageRange (VAL(_id), VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(_range[_i])),
         (DevLogErr(_nid, 'Error in Range setting of input '//text(_i,1)//' in GP1'); abort();));

/* channel ON/OFF of Group 1 */
    if_error(Tkvx4244_32->tkvx4244_confGroupTwoChannels (VAL(_id), VAL(_range0==7),VAL(_range1==7),VAL(_range2==7),VAL(_range3==7), VAL(0)),
       (DevLogErr(_nid, 'Error in channels ON/OFF in GP1'); abort();));
    if_error(Tkvx4244_32->tkvx4244_confGroupTwoChannels (VAL(_id), VAL(_range0<>7),VAL(_range1<>7),VAL(_range2<>7), VAL(_range3<>7), VAL(1)),
       (DevLogErr(_nid, 'Error in channels ON/OFF in GP1'); abort();));
    

/* Voltage range of Group 2 */
   for(_i=0; _i<4; _i++)
      var('_range'//text(_i,1), if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE +_i*6+78)), (DevLogErr(_nid, 'Missing #, Range of input '//text(_i,1)//' in GP2'),7)));
   _range = [_range0, _range1, _range2, _range3];
   for(_i=0; _i<4; _i++)
      if_error(Tkvx4244_32->tkvx4244_confGpThreeVoltageRange (VAL(_id), VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(_range[_i])),
         (DevLogErr(_nid, 'Error in Range setting of input '//text(_i,1)//' in GP2'); abort();));

/* channel ON/OFF of Group 2 */
   if_error(Tkvx4244_32->tkvx4244_confGroupThreeChannels (VAL(_id), VAL(_range0==7),VAL(_range1==7),VAL(_range2==7),VAL(_range3==7), VAL(0)),
     (DevLogErr(_nid, 'Error in channels ON/OFF in GP2'); abort();));
   if_error(Tkvx4244_32->tkvx4244_confGroupThreeChannels (VAL(_id), VAL(_range0<>7),VAL(_range1<>7),VAL(_range2<>7), VAL(_range3<>7), VAL(1)),
     (DevLogErr(_nid, 'Error in channels ON/OFF in GP2'); abort();));
    

/* Voltage range of Group 3 */
   for(_i=0; _i<4; _i++)
      var('_range'//text(_i,1), if_error(data(DevNodeRef(_nid, _N_INPUT_RANGE +_i*6+117)), (DevLogErr(_nid, 'Missing #, Range of input '//text(_i,1)//' in GP3'),7)));
   _range = [_range0, _range1, _range2, _range3];
   for(_i=0; _i<4; _i++)
      if_error(Tkvx4244_32->tkvx4244_confGpFourVoltageRange (VAL(_id), VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(_range[_i])),
         (DevLogErr(_nid, 'Error in Range setting of input '//text(_i,1)//' in GP3'); abort();));

/* channel ON/OFF of Group 3 */
   if_error(Tkvx4244_32->tkvx4244_confGroupFourChannels (VAL(_id), VAL(_range0==7),VAL(_range1==7),VAL(_range2==7),VAL(_range3==7), VAL(0)),
      (DevLogErr(_nid, 'Error in channels ON/OFF in GP3'); abort();));
   if_error(Tkvx4244_32->tkvx4244_confGroupFourChannels (VAL(_id), VAL(_range0<>7),VAL(_range1<>7),VAL(_range2<>7), VAL(_range3<>7), VAL(1)),
      (DevLogErr(_nid, 'Error in channels ON/OFF in GP3'); abort();));


/* Sample rate setup */
/* Group  sample_period(frequency and dely) Double형이다., sample_numpre delay를 setting 한다 source: EXTERNAL=0 , FREQUENCY = 1  Group =0 */
   for(_i=0; _i<4; _i++)
   {
      _period = if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD+_i*39)), (DevLogErr(_nid, 'Missing #, Sampling rate of GP'//text(_i,1)),10000d0));
      if_error(Tkvx4244_32->tkvx4244_confSamplingInterval (VAL(_id), VAL(1), VAL(1), _period, VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(0)),
        (DevLogErr(_nid, "Error in Sampling Rate"); abort();));
   }


/* Group samlple_num( sample count setting)  Long 형이다. */
   for(_i=0; _i<4; _i++)
   {
      _count = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM+_i*39)),(DevLogErr(_nid, 'Missing #, Sample number of GP'//text(_i,1)),1024));
      if_error(Tkvx4244_32->tkvx4244_confDataCollectCount (VAL(_id), VAL(_count),  VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(0)),
        (DevLogErr(_nid, "Error in Sampling Count"); abort();));
    }

/* Trigger Setting */
   if_error(Tkvx4244_32->tkvx4244_confArmSrcMemZero (VAL(_id),VAL(1), VAL(1), VAL(1), VAL(1), VAL(1), VAL(0), VAL(2)),(DevLogErr(_nid, "Error in Arm Source"); abort();));
   for (_i=0; _i<4; _i++)
      var("_trigger"//text(_i,1),if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE + _i*39)),(DevLogErr(_nid, 'Missing #, Trigger source in GP'//text(_i,1)),1)));

/* Group 0  */
   for (_i=0; _i<13; _i++)
   {
      var(trim("_trig"//adjustl(_i)),IAND(_trigger0,1));
      _trigger0= ISHFT(_trigger0,-1);
   }
   if_error(Tkvx4244_32->tkvx4244_confGpOneTrigSrcLogic (VAL(_id), VAL(_trig12), VAL(_trig0), VAL(_trig1), VAL(_trig2), VAL(_trig3), VAL(_trig4), VAL(_trig5), VAL(_trig6), VAL(_trig7), VAL(_trig8), VAL(_trig9), VAL(_trig10), VAL(_trig11)),(DevLogErr(_nid, "Error in Trigger Setting GP0"); abort();));

/* Group 1  */
   for (_i=0; _i<13; _i++)
   {
      var(trim("_trig"//adjustl(_i)),IAND(_trigger1,1));
      _trigger1= ISHFT(_trigger1,-1);
   }
   if_error(Tkvx4244_32->tkvx4244_confGpTwoTrigSrcLogic (VAL(_id), VAL(_trig12), VAL(_trig0), VAL(_trig1), VAL(_trig2), VAL(_trig3), VAL(_trig4), VAL(_trig5), VAL(_trig6), VAL(_trig7), VAL(_trig8), VAL(_trig9), VAL(_trig10), VAL(_trig11)),(DevLogErr(_nid, "Error in Trigger Setting GP1"); abort();));
     
/* Group 2  */
   for (_i=0; _i<13; _i++)
   {
      var(trim("_trig"//adjustl(_i)),IAND(_trigger2,1));
      _trigger2= ISHFT(_trigger2,-1);
   }
   if_error(Tkvx4244_32->tkvx4244_confGpThreeTrigSrcLogic (VAL(_id), VAL(_trig12), VAL(_trig0), VAL(_trig1), VAL(_trig2), VAL(_trig3), VAL(_trig4), VAL(_trig5), VAL(_trig6), VAL(_trig7), VAL(_trig8), VAL(_trig9), VAL(_trig10), VAL(_trig11)),(DevLogErr(_nid, "Error in Trigger Setting GP2"); abort();));
     
/* Group 3  */
   for (_i=0; _i<13; _i++)
   {
      var(trim("_trig"//adjustl(_i)),IAND(_trigger3,1));
      _trigger3= ISHFT(_trigger3,-1);
   }
   if_error(Tkvx4244_32->tkvx4244_confGpFourTrigSrcLogic (VAL(_id), VAL(_trig12),VAL(_trig0), VAL(_trig1), VAL(_trig2), VAL(_trig3), VAL(_trig4), VAL(_trig5), VAL(_trig6), VAL(_trig7), VAL(_trig8), VAL(_trig9), VAL(_trig10), VAL(_trig11)),(DevLogErr(_nid, "Error in Trigger Setting GP3"); abort();));

   TreeClose();
   return (1);
}
