public fun l6810__trigger(as_is _nid, optional _method)
{
  _name = DevNodeRef(_nid,1);
  return(DevCamChk(_name,CamPiow(_name,0,25,_d=0,16),1,1));
}
