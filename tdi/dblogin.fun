public fun DbLogin(in _host, in _user, in _pass)
{
  return (MdsSql->Login_Sybase(_host, _user, _pass));
}
