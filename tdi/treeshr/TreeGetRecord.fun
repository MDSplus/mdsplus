/* Future support for multi-shot trending --- not currently used */
public fun TreeGetRecord(in _nid, out _out) {
        _out=1;
	_status=TreeShr->TreeGetRecord(val(_nid),xd(_out));
        return(_status);
}

