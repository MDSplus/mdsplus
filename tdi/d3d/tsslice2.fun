/*
/       This function provides the appropriate MDSplus expression for TS pointnames
/       that are using the following syntax:
/
/               TS<quantity>_C## or TS<quantity>_T## or TS<quantity>_D##
/
/	Where it will return the data of that system's array index starting from 00.
/
/       See findts() for more information.
/
*/

PUBLIC FUN TSSLICE2 (IN _sig, IN _arg, IN _sys)
{
  _array = DATA(_s = BUILD_PATH("\\"//_sig))[*,INT(_arg)];
  _d = DATA(DIM_OF(_s,0));

  if (INDEX(_sig,"TSNE") ge 0) {
    _u = "/m^3";
  } else {
    _u = UNITS_OF(_s);
  }

  _sig = MAKE_SIGNAL(MAKE_WITH_UNITS(_array,_u),,MAKE_WITH_UNITS(_d,"ms")); 

  return(_sig);

}
