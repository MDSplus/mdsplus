Public fun OPCRead(in _tag, inout _val)
{
    _status = MdsOpcClient->OPCRead( _tag, xd(_val) );

    return ( _status );
}