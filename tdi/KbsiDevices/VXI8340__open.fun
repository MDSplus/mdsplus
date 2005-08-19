public fun VXI8340__open(as_is _nid, optional _method)
{
    private _N_ADDRESS = 1;
    private _N_ID = 2;

    _address = if_error(data(DevNodeRef(_nid, _N_ADDRESS)),(DevLogErr(_nid, "Missing VXI Address"); abort();));
    _id = 0ul;
    _rsrc = 0u;
    _status = 0ul;

    if_error(vxi8340->v8340_opendefault (REF(_rsrc)),(DevLogErr(_nid, "Error in VIOpenDefaultRM"); abort();));
    _stat=if_error(vxi8340->v8340_open (VAL(_rsrc), _address, VAL(0), VAL(0), REF(_id)),(DevLogErr(_nid, "Error in Initialization"); abort();));
    TreeShr->TreePutRecord(VAL(DevHead(_nid)+_N_ID), XD(_id), VAL(0));
    TreeClose();
    return (1);
}
