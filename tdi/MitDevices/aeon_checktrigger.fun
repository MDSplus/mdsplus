public fun aeon_checktrigger(in _name)
{
  if (getenv("DEBUG_DEVICES") != "") {
      write (*, 'AEON_CHECKTRIGGER( _name='//_name//')');
  }
  CamPiow(_name, 0, 8, _d=0, 16);
  return (CamXandQ());
}
