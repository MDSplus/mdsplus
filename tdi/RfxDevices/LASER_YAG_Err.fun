public fun LASER_YAG_Err( in _error_nr )
{
	if(_error_nr == 0 )
		return ( "Error: control sum error" );
	else if(_error_nr == 1)
		return ( "Error: error of the command format; the abovementioned structure of a command is violated");
	else if(_error_nr == 2)
		return ( "Error: unknown command - the command identifier not compatible with the abovementioned");
	else if(_error_nr == 3)	
		return ( "Error: parameter value exceeds that allowed for the power supply" );		
	else if(_error_nr == 4)
		return (  "Error: the command cannot be executed because of wrong sequence order of commans" );
	else if(_error_nr == 5)
		return (  "Error: the power supply is in internal sync mode - the switch LOCALE/REMOTO on the front panel is in the LOCAL position" );
	else if(_error_nr == 6)
		return (  "Error: a wrong number of device" );
	else if(_error_nr == 7)
		return (  "Error: the command can not be executed since the execution of the earlier command has not been completed" );
	else if(_error_nr == 8)
		return (  "Error: error on the answer format" );
	else if(_error_nr == 9)
		return (  "Error: RS232 communication error" );

	return (1);
}















