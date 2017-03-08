public fun aeon_arm(in _name, in _offset_reg, in _status_reg)
{
  if (getenv("DEBUG_DEVICES") != "")
      write (*, 'AEON_ARM( _name='//_name//', _offset_reg='//_offset_reg//', _status_reg='//_status_reg//')');
  DevCamChk(CamPiow(_name, 0, 17, _offset_reg, 16),1,1);
  DevCamChk(CamPiow(_name, 0, 16, _status_reg, 16),1,1);
  DevCamChk(CamPiow(_name, 0, 26, _d=0, 16),1,1);
  DevCamChk(CamPiow(_name, 1, 26, _d=0, 16),1,1);
  return(1);
}
