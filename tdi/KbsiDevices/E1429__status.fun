public fun E1429__status(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _cond = 0;
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(hpe1429_32->hpe1429_operCond_Q (VAL(_id), REF(_cond)), (DevLogErr(_nid, "Error in querying operation condition"); abort();));
    write(*,'CAL', text(_cond&1,2), ' Wait for ARM', text(_cond>>6&1,2), ' BUSY', text(_cond>>8&1,2),' READY', text(_cond>>9&1,2));
    TreeClose();
    return (1);
}