public fun PutRow(as_is _node, in _bufsize, in _timestamp, in _row) {
/* PutRow(node, buffer-size, quadword-timestamp, row-data)

  Loads a row in to a timestamped segment.

  _node == a nid or node name
  _bufsize == number of rows per segment allocation
  _timestamp == quadword timestamp of the row
  _row == scalar or array of values for the row to be added
*/
  _nid=getnci(_node,"NID_NUMBER");
  return(TreeShr->TreePutRow(val(_nid),val(_bufsize),quadword(_timestamp),descr(_row)));
}
