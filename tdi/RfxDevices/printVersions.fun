public fun printVersions()
{
    _status = MdsConnect("150.178.34.45");
    if(_status == 0)
    {
    	write(*, "Cannot connect to DEQU");
		_dequ_ver = "Unknown";
    }
    else
    {
    	_dequ_ver = MdsValue("support->getDequVersion:DSC()");
		MdsDisconnect();
	}
	write(*, "DEQU version : ", _dequ_ver);
	
    _status = MdsConnect("150.178.34.46");
    if(_status == 0)
    {
    	write(*, "Cannot connect to DFLU");
		_dflu_ver = "Unknown";
    }
    else
    {
    	_dflu_ver = MdsValue("support->getDfluVersion:DSC()");
		MdsDisconnect();
    }
	write(*, "DFLU version : ", _dflu_ver);

	_status = MdsConnect("150.178.34.30");
    if(_status == 0)
    {
    	write(*, "Cannot connect to EDA1");
		_eda1_ver = "Unknown";
 	}
    else
    {
    	_eda1_ver = MdsValue("support->getEda1Version:DSC()");
		MdsDisconnect();
    }
	write(*, "EDA1 version : ", _eda1_ver);

	_status = MdsConnect("150.178.34.31");
    if(_status == 0)
    {
    	write(*, "Cannot connect to EDA3");
		_eda3_ver = "Unknown";
    }
    else
    {
    	_eda3_ver = MdsValue("support->getEda3Version:DSC()");
		MdsDisconnect();
    }
	write(*, "EDA3 version : ", _eda3_ver);
	
    _status = MdsConnect("150.178.34.29");
    if(_status == 0)
    {
    	write(*, "Cannot connect to MHD_AC");
		_mhd_ac_ver = "Unknown";
	}
    else
    {
    	_mhd_ac_ver = MdsValue("support->getMhdAcVersion:DSC()");
		MdsDisconnect();
    }
	write(*, "MHD_AC version : ", _mhd_ac_ver);
	
    _status = MdsConnect("150.178.34.28");
    if(_status == 0)
    {
    	write(*, "Cannot connect to MHD_BC");
		_mhd_bc_ver = "Unknown";
    }
    else
    {
    	_mhd_bc_ver = MdsValue("support->getMhdBcVersion:DSC()");
		MdsDisconnect();
    }
	write(*, "MHD_BC version : ", _mhd_bc_ver);
	
	_status = MdsConnect("150.178.34.27");
    if(_status == 0)
    {
    	write(*, "Cannot connect to MHD_BR");
		_mhd_br_ver = "Unknown";
    }
    else
    {
    	_mhd_br_ver = MdsValue("support->getMhdBrVersion:DSC()");
		MdsDisconnect();
    }
	write(*, "MHD_BR version : ", _mhd_br_ver);


	_status = MdsConnect("150.178.34.15");
    if(_status == 0)
    {
    	write(*, "Cannot connect to MHD_BR_I");
		_mhd_br_i_ver = "Unknown";
    }
    else
    {
    	_mhd_br_i_ver = MdsValue("support->getMhdBrVersion:DSC()");
		MdsDisconnect();
    }
	write(*, "MHD_BR_I version : ", _mhd_br_i_ver);


    return (1);
}


    
