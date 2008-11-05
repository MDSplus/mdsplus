public fun RS232Read(in _hComm, in _bufLen)
{
   _out = zero([_bufLen], 0B);	
   _len = RS232Lib->RSRead( _hComm,  ref( _out ) , val(_bufLen) );
   
   if( _len <= 0 )
	  return ( [] );
	
	return ( _out[0 : _len - 1] );
   
}
