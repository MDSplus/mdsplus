public fun setdiff1d(in _a, in _b) {
/*
  setdiff1d(in _a, in _b) returns the elements of _a that are not in _b.
*/
  public __intersect_a=_a;
  public __intersect_b=_b;
  public __intersect_c=*;
  py(["from MDSplus import Data,makeData",
      "from numpy.lib.arraysetops import setdiff1d",
      "makeData(setdiff1d(Data.getTdiVar('__intersect_a').data(),Data.getTdiVar('__intersect_b').data())).setTdiVar('__intersect_c')"]);
  return(public __intersect_c);
}

