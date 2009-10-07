public fun treeGetSource(in _experiment)
{
   _source = _experiment//"_ver";
   return( getenv( _source ) );
}
