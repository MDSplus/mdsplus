/*
   in _path  ; must be \MHD_BR::BRAD_FSIGS to access Fault signals or
			  \MHD_BR::BRAD_FSIGS to access warning signals
   in _upDown; must be set to 1 to access up - down radial field signals or 0 to access
               in - out radial field signals
   in _id    ; signal index  
*/
public fun getBradErrorSignalName(in _path, in _upDown, in _id)
{
	_sigs = data( _path );


	_numSigsTot = size( _sigs );
	
	for( _i = 0; _i < _numSigsTot; _i++)
		if( len( trim ( _sigs[ _i ] ) ) == 0 )
			break;
			
	_numSigsUpDown  = _i - 1;
	
	_numSigInOut = _numSigsTot - _i - 1;	

	
	if( _upDown == 1 )
	{	
	
		if( _id <= _numSigsUpDown )
		{
			return ( _sigs[ _id ] );
		}
		else
			return ( "" );	
	} 
	else
	{
		if( _id <= _numSigInOut )
			return ( _sigs[ _i + _id ]  );		
		else
			return ( "" );	
	}
	

}
