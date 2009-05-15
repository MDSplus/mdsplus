public fun RfxPulseSetVer( in _version )
{
    _exp = if_error($expt, *, *);
    if( !size( _exp ) )
	return (0);

    _date = RfxGetVersionDate( _exp, _version );

    _status = tcl("set view \""//_date//"\"");

    return ( _status );
}