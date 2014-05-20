public fun intersect1d(in _a, in _b) {
/*
  intersect1d(in _a, in _b) returns the intersection of two 1d arrays.
*/
  public __intersect_a=_a;
  public __intersect_b=_b;
  public __intersect_c=*;
  py(["from MDSplus import Data,makeData",
      "from numpy.lib.arraysetops import intersect1d",
      "makeData(intersect1d(Data.getTdiVar('__intersect_a').data(),Data.getTdiVar('__intersect_b').data())).setTdiVar('__intersect_c')"]);
  return(public __intersect_c);
}


