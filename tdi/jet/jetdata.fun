PUBLIC FUN JETDATA (IN _file, IN _signal, OPTIONAL IN _shotin, OPTIONAL IN _type)
{
  if (!allocated(public _JET$$$LOGGEDIN)) abort();

  _ndata = 0;
  _nt = 0;
  _nx = 0;

  if (NOT PRESENT(_type)) _type = "PPF";
  if (NOT PRESENT(_shotin)) _shotin = PUBLIC _shot;

  _url = "http://data.jet.uk/"//_type//"/"//TRIM(ADJUSTL(_shotin))//"/"//_file//"/"//_signal//CHAR(0);
  _i = mdsjet->mdsjet(ref(_url), ref(_ndata), ref(_nt), ref(_nx));
  if (_i ne 0) abort();

  _t = ARRAY(_nt, 0.d0);
  if (_nx > 1)
  {
    _data = ARRAY([_nt, _nx], 0.0);
    _x = ARRAY(_nx, 0.0);
  }
  else
  {	
    _data = ARRAY(_ndata,0.0);
    _x = *;
  }

  _j = mdsjet->mdsjetdata(ref(_data), ref(_t), ref(_x));

  if (_nx > 1)
  {
    return(MAKE_SIGNAL(_data,,_t,_x));
  }
  else
  {	
    return(MAKE_SIGNAL(_data,,_t));
  }

}