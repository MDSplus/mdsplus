PUBLIC FUN LOADDATA(IN _actionpath, OPTIONAL IN _machine, OPTIONAL IN _command)
{
	if (NOT PRESENT(_machine)) _machine = "metis";
        if (NOT PRESENT(_command)) 
        {     
            _command = "/f/mdsplus/dispatching/loaddata "//$SHOT//" "//$EXPT//" "//_actionpath//" >&/dev/null &";
        }

        _cmd = "/usr/bin/ssh -q "//_machine//" "//_command;
        write (*,"ACTION: "//_actionpath); 
        write (*,"CMD: "//_cmd);
	SPAWN(_cmd);
	/* write (*,"DONE"); */
	return(1);

}	
