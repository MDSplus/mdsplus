public fun MPBEncoder__trigger(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    _name =  if_error(DevNodeRef(_nid, _N_NAME), (DevLogErr(_nid, 'Cannot resolve CAMAC name');abort();));
    _w = 0;
    _status=DevCamChk(_name, CamPiow(_name, 0, 25, _w, 16), 1,*);
    return(_status);
}









