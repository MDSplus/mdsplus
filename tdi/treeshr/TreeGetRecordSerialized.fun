/* FOR INTERNAL USE ONLY */
public fun TreeGetRecordSerialized(in _nid)
{
    _out = 1;
    _status = TreeShr->TreeGetRecord(val(_nid), xd(_out));
    return(execute("SerializeOut(`_out)"));
};
