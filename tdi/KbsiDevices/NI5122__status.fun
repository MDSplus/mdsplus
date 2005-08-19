public fun NI5122__status(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _stat = 0W;
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    if_error(ni5122->NiScope_acquisitionstatus (VAL(_id),REF(_stat)), (DevLogErr(_nid, "Error in reading Status"); abort();));
    if(_stat eq 0) write(*,"The acquisition is in progress");  
    else if(_stat eq 1) write(*,"The acquisition is complete"); 
    else if(_stat eq -1) write(*,"The acquisition is in an unknown state"); else write(*,"Niscope_Acquisitionstatus error");
/* _stat value is 0 or 1 or -1 */
    TreeClose();
    return (1);
}

