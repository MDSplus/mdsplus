public fun CCDGetChannel(in _node, in _channel)
{

	_data = raw_of(_node);

	_dims = shape(_data);

	_data_sig = _data[*,_channel,*];

	_dims = shape(_data_sig);

	_data_sig = set_range(_dims[0], _dims[2], _data_sig);

	_data_sig1 = [];
	for(_i = 0; _i < _dims[0]; _i++)
	 _data_sig1 = [_data_sig1, _data_sig[_i, *]];

	_data_sig1 = set_range(_dims[2], _dims[0], _data_sig1);

	_times = dim_of( _node, 0);
	
	_signal = 'build_signal( ($VALUE) , `_data_sig1, `_times, 0:`_dims[0] - 1 )';

	return( compile(_signal) );

}


