public fun VXI8340__setup(as_is _nid, optional _method)
{
   private _N_ID = 2;
   private _N_SOURCE = 5;
   private _N_DESTINATION = 6;
   
   private _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not opened!"); abort();));
   private _source = if_error(data(DevNodeRef(_nid, _N_SOURCE)),(DevLogErr(_nid, "Missing # of trigger source"); abort();));
   private _desti = if_error(data(DevNodeRef(_nid, _N_DESTINATION)),(DevLogErr(_nid, "Missing # of trigger destination"); abort();));

/* soft trigger setting      if_error(vxi8340->v8340_sserttrigger (VAL(_id), VAL(_proto)),(DevLogErr(_nid, "Error in Assert Trigger Protocal"); abort();));  */
   if_error(vxi8340->v8340_maptrigger (VAL(_id), VAL(_source), VAL(_desti), VAL(0)),(DevLogErr(_nid, "Error in Input setup"); abort();));

    TreeClose();
   return (1);
}
