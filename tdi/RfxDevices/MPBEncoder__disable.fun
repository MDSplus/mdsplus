public fun MPBEncoder__disable(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    _name =  if_error(DevNodeRef(_nid, _N_NAME), (DevLogErr(_nid, 'Cannot resolve CAMAC name');abort();));
    _w = 63;
    _status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*);
    return(_status);
}









