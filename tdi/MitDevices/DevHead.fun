public fun DevHead(in _nid)
{
  /*  Return head nid of device */
  return(getnci(_nid,"nid_number") - getnci(_nid,"conglomerate_elt") + 1);
}
