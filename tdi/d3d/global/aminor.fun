/* aminor(_efit)  
   This function returns \aminor for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun aminor(in _efit) {

   _aminor = data(using_signal("\\aminor",_efit));
   return(_aminor);

}
