/*
/       This function provides the appropriate MDSplus expression for TS pointnames
/       that are using the following syntax:
/
/               TS<quantity>_C## or TS<quantity>_T## or TS<quantity>_D##
/
/	Where it will return the data of that system's channel index starting from 00.
/
/       See findts() for more information.
/
*/

PUBLIC FUN TSSLICE2 (IN _sig, IN _arg, IN _sys)
{
   	if (INDEX(_sys,"TAN") ge 0) {
      	  _z = DATA(BUILD_PATH("\\TSR_"//_sys));  
	} else {
	  _z = DATA(BUILD_PATH("\\TSZ_"//_sys));  
	}

        _array = DATA(_s = BUILD_PATH("\\"//_sig))[*,INT(_arg)];
	_d = DIM_OF(_s,0);

	if (INDEX(_sig,"TSNE") ge 0) {
	  _u = "/m^3";
	} else {
	  _u = UNITS_OF(_s);
	}

	_zval = ADJUSTL(CVT(_z[INT(_arg)],"123456789."));

   	if (INDEX(_sys,"TAN") ge 0) {
	  _units = _u //" at R = "// EXTRACT(0,INDEX(_zval,"S"),_zval) // " m";
	} else {
	  _units = _u //" at Z = "// EXTRACT(0,INDEX(_zval,"S"),_zval) // " m";
	}

        _sig = MAKE_SIGNAL(MAKE_WITH_UNITS(_array,_units),,_d); 

	return(_sig);

}
