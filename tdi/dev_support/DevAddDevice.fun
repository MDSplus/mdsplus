
public fun DevAddDevice(in _path, in _type, optional out _nidout)
{

    fun private DevAddCalledDevice(in _path, in _type) 
    {
    	_models = MdsDevices();
    	_model = UPCASE(_type)//'\000';
    	for (_idx=0; _idx<size(_models); _idx++, _idx++) if (UPCASE(_models[_idx])==_model) break;
    	if (_idx == size(_models)) return(0);
    	_image = trim(_models[_idx+1]);
    	_image = extract(0, len(_image)-1, _image);
    	_nidout = 0l;
    	_command = '_stat = '//_image//'->'//_type//'__add(descr("'//_path//'"), descr(" "), _nidout)';
    	execute(_command);
	return(_stat);
     }

  _cmd = "_stat = "//_type//'__add("'//_path//'", "'//_type//'")';
  if_error(execute(_cmd), _stat=DevAddCalledDevice(_path, _type), return(0));
  write (*, "about to return "//_stat);
  return(_stat);
}
