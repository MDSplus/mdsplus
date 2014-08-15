public fun VX4244__close(as_is _nid, optional _method)
{
    private _N_ID = 2;

    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(Tkvx4244_32->tkvx4244_close(VAL(_id)), (DevLogErr(_nid, "Error in closing"); abort();));
/*  PEL_UNLOAD  */
    TreeClose();
    return (1);
}
