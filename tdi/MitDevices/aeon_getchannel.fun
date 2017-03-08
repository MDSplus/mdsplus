public fun aeon_getchannel(in _name, in _addr, in _chan, in _points)
{
  if (getenv("DEBUG_DEVICES") != "")
      write (*, 'AEON_GETCHANNEL( _name='//_name//', _addr='//_addr//', _chan='//_chan//', _points='//_points//')');

  DevCamChk(CamPiow(_name, 1, 17, _addr, 16),1,1);
  _buffer = zero(_points, 0W);
  DevCamChk(CamStopw(_name, _chan, 2, _points, _buffer, 16),1,1);
  _buffer &= 0xFF;
  return(_buffer);
}
