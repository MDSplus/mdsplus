/* 
   DAMPHASE(IN _damid, IN _shot)
   
   This TDI function sends a DAM phase post to the data analysis monitor
   and declares a MDSplus event for the phase.
*/

PUBLIC FUN DAMPHASE(IN _damid, IN _shot, OPTIONAL IN _eventname) {

	/* Declare the phase as a MDSplus event */
	if (present(_eventname)) _status = setevent(_eventname,_shot);

        /* Declare the phase to the data analysis monitor */
        _machine = "talos.gat.com";
        _damid = TRIM(ADJUSTL(""//_damid));
	_shot  = TRIM(ADJUSTL(""//_shot));
        _command = "sendpost id="//_damid//" ok=TRUE shot="//_shot//" &";
        _cmd = "ssh -q "//_machine//" \""//_command//"\"";
	write (*,_cmd);
        SPAWN(_cmd);

        return(1);
}

