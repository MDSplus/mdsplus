public fun FLU_GAIN__init(as_is _nid, optional _method)
{

   private _K_CONG_NODES = 181;
   private _N_HEAD = 0;
   private _N_COMMENT = 1;
   private _N_ADC = 2;
   private _N_ACQ_SIGNALS = 3;

   private _K_NUM_CHANNEL = 16;
   private _K_NODES_PER_CHANNEL = 11;
   private _N_CHANNEL_1 = 4;
   private _N_CHAN_ATTENUATION   =  1;
   private _N_CHAN_LIN_NAME   =  2;
   private _N_CHAN_LIN_CALIB  =  3;
   private _N_CHAN_LIN_INPUT  =  4;
   private _N_CHAN_LIN_OUTPUT =  5;
   private _N_CHAN_INT_NAME   =  6;
   private _N_CHAN_INT_CALIB  =  7;
   private _N_CHAN_INT_GAIN   =  8;
   private _N_CHAN_INT_INPUT  =  9;
   private _N_CHAN_INT_OUTPUT = 10;

   private _INVALID = -1;

 write(*, "INIT EQU_GAIN");

   DevNodeCvt(_nid, _N_ACQ_SIGNALS, ['INTEGRAL', 'LINEAR'],[0,1], _acq_signal = _INVALID);
   if( _acq_signal == _INVALID)
   {
	    DevLogErr(_nid, "Missing acquisition signal modality"); 
	    abort();
   }


 
   _adc = if_error(getnci(getnci(DevNodeRef(_nid, _N_ADC), 'record'), 'fullpath'), "");
   if(_adc == "")
   {
	    DevLogErr(_nid, "Missing Acquisition device module path"); 
	    abort();
   }
 
   
	for(_i = 1; _i <= _K_NUM_CHANNEL; _i++)
   	{
 		_head_channel = _N_CHANNEL_1 + (( _i - 1) *  _K_NODES_PER_CHANNEL);
	
		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 
	
			if(_i < 10)
				_path = _adc // '.CHANNEL_0' // TEXT(_i, 1) //':DATA';
			else
				_path = _adc // '.CHANNEL_' // TEXT(_i, 2) //':DATA';

			if(_acq_signal)
			{
				DevPut(_nid, _head_channel + _N_CHAN_LIN_OUTPUT, build_path(_path));
				_nid1 = DevHead(_nid) +  _head_channel + _N_CHAN_INT_OUTPUT;
			    TreeShr->TreePutRecord(val(_nid1),val(0),val(0));
	 		}
			else
			{
				DevPut(_nid, _head_channel + _N_CHAN_INT_OUTPUT, build_path(_path));
				_nid1 = DevHead(_nid) +  _head_channel + _N_CHAN_LIN_OUTPUT;
			    TreeShr->TreePutRecord(val(_nid1),val(0),val(0));
			}

		}
	}


	return (1);
}
