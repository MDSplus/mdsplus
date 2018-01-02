public fun tr_put(in _name, in _idim, in _rdim, in _units, in _label, optional in _data, optional in _expr)
{

  private fun tr_put_record(in _nodename, in _data)
  {
      Return(TreeShr->TreePutRecord(val(getnci(_nodename,'nid_number')),xd(_data),val(0)));
  }

  private fun tr_add_nodes(in _name, in _idim, in _rdim, in _units, in _label, in _mds_path, in _mds_out_path)
  {

    private fun tr_put_record(in _nodename, in _data)
    {
       Return(TreeShr->TreePutRecord(val(getnci(_nodename,'nid_number')),xd(_data),val(0)));
    }

    private fun tr_add_node(in _node,in _usage)
    {
      _status = 1;
      if (if_error(getnci(_node,"NID_NUMBER"),-1) == -1)
      {
         if (!(getdbi("OPEN_FOR_EDIT") & 1))
         {
           _status = tcl('edit '//$EXPT//'/SHOT='//$SHOT);
           if (!(_status & 1)) return(_status);
         }
         _status = tcl('add node '//_node//'/usage='//_usage);
         _status = tcl('set node '//_node//'/compress_on_put');
      }
      return(_status);
    };

    private fun tr_add_tag(in _node, in _tag)
    {
      _status = 1;
      if (if_error(getnci('\\'//_tag,"NID_NUMBER"),-1) == -1)
      {
        if (!(getdbi("OPEN_FOR_EDIT") & 1))
        {
          _status = tcl('edit '//$EXPT//'/SHOT='//$SHOT);
          if (!(_status & 1)) return(_status);
        }
        _status = tcl('add tag '//_node//' '//_tag);
      }
      return(_status);
    };

    _fullpath = _mds_path//_name;
    _status = tr_add_node(_fullpath,'signal');
    if (!(_status & 1)) return(_status);
    _status = tr_add_node(_fullpath//':RPLABEL','text');
    if (!(_status & 1)) return(_status);
    _status=tr_put_record(_fullpath//':RPLABEL',_label);
    if (!(_status & 1)) return(_status);
    _status = tr_add_node(_fullpath//':LABEL','text');
    if (!(_status & 1)) return(_status);
    _status=tr_put_record(_fullpath//':LABEL',len(trim(_units)) != 0 ? _label//'('//_units//')' : _label);
    if (!(_status & 1)) return(_status);
    _status = tr_add_node(_fullpath//':UNITS','text');
    if (!(_status & 1)) return(_status);
    _status=tr_put_record(_fullpath//':UNITS',_units);
    if (!(_status & 1)) return(_status);
    if (_idim > 1) 
    {
      _status = tr_add_node(_fullpath//':XAXIS','text');
      if (!(_status & 1)) return(_status);
      _status=tr_put_record(_fullpath//':XAXIS',_rdim);
      if (!(_status & 1)) return(_status);
    };

    _fullpath = _mds_out_path//_name;
    _status = tr_add_node(_fullpath,'signal');
    if (!(_status & 1)) return(_status);
    _node=build_path(_mds_path//_name);
    _status=tr_put_record(_fullpath,_node);
    if (!(_status & 1)) return(_status);
    _status = tr_add_node(_fullpath//':RPLABEL','text');
    if (!(_status & 1)) return(_status);
    _status=tr_put_record(_fullpath//':RPLABEL',_label);
    if (!(_status & 1)) return(_status);
    _status = tr_add_node(_fullpath//':LABEL','text');
    if (!(_status & 1)) return(_status);
    _status=tr_put_record(_fullpath//':LABEL',len(trim(_units)) != 0 ? _label//'('//_units//')' : _label);
    if (!(_status & 1)) return(_status);
    _status = tr_add_node(_fullpath//':UNITS','text');
    if (!(_status & 1)) return(_status);
    _status=tr_put_record(_fullpath//':UNITS',_units);
    if (!(_status & 1)) return(_status);
    if (_idim > 1) 
    {
      _status = tr_add_node(_fullpath//':XAXIS','text');
      if (!(_status & 1)) return(_status);
      _status=tr_put_record(_fullpath//':XAXIS',_rdim);
      if (!(_status & 1)) return(_status);
    }

    _status = tr_add_tag(_mds_path//_name,_name);
    return(_status);
  }


  _mds_path='\\TOP.TRANSP_OUT:';
  _mds_out_path= _idim == 1 ? '\\TOP.OUTPUTS.ONE_D:' : '\\TOP.OUTPUTS.TWO_D:';
  _readfun = _idim == 1 ? 'TR_READFUN' : 'TR_READPRO';
  _fullpath=_mds_path//_name;
  _status=tr_add_nodes(_name,_idim,_rdim,_units,_label,_mds_path,_mds_out_path);
  if ((_status & 1) != 1) return(_status);
  if (present(_expr))
  {
    _status=tr_put_record(_fullpath,compile(_expr));
  }
  else
  {
    _sig=make_with_units(_data,build_path(_fullpath//':UNITS'));
    if (_idim == 1)
    {
      _time=build_path(_mds_path//'time1d');
      _status=tr_put_record(_fullpath,make_signal(_sig,*,_time));
    }
    else
    {
      _x=build_path(_mds_path//_rdim);
      _time=build_path(_mds_path//'TIME2D');
      _status=tr_put_record(_fullpath,make_signal(_sig,*,_x,_time));
    }
  }
  return(_status);
}

