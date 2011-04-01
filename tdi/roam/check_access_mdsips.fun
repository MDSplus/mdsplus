public fun check_access_mdsips(in _remote_username, in _match1, in _match2) {
  _name=extract(0,index(_match1,"@"),_match1);
  _site_resource=getenv("ROAM_SITE_RESOURCE");
  _data_resource=getenv("ROAM_MDSIP_RESOURCE");

  if (check_access(_site_resource,"Access","nobody") ||
      check_access(_data_resource,"Read","nobody")   ||
      check_access(_site_resource,"Noaccess",_name)  ||
      check_access(_data_resource,"Noaccess",_name)  ||
      (!check_access(_site_resource,"Access","everyone") && !check_access(_site_resource,"Access",_name)) ||
      (!check_access(_data_resource,"Read","everyone") && !check_access(_site_resource,"Read",_name))) abort();

  _aux="";
  if (check_access(_data_resource,"Write",_name, _aux) && len(_aux) > 0) return(_aux);
  _aux=getenv("ROAM_MDSIP_GUEST_ACCOUNT");
  if (len(_aux) > 0) return(_aux);
  abort();
}
