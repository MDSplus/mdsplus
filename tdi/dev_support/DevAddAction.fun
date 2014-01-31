public fun DevAddAction(in _actpath,in _phase,in _method,in _seq,in _server,in _devpath,out _nid)
{
  _devpath_full=getnci(_devpath,"fullpath");
  return(DevAddNode(_actpath,'ACTION',MAKE_ACTION(MAKE_DISPATCH(2,_server,_phase,_seq,*),
             execute("BUILD_METHOD(*,'"//_method//"',"//_devpath_full//")")),'/noshot_write',_nid));
}

