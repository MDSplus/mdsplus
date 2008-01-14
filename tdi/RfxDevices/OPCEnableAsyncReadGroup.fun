public fun OPCEnableAsyncReadGroup(in _serverHandle, in _gName, in _args)
{
    _status = opcAccess->OPCEnableAsyncReadGroup(  val( _serverHandle ), _gName, VAL(0), _args, val(sizeof(_args) + 1));
     return(_status);
}
