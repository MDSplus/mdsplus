PUBLIC FUN JETDATA (IN _ppf, IN _signal, IN _shot)
{  
  if (!allocated(public _JET$$$LOGGEDIN)) abort();
  _ndata = 0;
  _nt = 0;
  _nx = 0;
  _url = "http://data.jet.uk/PPF/"//TRIM(ADJUSTL(_shot))//"/"//_ppf//"/"//_signal//CHAR(0);
  write (*,"-->",_url,"<--");
  _i = mdsjet->mdsjet(ref(_url), ref(_ndata), ref(_nt), ref(_nx));
  write (*,"GOT IT!"); 
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