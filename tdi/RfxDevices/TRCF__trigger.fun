public fun TRCF__trigger(as_is _nid, optional _method)
{
    private _N_NAME = 1;
    _name = DevNodeRef(_nid, _N_NAME);
    DevCamChk(_name, CamPiow(_name, 2,25, _dummy=0, 16),1,1); 
}