/* FOR INTERNAL USE ONLY */
public fun TreeAbsPath(in _inpath)
{
  _abspath = repeat(' ',512);
  return((TreeShr->TreeAbsPathDsc(_inpath,descr(_abspath)) & 1) ? upcase(trim(_abspath)) : "");
}
