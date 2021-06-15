public fun getSpiderShotTime(optional in _shot, optional in _unixTime)
{

/*
TAI, Temps Atomique International, is the international atomic time scale based on a continuous counting of the SI second. TAI is currently ahead of UTC by 37 seconds. TAI is always ahead of GPS by 19 seconds.

POSIX_TIME_AT_EPICS_EPOCH 631152000u

https://www.ietf.org/timezones/data/leap-seconds.list

lapse time 3754944000 37

*/

   if(present(_shot))
       treeopen("spider", _shot);

  _tai_ns = if_error(data(build_path("\\TIMING_ABS_TMSP.PULSE_TIME:TAI_NS")), -1);


  if( size( _tai_ns ) == 1 )
  {
       if( _tai_ns == -1 )
       { 
           _tai_ns  = if_error(data(build_path("\\BON_ABS_TMSP.PULSE_TIME:TAI_NS")),[(37Q * 1000Q + 631152000000Q ) * 1000000]);
       } 
       _tai_ns  = _tai_ns[0];
  }  
  else {
       _tai_ns  = _tai_ns[0];
  }

  if(present(_shot))
       treeclose();

  
   if(present(_unixTime))
      _time = quadword ( ( _tai_ns  / 1000000Q - 37Q * 1000Q                 ) * 1000000 ); /*Unix time*/
   else
      _time = quadword ( ( _tai_ns  / 1000000Q - 37Q * 1000Q - 631152000000Q ) * 1000000 ); /*Epics time*/

   return( _time  );
} 

