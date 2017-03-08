public fun compare(in _arg1, in _arg2)
{
/*  Compare arguments for equality. 1=equal 0=not equal */
  return(MdsShr->MdsCompareXd(xd(_arg1),xd(_arg2)));
}

