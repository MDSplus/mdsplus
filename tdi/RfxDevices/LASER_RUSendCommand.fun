public fun LASER_RUSendCommand(in _hComm, in _cmnd, in _cmndLen, in _outValue)
{
   return( RS232Lib->LaserRUSendCommand(_hComm, _cmnd, val(_cmndLen), _outValue ) );
}
