public fun T2Control__abort(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_IP = 2;
    private _N_FREQUENCY = 3;
    private _N_CALIBRATION = 4;
    private _N_INIT_CONTROL = 5;
    private _N_TRIG1_CONTROL = 6;
    private _N_TRIG2_CONTROL = 7;
    private _N_TRIG1_TIME = 8;
    private _N_DURATION = 9;
    private _N_TRIG2_TIME = 10;
    private _N_PAR1_NAME = 11;
    private _N_PAR1_VALUE = 12; 

write(*, 'T2Control abort');

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);
	_status = MdsValue('Feedback->stopFeedback()');
    MdsDisconnect();
    return (1);
}
