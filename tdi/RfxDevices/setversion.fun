public fun setVersion(in _experiment, in _version)
{

   if(_version == "active")
     _ver = -1; 
   else 
   if(_version == "InProgress")
     _ver = -2;
   else
     _ver = _version;
     
   return( RfxUtils->setPulseVersion("/usr/local/mdsplus/etc/experimentsVersionPath", experiment, _ver) );
     
}
