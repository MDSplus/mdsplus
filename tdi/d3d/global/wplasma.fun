/* wplasma(in _efit)
   This funtion calls wdot.fun for a given efit, which calculates and saves
   wplasma as a public MDSplus variable __wplasma.
   20080829 SMF
*/ 

public fun wplasma(in _efit) {
   _temp = wdot(_efit);
   return(__wplasma);
}
