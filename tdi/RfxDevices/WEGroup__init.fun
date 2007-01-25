public fun WEGroup__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES =     211;
    private _N_HEAD =			0;
    private _N_COMMENT =		1;

    private _N_CHANS_PATH =		2;

    private _N_WE7116_START_TIME =	4;
	private _N_WE7116_END_TIME=		5;
	private _N_WE7116_COUPLING =	6;
	private _N_WE7116_RANGE =		7;
	private _N_WE7116_OFFSET =		8;
	private _N_WE7116_FILTER =		9;

    private _N_WE7275_START_TIME =	11;
	private _N_WE7275_END_TIME =	12;
	private _N_WE7275_COUPLING =	13;
	private _N_WE7275_RANGE =		14;
	private _N_WE7275_AAF =			15;
	private _N_WE7275_FILTER =		16;

 	private _N_WE7116_CHAN_START_TIME =	1;
	private _N_WE7116_CHAN_END_TIME =	2;
	private _N_WE7116_CHAN_COUPLING =	5;
	private _N_WE7116_CHAN_RANGE =		6;
	private _N_WE7116_CHAN_OFFSET =		7;
	private _N_WE7116_CHAN_FILTER =		8;

    private _N_WE7275_CHAN_START_TIME =	1;
	private _N_WE7275_CHAN_END_TIME =	2;
	private _N_WE7275_CHAN_COUPLING =	5;
	private _N_WE7275_CHAN_RANGE =		6;
	private _N_WE7275_CHAN_AAF =		9;
	private _N_WE7275_CHAN_FILTER =		10;



write(*, "WEGroup__init");

	_error = 0;

	_chans_nid = if_error(getnci(DevNodeRef(_nid, _N_CHANS_PATH), 'record'), ( _error = 1 ) );

    if(_error == 1)
    {
    	DevLogErr(_nid, "Invalid channels list");
 		abort();
    }

	_num_chan = ndesc(_chans_nid);

	for(_i = 0; _i < _num_chan; _i++)
	{
		_ch_nid       = compile(getnci(arg_of(_chans_nid, _i), 'fullpath'));

		_chan_offset  = getnci(arg_of(_chans_nid, _i), 'conglomerate_elt') - 1;
		
/*
write(*, _ch_nid);
*/
		_dev_head_nid = compile(getnci(DevHead(_ch_nid), 'fullpath'));
/*
write(*, _dev_head_nid, _chan_offset);
*/
		_model_type = model_of(_dev_head_nid);

		if(_model_type == "WE7275" || _model_type == "WE7275_9")
		{
/*
write(*, "MODEL ", _model_type);
*/
			_value = data(DevNodeRef(_nid, _N_WE7275_START_TIME));
/*
write(*, "Start time ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7275_CHAN_START_TIME), _value);
*/
			DevPut(_dev_head_nid, _chan_offset + _N_WE7275_CHAN_START_TIME, _value);

			_value = data(DevNodeRef(_nid, _N_WE7275_END_TIME));
/*
write(*, "End time ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7275_CHAN_END_TIME), _value);
*/
			DevPut(_dev_head_nid, _chan_offset + _N_WE7275_CHAN_END_TIME, _value);

			_value = data(DevNodeRef(_nid, _N_WE7275_COUPLING));
/*
write(*, "Coupling ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7275_CHAN_COUPLING),  _value);
*/
			DevPut(_dev_head_nid, _chan_offset + _N_WE7275_CHAN_COUPLING, _value);			
			
			_value = data(DevNodeRef(_nid, _N_WE7275_RANGE));
/*
write(*, "Range ",  DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7275_CHAN_RANGE), _value);
*/
			DevPut(_dev_head_nid, _chan_offset + _N_WE7275_CHAN_RANGE, _value);			
			
			_value = data(DevNodeRef(_nid, _N_WE7275_AAF));
/*
write(*, "AAF ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7275_CHAN_AAF), _value);
*/
			DevPut(_dev_head_nid, _chan_offset + _N_WE7275_CHAN_AAF, _value);
			
			_value = data(DevNodeRef(_nid, _N_WE7275_FILTER));
/*
write(*, "Filter ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7275_CHAN_FILTER), _value);
*/
			DevPut(_dev_head_nid, _chan_offset + _N_WE7275_CHAN_FILTER, _value);
		}
		else
		{
			if(_model_type == "WE7116" || _model_type == "WE7116_9")
			{
/*
write(*, "Model ", _model_type);
*/
				_value = data(DevNodeRef(_nid, _N_WE7116_START_TIME));
/*
write(*, "Start time ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7116_CHAN_START_TIME) , _value);
*/
			    DevPut(_dev_head_nid, _chan_offset + _N_WE7116_CHAN_START_TIME, _value);

				_value = data(DevNodeRef(_nid, _N_WE7116_END_TIME));
/*
write(*, "End time ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7116_CHAN_END_TIME) , _value);
*/
			    DevPut(_dev_head_nid, _chan_offset + _N_WE7116_CHAN_END_TIME, _value);

				_value = data(DevNodeRef(_nid, _N_WE7116_COUPLING));
/*
write(*, "Coupling ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7116_CHAN_COUPLING), _value);
*/
			    DevPut(_dev_head_nid, _chan_offset + _N_WE7116_CHAN_COUPLING, _value);

				_value = data(DevNodeRef(_nid, _N_WE7116_RANGE));
/*
write(*,"Range ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7116_CHAN_RANGE), _value);
*/
			    DevPut(_dev_head_nid, _chan_offset + _N_WE7116_CHAN_RANGE, _value);

				_value = data(DevNodeRef(_nid, _N_WE7116_OFFSET));
/*
write(*,"Offset ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7116_CHAN_OFFSET), _value);
*/
			    DevPut(_dev_head_nid, _chan_offset + _N_WE7116_CHAN_OFFSET, _value);

				_value = data(DevNodeRef(_nid, _N_WE7116_FILTER));
/*
write(*, "Filter ", DevNodeRef( _dev_head_nid, _chan_offset + _N_WE7116_CHAN_FILTER), _value);
*/
			    DevPut(_dev_head_nid, _chan_offset + _N_WE7116_CHAN_FILTER, _value);
			}
			else
			{
				_msg = "Invalid channel path definition : "//_ch_nid;
    			DevLogErr(_nid, _msg);
 				abort();
 			}			
		}
	}


	return(1);

}
