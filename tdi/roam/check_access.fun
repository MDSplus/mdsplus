public fun check_access(in _resource, in _permission, in _dn, optional out _aux)
{
  _aux_ptr=0;
  _status=Roam_gcc32->roam_check_access(getenv("ROAM_SERVER"),val(0),_resource,_permission,_dn,ref(_aux_ptr));
  _aux=cstring(_aux_ptr);
  return(_status == 0);
}
