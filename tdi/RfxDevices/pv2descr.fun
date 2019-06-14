public fun pv2descr(in _pv)
{
       _tag=translate("\\"// _pv, '__','-:')//":DESCRIPTION";
       return( build_path(_tag) );
}
