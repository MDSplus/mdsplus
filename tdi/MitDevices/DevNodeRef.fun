public fun DevNodeRef(in _nid, in _offset)
{
  /* return node reference of device part */
  return(compile(
    getnci(getnci(_nid,"nid_number")-getnci(_nid,"conglomerate_elt")+_offset+1,'fullpath')));
}
