public fun MPBRecorder__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_START_EVENT = 3;
    private _N_REC_EVENTSE = 4;
    private _N_REC_TIMES = 5;

    _name =  if_error(DevNodeRef(_nid, _N_NAME), (DevLogErr(_nid, 'Cannot resolve CAMAC name');abort();));
    _w = 0;
    /*_status=DevCamChk(_name, CamPiow(_name, 0, 24,  _w, 16), 1,*);*/ 
    _status=CamPiow(_name, 0, 24,  _w, 16); 
    _w = 0;
    /*_status=DevCamChk(_name, CamPiow(_name, 0, 8, _w, 16), 1,*); */
    _status=CamPiow(_name, 0, 8, _w, 16);
    return (1);
}

