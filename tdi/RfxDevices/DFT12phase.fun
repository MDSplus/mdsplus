public fun DFT12phase(in _sig0, in _sig1, in _sig2, in _sig3, in _sig4, in _sig5, in _sig6, in _sig7, in _sig8, in _sig9, in _sig10, in _sig11, in _n)
{

  _nSamples = 12;
  _phi0 = $PI / _nSamples;
  
  _deltaPhi = 2 * $PI / _nSamples;
  _phi =  [ _phi0 + 0 *_deltaPhi, _phi0 + 1 * _deltaPhi,_phi0 + 2 * _deltaPhi, _phi0 + 3 * _deltaPhi, _phi0 + 4  * _deltaPhi, _phi0 + 5 *  _deltaPhi,
            _phi0 + 6 *_deltaPhi, _phi0 + 7 * _deltaPhi,_phi0 + 8 * _deltaPhi, _phi0 + 9 * _deltaPhi, _phi0 + 10 * _deltaPhi, _phi0 + 11 * _deltaPhi ];
	   
	   
  _sig1Resampled  = sig_resample(_sig1, _sig0);
  _sig2Resampled  = sig_resample(_sig2, _sig0);
  _sig3Resampled  = sig_resample(_sig3, _sig0);
  _sig4Resampled  = sig_resample(_sig4, _sig0);
  _sig5Resampled  = sig_resample(_sig5, _sig0);
  _sig6Resampled  = sig_resample(_sig6, _sig0);
  _sig7Resampled  = sig_resample(_sig7, _sig0);
  _sig8Resampled  = sig_resample(_sig8, _sig0);
  _sig9Resampled  = sig_resample(_sig9, _sig0);
  _sig10Resampled = sig_resample(_sig10, _sig0);
  _sig11Resampled = sig_resample(_sig11, _sig0);

  _re = _sig0             * cos( -1 * _n * _phi[0]) +
        _sig1Resampled    * cos( -1 * _n * _phi[1]) +
        _sig2Resampled    * cos( -1 * _n * _phi[2]) +
        _sig3Resampled    * cos( -1 * _n * _phi[3]) +       
        _sig4Resampled    * cos( -1 * _n * _phi[4]) +       
        _sig5Resampled    * cos( -1 * _n * _phi[5]) +       
        _sig6Resampled    * cos( -1 * _n * _phi[6]) +
        _sig7Resampled    * cos( -1 * _n * _phi[7]) +
        _sig8Resampled    * cos( -1 * _n * _phi[8]) +       
        _sig9Resampled    * cos( -1 * _n * _phi[9]) +       
        _sig10Resampled   * cos( -1 * _n * _phi[10]) +         
        _sig11Resampled   * cos( -1 * _n * _phi[11]) ; 

    
  _im = _sig0             * sin( -1 * _n * _phi[0]) +
        _sig1Resampled    * sin( -1 * _n * _phi[1]) +
        _sig2Resampled    * sin( -1 * _n * _phi[2]) +
        _sig3Resampled    * sin( -1 * _n * _phi[3]) +       
        _sig4Resampled    * sin( -1 * _n * _phi[4]) +       
        _sig5Resampled    * sin( -1 * _n * _phi[5]) +       
        _sig6Resampled    * sin( -1 * _n * _phi[6]) +
        _sig7Resampled    * sin( -1 * _n * _phi[7]) +
        _sig8Resampled    * sin( -1 * _n * _phi[8]) +       
        _sig9Resampled    * sin( -1 * _n * _phi[9]) +       
        _sig10Resampled   * sin( -1 * _n * _phi[10]) +         
        _sig11Resampled   * sin( -1 * _n * _phi[11]) ;  

  _phase = atan2(  _im, _re);   

  _phaseSig = make_signal(_phase, *, dim_of(_sig0));
   
  return ( _phaseSig );                            

}
