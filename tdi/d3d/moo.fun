PUBLIC FUN MOO (IN _certype, IN _system, IN _channel) {

  _lightspeed_kms = 299792.5;

  _dispersion = DATA(BUILD_PATH("\\IONS::TOP.CER.CALIBRATION."//_system//"."//_channel//":DISPERSION"));
  _isign = DATA(BUILD_PATH("\\IONS::TOP.CER.CALIBRATION."//_system//"."//_channel//":ISIGN"));
  _fiducial = DATA(BUILD_PATH("\\IONS::TOP.CER.CALIBRATION."//_system//"."//_channel//":FIDUCUAL"));
 _order = DATA(BUILD_PATH("\\IONS::TOP.CER.CALIBRATION."//_system//"."//_channel//":ORDER"));
  _wavelength = DATA(BUILD_PATH("\\IONS::TOP.CER.CALIBRATION."//_system//"."//_channel//":WAVELENGTH"));
  _loc = DATA(BUILD_PATH("\\IONS::TOP.CER."//_certype//"."//_system//"."//_channel//":LOC"));

  return(MAKE_SIGNAL(MAKE_WITH_UNITS(((_lightspeed_kms * _dispersion * _isign * (_fiducial - _loc)) / (_order * _wavelength)),'km/s'),*,MAKE_WITH_UNITS(_certime,'ms')));

}
