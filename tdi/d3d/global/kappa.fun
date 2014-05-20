/* kappa(_efit)  
   This function returns \kappa for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun kappa(in _efit) {

   _kappa = data(using_signal("\\kappa",_efit));
   return(_kappa);

}
