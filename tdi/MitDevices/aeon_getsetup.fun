public fun aeon_getsetup(in _name)
{
  if (getenv("DEBUG_DEVICES") != "")
      write (*, 'AEON_GETSETUP( _name='//_name//')');
  _status_reg = 0WU;
  DevCamChk(CamPiow(_name, 0, 0, _status_reg, 16),1,1);
  _gain_reg = 0WU;
  DevCamChk(CamPiow(_name, 0, 1, _gain_reg, 16),1,1);
  _addr = 0WU;
  DevCamChk(CamPiow(_name, 1, 1, _addr, 16),1,1);
  return([_status_reg, _gain_reg, _addr]);
}
