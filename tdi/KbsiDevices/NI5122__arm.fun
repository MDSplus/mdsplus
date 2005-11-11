public fun NI5122__arm(as_is _nid, optional _method)
{
    private _N_ID = 2;

    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(ni5122->NiScope_configuretriggerimmediate(VAL(_id)), (DevLogErr(_nid, "Error in Immediate triggering"); abort();));
    TreeClose();
    return (1);
}
