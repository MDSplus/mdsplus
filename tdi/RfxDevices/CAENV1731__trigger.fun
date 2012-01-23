public fun CAENV1731__trigger(as_is _nid, optional _method)
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
/* OLD     _handle = if_error(_handle, public _handle = CAENVME_Init(_board_id)); */
    if(_board_id == 0)
    {
		_handle_0 = if_error(_handle_0, public _handle_0 = CAENVME_Init(_board_id));
		_handle = _handle_0;
    } else if(_board_id ==  1)
    {
		_handle_1 = if_error(_handle_1, public _handle_1 = CAENVME_Init(_board_id));
		_handle = _handle_1;
    } else if(_board_id ==  2)
    {
		_handle_2 = if_error(_handle_2, public _handle_2 = CAENVME_Init(_board_id));
		_handle = _handle_2;
    } else if(_board_id ==  3)
    {
		_handle_3 = if_error(_handle_3, public _handle_3 = CAENVME_Init(_board_id));
		_handle = _handle_3;
    } else if(_board_id ==  4)
    {
		_handle_4 = if_error(_handle_4, public _handle_4 = CAENVME_Init(_board_id));
		_handle = _handle_4;
    } else 
    {
    	DevLogErr(_nid, "Board ID not supported: "// _board_id);
 	abort();
    }

 
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

    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8108, 0L);
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error Generating Software Trigger');
 	abort();
    }
    return (1);
}
