FUN PUBLIC NIMROD_PHYS_DB3(IN _shot, IN _FOM, IN _FVSC, IN _FTHC, IN _TAU_R, IN _PRFAC, IN _AGEOM, IN _RGEOM, IN _RMAXIS, IN _ZMAXIS, IN _BGEOM, IN _NU_E, IN _P_MODEL, IN _HEAT_SOURCE, IN _GAMMA_HEAT)
{
   /* This should not have beed needed.  However, when the exact same code was placed in nimrod_phys_db2.fun, 
      MDS+ complained abut an unknown variable.  There was no syntax errors, and line limit, and variable limit
      was not a factor (etc).  When, the exact code was created into this new TDI function, it worked. Essentailly,
      I was getting a TDI error from the MDS+ server, that simply should not have been occuring.  Thus, more
      lines of code to work around MDS+.  Yours truly, SF. */ 
   _cmd = "/mdstrees2/nimrod_temp/nimrod_phys_db3 "//_shot//" "//_FOM//" "//_FVSC//" "//_FTHC//" "//_TAU_R//" "//_PRFAC//" "//_AGEOM//" "//_RGEOM//" "//_RMAXIS//" "//_ZMAXIS//" "//_BGEOM//" "//_NU_E//" '"//_P_MODEL//"' '"//_HEAT_SOURCE//"' "//_GAMMA_HEAT;
   SPAWN(_cmd);
   return(1);
}
