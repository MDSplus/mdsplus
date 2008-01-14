public fun OPCRemoveGroup(in _serverHandle, in _gName)
{
	_status = 0;

    _status = opcAccess->OPCRemoveGroup(  val( _serverHandle ), _gName );

    return ( _status );

}