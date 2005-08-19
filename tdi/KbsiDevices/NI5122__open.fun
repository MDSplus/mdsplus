public fun NI5122__open(as_is _nid, optional _method)
{
    private _N_ADDRESS = 1;
    private _N_ID = 2;

    _address = if_error(data(DevNodeRef(_nid, _N_ADDRESS)),(DevLogErr(_nid, "Missing PXI Address"); abort();));
    _id = 0u;
    if_error(ni5122->NiScope_init(_address,REF(_id)),(DevLogErr(_nid, "Error in Initialization"); abort();));
    TreeShr->TreePutRecord(VAL(DevHead(_nid)+_N_ID), XD(_id), VAL(0));
    TreeClose();
    return (1);
}
