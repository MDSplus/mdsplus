public fun setenv(in _cmd)
{
/*  Use cmd such as foo=gub to define environment variable foo as "gub" */
  return(MdsShr->MdsPutEnv(_cmd));
}
