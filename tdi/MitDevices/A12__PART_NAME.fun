public fun A12__PART_NAME(as_is _nid, in _method)
{
  switch (getnci(_nid,'conglomerate_elt'))
  {
    case (1WU)  return('');
    case (2WU)  return(':NAME');
    case (3WU)  return(':COMMENT');
    case (4WU)  return(':EXT_CLOCK_IN');
    case (5WU)  return(':STOP_TRIG');
    case (6WU)  return(':INPUT_1');
    case (7WU)  return(':INPUT_1:STARTIDX');
    case (8WU)  return(':INPUT_1:ENDIDX');
    case (9WU)  return(':INPUT_2');
    case (10WU) return(':INPUT_2:STARTIDX');
    case (11WU) return(':INPUT_2:ENDIDX');
    case (12WU) return(':INPUT_3');
    case (13WU) return(':INPUT_3:STARTIDX');
    case (14WU) return(':INPUT_3:ENDIDX');
    case (15WU) return(':INPUT_4');
    case (16WU) return(':INPUT_4:STARTIDX');
    case (17WU) return(':INPUT_4:ENDIDX');
    case (18WU) return(':INPUT_5');
    case (19WU) return(':INPUT_5:STARTIDX');
    case (20WU) return(':INPUT_5:ENDIDX');
    case (21WU) return(':INPUT_6');
    case (22WU) return(':INPUT_6:STARTIDX');
    case (23WU) return(':INPUT_6:ENDIDX');
    case (24WU) return(':INIT_ACTION');
    case (25WU) return(':STORE_ACTION');
  }
  return('');
}
