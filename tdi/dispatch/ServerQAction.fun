public fun ServerQAction(in _addr, in _port, in _op, in _flags, in _jobid, optional in _p1, optional in _p2, optional in _p3,
                         optional in _p4, optional in _p5, optional in _p6, optional in _p7, optional in _p8)
{
  if (present(_p8))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2,_p3,_p4,_p5,_p6,_p7,_p8));
  if (present(_p7))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2,_p3,_p4,_p5,_p6,_p7));
  if (present(_p6))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2,_p3,_p4,_p5,_p6));
  if (present(_p5))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2,_p3,_p4,_p5));
  if (present(_p4))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2,_p3,_p4));
  if (present(_p3))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2,_p3));
  if (present(_p2))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1,_p2));
  if (present(_p1))
    return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid,_p1));
  return(MdsServerShr->ServerQAction(_addr,_port,_op,_flags,_jobid));
}
