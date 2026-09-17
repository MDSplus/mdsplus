/* 
   DAMPHASE(IN _phase, IN _shot)
   
   This TDI function:
     - sends a DAM post
     - sends a MDSplus event
*/

PUBLIC FUN DAMPHASE(IN _phase, IN _shot) {

	/* Declare the phase as a MDSplus event */
	_status = setevent(_phase,_shot);

        /* Declare the phase to the data analysis monitor */
        _machine = "omega01.gat.com";
	_shot  = TRIM(ADJUSTL(""//_shot));
        _command = "/fusion/usc/src/mdsplus/d3d/dispatching/sendpost name="//_phase//"\\&nbspPHASE ok=TRUE shot="//_shot//" code=phase officer=mdsadmin &";
        _cmd = "ssh -q "//_machine//" \""//_command//"\"";
	write (*,_cmd);
        SPAWN(_cmd);

        return(1);
}

