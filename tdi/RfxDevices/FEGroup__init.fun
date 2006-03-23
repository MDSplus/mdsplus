public fun FEGroup__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES   =   4;
    private _N_HEAD 		=	0;
    private _N_COMMENT 		=	1;
    private _N_CHANS_PATH   =	2;
    private _N_GAIN 		=	3;

write(*, "FEGroup__init");

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

		_chan_gain_nid  = getnci(arg_of(_chans_nid, _i), 'conglomerate_elt') - 1;
		
/*
write(*, _ch_nid);
*/
		_dev_head_nid = compile(getnci(DevHead(_ch_nid), 'fullpath'));
/*
write(*, _dev_head_nid, _chan_gain_nid);
*/
		_model_type = model_of(_dev_head_nid);

		if(_model_type == "ISIS_GAIN")
		{
/*
write(*, "MODEL ", _model_type);
*/
			_value = data(DevNodeRef(_nid, _N_GAIN));
/*
write(*, "Gain ", DevNodeRef( _dev_head_nid, _chan_gain_nid), _value);
*/
			DevPut(_dev_head_nid, _chan_gain_nid, _value);

		}
		else
		{
			_msg = "Invalid channel path definition : "//_ch_nid;
			DevLogErr(_nid, _msg);
			abort();
		}
	}

	return(1);

}
