public fun treeSetSource(in _experiment, in _version)
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

    TreeShr->TreeClose( val(0), val(0) );

   _fileConf = "/usr/local/mdsplus/etc/experimentsVersionPath";
   if( getenv('activateTestRegion') == "TRUE" )
	_fileConf = "/usr/local/rfx/data_acquisition/scripts/rfxVersioning/testExperimentVersionPath";

   return( RfxUtils->setPulseVersion( _fileConf, _experiment,  _ver  ) );
}
