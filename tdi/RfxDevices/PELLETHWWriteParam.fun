public fun PELLETHWWriteParam(in _fd, in _param, in _value)
{
    _status = libPellet->PelletWriteParam( val( _fd ), _param, fs_float(_value) );

	return( _status );
}
