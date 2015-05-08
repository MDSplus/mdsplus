public fun VX4244__error(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _n=1;
    private _message= "                                                                                                                      ";
    
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(Tkvx4244_32->tkvx4244_error_query (VAL(_id),REF(_n),REF(_message)), (DevLogErr(_nid, "Error in software triggering"); abort();));
    while(_n<>0)
    {
       write(*,getnci(DevHead(_nid),'NODE_NAME'), _n, _message);
       if_error(Tkvx4244_32->tkvx4244_error_query (VAL(_id),REF(_n),REF(_message)), (DevLogErr(_nid, "Error in software triggering"); abort();));
    }
    return (1);
}
