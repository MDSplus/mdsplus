
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
    	if_error(execute(_command),return(0));
	return(_stat);
     }
     fun private DevAddPythonDevice(in _path, in _type) {
        if (getenv("PyLib") == "") abort();
    	_models = MdsDevices();
    	_model = UPCASE(_type)//'\000';
    	for (_idx=0; _idx<size(_models); _idx++, _idx++) if (UPCASE(_models[_idx])==_model) break;
    	if (_idx == size(_models)) return(0);
    	_module = trim(_models[_idx+1]);
    	_module = extract(0, len(_module)-1, _module);
	_model = extract(0,len(_model)-1, _model);
    	_nidout = 0l;
        _stat = Py(["from MDSplus import Tree","from "//_module//" import "//_model,_model//".Add(Tree(),'"//_path//"')"]);
        if (_stat == 0) abort();
	return(_stat);
     }

   _path = (extract(0, 1, _path) == '\\') ? '\\'//_path : _path;
  _cmd = "_stat = "//_type//'__add("'//_path//'", "'//_type//'")';
  if_error(execute(_cmd), _stat=DevAddPythonDevice(_path, _type), _stat=DevAddCalledDevice(_path,_type) ,return(0));
  return(_stat);
}
