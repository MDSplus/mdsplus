public fun OPCTest()
{
	_handle = opcconnect("Matrikon.OPC.Simulation", 300);
/*
	write (* , opcBrowseItem("Matrikon.OPC.Simulation", "", 0));
*/
	opcaddgroup(_handle, "Test", 1000);

	opcadditems(_handle, "Test", "Random.Int1;Random.Int2;Bucket Brigade.cacca",3);

	opcenableasyncreadgroup(_handle, "Test", "Test");

	for(_i = 0; _i < 10; _i++)
	{
		wait(1);
		_out = opcreadasyncgroup(_handle, "Test");

		write(*, SerializeIn(_out) );
	}


	opcdisableasyncreadgroup(_handle, "Test");
	opcdisconnect(_handle);

}
