public fun RfxPulseOpenSetVer( in _exp, in _shot, in _version )
{
    tcl("set tree "//_exp//"/shot="//_shot);

    _status = RfxPulseSetVer( _version );
    
    return ( _status );
}