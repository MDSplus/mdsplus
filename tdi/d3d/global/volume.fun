/* volume(_efit)  
   This function returns \volume for a given efit.  It requires
   an open mdsplus tree (ex. transport) and is used by the 
   transport global tdi routines.
   20080904 SF
*/

public fun volume(in _efit) {

   _volume = data(using_signal("\\volume",_efit));
   if ( lt(_volume[0],0.0) ) {_temp=0.;} else {_temp=_volume[0];}
   for (_i=1;_i<SIZE(_volume);_i++) {
      if ( lt(_volume[_i],0.0) ) {_temp = [_temp,0.];} else {_temp=[_temp,_volume[_i]];}
   }
   _volume = _temp; _temp = 0.;
   return(_volume);

}
