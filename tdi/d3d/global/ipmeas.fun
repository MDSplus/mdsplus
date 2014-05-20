/* ipmeas(_efit)  
   This function returns \ipmeas for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun ipmeas(in _efit) {

   _ipmeas = data(using_signal("\\ipmeas",_efit)) / 1.0e6;
   return(_ipmeas);

}
