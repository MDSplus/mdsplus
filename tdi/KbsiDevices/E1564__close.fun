public fun E1564__close(as_is _nid, optional _method)
{
    private _N_ID = 2;

    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(hpe1564_32->hpe1564_close(VAL(_id)), (DevLogErr(_nid, "Error in closing"); abort();));

    TreeClose();
    return (1);
}