blic fun OPCRead(in _tag, out _val)
{
    return ( MdsOpcInterface->OPCRead:INT(_tag, xd(_val)) );
}