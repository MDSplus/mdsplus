public fun check_access_jobmanager(in _name)
{
  _site_resource=getenv("ROAM_SITE_RESOURCE");
  _jobcontrol_resource("ROAM_JOBCONTROL_RESOURCE");

  if (check_access(_site_resource,"Access","nobody") ||
      check_access(_jobcontrol_resource,"Execute","nobody") ||
      check_access(_site_resource,"Noaccess",_name) ||
      check_access(_jobcontrol_resource,"Noaccess",_name) ||
      (!check_access(_site_resource,"Access","everyone") && !check_access(_site_resource,"Access",_name))) abort();

  _aux="";
  if (check_access(_jobcontrol_resource,"Execute",_name, _aux) && len(_aux) > 0) return(_aux);
  abort();
}
