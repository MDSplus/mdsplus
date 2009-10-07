public fun setVersion(in _experiment, in _version)
{

   if( _version == "update" )
     _ver = -1; 
   else 
   if( _version == "inProgress" )
     _ver = -2; 
   else 
   if( _version == "active" )
     _ver = -3;
   else
     _ver = _version;
     
   return( RfxUtils->setPulseVersion("/usr/local/mdsplus/etc/experimentsVersionPath", _experiment,  _ver  ) );
     
}
