public fun GPIEnabled()
{
	return ( \RFX::DIAG_TIMES_SETUP.DGPI:TRIG_STATE == "ENABLED" ? 1. : 0. );
}
