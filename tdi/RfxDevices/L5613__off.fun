public fun L5613__off(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_MODEL = 3;
    private _N_CHAN_A_INPUT = 5;
    private _N_CHAN_A_OUTPUT = 6;
    private _N_CHAN_A_RANGE = 7;
    private _N_CHAN_B_INPUT = 9;
    private _N_CHAN_B_OUTPUT = 10;
    private _N_CHAN_B_RANGE = 11;

    _name = DevNodeRef(_nid, _N_NAME);
    _w = 0;
    DevCamChk(_name, CamPiow(_name, 0, 24, _w, 16),1,*); 
    DevCamChk(_name, CamPiow(_name, 1, 24, _w, 16),1,*); 

    return (1);
}


