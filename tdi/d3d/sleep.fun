PUBLIC FUN SLEEP(IN _seconds)
{
        write(*,"About to spawn");
	_cmd = "/usr/bin/echo "//$SHOT;
	write(*,"Spawn status = ",SPAWN(_cmd));
	_cmd = "/usr/bin/sleep "//_seconds;
	SPAWN(_cmd);
	return(1);

}	
