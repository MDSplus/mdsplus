public fun aeon_trigger(in _name)
{
  if (getenv("DEBUG_DEVICES") != "")
      write (*, 'AEON_TRIGGER( _name='//_name//')');
  CamPiow(_name, 0, 25, _offset_reg, 16);
  return(1);
}
