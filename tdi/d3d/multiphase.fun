/*
  MULTIPHASE.FUN
 
  This TDI function is for use with the multiphase system.  This is
  a way to set up a phase to be dispatched after dependent phases
  have been dispatched.

  To use, set up a tree structure like this example:
 
  .MULTIPHASE
   |
   |-----:PHASE
   |
   |-----:MAG (or any other phase)
   |      |
   |      |-----:DONE
   |
   |-----:NB (or any other phase)
          |
          |-----:DONE
 
 The PHASE node contains the dependent phase that is dispatched after
 all the dependent phases (MAG and NB in this case) are received.

 MAG and NB are action nodes.

 Note that you must have the mdsdispatcher env var set to make use
 of this function.
 
*/

FUN PUBLIC MULTIPHASE(IN _fullpath)
{
	/* set this phase as done */
	_status = TreePut( Build_Path(_fullpath//":DONE"), "$",1);

	/* check to see if all other requisite phases are done */
	_done = IF_ERROR(ALL(GETNCI(_fullpath//".-*:DONE","RECORD")),0);

	/* dispatch the dependent phase if all requisite phases are done */
      	If (_done > 0)
	{
		_phase = BUILD_PATH(_fullpath//".-:PHASE");
		TCL("DISPATCH/COMMAND/SERVER=mdsdispatcher DISPATCH/PHASE "//_phase);
	} 

	/* return 1 to indicate success */
 	RETURN(1);
}





