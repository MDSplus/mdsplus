public fun a14__start(as_is _nid, optional _method)
{
  _name      = DevNodeRef(_nid,1);
  return(DevCamChk(_name,CamPiow(_name,0,25,_d=0,24),1,*));  /* Start */
}
