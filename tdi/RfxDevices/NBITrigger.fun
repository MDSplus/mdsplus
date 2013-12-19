public fun NBITrigger()
{
	_phase = "";

    _status = mdsconnect("150.178.34.151:8100");
	if( _status != 0 )
	{
		_handle = mdsvalue('OPCConnect("OPC.SimaticNET", 500)');
		
		if( _handle > 0)
		{
			_phase = mdsvalue('OPCGet('//_handle//', "\\\\S7:\\\\Supervisore\\\\DB5.Task[0].StateName")');
			if( mdsvalue( "OPCerror("//_handle//")") )
			{
			    _msg = mdsvalue( "OPCErrorMessager("//_handle//")");
				write(*, "Impossibile rilevare la fase corrente dell'impulso : "//_msg);
			}
		    else
		    {
				_phase = extract(1, 3, _phase); 
			} 			
		}
		else
		{
			write(*, " Impossibile rilevare la fase corrente dell'impulso : "//getmsg(_status));
			return (1);
		}

		mdsvalue('OPCDisconnect('//_handle//')');
			
		MdsDisconnect();
		
		if( _phase == "")
		    return (1);
	}
	else
	{
		write(*, " Impossibile rilevare la fase corrente dell'impulso : "//getmsg(_status));
		return (1);
	}

    if( _phase != "PRE" && _phase != "INI" && _phase != "PON"  )
	{
	    write(*, "Operazione consentita nella fase "// _phase //" dell'impulso");
		
		tcl("set tree dsfm/shot=1");

		_nodename = "\\DSFM_RAW::DSFM_DEC:SYNCHRONIZE";
		_data = "DISABLED";
		TreeShr->TreePutRecord( val(getnci(_nodename,'nid_number')),xd(_data),val(0) );
		
		_nodename = "\\DSFM_RAW::TRIG_NBI:TRIGGER_MODE";
		_data = "SOFTWARE";
		TreeShr->TreePutRecord( val(getnci(_nodename,'nid_number')),xd(_data),val(0) );
		
		tcl("do/method \\DSFM_RAW::TRIG_NBI init");
		tcl("do/method \\DSFM_RAW::DSFM_DEC init");
		
		tcl("close");
	    write(*, "Operazione eseguita");
		
	}
	else
	{
	    write(*, "Operazione NON consentita nella fase "// _phase //" dell'impulso");
	}
	
	return (0);

}
