/* rsurf(_efit)  
   This function returns \rsurf for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun rsurf(in _efit) {

   _rsurf = data(using_signal("\\rsurf",_efit));
   return(_rsurf);

}
