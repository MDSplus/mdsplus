public fun E1463__arm(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_ADDRESS = 1;
    private _N_COMMENT = 2;
    private _N_TRIG_MODE = 3;
    private _N_TRIG_SOURCE = 4;
    private _N_SYNCH_MODE = 5;
    private _N_SYNCH_SOURCE = 6;
    private _N_FRAME_1 = 7;
    private _N_EXP_TIME = 8;
    private _N_N_SCANS = 9;
    private _N_HEAD_TEMP = 10;
    private _N_SCAN_MASK = 11;
    private _N_DATA = 12;
    private _N_BACK = 13;

write(*, 'Parte E1463__arm');

    _address = if_error(data(DevNodeRef(_nid, _N_ADDRESS)),(DevLogErr(_nid, "Missing GPIB Address"); abort();));
write(*, 'Address: ', _address);

    _command1 = 'MSK 3;';
    DevNodeCvt(_nid, _N_SYNCH_MODE, ['NORMAL', 'LINE', 'EXTERNAL'], ['NS', 'LS', 'XS'], _synch_mode = 'LS');
    _command1 = _command1 // _synch_mode;
    _n_scans = if_error(data(DevNodeRef(_nid, _N_N_SCANS)),(DevLogErr(_nid, "The number of scan lines is not defined"); abort();));
    _command1 = _command1 // ';J ' // trim(adjustl(_n_scans));
    DevNodeCvt(_nid, _N_FRAME_1, ['NORMAL', 'BACKGROUND'], ['2', '1'], _mem_def = '1');
    _command1 = _command1 // ';MEM ' // _mem_def;
    DevNodeCvt(_nid, _N_TRIG_MODE, ['EXTERNAL', 'SOFTWARE'], ['17', '7'], _da_def = '7');
    _command1 = _command1 // ';K 0;CRM;DA '// _da_def;
    _command2 = 'MSK 0; NCRUN';
    write(*, _command1);
    write(*, _command2);
    _id = GPIBGetId(_address);
    if(_id == 0)
    {
	DevLogErr(_nid, "Cannot initialize GPIB");
	abort();
    }
    if_error(GPIBWriteW(_id, _command1),(DevLogErr(_nid, "Error in GPIB Write"); abort();)); 
    if_error(GPIBWrite(_id, _command2),(DevLogErr(_nid, "Error in GPIB Write"); abort();)); 


    return (1);
}