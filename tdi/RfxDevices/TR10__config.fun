public fun TR10__config(as_is _nid, optional _method)
{
    private _INVALID = 10E20;

    write(*, 'TR10 Config');

    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
        DevLogErr(_nid, "Invalid Board ID specification");
                abort();
    }
  else
	{
		_status = TR10HWConfig(_board_id);
	}

   write(*, "TR10 Configured = ", _status);
}
