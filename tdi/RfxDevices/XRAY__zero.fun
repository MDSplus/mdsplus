public fun XRAY__zero(as_is _nid, optional _method)
{

    private _K_CONG_NODES = 793;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_CAL_EXP = 2;
    private _N_CAL_SHOT = 3;
    private _N_IP_ADDR_0 = 4;
    private _N_IP_ADDR_1 = 5;
    private _N_FREQUENCY = 6;
    private _N_TRIG_SOURCE = 7;
    private _N_DURATION = 8;
    private _N_HOR_HEAD = 9;
    private _N_HEAD_POS = 10;

    private _K_NODES_PER_CHANNEL = 10;
    private _N_CHANNEL_0 = 11;
	private _N_CHAN_CHANNEL_ID = 1;
	private _N_CHAN_GAIN = 2;
    private _N_CHAN_FILTER = 3;
    private _N_CHAN_TR_IMPEDANCE = 4;
    private _N_CHAN_BIAS = 5;
    private _N_CHAN_CALIBRATION = 6;
    private _N_CHAN_BANDWIDTH = 7;
	private _N_CHAN_STATUS = 8;
    private _N_CHAN_DATA = 9;

	private _K_CHANNELS		= 78;

    _status = 0;

    _ip_addr_0 = if_error(data(DevNodeRef(_nid, _N_IP_ADDR_0)), "");
	if( _ip_addr_0 == ""  )
	{
    	DevLogErr(_nid, "Invalid Crate IP specification rack 0");
	    abort();
	}


    _ip_addr_1 = if_error(data(DevNodeRef(_nid, _N_IP_ADDR_1)), "");
	if( _ip_addr_1 == "" )
	{
    	DevLogErr(_nid, "Invalid Crate IP specification rack 1");
	    abort();
	}
   
   	_chan_id = [];

	for(_i = 0; _i < _K_CHANNELS; _i++)
	{

          _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

		   if(DevIsOn(DevNodeRef(_nid, _chan_nid)))
		   {
				_enabled = 1;
		   }
		   else
		   {
				_enabled = 0;
		   }


		  _id = TomoChanId( _i+1, _enabled, XrayChMapping() );
		  _chan_id = [_chan_id, _id];

	}


	write(*, "Zero rack ", _ip_addr_0);

	_cmd = 'MdsConnect("'//_ip_addr_0//'")';
	_status = execute(_cmd);

	_errors_0 = zero(_K_CHANNELS, 0);

	if(_status != 0)
    {
       _expr = "XrayHWZero(0, $)";  

       _errors_0 = MdsValue(_expr, _chan_id, 0);
	
	   MdsDisconnect();

	   write(*, "Fine Zero rack ", _ip_addr_0);
	 }
	 else
     {   	
	 
		DevLogErr(_nid, "Cannot connect to VME rack 0");
	    abort();

     }


	 write(*, "Zero rack ", _ip_addr_1);

	_cmd = 'MdsConnect("'//_ip_addr_1//'")';
	_status = execute(_cmd);

	_errors_1 = zero(_K_CHANNELS, 0);

	if(_status != 0)
    {
       _expr = "XrayHWZero(0, $)";  

       _errors_1 = MdsValue(_expr, _chan_id, 0);
	
	   MdsDisconnect();

	   write(*, "Fine Zero rack ", _ip_addr_1);
	 }
	 else
     {   	
		DevLogErr(_nid, "Cannot connect to VME rack 1");
	    abort();
     }
 
                
	 for(_i = 0; _i < _K_CHANNELS; _i++)
	 {


	   if( TomoChanIsActive(_chan_id[_i]) )
	   {

          if(TomoVmeRack(_chan_id[_i]) == 0)
		    if(_errors_0[ _i ] != 0)
				DevLogErr(_nid,  TomoErrorMessage( _errors_0[ _i ] ) );
		  else
		    if(_errors_1[ _i ] != 0)
				DevLogErr(_nid,  TomoErrorMessage( _errors_1[ _i ] ) );
	          
	   }
	 }


     return(1);


}
