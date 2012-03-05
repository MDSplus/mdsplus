public fun a14__init(as_is _nid, optional _method)
{
  _name         = DevNodeRef(_nid,1);
  _clock_divide = DevNodeRef(_nid,3);
  _ext_clock    = DevNodeRef(_nid,4);
  _pts          = DevNodeRef(_nid,6);
  _mode         = DevNodeRef(_nid,7);
  _clk_polarity = DevNodeRef(_nid,9);
  _str_polarity = DevNodeRef(_nid,10);
  _stp_polarity = DevNodeRef(_nid,11);
  _gated        = DevNodeRef(_nid,12);
  _clock_speed  = data(build_signal([0,1,2,3,4,5,6],*,[1,2,4,10,20,40,100])[_clock_divide]);
  _is_ext_clock = getnci(_ext_clock,"LENGTH") > 0;
  _sr = _clock_speed | 
        (_is_ext_clock << 3) | 
        ((_mode & 7) << 8) | 
        ((_gated != 0) << 11) | 
        ((_stp_polarity != 0) << 12) |
        ((_str_polarity != 0) << 13) |
        ((_clk_polarity != 0) << 14);
  DevCamChk(_name,CamPiow(_name,1,9,_d=0,24),1,*);  /* Reset - CMD mode */
  DevCamChk(_name,CamPiow(_name,4,17,_sr,24),1,*);  /* Load status register */
  DevCamChk(_name,CamPiow(_name,4,1,_sr,24),1,*);   /* Read status register */
  _ra_size = (_sr >> 15) & 3;
  _pts_set = (_ra_size == 0) ? (_pts + 1023) / 1024 : (_pts + 2047)/2048;
  DevCamChk(_name,CamPiow(_name,3,17,_pts,24),1,*); /* Load PTS register */
  DevCamChk(_name,CamPiow(_name,2,17,_d=0,24),1,*);    /* Zero memory pointer */
  DevCamChk(_name,CamPiow(_name,0,26,_d=0,24),1,*); /* Enable LAM */
  DevCamChk(_name,CamPiow(_name,2,9,_d=0,24),1,*);  /* Put in DAQ mode */
  _sr_mode = (_sr >> 8) & 7;
  if (_sr_mode == 1)
    DevCamChk(_name,CamPiow(_name,0,25,_d=0,24),1,*);  /* Start */
  return(1);
}
