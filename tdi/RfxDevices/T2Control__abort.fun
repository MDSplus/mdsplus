public fun T2Control__abort(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_IP = 2;

rite(*, 'T2Control abort');

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);
	_status = MdsValue('Feedback->stopFeedback()');
    MdsDisconnect();
    return (1);
}
