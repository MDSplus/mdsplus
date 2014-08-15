public fun VX4244__status(as_is _nid, optional _method)
{
   private _N_ID = 2;
   private _stat = zero(8,0BU);
   _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
   if_error(Tkvx4244_32->tkvx4244_readStatus(VAL(_id),VAL(1),REF(_stat)), (DevLogErr(_nid, "Error in reading status"); abort();));

/* The following expression is simple but causes some problem on Windows 98 */
/*    _n = compile(_stat); */
/* So, the following algorithm translates a string to a number */
   _n = 0;
   _m = 1;
   for (_i=2;_i>=0;_i--)
   {
      if (_stat[_i] > 47)
      {
         _n += (_stat[_i]-48) * _m;
         _m *= 10;
      }
   }
   write(*,'Measure complete(',text(btest(_n,0),1),text(btest(_n,1),2),text(btest(_n,2),2),text(btest(_n,3),2),
	') Event Status',text(btest(_n,5),2),' Service Request Pending',text(btest(_n,6),2));
    TreeClose();
   return (1);
}
