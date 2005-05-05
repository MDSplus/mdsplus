public fun ST133HeaderParam( in _header, in _offset, in _type)
{

	_data = _type;
	_size = sizeof(_type);
	
	ST133->ST133HeaderParam( ref(_header) , val(_offset), ref(_data), val(_size) );
	
	return ( _data );
}


