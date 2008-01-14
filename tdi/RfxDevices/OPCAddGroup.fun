public fun OPCAddGroup(in _serverHandle, in _gName, in _uRate)
{
	_status = 0;

    _status = opcAccess->OPCAddGroup( val( _serverHandle ), _gName, val(_uRate) );

    return ( _status );

}