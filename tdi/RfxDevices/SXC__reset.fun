public fun SXC__reset(as_is _nid, optional _method)
{

	private _K_CONG_NODES = 190;
	private _N_HEAD = 0;
	private _N_COMMENT = 1;
	private _N_IP_ADDR = 2;
	private _N_FREQUENCY = 3;
	private _N_TRIG_MODE = 4;
	private _N_TRIG_SOURCE = 5;
	private _N_DURATION = 6;
	private _N_HOR_HEAD = 7;
	private _N_HEAD_POS = 8;
	
	private _K_NODES_PER_CHANNEL = 9;
	private _N_CHANNEL_0 = 9;
	private _N_CHAN_CHANNEL_ID = 1;
	private _N_CHAN_AMP_TYPE = 2;
	private _N_CHAN_GAIN = 3;
	private _N_CHAN_FILTER = 4;
	private _N_CHAN_TR_IMPEDANCE = 5;
	private _N_CHAN_BIAS = 6;
	private _N_CHAN_STATUS = 7;
	private _N_CHAN_DATA = 8;
	
	private _K_CHANNELS	= 20;

    _status = 0;

    _ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
	if( _ip_addr == ""  )
	{
    	DevLogErr(_nid, "Invalid Crate IP specification rack 0");
	    abort();
	}

   
   	_chan_id     = [];

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


	write(*, "Reset rack ", _ip_addr);

	_cmd = 'MdsConnect("'//_ip_addr//'")';
	_status = execute(_cmd);

	_errors_0 = zero(_K_CHANNELS, 0);

	if(_status != 0)
    {
       _expr = "XrayHWReset(0, $)";  

       _errors = MdsValue(_expr, _chan_id, 0);
	
	   MdsDisconnect();

	   write(*, "Fine Reset rack ", _ip_addr);
	 }
	 else
     {   	
	 
		DevLogErr(_nid, "Cannot connect to VME rack 0");
	    abort();

     }

                
	 for(_i = 0; _i < _K_CHANNELS; _i++)
	 {

	   if( TomoChanIsActive(_chan_id[_i]) )
	   {

		    if(_errors[ _i ] != 0)
				DevLogErr(_nid,  TomoErrorMessage( _errors[ _i ] ) );
	          
	    }
	 }


     return(1);


}
