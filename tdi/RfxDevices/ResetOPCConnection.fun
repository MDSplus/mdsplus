public fun ResetOpcConnection()
{
	mdsconnect("150.178.34.151:8100");

	for(_i = 1; _i <=10; _i++)
	{
		write(*, "Close OPC ", _i );
		mdsvalue("OpcDisconnect("//_i//")");
    } 

	mdsdisconnect();
}
