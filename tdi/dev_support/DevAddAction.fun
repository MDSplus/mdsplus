public fun DevAddAction(in _actpath,in _phase,in _method,in _seq,in _server,in _devpath,out _nid)
{
  return(DevAddNode(_actpath,'ACTION',MAKE_ACTION(MAKE_DISPATCH(2,_server,_phase,_seq,*),
             MAKE_METHOD(*,_method,BUILD_PATH(_devpath))),'/noshot_write',_nid));
}