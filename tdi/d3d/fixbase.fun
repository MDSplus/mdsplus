/*	FIXBASE.FUN

	Repackages TRANSP 1D output signals for C-Mod tree with BUILD_DIM() on time dimension
	so that TDI subscripting works for Dave Greenwood.

	Will be made obsolete by rewriting the time dimensions in the C-Mod trees at some point.


	Jeff Schachter, 1999.12.08

*/

PUBLIC FUN FIXBASE (IN _signal)
{
	return(MAKE_SIGNAL(MAKE_WITH_UNITS(DATA(_signal),UNITS(_signal)),,MAKE_DIM(*,\TIME1D)));
}
	

