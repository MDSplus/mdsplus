public fun E1564__status(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _stat = 0W;
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(hpe1564_32->hpe1564_status_Q(VAL(_id),REF(_stat)), (DevLogErr(_nid, "Error in reading Status"); abort();));
    _limit = 'Limit exceeded('//text(_stat&1,1);
    for (_i=0;_i<3;_i++) _limit = _limit//text((_stat>>=1)&1,2);
    write(*,_limit,') Overload',text((_stat>>=1)&1,2),' Pretrig. read completed',text((_stat>>=1)&1,2),
	' measure completed',text((_stat>>=1)&1,2),' Trig. received',text((_stat>>=1)&1,2));
    TreeClose();
    return (1);
}