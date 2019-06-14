public fun getSpiderStateTime(in _spiderStateCode, optional in _shot)
{

   if(present(_shot))
       treeopen("spider", _shot);

   _data = data(build_path("\\NB_SIGV_FSM_ST_STAT:VAL"));
   _time = dim_of(build_path("\\NB_SIGV_FSM_ST_STAT:VAL"));

   if(present(_shot))
       treeclose();

   _stateTime = sum((_data == _spiderStateCode) * _time);

   return( _stateTime );
} 

