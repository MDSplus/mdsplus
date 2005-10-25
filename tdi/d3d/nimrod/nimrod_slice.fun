FUN PUBLIC NIMROD_SLICE(IN _slice)
{

/*
	This function allows NIMROD users running nimds to add timeslices
	to a NIMROD tree.  Note that it executes a TCL script located in a 
	particular directory on atlas ($nimrod_path)

	The old Fortran in mds_mod.f that did this is:

          tclstring = "TCL('@"//'"/mdstrees1/mdstest/ssi/nimrod'
     >//'/nimrod_slice.tcl '//TRIM(slice_list(islice))//'"'//"')"//c0

	Jeff Schachter and Sean Flanagan, 2001.07.20

*/

	IF (ICHAR(UPCASE(TRIM(_slice))) < 65)
	{
	   WRITE(*,"Invalid slice - it does not start with a letter\n\r");
	   ABORT();
	}

	_path = TRANSLATELOGICAL("nimrod_path");
	
        _cmd = '@"'//_path//'/nimrod_slice.tcl" '//TRIM(_slice);
/*	_cmd = '@"'//_path//'/nimrod_slice.tcl '//TRIM(_slice)//'"'; */  /* need kludged quote for mdsdcl bug */

/*	_cmd = '@'//_path//'/nimrod_slice.tcl '//TRIM(_slice); */ /* mdsdcl bug fixed */
/*	WRITE(*,_cmd); */

        _status = TCL(_cmd);      
        _cmd2 = _path//'/nimrod_slice_ref '//TRIM(_slice)//' '//$SHOT;
        SPAWN(_cmd2);
 

        RETURN(_status);

}

	

