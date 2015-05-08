public fun E1564__arm(as_is _nid, optional _method)
{
    private _N_ID = 2;

    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(hpe1564_32->hpe1564_trigImm(VAL(_id)), (DevLogErr(_nid, "Error in software triggering"); abort();));

    TreeClose();
    return (1);
}