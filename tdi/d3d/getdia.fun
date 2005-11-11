FUN PUBLIC GETDIA(IN _shot, OUT _t, OUT _phi, OPTIONAL OUT _sigphi, OPTIONAL IN _tavg)
{

  _n=4096;
  _t=ARRAY(_n,0.0);
  _phi = ARRAY(_n,0.0);
  _sigphi = ARRAY(_n,0.0);

  if (NOT PRESENT(_tavg)) _tavg = 0.;

  _ierr = ARRAY(3, 0L);

  _status = libMdsD3D->getdia_(_shot, REF(_t), REF(_n), _tavg, REF(_ierr), REF(_phi), REF(_sigphi));

  RETURN(_status);

}

