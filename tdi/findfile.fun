public fun findfile(in _filespec)
{
  _context=0Q;  /* REQUIRED ON ALPHA - will hold a memory address and it must be capable of 64 bits */
  _filename=repeat(' ',150);
  _list = *;
  _status=MdsShr->LibFindFile(descr(_filespec),descr(_filename),ref(_context));
  while(_status)
  {
    _list = [_list,_filename];
    _status=MdsShr->LibFindFile(descr(_filespec),descr(_filename),ref(_context));
  }
  return(_list);

}
