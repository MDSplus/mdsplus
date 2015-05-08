public fun E1564__error(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _n=0;
    private _message= repeat(' ',128);
    
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    for(_i=0; _i<10; _i++)
    {
       if_error(hpe1564_32->hpe1564_error_query (VAL(_id),REF(_n),REF(_message)), (DevLogErr(_nid, "Error in hpe1564_error_query()!");abort();));
       if(_n==0) break;
        write(*,getnci(DevHead(_nid),'NODE_NAME'), _n, _message);
    }
    TreeClose();
    return (1);
}
