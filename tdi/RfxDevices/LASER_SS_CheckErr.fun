public fun LASER_SS_Err(_in error_nr)
{
	switch(_error_nr])
		{
		case(0)
			write(*, "Execute command Error: control sum error" );
		break;
		case(1)
			write(*, "Execute command Error: error of the command format");
		break;
		case(2)
			write(*, "Execute command Error: unknown command");
		break;
		case(3)	
			write(*, "Execute command Error: parameter value exceeds that allowed for the power supply" );		
		break;
		case(4)
			write(*, "Execute command Error: wrong sequence order" );
		break;
		case(5)
			write(*, "Execute command Error: power supply is in internal sync mode " );
		break;
		case(6)
			write(*, "Execute command Error: wrong number of device" );
		break;
		case(7)
			write(*, "Execute command Error: earlier command has not completed" );
		break;

		}
	}

	return ();

}















