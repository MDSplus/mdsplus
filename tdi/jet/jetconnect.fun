PUBLIC FUN JETCONNECT (IN _user, IN _password)
{
  mdsjet->mdsjet_setidentity(_user, _password);
  public _JET$$$LOGGEDIN = 1;
  return(1);
}
