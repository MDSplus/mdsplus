public fun SetDatabase(in _dbname)
{
  _status = GetDbInfo(_dbname, _dbhost, _dbuser, _dbpass);
  if (_status) 
  {
    _status = MdsSql->Login_Sybase(_dbhost, _dbuser, _dbpass);
    if (_status) 
    {
      _status = DSQL("use ?", _dbname) == 0;
    }
  }
  return(_status);
}
