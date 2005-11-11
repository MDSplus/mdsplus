FUN PUBLIC NIMROD_PHYS_DB(IN _shot, IN _mx, IN _my, IN _lphi, IN _elecd, IN _kinvisc, IN _dtm, IN _taua, IN _sfac, IN _gridshape, IN _GEOM, IN _OHMS, IN _ADVECT, IN _SI_FAC_MHD, IN _SI_FAC_PRES, IN _SI_FAC_J0, IN _SI_FAC_HALL, IN _SI_FAC_NL, IN _MHD_SI_ISO, IN _NDENS, IN _DVAC, IN _VSINK_RATE, IN _POLY_DEGREE, IN _DIVBD, IN _FDIVB, IN _FB_VXB, IN _FV_VDGV, IN _FP_VDGP)
{
   /*  Loads several nimrod physics quantities into the code run databas */
   _cmd = "/mdstrees2/nimrod_temp/nimrod_phys_db "//_shot//" "//_mx//" "//_my//" "//_lphi//" "//_elecd//" "//_kinvisc//" "//_dtm//" "//_taua//" "//_sfac//" '"//_gridshape//"' '"//_geom//"' '"//_ohms//"' '"//_advect//"' "//_si_fac_mhd//" "//_si_fac_pres//" "//_si_fac_j0//" "//_si_fac_hall//" "//_si_fac_nl//" "//_mhd_si_iso//" "//_ndens//" "//_dvac//" "//_vsink_rate//" "//_poly_degree//" "//_divbd//" "//_fdivb//" "//_fb_vxb//" "//_fv_vdgv//" "//_fp_vdgp;
   SPAWN(_cmd);
   return(1);
}
