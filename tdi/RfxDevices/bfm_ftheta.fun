public fun bfm_ftheta()
{
/**************************************/
/* Create F-Theta curve for BFM model */
/* Emilio Martines - January 2005     */
/**************************************/

/* Since there are no Bessel functions in TDI, I use a 6th order degree */
/* polynomial approximation to the actual curve.                        */
/* The error made with this method is everywhere less than 0.005.       */

  _theta = (0.:100.)*0.015;
  _f = 0.997553 + 0.0963003*_theta -1.31316*_theta^2 + 2.67894*_theta^3
                - 4.19606*_theta^4 + 2.98117*_theta^5 - 0.857705*_theta^6;
  return(make_signal(_f,*,_theta));
}