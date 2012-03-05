public fun A12__INIT(as_is _nid, optional _method)
{
  _name = DevNodeRef(_nid,1);
  _status = DevCamChk(_name,CamPiow(_name,0,9,_d=0,16),1,1);
  _status = DevCamChk(_name,CamPiow(_name,0,9,_d=0,16),1,1);
  _ext_clock = DevNodeRef(_nid,3); 
  _ext = getnci(_ext_clock, 'length') == 0;
  _status = DevCamChk(_name,CamPiow(_name,0,17,_ext,16),1,1);
  _status = DevCamChk(_name,CamPiow(_name,0,26,_d=0,16),1,1);
  return(1);
}

