public fun OPCPut(in _tag, in _value)
{
    return ( MdsOpcClient->OPCWrite(_tag, _value));
}
