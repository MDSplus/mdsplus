public fun E1429__error(as_is _nid, optional _method)
{
   private _N_ID = 2;
   private _n=0;
   private _message= repeat(' ',128);

   _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
   for(_i=0; _i<20; _i++)
   {
      if_error(hpe1429_32->hpe1429_error_query (VAL(_id),REF(_n),REF(_message)), (DevLogErr(_nid, "Error in hpe1429_error_query()!");abort();));
      if(_n==0) break;
      if(_n==-1) {write(*,getnci(DevHead(_nid),'NODE_NAME'), ' migit not be opened!'); break;}
      write(*,getnci(DevHead(_nid),'NODE_NAME'), _n, _message);
   }
    TreeClose();
   return (1);
}
