public fun VXI8340__close(as_is _nid, optional _method)
{
    private _N_ID = 2;
/*    private _N_RSRC = 3;  */

    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
/*    _rsrc = if_error(data(DevNodeRef(_nid, _N_RSRC)), (DevLogErr(_nid, "Driver is not initialized!"); abort();)); */

    if_error(vxi8340->v8340_obclose (VAL(_id)), (DevLogErr(_nid, "Error in closing"); abort();));
    if_error(vxi8340->v8340_shclose (VAL(_rsrc)), (DevLogErr(_nid, "Error in closing"); abort();));  

    TreeClose();
    return (1);
}
