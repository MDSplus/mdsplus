/* dens(_efit)  
   This function returns \densv2 or \densr0 for a given efit.  
   It requires an open mdsplus tree (ex. transport) and is used 
   by the transport global tdi routines.
   20080904 SF
*/

public fun dens(in _efit) {

   _densv2 = data(using_signal("\\densv2",_efit));
   if ( GT(COUNT(_densv2 < 0),0) ) { 
      _dens = data(using_signal("\\densr0",_efit)); 
   } else { _dens = _densv2; }      
   return(_dens);

}
