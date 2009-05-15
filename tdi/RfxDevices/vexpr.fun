public fun vexpr(as_is _expr, in _version)
{
    _error = 0;
    
    _expt = if_error( $EXPT, _error = 1);	

    if( _error == 0 )
    {
	_date = RfxGetVersionDate( _expt, _version );
    }
    
    if( _error == 0 )
    {
	_expr_str = execute( 'decompile( `_expr )' ) ;
        _status = tcl('set view  "'//_date//'"');
   
	if( _status & 1 )
	{	
	    _out = if_error( execute( _expr_str ), _error = 1);	
    
	}
	else
	    _error = 1;
	
	tcl('set view now');
    } 
        
    if( _error == 1 )
	abort();
    else
	return ( _out );
}