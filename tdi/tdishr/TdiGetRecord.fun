/* Future support for multi-shot trending --- not currently used */
public fun TdiGetRecord(in _nid, out _out) {
        _out=1;
	_status=TreeShr->TreeGetRecord(val(_nid),xd(_out));
        return(_status);
}

