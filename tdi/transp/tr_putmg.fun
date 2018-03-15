public fun tr_putmg(in _name, in _label, in _content, in _consign)
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
    }
    return(_status);
  };

  _status = tr_add_node('\\TOP.MULTIGRAPHS','structure');
  if (!(_status & 1)) return(_status);

  _fullpath = '\\TOP.MULTIGRAPHS.'//_name;
  _status = tr_add_node(_fullpath,'structure');
  if (!(_status & 1)) return(_status);
  _status = tr_add_node(_fullpath//':LABEL','text');
  if (!(_status & 1)) return(_status);
  _status=tr_put_record(_fullpath//':LABEL',_label);
  if (!(_status & 1)) return(_status);
  _status = tr_add_node(_fullpath//':CONTENT','text');
  if (!(_status & 1)) return(_status);
  _status=tr_put_record(_fullpath//':CONTENT',_content);
  if (!(_status & 1)) return(_status);
  _status = tr_add_node(_fullpath//':CONSIGN','numeric');
  if (!(_status & 1)) return(_status);
  _status=tr_put_record(_fullpath//':CONSIGN',_consign);
  if (!(_status & 1)) return(_status);

  return(_status);

}

