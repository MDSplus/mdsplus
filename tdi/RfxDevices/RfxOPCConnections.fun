public fun RfxOPCConnections()
{
	mdsconnect("150.178.34.151:8100");
	_a = mdsvalue('opcserverconnections()');
	write(*, _a);
	mdsdisconnect();
}
