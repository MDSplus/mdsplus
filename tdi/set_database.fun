public fun set_database (in _name) 
{
  _dir = getenv('SYS$LOGIN');
  if (len(_dir) == 0) {
    _dir = getenv('USERPROFILE')//'\\';
    if (len(_dir) == 1) {
      _dir = getenv('HOME')//'/';
    }
  }
  if (len(_dir) == 0) {
    Abort("SYS$LOGIN, USERPROFILE or HOME must be defined for SET_DATABASE.FUN");
  }
  _file = _dir//trim(_name)//'.sybase_login';
  _unit = fopen(_file, 'r');
  if (len(_unit) == 0) {
    _file = _dir//lcase(trim(_name))//'.sybase_login';
    _unit = fopen(_file, 'r');
    if (len(_unit) == 0) {
      _file = '/etc/sybase/'//lcase(trim(_name))//'.sybase_login';
      _unit = fopen(_file, 'r');
      if (len(_unit) == 0) {
        Abort("Could not open database file /"//_file//"/");
      }
    }
  } 
  _mdshost = read(_unit);
  _dbhost = read(_unit);
  _dbname = read(_unit);
  _username = read(_unit);
  _password = read(_unit);
  fclose(_unit);
  dblogin(_dbhost, _username, _password);
  dsql('USE '//_dbname);
  dsql('set textsize 8192');
}
