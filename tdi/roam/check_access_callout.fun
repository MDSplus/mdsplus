public fun check_access_callout(in _type,in _perm,in _name)
{
  _site_resource=getenv("ROAM_SITE_RESOURCE");
  _callout_resource=getenv("ROAM_"//_type//_RESOURCE");
  if (len(_site_resource) > 0)
  {
    if (check_access(_site_resource,"Access","nobody")) abort(1);
    if (check_access(_site_resource,"Noaccess",_name)) abort(2);
    if (!check_access(_site_resource,"Access","everyone") && !check_access(_site_resource,"Access",_name)) abort(3);
  }
  _aux="";
  if (len(_callout_resource) > 0)
  {
    if (check_access(_callout_resource,_perm,"nobody")) abort(4);
    if (check_access(_callout_resource,_perm,_name, _aux) && len(_aux) > 0) 
      return(_aux);
  }
  abort(5);
}
