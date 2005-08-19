/* Version Number 4.0*/
/* 2005.8.18 */
/* return value make double(real)*/
/* sampling rate, verti_Range_Offset, Channel Probe Attenuation, Trigger Level, Trigger Delay */

public fun NI5122__setup(as_is _nid, optional _method)
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
    private _N_CHANNEL_IMPE =16;
    private _N_VOLT_RANGE =17;
    private _N_RANGE_OFFSET =18;
    private _N_CH_COUPLING =19;
    private _N_PROBE_ATTE =20;


    _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
/* Channels Sample number, rate */
   private _channel = if_error(data(DevNodeRef(_nid, _N_CHANNEL)),(DevLogErr(_nid, "Missing # of Channel setting"); abort();));
   private _chimpe = if_error(data(DevNodeRef(_nid, _N_CHANNEL_IMPE)),(DevLogErr(_nid, "Missing # of channel impedence"); abort();));
   private _samrate = real(if_error(data(DevNodeRef(_nid, _N_SAM_RATE)),(DevLogErr(_nid, "Missing sample rate"); abort();)),53UB);
   private _samnum = if_error(data(DevNodeRef(_nid, _N_SAM_NUM)),(DevLogErr(_nid, "Missing sample number"); abort();));
  if_error(ni5122->NiScope_ConfigureHorizontalTiming (VAL(_id), _samrate,VAL(_samnum)),(DevLogErr(_nid, "Error in Sample setup Configure Horizontal Time"); abort();));

/*   private _period = real(if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD)),(DevLogErr(_nid, "Missing sampling time"); abort();)),53UB);  */
/* 여기서 우리는 함수에 문자를 넘겨 줘야하는데 java Setup에서 문자를 넘겨주는 함수가 정의(DeviceChoice)되어 있지 않다 */
/* 그리하여 TDI에서 어레이를 만들어서 함수에 넣는다.이를 java에서 넘겨주려면 DeviceChoice에 함수를 정의 해줘야한다.결국 수정을 해야한다. */

/* Channels Configuration channel impedence */
   _chname=["0","1"];
   for(_i=0; _i<2; _i++) {
   private _chimpe = if_error(data(DevNodeRef(_nid, _N_CHANNEL_IMPE + _i*9)),(DevLogErr(_nid, "Missing # of Channel impedence"); abort();));
   if_error(ni5122->NiScope_ConfigureChanCharacteristics (VAL(_id), trim(_chname[_i]), _chimpe),(DevLogErr(_nid, "Error in Configure Channel impedence"); abort();));
   }

/* Channels Configuration Voltage, offset,*/
   for(_i=0; _i<2; _i++) {
   private _range = if_error(data(DevNodeRef(_nid, _N_VOLT_RANGE+ _i*9)),(DevLogErr(_nid, "Missing # of voltage range"); abort();));
   private _offset = real(if_error(data(DevNodeRef(_nid, _N_RANGE_OFFSET+ _i*9)),(DevLogErr(_nid, "Missing # of voltage range offset"); abort();)),53UB);
   private _chcoupl = if_error(data(DevNodeRef(_nid, _N_CH_COUPLING+ _i*9)),(DevLogErr(_nid, "Missing # of channel input signal coupling"); abort();));
   private _probeatte = real(if_error(data(DevNodeRef(_nid, _N_PROBE_ATTE+ _i*9)),(DevLogErr(_nid, "Missing # of probe attenuation"); abort();)),53UB);
   if_error(ni5122->NiScope_ConfigureVertical (VAL(_id), trim(_chname[_i]),_range, _offset, VAL(_chcoupl),_probeatte),(DevLogErr(_nid, "Missing # of Configure Vertical"); abort();));
   }
/* Trigger setup*/
    _trig= ["VAL_IMMEDIATE","0","1","VAL_EXTERNAL"];
   private _trigsource = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)),(DevLogErr(_nid, "Missing # of trigger source"); abort();));
   private _triglevel = real(if_error(data(DevNodeRef(_nid, _N_TRIG_LEVEL)),(DevLogErr(_nid, "Missing # of trigger level"); abort();)),53UB);
   private _trigslope = if_error(data(DevNodeRef(_nid, _N_TRIG_SLOPE)),(DevLogErr(_nid, "Missing # of trigger slope"); abort();));
   private _trigcoupl = if_error(data(DevNodeRef(_nid, _N_TRIG_COUPL)),(DevLogErr(_nid, "Missing # of trigger coupling"); abort();));
   private _trigdelay = real(if_error(data(DevNodeRef(_nid, _N_TRIG_DELAY)),(DevLogErr(_nid, "Missing # of trigger delay"); abort();)),53UB);


   if (_trigsource eq 0) 
   {
   if_error(ni5122->NiScope_configuretriggerimmediate(VAL(_id)), (DevLogErr(_nid, "Error in Immediate triggering"); abort();));
   }
   else if (_trigsource ne 0) 
   {
   if_error(ni5122->NiScope_ConfigureTriggerEdge (VAL(_id),trim(_trig[_trigsource]),_triglevel, VAL(_trigslope),VAL(_trigcoupl),_trigdelay),(DevLogErr(_nid, "Missing # of Configure Trigger Edge"); abort();));
   }
   else write(*,'NiScope_Setup error at TDI fun');
   TreeClose();
   return (1);
}
