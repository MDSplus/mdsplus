/*	NEBAR.FUN

	Returns line averaged density on timebase from CO2 intereferometer chord _CHORD

	_CHORD = "R0", "V1", "V2", or "V3"
*/

PUBLIC FUN NEBAR (IN _chord)
{

	
	_SpathStr = "\\PATH"//_chord;
	_spath = BUILD_PATH(_SpathStr);

	/* can't do: USING(BUILD_PATH("\\NELINE_"//_chord, \ELECTRONS::TOP, *, "ELECTRONS") for some reason */

/*	switch (_CHORD) { 
	  CASE ("R0") _lineint = USING(DATA(\NELINE_R0), \ELECTRONS::TOP, *, "ELECTRONS"); break;
	  CASE ("V1") _lineint = USING(DATA(\NELINE_V1), \ELECTRONS::TOP, *, "ELECTRONS"); break;
	  CASE ("V2") _lineint = USING(DATA(\NELINE_V2), \ELECTRONS::TOP, *, "ELECTRONS"); break;
	  CASE ("V3") _lineint = USING(DATA(\NELINE_V3), \ELECTRONS::TOP, *, "ELECTRONS"); break;
	}
*/

	_lineint = USING(DATA(BUILD_PATH("\\NELINE_"//_chord)), \ELECTRONS::TOP, *, "ELECTRONS");
	_times = USING(DATA(DIM_OF(BUILD_PATH("\\NELINE_"//_chord))), \ELECTRONS::TOP, *, "ELECTRONS");

	_efit_time = BUILD_PATH("\\ATIME");

	_mask = ((_times GE MINVAL(_efit_time))  && (_times LE MAXVAL(_efit_time)));
	_newtimes = PACK(_times,_mask);
	return (MAKE_SIGNAL(MAKE_WITH_UNITS(PACK(_lineint,_mask) / 2. / CSVAL(_newtimes,CSAKM(_spath)) * 1.e-6,"/cm^3")
		,*,MAKE_WITH_UNITS(_newtimes,UNITS(_efit_time)))); 

}

