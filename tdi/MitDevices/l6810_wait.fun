public fun l6810_wait(in _name) {
  _status = DevCamChk(_name,CamPiow(_name,0,11,_d=0,16),1,*);
  for (_tries=0; _tries<100 && not CamXandQ(); _tries++) {
    _status = DevCamChk(_name, CamPiow(_name, 0, 11,_d=0, 16), 1, *);
  }
} 
