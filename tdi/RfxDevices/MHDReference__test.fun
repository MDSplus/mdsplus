public fun MHDReference__test(as_is _nid, optional _method)
{
    private _N_VME_IP = 1;
    private _N_DESCRIPTION = 2;
    private _N_SIG_BASE = 3;
	
	_y = sin(0:6.28:0.01);
	_x = [0:6.28:0.01];

    for( _i = 0; _i < 192; _i++ )
    {
	
		_signalNid = DevHead(_nid) + _N_SIG_BASE + _i;
		
		write(*, getnci( _signalNid , "FULLPATH") );
		
		write( *, _y );
		
		_yval = ( _y * ( _i + 1 ) );
		write(*, "1");
	    _signal = compile('build_signal(`_yval,,`_x)');
		write(*, "2");

		TreeShr->TreePutRecord( val( _signalNid ),  xd( _signal ), val(0) );
		write(*, "3");
		
    }
	
    return (1);
}
