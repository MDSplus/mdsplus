public fun getVersions()
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
    _nid = getnci("\\VERSIONS:VME_DEQU", "NID_NUMBER");
    _statusDEQU0 = TreeShr->TreePutRecord(val(_nid),xd(_dequ_ver),val(0));
    _nid = getnci("\\DEQU_RAW::CONTROL:VERSION", "NID_NUMBER");
    _statusDEQU1 = TreeShr->TreePutRecord(val(_nid),xd(_dequ_ver),val(0));
	
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
    _nid = getnci("\\VERSIONS:VME_DFLU", "NID_NUMBER");
    _statusDFLU0 = TreeShr->TreePutRecord(val(_nid),xd(_dflu_ver),val(0));
    _nid = getnci("\\DFLU_RAW::CONTROL:VERSION", "NID_NUMBER");
    _statusDFLU1 = TreeShr->TreePutRecord(val(_nid),xd(_dflu_ver),val(0));

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
    _nid = getnci("\\VERSIONS:VME_EDA1", "NID_NUMBER");
    _statusEDA10 = TreeShr->TreePutRecord(val(_nid),xd(_eda1_ver),val(0));
    _nid = getnci("\\EDA1::CONTROL:VERSION", "NID_NUMBER");
    _statusEDA11 = TreeShr->TreePutRecord(val(_nid),xd(_eda1_ver),val(0));

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
    _nid = getnci("\\VERSIONS:VME_EDA3", "NID_NUMBER");
    _statusEDA30 = TreeShr->TreePutRecord(val(_nid),xd(_eda3_ver),val(0));
    _nid = getnci("\\EDA3::CONTROL:VERSION", "NID_NUMBER");
    _statusEDA31 = TreeShr->TreePutRecord(val(_nid),xd(_eda3_ver),val(0));
	
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
    _nid = getnci("\\VERSIONS:VME_MHD_AC", "NID_NUMBER");
    _statusMHD_AC0 = TreeShr->TreePutRecord(val(_nid),xd(_mhd_ac_ver),val(0));
    _nid = getnci("\\MHD_AC::CONTROL:VERSION", "NID_NUMBER");
    _statusMHD_AC1 = TreeShr->TreePutRecord(val(_nid),xd(_mhd_ac_ver),val(0));
	
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
    _nid = getnci("\\VERSIONS:VME_MHD_BC", "NID_NUMBER");
    _statusMHD_BC0 = TreeShr->TreePutRecord(val(_nid),xd(_mhd_bc_ver),val(0));
    _nid = getnci("\\MHD_BC::CONTROL:VERSION", "NID_NUMBER");
    _statusMHD_BC1 = TreeShr->TreePutRecord(val(_nid),xd(_mhd_bc_ver),val(0));
	
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
    _nid = getnci("\\VERSIONS:VME_MHD_BR", "NID_NUMBER");
    _statusMHD_BR0 = TreeShr->TreePutRecord(val(_nid),xd(_mhd_br_ver),val(0));
    _nid = getnci("\\MHD_BR::CONTROL:VERSION", "NID_NUMBER");
    _statusMHD_BR1 = TreeShr->TreePutRecord(val(_nid),xd(_mhd_br_ver),val(0));

/*
	
    write(*,"DEQU_VME ", _dequ_ver,_statusDEQU0,_statusDEQU1,getenv('rfx_path'));
    write(*,"DFLU_VME ", _dflu_ver,_statusDFLU0,_statusDFLU1);
    write(*,"EDA1_VME ", _eda1_ver, _statusEDA10,_statusEDA11);
    write(*,"EDA3_VME ", _eda3_ver, _statusEDA30,_statusEDA31);
    write(*,"MHD_AC_VME ", _mhd_ac_ver,_statusMHD_AC0,_statusMHD_AC1);
    write(*,"MHD_BC_VME ", _mhd_bc_ver, _statusMHD_AC0,_statusMHD_AC1);
    write(*,"MHD_BR_VME ", _mhd_br_ver, _statusMHD_BR0, _statusMHD_BR1);
*/  
    return (1);
}


    
