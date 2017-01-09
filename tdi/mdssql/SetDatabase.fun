public fun SetDatabase(in _dbname)
{
  if (allocated(public _currentDBname)) {
    if (_currentDBname == _dbname) {
      return(1);
    }
 }
  _status = GetDbInfo(_dbname, _dbhost, _dbuser, _dbpass);
  if (_status) 
  {
    _status = MdsSql->Login_Sybase(_dbhost, _dbuser, _dbpass);
    if (_status) 
    {
      _status = DSQL("use ?", _dbname) == 0;
      if (_status) {
	public _currentDBname = _dbname;
      }
    }
  }
  return(_status);
}
