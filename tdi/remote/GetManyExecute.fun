public fun GetManyExecute(in _getMany) {
/*
  GetManyExecute expects a serialized List data type whose elements are Dictionary data type with the fields, 'name','exp','args'.
  GetManyExecute returns a serialized Dictionary data type where ans['name']= a Dictionary data type containing either the field 'value' whose value is the data(value of the expression) or 'error' whose value is the error string.
*/
  public __getManyIn__=_getMany;
  Py('Connection.__processGetMany__()');
  return(public __getManyOut__);
}

