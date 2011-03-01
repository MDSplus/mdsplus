public fun LASER_SS_Err( in _error_nr )
{
	if(_error_nr == '0' )
		write(*, "Error: control sum error" );
	else if(_error_nr == '1')
		write(*, "Error: error of the command format");
	else if(_error_nr == '2')
		write(*, "Error: unknown command");
	else if(_error_nr == '3')	
		write(*, "Error: parameter value exceeds that allowed for the power supply" );		
	else if(_error_nr == '4')
		write(*, "Error: wrong sequence order" );
	else if(_error_nr == '5')
		write(*, "Error: power supply is in internal sync mode " );
	else if(_error_nr == '6')
		write(*, "Error: wrong number of device" );
	else if(_error_nr == '7')
		write(*, "Error: earlier command has not completed" );

	return (1);
}















