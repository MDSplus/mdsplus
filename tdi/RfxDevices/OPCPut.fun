public fun OPCPut(in _tag, in _value, optional in _nElem)
{
write(*, _tag, _value);

	if (present(_nElem))
	{
		return ( MdsOpcClient->OPCWrite(_tag, _value, val(_nElem)));
	}
	else
		return ( MdsOpcClient->OPCWrite(_tag, _value, val(0)));
}
