public fun XRAY__setAmpType(as_is _nid, optional _method)
{

	private _K_CONG_NODES = 714;
	private _N_HEAD = 0;
	private _N_COMMENT = 1;
	private _N_IP_ADDR_0 = 2;
	private _N_IP_ADDR_1 = 3;
	private _N_FREQUENCY = 4;
	private _N_TRIG_MODE = 5;
	private _N_TRIG_SOURCE = 6;
	private _N_DURATION = 7;
	private _N_HOR_HEAD = 8;
	private _N_HEAD_POS = 9;
	
	private _K_NODES_PER_CHANNEL = 9;
	private _N_CHANNEL_0 = 10;
	private _N_CHAN_CHANNEL_ID = 1;
	private _N_CHAN_AMP_TYPE = 2;
	private _N_CHAN_GAIN = 3;
	private _N_CHAN_FILTER = 4;
	private _N_CHAN_TR_IMPEDANCE = 5;
	private _N_CHAN_BIAS = 6;
	private _N_CHAN_STATUS = 7;
	private _N_CHAN_DATA = 8;
	
	private _K_CHANNELS	= 78;

    _status = 0;

	for(_i = 0; _i < _K_CHANNELS; _i++)
	{

          _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

		  _id = TomoChanId( _i+1, 1, XrayChMapping() );

		  if(TomoAmpType(_id) == 21)
		     _status = DevPut(_nid, _chan_nid + _N_CHAN_AMP_TYPE, "STANDARD");            
		  else
			 _status = DevPut(_nid, _chan_nid + _N_CHAN_AMP_TYPE, "ENHANCED");  

		  if( $shot == -1)
	      {
			  write(*, "Reset channel_id");
              _status = DevPut(_nid, _chan_nid + _N_CHAN_CHANNEL_ID, -1);  
		  }


	write(*, _status);
}

    return(1);
}
