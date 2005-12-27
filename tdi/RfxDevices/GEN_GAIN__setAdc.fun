public fun GEN_GAIN__setAdc(as_is _nid, optional _method)
{

   private _K_CONG_NODES = 83;
   private _N_HEAD = 0;
   private _N_COMMENT = 1;
   private _N_ADC = 2;

   private _K_NUM_CHANNEL = 16;
   private _K_NODES_PER_CHANNEL = 5;
   private _N_CHANNEL_1 = 3;
   private _N_CHAN_NAME   =  1;
   private _N_CHAN_CALIB  =  2;
   private _N_CHAN_INPUT  =  3;
   private _N_CHAN_OUTPUT =  4;

   private _INVALID = -1;

   _adc = if_error(getnci(getnci(DevNodeRef(_nid, _N_ADC), 'record'), 'fullpath'), "");
   if(_adc == "")
   {
	    DevLogErr(_nid, "Missing Acquisition device module path"); 
	    abort();
   }
   
 
   
	for(_i = 1; _i <= _K_NUM_CHANNEL; _i++)
   	{
 		_head_channel = _N_CHANNEL_1 + (( _i - 1) *  _K_NODES_PER_CHANNEL);
	
	
		if(_i < 10)
			_path = _adc // '.CHANNEL_0' // TEXT(_i, 1) //':DATA';
		else
			_path = _adc // '.CHANNEL_' // TEXT(_i, 2) //':DATA';


		DevPut(_nid, _head_channel + _N_CHAN_OUTPUT, build_path(_path));
	}


	return (1);
}
