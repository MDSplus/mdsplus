public fun VX4244__arm(as_is _nid, optional _method)
{
    private _N_ID = 2;

    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(Tkvx4244_32->tkvx4244_write(VAL(_id),"VXI:TRIG\n"), (DevLogErr(_nid, "Error in software triggering"); abort();));
/*  PEL_ARM  */     
    TreeClose();
    return (1);
}
