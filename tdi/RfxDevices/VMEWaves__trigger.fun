public fun VMEWaves__trigger(as_is _nid, optional _method)
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

/*write(*, 'Parte VMEWaves__trigger');
*/
    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);
    for(_chan = 0; _chan < 32; _chan++)
    {
  		if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL))))
		{
			MdsValue('waveform->trigger($1)', _chan);
		}
    }
    MdsDisconnect();
    return (1);
}
