public fun check_access(in _resource, in _permission, in _dn, optional out _aux)
{
/*
  ROAM_SERVER environment variable can be set to either a server name such as roam.fusiongrid.org
              or an array of servers such as:
      export ROAM_SERVER="['roam.fusiongrid.org','cert.fusiongrid.org']"
*/
  _aux_ptr=0;
  _servers=if_error(execute(getenv('ROAM_SERVER')),getenv('ROAM_SERVER'));
  for(_status=-1,_idx=0;_status == -1 && _idx < size(_servers);_idx++)
  {
    _status=Roam->roam_check_access(_servers[_idx],val(1),_resource,_permission,_dn,ref(_aux_ptr));
  }
  _aux=cstring(_aux_ptr);
  return(_status == 0);
}
