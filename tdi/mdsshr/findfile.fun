public fun findfile(in _filespec, optional _recurse)
{
  _context=0Q;  /* REQUIRED ON ALPHA - will hold a memory address and it must be capable of 64 bits */
  _filename=repeat(' ',150);
  _list = *;
  _routine = present(_recurse) ?  'LibFindFileRecurseCaseBlind' : 'LibFindFile';
  _status=build_call(8,'MdsShr',_routine,descr(_filespec),descr(_filename),ref(_context));
  while(_status)
  {
    _list = [_list,trim(_filename)];
    _status=build_call(8,'MdsShr',_routine,descr(_filespec),descr(_filename),ref(_context));
  }
  return(_list);

}
