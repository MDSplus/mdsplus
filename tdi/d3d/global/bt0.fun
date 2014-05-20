/* bt0(_efit)  
   This function returns \bt0 for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun bt0(in _efit) {

   _bt0 = data(using_signal("\\bt0",_efit));
   return(_bt0);

}
