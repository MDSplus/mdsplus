public fun VMEWaves__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 164;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_IP = 2;
    private _N_BASE_FREQ = 3;
    private _K_NODES_PER_CHANNEL = 5;
    private _N_CHANNEL_0 = 4;
    private _N_CHAN_TRIGGER_TIME = 1;
    private _N_CHAN_FREQUENCY = 2;
    private _N_CHAN_WAVE_X = 3;
    private _N_CHAN_WAVE_Y = 4; 

/*write(*, 'Parte VMEWaves__init');
*/
    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);
    for(_chan = 0; _chan < 32; _chan++)
    {
  	if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL))))
	{
/*write(*, 'is on');
*/	    _trigger = data(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_TIME));
/*write(*, float(_trigger));
*/	    _frequency = data(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL) + _N_CHAN_FREQUENCY));
/*write(*, _frequency);
*/	    _x = data(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL) + _N_CHAN_WAVE_X));
/*write(*, _x);
*/	    _y = data(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL) + _N_CHAN_WAVE_Y));
/*write(*, _y);
*/	    _samples = size(_x);
/*write(*, _samples);
*/	    if(_samples > size(_y))
		_samples = size(_y);
	    _status = MdsValue('waveform->initWave($1, $2, $3, $4, $5, $6)', _chan, float(_trigger), float(_frequency), _samples, float(_x), float(_y));
		if(_status == 0)
		{
			DevLogErr(_nid, "Inconsistent waveform definition. See vxWorks console for details");
			abort();
		}
   	}
    }
    MdsDisconnect();
    return (1);
}