public fun O2( in _name, in _shot, in _channel, in _server)
{
   _size = 0;
   _type = 0;
   _status = Retrieve->MDSplusRetrieveGetSize(_name, _shot, _channel, ref(_size), ref(_type), _name, _server);
   _dum = write(*,"_status = ",_status);
   _dum = write(*,"_size = ",_size);
   _dum = write(*,"_type = ",_type);
   _data = zero((_size+1)/2,0w);
   _status = Retrieve->MDSplusRetrieveGetData(_name, _shot, _channel, ref(_data), ref(_size), ref(_type), _name, _server);
   _dum = write(*,"_status = ",_status);
   return(_data);
}

