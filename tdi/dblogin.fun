public fun DbLogin(in _host, in _user, in _pass)
{
  if (!present(_user) || len(_user) == 0) {
    _user=whoami();
  }
  return (MdsSql->Login_Sybase(_host, _user, _pass));
}
