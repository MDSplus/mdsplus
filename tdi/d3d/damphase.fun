/* Those who were responsible for the previous comment have been sacked. */
/* Flanagan wrote this, so there are no comments.                        */
/* Somebody edited this later, so now there are comments...
   This TDI fun is used to tell DAM about MDSplus phases.  All it does
   is do a sendpost with info about the phase that was just dispatched.
   It should be used in the action nodes in the PHASES branch of the D3D
   tree. -jrb 20030313
*/

PUBLIC FUN DAMPHASE(IN _damid, IN _shot) {
        _machine = "delphi";
        _damid = ADJUSTL(""//_damid);
        _command = "/f/mdsplus/dispatching/sendpost id="//_damid//" ok=TRUE shot="//_shot//" >& /dev/null &";
        write (*,_command);
        _cmd = "/usr/bin/rsh -n "//_machine//" \""//_command//"\"";
        SPAWN(_cmd);

        /* Changing over to id driven posting.  Web page is being done through DAM. _PHASE no longer passed. */ 
        /* write this info to a file on the web server for Qian's web page stuff 
        _cmd2 = "/usr/local/bin/mds-postweb.sh " // _phase // " " // _shot // " >& /dev/null &";
        SPAWN(_cmd2); */

        return(1);
}

