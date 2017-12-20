/*	DENSITY_OLD.FUN

	Returns DENV2/PATHV2 if available, if not returns DENR0/PATHR0 if available.
	If not returns error.  PATHV2 (PATHR0) is interpolated onto the DENV2 (DENR0)
	timebase by NEBAR.

        WARNING - SF 5/16/03 
	This TDI function is not intended to be evaluated from the D3D tree.  Since the
        EFIT trees were built using the same tag names, multiple instances of the tags:
		\DENSV2, \DENSR0, \PATHV2, \PATHR0
        can be found from the D3D tree.  This means that, when evaluating any of these
        EFIT tags, the first instance of the tag is always picked up.  Or, in other words, 
        whichever EFIT completed first will be the EFIT tree from wich the tags are taken.  
        Obviously, this will cause problems if this TDI function is run from inside the D3D 
        tree, as the tags are not unique, and there is no way to magically know which EFIT the 
        user is attempting to look at DENSITY data for.

        Given the fundamental design of the D3D tree and the multiple EFIT trees, the above 
        problem is expected, and is not an issue with the following TDI function.
        As such, this TDI function cannot be expected to work, given the durrent design 
        of the EFIT trees.   

	UPDATE - SF 1/13/15
	This function is deprecated.  It is being kept around to be used as \DENSITY0 for
	DIII-D EFITs.  Used only for comparison purposes with the new DENSITY() function.

*/

PUBLIC FUN DENSITY_OLD (OPTIONAL OUT _label)
{

	if ($SHOT ge 95898) { 

	  _label = "Density from either DENSV2 or DENSR0";
  	  _l = GETNCI(BUILD_PATH("\\PATHV2"),"LENGTH");

	  if (_l gt 0) {
	    _s = "V2";
	  } else {
	    _s = "R0";
	  }

	  return (NEBAR(_s));

	} else { 

	  _l = GETNCI(BUILD_PATH("\\DENSV2"),"LENGTH");
	  if (_l gt 0) {
	    _s = BUILD_PATH("\\DENSV2");
	    _label = BUILD_PATH("\\DENSV2:LABEL");
	  } else {
	    _l = GETNCI(BUILD_PATH("\\DENSR0"),"LENGTH");
	    if (_l gt 0) {
	      _s = BUILD_PATH("\\DENSR0");
	      _label = BUILD_PATH("\\DENSR0:LABEL");
	    } else {
	      _s = 0.;
	    }
	  }
	  return(_s);
	}


}

