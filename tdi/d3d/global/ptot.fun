/* ptot(in _efit)
   This function interpolates \ptot (efit01 timebase) onto 
   the timebase of the requested efit. The transport tree 
   must be open before calling this function.  
   20080730 SF
*/ 

public fun ptot(in _efit) {

   _ptot_data = data(build_path("\\ptot"));
   _ptot_time = data(dim_of(build_path("\\ptot")));
   _atime = data(using_signal("\\atime",_efit));

   _iptot = interpolate_ga( _atime[0], _ptot_time, _ptot_data ); 
   for(_i=1;_i<=size(_atime)-1;_i++) { 
      _iptot = [_iptot,interpolate_ga(_atime[_i],_ptot_time,_ptot_data)]; 
   }
   public __sig = make_signal(_iptot,*,_atime);   
   return(public __sig);

}
