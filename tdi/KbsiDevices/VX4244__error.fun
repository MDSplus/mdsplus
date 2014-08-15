public fun VX4244__error(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _n=0;
    private _message= repeat(' ',128);
    
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    for(_i=0; _i<10; _i++)
    {
       if_error(Tkvx4244_32->tkvx4244_error_query (VAL(_id),REF(_n),REF(_message)), (DevLogErr(_nid, "Error in tkvx4244_error_query()!");abort();));
       if(_n==0) break;
       write(*,getnci(DevHead(_nid),'NODE_NAME'), _n, _message);
    }
    TreeClose();
    return (1);
}
