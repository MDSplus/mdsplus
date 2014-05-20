FUN PUBLIC GETDIA(IN _shot, OUT _t, OUT _phi, OPTIONAL OUT _sigphi, OPTIONAL IN _tavg)
{

  _n=4096;
  _t=zero(_n,0.0);
  _phi = zero(_n,0.0);
  _sigphi = zero(_n,0.0);
  if (not present(_tavg)) _tavg = 0.0;
  _ierr = zero(3,0);
  _status = build_call(0,'/c/source/getdia/linux/getdia','getdia_',
                       _shot, REF(_t), _n, _tavg, REF(_ierr),
                       REF(_phi), REF(_sigphi)); 
  RETURN(_phi);

}

