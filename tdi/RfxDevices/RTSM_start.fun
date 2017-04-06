public fun RTSM_start()
{
	wait(5);
	tcl('do/method \\ISEPS::RTSM pre_req');
	wait(5);
	tcl('do/method \\ISEPS::RTSM pulse_req');
	wait(5);
	return (1);
}

