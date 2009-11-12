public fun PutManyExecute(in _putMany) {
/*
  PutManyExecute expects a serialized List data type where each element of the list is a
  Dictionary data type whose elements are 'node','exp', and 'args' where 'node' is the node name,
  'exp' is an expression string and 'args' is a List datatype of the arguments to the expression.
  If no arguments, then 'args' should be an List data type of zero elements.

  PutManyExecute should return a serialized Dictionary data type where ans['node-name']='Success' or the-error-string.
*/
  public __putManyIn__=_putMany;
  Py('Connection.__processPutMany__()');
  return(public __putManyOut__);
}

