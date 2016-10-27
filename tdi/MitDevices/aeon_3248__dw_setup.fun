public fun aeon_3248__dw_setup(as_is _nid, optional in _method)
{
  _head = DevHead(_nid);  
  _path=getnci(_head, "FULLPATH");
  _tree = getdbi("NAME");
  JavaMds->deviceSetup("AEON_3248",_tree, val($shot),_path,val(20),val(20)) ;
  return(1);
}
