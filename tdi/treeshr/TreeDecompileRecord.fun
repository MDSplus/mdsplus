/* FOR INTERNAL USE ONLY */
public fun TreeDecompileRecord(in _nid)
{
    _out = 1;
    _status = TreeShr->TreeGetRecord(val(_nid), xd(_out));
    return(execute("decompile(`_out)"));
};
