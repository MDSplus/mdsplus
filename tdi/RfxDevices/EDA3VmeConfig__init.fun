public fun EDA3VmeConfig__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_DELTA_T = 1;
    private _N_THETA_NOM = 2;
    private _N_TAU_VTHETA = 3;
    private _N_TAU_VTOR = 4;
    private _N_TAU_D = 5;
    private _N_KP= 6;
    private _N_KI = 7;
    private _N_KD = 8;
    private _N_VCOMP_ON = 9;
    private _N_ICOMP_ON = 10;
    private _N_IFB_ON = 11;
    private _N_KVCOMP = 12;
    private _N_IP_TRIG = 13;
    private _N_T_TRIG = 14;
    private _N_DELTAVTORTR = 15;
    private _N_DELTATBT1  = 16;
    private _N_DELTATBT2 = 17;
    private _N_TUNING1 = 18;
    private _N_VME_IP = 19;

    private _ERROR = 1E38;

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    MdsDisconnect();
    _status = 0;
    _cmd = '_status = MdsConnect("'//_vme_ip//'")';
    execute(_cmd);

    if(_status == 0)
    {
	DevLogErr(_nid, 'Cannot connect to VME');
	abort();
    }

    _vals = [];
  
    for(_i = 0; _i < 18; _i++)
    {
        _curr_val = if_error(data(DevNodeRef(_nid, _N_DELTA_T+_i)), _ERROR);
        if(_curr_val == _ERROR)
        {
	      DevLogErr(_nid, 'Invalid value for parameter  '// _i);
		abort();
	  }
   	  _vals = [_vals, _curr_val];
    }
    write(*, _vals);
    _status = MdsValue('variables->setFloatArray($1, $2, $3)', 'fControlParamsPtr', float(_vals), size(_vals));
    if(_status == *)
    {
	  DevLogErr(_nid, 'Cannot write parameters to VME');
	  abort();
    }


    MdsDisconnect();
    return (1);
}
