/* Version Number 3.0*/
/* 2005.6.16 */
public fun NI5122__store(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _N_CHANNEL =3;
    private _N_SAM_RATE =5;
    private _N_SAM_NUM =6;
    private _N_TRIG_DELAY = 8;
    private _N_TRIG_SOURCE = 9;
    private _N_TRIG_LEVEL = 10;
    private _N_TRIG_SLOPE = 11;
    private _N_TRIG_COUPL = 12;
    private _N_TRIG_T0 = 13;
    private _N_CHANNEL_IMPE =16;
    private _N_VOLT_RANGE =17;
    private _N_RANGE_OFFSET =18;
    private _N_CH_COUPLING =19;
    private _N_PROBE_ATTE =20;
    private _N_FOO_0 = 21;

/* Get id to communicate with a digitizer */
   private _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
   private _channel = if_error(data(DevNodeRef(_nid, _N_CHANNEL)),(DevLogErr(_nid, "Missing # of Channel setting"); abort();));
   private _chimpe = if_error(data(DevNodeRef(_nid, _N_CHANNEL_IMPE)),(DevLogErr(_nid, "Missing # of channel impedence"); abort();));
   private _samrate = if_error(data(DevNodeRef(_nid, _N_SAM_RATE)),(DevLogErr(_nid, "Missing sample rate"); abort();));
   private _samnum = if_error(data(DevNodeRef(_nid, _N_SAM_NUM)),(DevLogErr(_nid, "Missing sample number"); abort();));
   private _trigdelay = if_error(data(DevNodeRef(_nid, _N_TRIG_DELAY)),(DevLogErr(_nid, "Missing # of trigger delay"); abort();));


/* Read data from a digitizer */
   _chgain = [0d0,0d0];
   _choffset = [0d0,0d0];
    private _samplerate = 0d0;
    private _recordlen = 0u;

    IF_ERROR(ni5122->NiScope_SampleRate(VAL(_id), REF(_samplerate)),(DevLogErr(_nid, "Error while SampleRate"); abort();));
    IF_ERROR(ni5122->NiScope_ActualRecordLength(VAL(_id), REF(_recordlen)),(DevLogErr(_nid,"Error while ActualRecordLength"); abort();));
/*    IF_ERROR(ni5122->NiScope_ActualNumWfms(VAL(_id), _channel, REF(_numwfms)),(DevLogErr(_nid, "Error while ActualNumWfms"); abort();));   //wave form channel count  */
/*    IF_ERROR(ni5122->NiScope_ActualMeasWfmSize(VAL(_id),REF(_measwfsize)),(DevLogErr(_nid, "Error while ActualMeasWfmSize"); abort();));                              */
  
   /* Rate -> Times  */
    _timerate = 1.0/_samplerate;
   private _dim = make_dim(make_window(0, _samnum - 1, 0.0), make_slope(make_with_units(_timerate,'s')));

   /* Raw data -> signal */


    if (_channel eq 0)
    {
write(*,"0 test recordlen:  ", _recordlen);
   _binarydata = zero(_recordlen,0W);
    IF_ERROR(ni5122->NiScope_Store(VAL(_id),"0", REF(_binarydata), REF(_chgain), REF(_choffset)),(DevLogErr(_nid, "Error while reading digitizer configuration"); abort();));
    _signal = compile('build_signal(build_with_units(((`_chgain[0])*$VALUE+(`_choffset[0])),"V"), (`_binarydata), (`_dim))');
    TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0), XD(_signal), VAL(0));
    }
    else if(_channel eq 1)
    {
write(*,"1 test"); 
   _binarydata = zero(_recordlen,0W);
     IF_ERROR(ni5122->NiScope_Store(VAL(_id), "1", REF(_binarydata), REF(_chgain), REF(_choffset)),(DevLogErr(_nid, "Error while reading digitizer configuration"); abort();));
    _signal = compile('build_signal(build_with_units(((`_chgain[0])*$VALUE+(`_choffset[0])),"V"), (`_binarydata), (`_dim))');
    TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0+9), XD(_signal), VAL(0));
    }
    else if(_channel eq 2)
    {
write(*,"0,1 test");
   _binarydata = zero(_recordlen * 2,0W);
    IF_ERROR(ni5122->NiScope_Store(VAL(_id),"0,1", REF(_binarydata), REF(_chgain), REF(_choffset)),(DevLogErr(_nid, "Error while reading digitizer configuration"); abort();));
   _signal = compile('build_signal(build_with_units(((`_chgain[0])*$VALUE+(`_choffset[0])),"V"), (`_binarydata[0:(_recordlen-1)]), (`_dim))');
    TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0), XD(_signal), VAL(0));
    _signal = compile('build_signal(build_with_units(((`_chgain[1])*$VALUE+(`_choffset[1])),"V"), (`_binarydata[(_recordlen:*)]), (`_dim))');
    TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0+9), XD(_signal), VAL(0));
    }
    else write(*,'NiScope_Store error at TDI fun');

    TreeClose();
    return (1);
}
