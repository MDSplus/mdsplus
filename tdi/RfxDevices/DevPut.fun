public fun DevPut(in _head_nid, in _offset, in _data)
{
     _nid = DevHead(_head_nid) + _offset;
     return(TreeShr->TreePutRecord(val(_nid),xd(_data),val(0)));
}

