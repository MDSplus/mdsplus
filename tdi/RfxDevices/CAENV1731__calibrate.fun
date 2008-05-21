public fun CAENV1731__calibrate(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 61;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_ADDRESS = 2;
    private _N_BOARD_ID = 19;
    private _INVALID = 10E20;

    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid Board ID specification");
 	abort();
    }

/* Initialize Library if the first time */
    _handle = if_error(_handle, public _handle = CAENVME_Init(_board_id));

 
   if(_handle == -1)
    {
    	DevLogErr(_nid, "Cannot Initialize VME Interface");
 	abort();
    }

    _vme_address=if_error(data(DevNodeRef(_nid, _N_VME_ADDRESS)), _INVALID);
    if(_vme_address == _INVALID)
    {
    	DevLogErr(_nid, "Invalid VME Address");
 	abort();
    }
/* Software Reset */
    _status = CAENVME_WriteCycle(_handle, long(_vme_address + 0x0EF24), long(0));
    if(_status != 0)
    {
    	DevLogErr(_nid, "Error resetting CAEN device");
 	abort();
    }

    for(_i = 0; _i < 8; _i++)
    {
   	_status = CAENVME_WriteCycle(_handle,  _vme_address + 0x0109C + (_i + 1)  * 0x100, long(2)); 
    	if(_status != 0)
    	{
    		DevLogErr(_nid, 'Error Calibrating Channel '// _i+1);
 		abort();
	}
    	_status = CAENVME_WriteCycle(_handle, _vme_address + 0x0109C + (_i + 1)  * 0x100, 0L);
    	if(_status != 0)
    	{
    		DevLogErr(_nid, 'Error Calibrating Channel ' // _i+1);
 		abort();
	}
	_status = CAENVME_ReadCycle(_handle, _vme_address + 0x0109C + (_i + 1) * 0x100, _cal = 0L);
    	if(_status != 0)
    	{
    		DevLogErr(_nid, 'Error Calibrating Channel ' // _i+1);
 		abort();
	}
	while((_cal & 2L) != 0)
 		_status = CAENVME_ReadCycle(_handle, _vme_address + 0x109C + _i * 0x100, _cal = 0L);
    }
    return (1);
}
