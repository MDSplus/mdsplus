public fun CAMERAReadFrames(in _sock, inout _n_frame, inout _x_pixel, inout _y_pixel)
{
	_dim = 0;
	_n_frame = 0;
	_x_pixel = 0;
	_y_pixel = 0;

	if( TcpClient->RecvData(val(_sock), ref(_dim), val(4)) != 4 )
	   return ( zero(0, 0B) );

write(*, "Buffer dimension ", _dim);

	if( TcpClient->RecvData(val(_sock), ref(_n_frame), val(4)) != 4 )
	   return ( zero(0, 0B) );

write(*, "Numero di frame ", _n_frame);

	if( TcpClient->RecvData(val(_sock), ref(_x_pixel), val(4)) != 4 )
	   return ( zero(0, 0B) );

write(*, "X pixels", _x_pixel);

	if( TcpClient->RecvData(val(_sock), ref(_y_pixel), val(4)) != 4 )
	   return ( zero(0, 0B) );

write(*, "Y pixels", _y_pixel);

	_dim = _dim - 12;

	_out = zero(_dim, 0B);
/*
	if( TcpClient->RecvData(val(_sock), ref(_out), val(_dim)) != _dim )
	   return ( zero(0, 0B) );
*/

	_dim = TcpClient->RecvData(val(_sock), ref(_out), val(_dim));

write(*, "Buffer dimension read", _dim);

	return ( _out );
}