/* DAMPHASE(IN _damid, IN _shot)
   This TDI function sends a DAM phase post to the data analysis monitor
   and declares and MDSplus event for the phase.
   20081111 SF

   20110419 SMF - use metis.gat.com

   20110504 SMF - Does anything even use events based on the phase name?
                  Turning this off until someone contacts DAAG since these
                  phases are probably useless.  

*/

PUBLIC FUN DAMPHASE(IN _damid, IN _shot) {

        /* Declare the phase to the data analysis monitor */
        _machine = "metis";
        _damid = ADJUSTL(""//_damid);
	_shot  = ADJUSTL(""//_shot);
        _command = "/f/mdsplus/dispatching/sendpost id="//_damid//" ok=TRUE shot="//_shot; /*//" >& /dev/null &";*/
        _cmd = "/usr/bin/ssh -q "//_machine//" \""//_command//"\"";
	write (*,_cmd);
        SPAWN(_cmd);

        /* Declare the phase to the event server 
        _cmd = "/f/mdsplus/dispatching/mdsplus_phase_event.sh "//_damid//" &";
        SPAWN(_cmd); */  

        return(1);
}

