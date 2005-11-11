PUBLIC FUN LOADDATA(IN _actionpath, OPTIONAL IN _machine, OPTIONAL IN _command)
{
	if (NOT PRESENT(_machine)) _machine = "nitron";
        if (NOT PRESENT(_command)) 
        {     
            _command = "/f/mdsplus/dispatching/loaddata "//$SHOT//" "//$EXPT//" "//_actionpath//" >&/dev/null &";
        }

        _cmd = "/usr/bin/rsh -n "//_machine//" "//_command;
        /* write (*,"ACTION: "//_actionpath); */
	SPAWN(_cmd);
	/* write (*,"DONE"); */
	return(1);

}	
