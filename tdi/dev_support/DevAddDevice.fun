
public fun DevAddDevice(in _path, in _type, optional out _nidout)
{
   fun private DevAddCalledDevice(in _path, in _type) 
   {
    	_models = MdsDevices();
    	_model = UPCASE(_type);
    	for (_idx=0; _idx<size(_models[0]); _idx++) {
	  if (UPCASE(_models[0,_idx])==_model) break;
	};
    	if (_idx == size(_models[0]))
	    return(0);
    	_image = trim(_models[1,_idx]);
	_rtn = _type//"__add";
    	_nidout = 0l;
	_addr = 0Q;
	_stat = MdsShr->LibFindImageSymbol(descr(_image),descr(_rtn),_addr);
	if (_stat & 1) { 
    	  _command = '_stat = '//_image//'->'//_type//'__add(descr("'//_path//'"), descr(" "), _nidout)';
    	  _stat = if_error(execute(_command),0);
	};
	return(_stat);
   }	
   _path_q = (extract(0, 1, _path) == '\\') ? '\\'//_path : _path;
   _cmd = "_stat = "//_type//'__add("'//_path_q//'", "'//_type//'")';
   _stat = if_error(execute(_cmd), DevAddCalledDevice(_path_q, _type),0);
   if (~(_stat & 1))
     _stat = DevAddPythonDevice(_path,_type,_nidout);
   return(_stat);
}
