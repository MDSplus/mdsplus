FUN PUBLIC NIMROD_PHYS_DB2(IN _shot, IN _PE_FRAC, IN _LOOP_VOLT, IN _K_PERP, IN _K_PLL, IN _MU_E, IN _MU_I, IN _DS_USE, IN _EQ_FLOW, IN _SEPARATE_PE, IN _NONLINEAR, IN _CONTINUITY, IN _ETA_MODEL, IN _SPLIT_DIVB, IN _SPLIT_VISC, IN _SPLIT_RESIST) 
{
   /*  MDS+ seems to only handle up to 32 quantities at once, so multiple routines are nedded */
   _cmd = "/mdstrees2/nimrod_temp/nimrod_phys_db2 "//_shot//" "//_PE_FRAC//" "//_LOOP_VOLT//" "//_K_PERP//" "//_K_PLL//" "//_MU_E//" "//_MU_I//" '"//_DS_USE//"' '"//_EQ_FLOW//"' "//_SEPARATE_PE//" "//_NONLINEAR//" "//_CONTINUITY//" "//_ETA_MODEL//" "//_SPLIT_DIVB//" "//_SPLIT_VISC//" "//_SPLIT_RESIST;
   SPAWN(_cmd);
   return(1);
}
