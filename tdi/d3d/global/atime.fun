/* atime(_efit)  
   This function returns \atime for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun atime(in _efit) {

   _atime = data(using_signal("\\atime",_efit));
   return(_atime);

}
