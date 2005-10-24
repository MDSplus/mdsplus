FUN PUBLIC CERROTSIGN(IN _system, IN _channel)

/*  This function determines the rotation sign correction for each chord 
  
    Jeff Schachter 99.05.12

*/

{

  _syschan = "\\TOP.CER.CALIBRATION."//_system//"."//_channel//":";

  /* To recognize CER from the D3D tree - check and change _syschan */
  _topd3d = '\\D3D::TOP';
  _top = GETNCI("\\TOP","FULLPATH");
  IF (_top EQ _topd3d) { _syschan = "\\TOP.IONS.CER.CALIBRATION."//_system//"."//_channel//":"; }

  if (_system == "TANGENTIAL")
  { 
    _node = TRIM(_syschan)//"PLASMA_PHI";
    _phideltA = DATA(BUILD_PATH(_node));
    if (_phideltA[0] lt 999.99)  /* missing value */
    {
      _phideltA = _phideltA[0];
    }
    ELSE
    { 
      _phideltA = _phideltA[3];
    }

    _node = TRIM(_syschan)//"LENS_PHI";
    _phideltB = DATA(BUILD_PATH(_node));
    _phidelt = _phideltA - _phideltB;
    _sign = -2 * (_phidelt gt 0. || _phidelt lt -180.) + 1;
  }
  ELSE
  {
    _node = TRIM(_syschan)//"PLASMA_Z";
    _zdeltA = DATA(BUILD_PATH(_node));
    if (_zdeltA[0] lt 999.99)  /* missing value */
    {
      _zdeltA = _zdeltA[0];
    }
    ELSE
    { 
      _zdeltA = _zdeltA[3];
    }

    _node = TRIM(_syschan)//"LENS_Z";
    _zdeltB = DATA(BUILD_PATH(_node));
    _zdelt = _zdeltA - _zdeltB;
    _sign = -2 * (_zdelt gt 0.) + 1;
  }
  RETURN (_sign);
}
