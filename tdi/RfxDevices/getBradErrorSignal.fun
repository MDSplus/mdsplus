/*
   in _path  ; must be \MHD_BR::BRAD_FSIGS to access Fault signals or
			  \MHD_BR::BRAD_FSIGS to access warning signals
   in _upDown; must be set to 1 to access up - down radial field signals or 0 to access
               in - out radial field signals
   in _id    ; signal index  
*/
public fun getBradErrorSignal(in _path, in _upDown, in _id)
{
	_Bv = make_signal( [0, 0],, [-2, 6]) ;
	
	_i = 0;
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
/*		
 			write(*, "Calcola ", _id);
*/		


			_dim = dim_of ( build_path( _sigs[ _id ] ) );
/*
			_mod = data(build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_MOD_2"));
			_phs = data(build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_PHS_2"));
			_m0 = (2./192.) * _mod * sin( _phs );
			_m0Sig = make_signal( _m0, ,dim_of( build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_MOD_2")) ); 
*/
            _m0Sig = MarteGetUserArray(\MHD_BR::MARTE, 'br', 1);

			_Bv = resample( _m0Sig, minval( _dim ), maxval( _dim ), 0.001 );
			_d  = resample( build_path( _sigs[ _id ] ), minval( _dim ), maxval( _dim ), 0.001 );

			_pos = execute(extract(16,1,  _sigs[ _id ] ));


			if( ( _pos & 1 ) == 0 )
			{
				_x = _d  - ( _pos == 2 ? 1. : -1.) *  _Bv;
			}
			else
				_x = _d;

			return ( make_signal( _x, , dim_of( _d ) )  );

		}
		else
			return ( make_signal( [0, 0],, [-2, 6]) );	
	} 
	else
	{
		if( _id <= _numSigInOut )
			return ( build_path( _sigs[ _i + _id ] ) );		
		else
			return ( build_signal( [0, 0],, [-2, 6]) );	
	}
	
	
}
