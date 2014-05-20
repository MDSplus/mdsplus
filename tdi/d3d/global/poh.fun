/* poh(_efit)  
   This function returns \poh for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun poh(in _efit) {

   _poh = data(using_signal("\\poh",_efit)) / 1.0e6;
   return(_poh);

}
