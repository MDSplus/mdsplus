public fun check_access_file(in _name)
{
   _site_resource=getenv("ROAM_SITE_RESOURCE");
   _gridftp_resource=getenv("ROAM_GRIDFTP_RESOURCE");

  if (check_access(_site_resource,"Access","nobody") ||
      check_access(_gridftp_resource,"Access","nobody") ||
      check_access(_site_resource,"Noaccess",_name) ||
      check_access(_gridftp_resource,"Noaccess",_name) ||
      (!check_access(_site_resource,"Access","everyone") && !check_access(_site_resource,"Access",_name))) abort();

  _aux="";
  if (check_access(_gridftp_resource,"Access",_name, _aux) && len(_aux) > 0) return(_aux);
  _aux=getenv("ROAM_GRIDFTP_GUEST_ACCOUNT");
  if (len(_aux) > 0) return(_aux);
  abort();
}
