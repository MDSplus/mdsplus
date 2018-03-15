/*
  LOADPHASE
  
  Description
    This TDI function calls the loadphase shell script, which records
    the time that a phase was dispatched in the .PHASES branch of the D3D
    tree, and posts that info to the DAM.

    The command gets dispatched to lsf002.

    The actionpath argument should be set to the structure node that
    contains the phase into (e.g. \TOP.PHASES.EFIT02_MDS).
*/
PUBLIC FUN LOADPHASE(IN _actionpath)
{

	/* This routine is obsolete */

	/*_machine = "lsf002";
	_command = "/f/mdsplus/dispatching/loadphase "//$SHOT//" "//$EXPT//" "//_actionpath//" >& /dev/null &";
	_cmd = "/usr/bin/rsh -n "//_machine//" "//_command;
	SPAWN(_cmd);
	*/
	return(1);
}
