// KsTranslateIosb.c
//-------------------------------------------------------------------------
//	Stuart Sherman
//	MIT / PSFC
//	Cambridge, MA 02139  USA
//
//	This is a port of the MDSplus system software from VMS to Linux, 
//	specifically:
//			CAMAC subsystem, ie libCamShr.so and verbs.c for CTS.
//-------------------------------------------------------------------------
//	$Id$
//-------------------------------------------------------------------------
// Mon Oct 15 16:35:42 EDT 2001	-- seperated out
//-----------------------------------------------------------


//-----------------------------------------------------------
int KsTranslateIosb( RequestSenseData *sense, int cam_status)
{
  char		dev_name[5];
  int			scsiDevice, status; 
  
  union {
    ErrorStatusRegister esr;
    int     l;
    BYTE	b[4];
  } u;
  
  if( MSGLVL(FUNCTION_NAME) )
    printf( "%s()\n", KT_ROUTINE_NAME );
  
  status = CamSERTRAERR;
  LastIosb.status = Shorten( CamSERTRAERR );
  if (cam_status == 0 || cam_status == 1 || cam_status == 2)
    {
      switch( sense->u2.esr.error_code ) {
      case 0: 
	status = 1;
	LastIosb.status = Shorten( CamDONE_Q );
	if( MSGLVL(DETAILS) )
	  printf("CamDONE_Q\n");
	break;
      case 7:
	status = 1;
	LastIosb.status = Shorten( CamDONE_NOQ );
	if( MSGLVL(DETAILS) )
	  printf("CamDONE_NOQ\n");
	break;
      case 8:
	status = CamDONE_NOX;
	LastIosb.status = Shorten( CamDONE_NOX );
	if( MSGLVL(DETAILS) )
	  printf("CamDONE_NOX\n");
	break;
      case 12:
	status = CamSCCFAIL;
	LastIosb.status = Shorten( CamSCCFAIL );
	if( MSGLVL(DETAILS) )
	  printf("CamSCCFAIL\n");
	break;
      default:
	status = CamSERTRAERR;
	LastIosb.status = Shorten( CamSERTRAERR );
	if( MSGLVL(DETAILS) )
	  printf("CamSERTRAERR\n");
	break;
      }
      LastIosb.x       = !sense->u2.esr.nox;
      LastIosb.q       = !sense->u2.esr.noq;
      LastIosb.err     =  sense->u2.esr.err;
      LastIosb.lpe     =  sense->u2.esr.lpe;
      LastIosb.tpe     =  sense->u2.esr.tpe;
      LastIosb.no_sync =  sense->u2.esr.no_sync;
      LastIosb.tmo     =  sense->u2.esr.tmo;
      LastIosb.adnr    =  sense->u2.esr.adnr;
      LastIosb.list    =  0;			// list processing not supported
      LastIosb.lbytcnt =  0;			// list processing not supported
    }      
 KsTranslateIosb_Exit:
      if( MSGLVL(DETAILS) ) {
	printf( "%s(): LastIosb.status = %d [0x%x]\n", KT_ROUTINE_NAME, LastIosb.status, LastIosb.status );
    printf( "%s(): LastIosb.x = %d\n",             KT_ROUTINE_NAME, LastIosb.x                       );
    printf( "%s(): LastIosb.q = %d\n",             KT_ROUTINE_NAME, LastIosb.q                       );
  }
  
  return status;
}
