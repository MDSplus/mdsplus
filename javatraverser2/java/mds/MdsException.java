/*
 * This module was generated using mdsshr/gen_device.py
 * To add new status messages modify one of the
 * "xxxx_messages.xml files (or add a new one)
 * and then in mdsshr do:
 * python gen_devices.py
 */
package mds;

import java.awt.Color;
import java.io.IOException;
import javax.swing.JLabel;

@SuppressWarnings("serial")
public class MdsException extends IOException{
    public static final class MdsAbortException extends MdsException{
        public MdsAbortException(){
            super("Transaction aborted", 0);
        }
    }
    private static JLabel   statusLabel                       = null;
    public static final int DevBAD_ENDIDX                     = 04737100012;
    public static final int DevBAD_FILTER                     = 04737100022;
    public static final int DevBAD_FREQ                       = 04737100032;
    public static final int DevBAD_GAIN                       = 04737100042;
    public static final int DevBAD_HEADER                     = 04737100052;
    public static final int DevBAD_HEADER_IDX                 = 04737100062;
    public static final int DevBAD_MEMORIES                   = 04737100072;
    public static final int DevBAD_MODE                       = 04737100102;
    public static final int DevBAD_NAME                       = 04737100112;
    public static final int DevBAD_OFFSET                     = 04737100122;
    public static final int DevBAD_STARTIDX                   = 04737100132;
    public static final int DevNOT_TRIGGERED                  = 04737100142;
    public static final int DevFREQ_TO_HIGH                   = 04737100152;
    public static final int DevINVALID_NOC                    = 04737100162;
    public static final int DevRANGE_MISMATCH                 = 04737100172;
    public static final int DevCAMACERR                       = 04737100202;
    public static final int DevBAD_VERBS                      = 04737100212;
    public static final int DevBAD_COMMANDS                   = 04737100222;
    public static final int DevCAM_ADNR                       = 04737100232;
    public static final int DevCAM_ERR                        = 04737100242;
    public static final int DevCAM_LOSYNC                     = 04737100252;
    public static final int DevCAM_LPE                        = 04737100262;
    public static final int DevCAM_TMO                        = 04737100272;
    public static final int DevCAM_TPE                        = 04737100302;
    public static final int DevCAM_STE                        = 04737100312;
    public static final int DevCAM_DERR                       = 04737100322;
    public static final int DevCAM_SQ                         = 04737100332;
    public static final int DevCAM_NOSQ                       = 04737100342;
    public static final int DevCAM_SX                         = 04737100352;
    public static final int DevCAM_NOSX                       = 04737100362;
    public static final int DevINV_SETUP                      = 04737100372;
    public static final int DevPYDEVICE_NOT_FOUND             = 04737100402;
    public static final int DevPY_INTERFACE_LIBRARY_NOT_FOUND = 04737100412;
    public static final int DevIO_STUCK                       = 04737100422;
    public static final int DevUNKOWN_STATE                   = 04737100432;
    public static final int DevWRONG_TREE                     = 04737100442;
    public static final int DevWRONG_PATH                     = 04737100452;
    public static final int DevWRONG_SHOT                     = 04737100462;
    public static final int DevOFFLINE                        = 04737100472;
    public static final int DevTRIGGERED_NOT_STORED           = 04737100502;
    public static final int DevNO_NAME_SPECIFIED              = 04737100512;
    public static final int DevBAD_ACTIVE_CHAN                = 04737100522;
    public static final int DevBAD_TRIG_SRC                   = 04737100532;
    public static final int DevBAD_CLOCK_SRC                  = 04737100542;
    public static final int DevBAD_PRE_TRIG                   = 04737100552;
    public static final int DevBAD_POST_TRIG                  = 04737100562;
    public static final int DevBAD_CLOCK_FREQ                 = 04737100572;
    public static final int DevTRIGGER_FAILED                 = 04737100602;
    public static final int DevERROR_READING_CHANNEL          = 04737100612;
    public static final int DevERROR_DOING_INIT               = 04737100622;
    public static final int ReticonNORMAL                     = 04737100631;
    public static final int ReticonBAD_FRAMES                 = 04737100642;
    public static final int ReticonBAD_FRAME_SELECT           = 04737100652;
    public static final int ReticonBAD_NUM_STATES             = 04737100662;
    public static final int ReticonBAD_PERIOD                 = 04737100672;
    public static final int ReticonBAD_PIXEL_SELECT           = 04737100702;
    public static final int ReticonDATA_CORRUPTED             = 04737100712;
    public static final int ReticonTOO_MANY_FRAMES            = 04737100722;
    public static final int J221NORMAL                        = 04737101451;
    public static final int J221INVALID_DATA                  = 04737101464;
    public static final int J221NO_DATA                       = 04737101472;
    public static final int TimingINVCLKFRQ                   = 04737102272;
    public static final int TimingINVDELDUR                   = 04737102302;
    public static final int TimingINVOUTCTR                   = 04737102312;
    public static final int TimingINVPSEUDODEV                = 04737102322;
    public static final int TimingINVTRGMOD                   = 04737102332;
    public static final int TimingNOPSEUDODEV                 = 04737102343;
    public static final int TimingTOO_MANY_EVENTS             = 04737102352;
    public static final int B2408NORMAL                       = 04737103111;
    public static final int B2408OVERFLOW                     = 04737103123;
    public static final int B2408TRIG_LIM                     = 04737103134;
    public static final int FeraNORMAL                        = 04737103731;
    public static final int FeraDIGNOTSTRARRAY                = 04737103742;
    public static final int FeraNODIG                         = 04737103752;
    public static final int FeraMEMNOTSTRARRAY                = 04737103762;
    public static final int FeraNOMEM                         = 04737103772;
    public static final int FeraPHASE_LOST                    = 04737104002;
    public static final int FeraCONFUSED                      = 04737104014;
    public static final int FeraOVER_RUN                      = 04737104024;
    public static final int FeraOVERFLOW                      = 04737104033;
    public static final int Hm650NORMAL                       = 04737104551;
    public static final int Hm650DLYCHNG                      = 04737104564;
    public static final int Hv4032NORMAL                      = 04737105371;
    public static final int Hv4032WRONG_POD_TYPE              = 04737105402;
    public static final int Hv1440NORMAL                      = 04737106211;
    public static final int Hv1440WRONG_POD_TYPE              = 04737106222;
    public static final int Hv1440BAD_FRAME                   = 04737106232;
    public static final int Hv1440BAD_RANGE                   = 04737106242;
    public static final int Hv1440OUTRNG                      = 04737106252;
    public static final int Hv1440STUCK                       = 04737106262;
    public static final int JoergerBAD_PRE_TRIGGER            = 04737107032;
    public static final int JoergerBAD_ACT_MEMORY             = 04737107042;
    public static final int JoergerBAD_GAIN                   = 04737107052;
    public static final int U_of_mBAD_WAVE_LENGTH             = 04737107652;
    public static final int U_of_mBAD_SLIT_WIDTH              = 04737107662;
    public static final int U_of_mBAD_NUM_SPECTRA             = 04737107672;
    public static final int U_of_mBAD_GRATING                 = 04737107702;
    public static final int U_of_mBAD_EXPOSURE                = 04737107712;
    public static final int U_of_mBAD_FILTER                  = 04737107722;
    public static final int U_of_mGO_FILE_ERROR               = 04737107732;
    public static final int U_of_mDATA_FILE_ERROR             = 04737107742;
    public static final int IdlNORMAL                         = 04737110471;
    public static final int IdlERROR                          = 04737110502;
    public static final int B5910aBAD_CHAN                    = 04737111312;
    public static final int B5910aBAD_CLOCK                   = 04737111322;
    public static final int B5910aBAD_ITERATIONS              = 04737111332;
    public static final int B5910aBAD_NOC                     = 04737111342;
    public static final int B5910aBAD_SAMPS                   = 04737111352;
    public static final int J412NOT_SORTED                    = 04737112132;
    public static final int J412NO_DATA                       = 04737112142;
    public static final int J412BADCYCLES                     = 04737112152;
    public static final int Tr16NORMAL                        = 04737112751;
    public static final int Tr16BAD_MEMSIZE                   = 04737112762;
    public static final int Tr16BAD_ACTIVEMEM                 = 04737112772;
    public static final int Tr16BAD_ACTIVECHAN                = 04737113002;
    public static final int Tr16BAD_PTS                       = 04737113012;
    public static final int Tr16BAD_FREQUENCY                 = 04737113022;
    public static final int Tr16BAD_MASTER                    = 04737113032;
    public static final int Tr16BAD_GAIN                      = 04737113042;
    public static final int A14NORMAL                         = 04737113571;
    public static final int A14BAD_CLK_DIVIDE                 = 04737113602;
    public static final int A14BAD_MODE                       = 04737113612;
    public static final int A14BAD_CLK_POLARITY               = 04737113622;
    public static final int A14BAD_STR_POLARITY               = 04737113632;
    public static final int A14BAD_STP_POLARITY               = 04737113642;
    public static final int A14BAD_GATED                      = 04737113652;
    public static final int L6810NORMAL                       = 04737114411;
    public static final int L6810BAD_ACTIVECHAN               = 04737114422;
    public static final int L6810BAD_ACTIVEMEM                = 04737114432;
    public static final int L6810BAD_FREQUENCY                = 04737114442;
    public static final int L6810BAD_FULL_SCALE               = 04737114452;
    public static final int L6810BAD_MEMORIES                 = 04737114462;
    public static final int L6810BAD_COUPLING                 = 04737114472;
    public static final int L6810BAD_OFFSET                   = 04737114502;
    public static final int L6810BAD_SEGMENTS                 = 04737114512;
    public static final int L6810BAD_TRIG_DELAY               = 04737114522;
    public static final int J_dacOUTRNG                       = 04737115232;
    public static final int IncaaBAD_ACTIVE_CHANS             = 04737116052;
    public static final int IncaaBAD_MASTER                   = 04737116062;
    public static final int IncaaBAD_EXT_1MHZ                 = 04737116072;
    public static final int IncaaBAD_PTSC                     = 04737116102;
    public static final int L8212BAD_HEADER                   = 04737116672;
    public static final int L8212BAD_MEMORIES                 = 04737116702;
    public static final int L8212BAD_NOC                      = 04737116712;
    public static final int L8212BAD_OFFSET                   = 04737116722;
    public static final int L8212BAD_PTS                      = 04737116732;
    public static final int L8212FREQ_TO_HIGH                 = 04737116742;
    public static final int L8212INVALID_NOC                  = 04737116752;
    public static final int MpbBADTIME                        = 04737117512;
    public static final int MpbBADFREQ                        = 04737117522;
    public static final int L8828BAD_OFFSET                   = 04737120332;
    public static final int L8828BAD_PRETRIG                  = 04737120342;
    public static final int L8828BAD_ACTIVEMEM                = 04737120352;
    public static final int L8828BAD_CLOCK                    = 04737120362;
    public static final int L8818BAD_OFFSET                   = 04737121152;
    public static final int L8818BAD_PRETRIG                  = 04737121162;
    public static final int L8818BAD_ACTIVEMEM                = 04737121172;
    public static final int L8818BAD_CLOCK                    = 04737121202;
    public static final int J_tr612BAD_ACTMEM                 = 04737121272;
    public static final int J_tr612BAD_PRETRIG                = 04737121302;
    public static final int J_tr612BAD_MODE                   = 04737121312;
    public static final int J_tr612BAD_FREQUENCY              = 04737121322;
    public static final int L8206NODATA                       = 04737121774;
    public static final int H912BAD_CLOCK                     = 04737122112;
    public static final int H912BAD_BLOCKS                    = 04737122122;
    public static final int H912BAD_PTS                       = 04737122132;
    public static final int H908BAD_CLOCK                     = 04737122232;
    public static final int H908BAD_ACTIVE_CHANS              = 04737122242;
    public static final int H908BAD_PTS                       = 04737122252;
    public static final int Dsp2904CHANNEL_READ_ERROR         = 04737122352;
    public static final int PyUNHANDLED_EXCEPTION             = 04737122472;
    public static final int Dt196bNO_SAMPLES                  = 04737122612;
    public static final int DevCANNOT_LOAD_SETTINGS           = 04737122642;
    public static final int DevCANNOT_GET_BOARD_STATE         = 04737122652;
    public static final int DevACQCMD_FAILED                  = 04737122662;
    public static final int DevACQ2SH_FAILED                  = 04737122672;
    public static final int DevBAD_PARAMETER                  = 04737122702;
    public static final int DevCOMM_ERROR                     = 04737122712;
    public static final int DevCAMERA_NOT_FOUND               = 04737122722;
    public static final int DevNOT_A_PYDEVICE                 = 04737122732;
    public static final int TreeALREADY_OFF                   = 01764300053;
    public static final int TreeALREADY_ON                    = 01764300063;
    public static final int TreeALREADY_OPEN                  = 01764300073;
    public static final int TreeALREADY_THERE                 = 01764300210;
    public static final int TreeBADRECORD                     = 01764300272;
    public static final int TreeBOTH_OFF                      = 01764300230;
    public static final int TreeBUFFEROVF                     = 01764300422;
    public static final int TreeCONGLOMFULL                   = 01764300442;
    public static final int TreeCONGLOM_NOT_FULL              = 01764300452;
    public static final int TreeCONTINUING                    = 01764304543;
    public static final int TreeDUPTAG                        = 01764300312;
    public static final int TreeEDITTING                      = 01764300622;
    public static final int TreeILLEGAL_ITEM                  = 01764300412;
    public static final int TreeILLPAGCNT                     = 01764300322;
    public static final int TreeINVDFFCLASS                   = 01764300472;
    public static final int TreeINVDTPUSG                     = 01764300612;
    public static final int TreeINVPATH                       = 01764300402;
    public static final int TreeINVRECTYP                     = 01764300502;
    public static final int TreeINVTREE                       = 01764300302;
    public static final int TreeMAXOPENEDIT                   = 01764300332;
    public static final int TreeNEW                           = 01764300033;
    public static final int TreeNMN                           = 01764300140;
    public static final int TreeNMT                           = 01764300150;
    public static final int TreeNNF                           = 01764300160;
    public static final int TreeNODATA                        = 01764300342;
    public static final int TreeNODNAMLEN                     = 01764300512;
    public static final int TreeNOEDIT                        = 01764300362;
    public static final int TreeNOLOG                         = 01764300652;
    public static final int TreeNOMETHOD                      = 01764300260;
    public static final int TreeNOOVERWRITE                   = 01764300602;
    public static final int TreeNORMAL                        = 01764300011;
    public static final int TreeNOTALLSUBS                    = 01764300043;
    public static final int TreeNOTCHILDLESS                  = 01764300372;
    public static final int TreeNOT_IN_LIST                   = 01764300702;
    public static final int TreeNOTMEMBERLESS                 = 01764300562;
    public static final int TreeNOTOPEN                       = 01764300352;
    public static final int TreeNOTSON                        = 01764300572;
    public static final int TreeNOT_CONGLOM                   = 01764300542;
    public static final int TreeNOT_OPEN                      = 01764300250;
    public static final int TreeNOWRITEMODEL                  = 01764300632;
    public static final int TreeNOWRITESHOT                   = 01764300642;
    public static final int TreeNO_CONTEXT                    = 01764300103;
    public static final int TreeOFF                           = 01764300240;
    public static final int TreeON                            = 01764300113;
    public static final int TreeOPEN                          = 01764300123;
    public static final int TreeOPEN_EDIT                     = 01764300133;
    public static final int TreePARENT_OFF                    = 01764300220;
    public static final int TreeREADERR                       = 01764300672;
    public static final int TreeREADONLY                      = 01764300662;
    public static final int TreeRESOLVED                      = 01764300021;
    public static final int TreeSUCCESS                       = 01764303101;
    public static final int TreeTAGNAMLEN                     = 01764300522;
    public static final int TreeTNF                           = 01764300170;
    public static final int TreeTREENF                        = 01764300200;
    public static final int TreeUNRESOLVED                    = 01764300462;
    public static final int TreeUNSPRTCLASS                   = 01764300432;
    public static final int TreeUNSUPARRDTYPE                 = 01764300552;
    public static final int TreeWRITEFIRST                    = 01764300532;
    public static final int TreeFAILURE                       = 01764307642;
    public static final int TreeLOCK_FAILURE                  = 01764307662;
    public static final int TreeFILE_NOT_FOUND                = 01764307652;
    public static final int TreeCANCEL                        = 01764306200;
    public static final int TreeNOSEGMENTS                    = 01764307672;
    public static final int TreeINVDTYPE                      = 01764307702;
    public static final int TreeINVSHAPE                      = 01764307712;
    public static final int TreeINVSHOT                       = 01764307732;
    public static final int TreeINVTAG                        = 01764307752;
    public static final int TreeNOPATH                        = 01764307762;
    public static final int TreeTREEFILEREADERR               = 01764307772;
    public static final int TreeMEMERR                        = 01764310002;
    public static final int TreeNOCURRENT                     = 01764310012;
    public static final int TreeFOPENW                        = 01764310022;
    public static final int TreeFOPENR                        = 01764310032;
    public static final int TreeFCREATE                       = 01764310042;
    public static final int TreeCONNECTFAIL                   = 01764310052;
    public static final int TreeNCIWRITE                      = 01764310062;
    public static final int TreeDELFAIL                       = 01764310072;
    public static final int TreeRENFAIL                       = 01764310102;
    public static final int TreeEMPTY                         = 01764310110;
    public static final int TreePARSEERR                      = 01764310122;
    public static final int TreeNCIREAD                       = 01764310132;
    public static final int TreeNOVERSION                     = 01764310142;
    public static final int TreeDFREAD                        = 01764310152;
    public static final int TreeCLOSEERR                      = 01764310162;
    public static final int TreeMOVEERROR                     = 01764310172;
    public static final int TreeOPENEDITERR                   = 01764310202;
    public static final int TreeREADONLY_TREE                 = 01764310212;
    public static final int TreeWRITETREEERR                  = 01764310222;
    public static final int TreeNOWILD                        = 01764310232;
    public static final int LibINSVIRMEM                      = 05301024;
    public static final int LibINVARG                         = 05301064;
    public static final int LibINVSTRDES                      = 05301044;
    public static final int LibKEYNOTFOU                      = 05301374;
    public static final int LibNOTFOU                         = 05301164;
    public static final int LibQUEWASEMP                      = 05301354;
    public static final int LibSTRTRU                         = 05300021;
    public static final int StrMATCH                          = 011102031;
    public static final int StrNOMATCH                        = 011101010;
    public static final int StrNOELEM                         = 011101030;
    public static final int StrINVDELIM                       = 011101020;
    public static final int StrSTRTOOLON                      = 011100164;
    public static final int MDSplusWARNING                    = 0200000;
    public static final int MDSplusSUCCESS                    = 0200011;
    public static final int MDSplusERROR                      = 0200022;
    public static final int MDSplusFATAL                      = 0200044;
    public static final int SsSUCCESS                         = 01;
    public static final int SsINTOVF                          = 02174;
    public static final int SsINTERNAL                        = -1;
    public static final int TdiBREAK                          = 01764700010;
    public static final int TdiCASE                           = 01764700020;
    public static final int TdiCONTINUE                       = 01764700030;
    public static final int TdiEXTRANEOUS                     = 01764700040;
    public static final int TdiRETURN                         = 01764700050;
    public static final int TdiABORT                          = 01764700062;
    public static final int TdiBAD_INDEX                      = 01764700072;
    public static final int TdiBOMB                           = 01764700102;
    public static final int TdiEXTRA_ARG                      = 01764700112;
    public static final int TdiGOTO                           = 01764700122;
    public static final int TdiINVCLADSC                      = 01764700132;
    public static final int TdiINVCLADTY                      = 01764700142;
    public static final int TdiINVDTYDSC                      = 01764700152;
    public static final int TdiINV_OPC                        = 01764700162;
    public static final int TdiINV_SIZE                       = 01764700172;
    public static final int TdiMISMATCH                       = 01764700202;
    public static final int TdiMISS_ARG                       = 01764700212;
    public static final int TdiNDIM_OVER                      = 01764700222;
    public static final int TdiNO_CMPLX                       = 01764700232;
    public static final int TdiNO_OPC                         = 01764700242;
    public static final int TdiNO_OUTPTR                      = 01764700252;
    public static final int TdiNO_SELF_PTR                    = 01764700262;
    public static final int TdiNOT_NUMBER                     = 01764700272;
    public static final int TdiNULL_PTR                       = 01764700302;
    public static final int TdiRECURSIVE                      = 01764700312;
    public static final int TdiSIG_DIM                        = 01764700322;
    public static final int TdiSYNTAX                         = 01764700332;
    public static final int TdiTOO_BIG                        = 01764700342;
    public static final int TdiUNBALANCE                      = 01764700352;
    public static final int TdiUNKNOWN_VAR                    = 01764700362;
    public static final int TdiSTRTOOLON                      = 01764700374;
    public static final int TdiTIMEOUT                        = 01764700404;
    public static final int ApdAPD_APPEND                     = 01767200012;
    public static final int ApdDICT_KEYVALPAIR                = 01767200022;
    public static final int ApdDICT_KEYCLS                    = 01767200032;
    public static final int MdsdclSUCCESS                     = 01000400011;
    public static final int MdsdclEXIT                        = 01000400021;
    public static final int MdsdclERROR                       = 01000400030;
    public static final int MdsdclNORMAL                      = 01000401451;
    public static final int MdsdclPRESENT                     = 01000401461;
    public static final int MdsdclIVVERB                      = 01000401472;
    public static final int MdsdclABSENT                      = 01000401500;
    public static final int MdsdclNEGATED                     = 01000401510;
    public static final int MdsdclNOTNEGATABLE                = 01000401522;
    public static final int MdsdclIVQUAL                      = 01000401532;
    public static final int MdsdclPROMPT_MORE                 = 01000401542;
    public static final int MdsdclTOO_MANY_PRMS               = 01000401552;
    public static final int MdsdclTOO_MANY_VALS               = 01000401562;
    public static final int MdsdclMISSING_VALUE               = 01000401572;
    public static final int ServerNOT_DISPATCHED              = 01770300010;
    public static final int ServerINVALID_DEPENDENCY          = 01770300022;
    public static final int ServerCANT_HAPPEN                 = 01770300032;
    public static final int ServerINVSHOT                     = 01770300042;
    public static final int ServerABORT                       = 01770300062;
    public static final int ServerPATH_DOWN                   = 01770300102;
    public static final int ServerSOCKET_ADDR_ERROR           = 01770300112;
    public static final int ServerINVALID_ACTION_OPERATION    = 01770300122;
    public static final int CamDONE_Q                         = 01000300011;
    public static final int CamDONE_NOQ                       = 01000300021;
    public static final int CamDONE_NOX                       = 01000310000;
    public static final int CamSERTRAERR                      = 01000314002;
    public static final int CamSCCFAIL                        = 01000314102;
    public static final int CamOFFLINE                        = 01000314152;
    public static final int TclNORMAL                         = 012400011;
    public static final int TclFAILED_ESSENTIAL               = 012400020;
    public static final int TclNO_DISPATCH_TABLE              = 012400030;

    public static final String getMdsMessage(final int status) {
        switch(status){
            case DevBAD_ENDIDX:
                return "%DEV-E-BAD_ENDIDX, unable to read end index for channel";
            case DevBAD_FILTER:
                return "%DEV-E-BAD_FILTER, illegal filter selected";
            case DevBAD_FREQ:
                return "%DEV-E-BAD_FREQ, illegal digitization frequency selected";
            case DevBAD_GAIN:
                return "%DEV-E-BAD_GAIN, illegal gain selected";
            case DevBAD_HEADER:
                return "%DEV-E-BAD_HEADER, unable to read header selection";
            case DevBAD_HEADER_IDX:
                return "%DEV-E-BAD_HEADER_IDX, unknown header configuration index";
            case DevBAD_MEMORIES:
                return "%DEV-E-BAD_MEMORIES, unable to read number of memory modules";
            case DevBAD_MODE:
                return "%DEV-E-BAD_MODE, illegal mode selected";
            case DevBAD_NAME:
                return "%DEV-E-BAD_NAME, unable to read module name";
            case DevBAD_OFFSET:
                return "%DEV-E-BAD_OFFSET, illegal offset selected";
            case DevBAD_STARTIDX:
                return "%DEV-E-BAD_STARTIDX, unable to read start index for channel";
            case DevNOT_TRIGGERED:
                return "%DEV-E-NOT_TRIGGERED, device was not triggered,  check wires and triggering device";
            case DevFREQ_TO_HIGH:
                return "%DEV-E-FREQ_TO_HIGH, the frequency is set to high for the requested number of channels";
            case DevINVALID_NOC:
                return "%DEV-E-INVALID_NOC, the NOC (number of channels) requested is greater than the physical number of channels";
            case DevRANGE_MISMATCH:
                return "%DEV-E-RANGE_MISMATCH, the range specified on the menu doesn't match the range setting on the device";
            case DevCAMACERR:
                return "%DEV-E-CAMACERR, Error doing CAMAC IO";
            case DevBAD_VERBS:
                return "%DEV-E-BAD_VERBS, Error reading interpreter list (:VERBS)";
            case DevBAD_COMMANDS:
                return "%DEV-E-BAD_COMMANDS, Error reading command list";
            case DevCAM_ADNR:
                return "%DEV-E-CAM_ADNR, CAMAC: Address not recognized (2160)";
            case DevCAM_ERR:
                return "%DEV-E-CAM_ERR, CAMAC: Error reported by crate controler";
            case DevCAM_LOSYNC:
                return "%DEV-E-CAM_LOSYNC, CAMAC: Lost Syncronization error";
            case DevCAM_LPE:
                return "%DEV-E-CAM_LPE, CAMAC: Longitudinal Parity error";
            case DevCAM_TMO:
                return "%DEV-E-CAM_TMO, CAMAC: Highway time out error";
            case DevCAM_TPE:
                return "%DEV-E-CAM_TPE, CAMAC: Transverse Parity error";
            case DevCAM_STE:
                return "%DEV-E-CAM_STE, CAMAC: Serial Transmission error";
            case DevCAM_DERR:
                return "%DEV-E-CAM_DERR, CAMAC: Delayed error from SCC";
            case DevCAM_SQ:
                return "%DEV-E-CAM_SQ, CAMAC: I/O completion with Q = 1";
            case DevCAM_NOSQ:
                return "%DEV-E-CAM_NOSQ, CAMAC: I/O completion with Q = 0";
            case DevCAM_SX:
                return "%DEV-E-CAM_SX, CAMAC: I/O completion with X = 1";
            case DevCAM_NOSX:
                return "%DEV-E-CAM_NOSX, CAMAC: I/O completion with X = 0";
            case DevINV_SETUP:
                return "%DEV-E-INV_SETUP, device was not properly set up";
            case DevPYDEVICE_NOT_FOUND:
                return "%DEV-E-PYDEVICE_NOT_FOUND, Python device class not found.";
            case DevPY_INTERFACE_LIBRARY_NOT_FOUND:
                return "%DEV-E-PY_INTERFACE_LIBRARY_NOT_FOUND, The needed device hardware interface library could not be loaded.";
            case DevIO_STUCK:
                return "%DEV-E-IO_STUCK, I/O to Device is stuck. Check network connection and board status.";
            case DevUNKOWN_STATE:
                return "%DEV-E-UNKOWN_STATE, Device returned unrecognized state string";
            case DevWRONG_TREE:
                return "%DEV-E-WRONG_TREE, Attempt to digitizerinto different tree than it was armed with";
            case DevWRONG_PATH:
                return "%DEV-E-WRONG_PATH, Attempt to store digitizer into different path than it was armed with";
            case DevWRONG_SHOT:
                return "%DEV-E-WRONG_SHOT, Attempt to store digitizer into different shot than it was armed with";
            case DevOFFLINE:
                return "%DEV-E-OFFLINE, Device is not on line.  Check network connection";
            case DevTRIGGERED_NOT_STORED:
                return "%DEV-E-TRIGGERED_NOT_STORED, Device was triggered but not stored.";
            case DevNO_NAME_SPECIFIED:
                return "%DEV-E-NO_NAME_SPECIFIED, Device name must be specifed - pleas fill it in.";
            case DevBAD_ACTIVE_CHAN:
                return "%DEV-E-BAD_ACTIVE_CHAN, Active channels either not available or invalid";
            case DevBAD_TRIG_SRC:
                return "%DEV-E-BAD_TRIG_SRC, Trigger source either not available or invalid";
            case DevBAD_CLOCK_SRC:
                return "%DEV-E-BAD_CLOCK_SRC, Clock source either not available or invalid";
            case DevBAD_PRE_TRIG:
                return "%DEV-E-BAD_PRE_TRIG, Pre trigger samples either not available or invalid";
            case DevBAD_POST_TRIG:
                return "%DEV-E-BAD_POST_TRIG, Post trigger samples either not available or invalid";
            case DevBAD_CLOCK_FREQ:
                return "%DEV-E-BAD_CLOCK_FREQ, Clock frequency either not available or invalid";
            case DevTRIGGER_FAILED:
                return "%DEV-E-TRIGGER_FAILED, Device trigger method failed";
            case DevERROR_READING_CHANNEL:
                return "%DEV-E-ERROR_READING_CHANNEL, Error reading data for channel from device";
            case DevERROR_DOING_INIT:
                return "%DEV-E-ERROR_DOING_INIT, Error sending ARM command to device";
            case ReticonNORMAL:
                return "%RETICON-S-NORMAL, successful completion";
            case ReticonBAD_FRAMES:
                return "%RETICON-E-BAD_FRAMES, frame count must be less than or equal to 2048";
            case ReticonBAD_FRAME_SELECT:
                return "%RETICON-E-BAD_FRAME_SELECT, frame interval must be 1,2,4,8,16,32 or 64";
            case ReticonBAD_NUM_STATES:
                return "%RETICON-E-BAD_NUM_STATES, number of states must be between 1 and 4";
            case ReticonBAD_PERIOD:
                return "%RETICON-E-BAD_PERIOD, period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec";
            case ReticonBAD_PIXEL_SELECT:
                return "%RETICON-E-BAD_PIXEL_SELECT, pixel selection must be an array of 256 boolean values";
            case ReticonDATA_CORRUPTED:
                return "%RETICON-E-DATA_CORRUPTED, data in memory is corrupted or framing error detected, no data stored";
            case ReticonTOO_MANY_FRAMES:
                return "%RETICON-E-TOO_MANY_FRAMES, over 8192 frame start indicators in data read from memory";
            case J221NORMAL:
                return "%J221-S-NORMAL, successful completion";
            case J221INVALID_DATA:
                return "%J221-F-INVALID_DATA, ignoring invalid data in channel !SL";
            case J221NO_DATA:
                return "%J221-E-NO_DATA, no valid data was found for any channel";
            case TimingINVCLKFRQ:
                return "%TIMING-E-INVCLKFRQ, Invalid clock frequency";
            case TimingINVDELDUR:
                return "%TIMING-E-INVDELDUR, Invalid pulse delay or duration, must be less than 655 seconds";
            case TimingINVOUTCTR:
                return "%TIMING-E-INVOUTCTR, Invalid output mode selected";
            case TimingINVPSEUDODEV:
                return "%TIMING-E-INVPSEUDODEV, Invalid pseudo device attached to this decoder channel";
            case TimingINVTRGMOD:
                return "%TIMING-E-INVTRGMOD, Invalid trigger mode selected";
            case TimingNOPSEUDODEV:
                return "%TIMING-I-NOPSEUDODEV, No Pseudo device attached to this channel ... disabling";
            case TimingTOO_MANY_EVENTS:
                return "%TIMING-E-TOO_MANY_EVENTS, More than 16 events used by this decoder";
            case B2408NORMAL:
                return "%B2408-S-NORMAL, successful completion";
            case B2408OVERFLOW:
                return "%B2408-I-OVERFLOW, Triggers received after overflow";
            case B2408TRIG_LIM:
                return "%B2408-F-TRIG_LIM, Trigger limit possibly exceeded";
            case FeraNORMAL:
                return "%FERA-S-NORMAL, successful completion";
            case FeraDIGNOTSTRARRAY:
                return "%FERA-E-DIGNOTSTRARRAY, The digitizer names must be an array of strings";
            case FeraNODIG:
                return "%FERA-E-NODIG, The digitizer names must be specified";
            case FeraMEMNOTSTRARRAY:
                return "%FERA-E-MEMNOTSTRARRAY, The memory names must be an array of strings";
            case FeraNOMEM:
                return "%FERA-E-NOMEM, The memory names must be specified";
            case FeraPHASE_LOST:
                return "%FERA-E-PHASE_LOST, Data phase lost No FERA data stored";
            case FeraCONFUSED:
                return "%FERA-F-CONFUSED, Fera Data inconsitant.  Data for this point zered.";
            case FeraOVER_RUN:
                return "%FERA-F-OVER_RUN, Possible FERA memory overrun, too many triggers.";
            case FeraOVERFLOW:
                return "%FERA-I-OVERFLOW, Possible FERA data saturated.  Data point zeroed";
            case Hm650NORMAL:
                return "%HM650-S-NORMAL, successful completion";
            case Hm650DLYCHNG:
                return "%HM650-F-DLYCHNG, HM650 requested delay can not be processed by hardware.";
            case Hv4032NORMAL:
                return "%HV4032-S-NORMAL, successful completion";
            case Hv4032WRONG_POD_TYPE:
                return "%HV4032-E-WRONG_POD_TYPE, HV40321A n and p can only be used with the HV4032 device";
            case Hv1440NORMAL:
                return "%HV1440-S-NORMAL, successful completion";
            case Hv1440WRONG_POD_TYPE:
                return "%HV1440-E-WRONG_POD_TYPE, HV1443 can only be used with the HV1440 device";
            case Hv1440BAD_FRAME:
                return "%HV1440-E-BAD_FRAME, HV1440 could not read the frame";
            case Hv1440BAD_RANGE:
                return "%HV1440-E-BAD_RANGE, HV1440 could not read the range";
            case Hv1440OUTRNG:
                return "%HV1440-E-OUTRNG, HV1440 out of range";
            case Hv1440STUCK:
                return "%HV1440-E-STUCK, HV1440 not responding with Q";
            case JoergerBAD_PRE_TRIGGER:
                return "%JOERGER-E-BAD_PRE_TRIGGER, bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7";
            case JoergerBAD_ACT_MEMORY:
                return "%JOERGER-E-BAD_ACT_MEMORY, bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8";
            case JoergerBAD_GAIN:
                return "%JOERGER-E-BAD_GAIN, bad gain specified, specify a value of 1,2,4 or 8";
            case U_of_mBAD_WAVE_LENGTH:
                return "%U_OF_M-E-BAD_WAVE_LENGTH, bad wave length specified, specify value between 0 and 13000";
            case U_of_mBAD_SLIT_WIDTH:
                return "%U_OF_M-E-BAD_SLIT_WIDTH, bad slit width specified, specify value between 0 and 500";
            case U_of_mBAD_NUM_SPECTRA:
                return "%U_OF_M-E-BAD_NUM_SPECTRA, bad number of spectra specified, specify value between 1 and 100";
            case U_of_mBAD_GRATING:
                return "%U_OF_M-E-BAD_GRATING, bad grating type specified, specify value between 1 and 5";
            case U_of_mBAD_EXPOSURE:
                return "%U_OF_M-E-BAD_EXPOSURE, bad exposure time specified, specify value between 30 and 3000";
            case U_of_mBAD_FILTER:
                return "%U_OF_M-E-BAD_FILTER, bad neutral density filter specified, specify value between 0 and 5";
            case U_of_mGO_FILE_ERROR:
                return "%U_OF_M-E-GO_FILE_ERROR, error creating new go file";
            case U_of_mDATA_FILE_ERROR:
                return "%U_OF_M-E-DATA_FILE_ERROR, error opening datafile";
            case IdlNORMAL:
                return "%IDL-S-NORMAL, successful completion";
            case IdlERROR:
                return "%IDL-E-ERROR, IDL returned a non zero error code";
            case B5910aBAD_CHAN:
                return "%B5910A-E-BAD_CHAN, error evaluating data for channel !SL";
            case B5910aBAD_CLOCK:
                return "%B5910A-E-BAD_CLOCK, invalid internal clock range specified";
            case B5910aBAD_ITERATIONS:
                return "%B5910A-E-BAD_ITERATIONS, invalid number of iterations specified";
            case B5910aBAD_NOC:
                return "%B5910A-E-BAD_NOC, invalid number of active channels specified";
            case B5910aBAD_SAMPS:
                return "%B5910A-E-BAD_SAMPS, number of samples specificed invalid";
            case J412NOT_SORTED:
                return "%J412-E-NOT_SORTED, times specified for J412 module were not sorted";
            case J412NO_DATA:
                return "%J412-E-NO_DATA, there were no times specifed for J412 module";
            case J412BADCYCLES:
                return "%J412-E-BADCYCLES, The number of cycles must be 1 .. 255";
            case Tr16NORMAL:
                return "%TR16-S-NORMAL, successful completion";
            case Tr16BAD_MEMSIZE:
                return "%TR16-E-BAD_MEMSIZE, Memory size must be in 128K, 256K, 512k, 1024K";
            case Tr16BAD_ACTIVEMEM:
                return "%TR16-E-BAD_ACTIVEMEM, Active Mem must be power of 2 8K to 1024K";
            case Tr16BAD_ACTIVECHAN:
                return "%TR16-E-BAD_ACTIVECHAN, Active channels must be in 1,2,4,8,16";
            case Tr16BAD_PTS:
                return "%TR16-E-BAD_PTS, PTS must be power of 2 32 to 1024K";
            case Tr16BAD_FREQUENCY:
                return "%TR16-E-BAD_FREQUENCY, Invalid clock frequency";
            case Tr16BAD_MASTER:
                return "%TR16-E-BAD_MASTER, Master must be 0 or 1";
            case Tr16BAD_GAIN:
                return "%TR16-E-BAD_GAIN, Gain must be 1, 2, 4, or 8";
            case A14NORMAL:
                return "%A14-S-NORMAL, successful completion";
            case A14BAD_CLK_DIVIDE:
                return "%A14-E-BAD_CLK_DIVIDE, Clock divide must be one of 1,2,4,10,20,40, or 100";
            case A14BAD_MODE:
                return "%A14-E-BAD_MODE, Mode must be in the range of 0 to 4";
            case A14BAD_CLK_POLARITY:
                return "%A14-E-BAD_CLK_POLARITY, Clock polarity must be either 0 (rising) or 1 (falling)";
            case A14BAD_STR_POLARITY:
                return "%A14-E-BAD_STR_POLARITY, Start polarity must be either 0 (rising) or 1 (falling)";
            case A14BAD_STP_POLARITY:
                return "%A14-E-BAD_STP_POLARITY, Stop polarity must be either 0 (rising) or 1 (falling)";
            case A14BAD_GATED:
                return "%A14-E-BAD_GATED, Gated clock must be either 0 (not gated) or 1 (gated)";
            case L6810NORMAL:
                return "%L6810-S-NORMAL, successful completion";
            case L6810BAD_ACTIVECHAN:
                return "%L6810-E-BAD_ACTIVECHAN, Active chans must be 1, 2, or 4";
            case L6810BAD_ACTIVEMEM:
                return "%L6810-E-BAD_ACTIVEMEM, Active memory must be power of 2 LE 8192";
            case L6810BAD_FREQUENCY:
                return "%L6810-E-BAD_FREQUENCY, Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]";
            case L6810BAD_FULL_SCALE:
                return "%L6810-E-BAD_FULL_SCALE, Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]";
            case L6810BAD_MEMORIES:
                return "%L6810-E-BAD_MEMORIES, Memories must 1 .. 16";
            case L6810BAD_COUPLING:
                return "%L6810-E-BAD_COUPLING, Channel source / coupling must be one of 0 .. 7";
            case L6810BAD_OFFSET:
                return "%L6810-E-BAD_OFFSET, Offset must be between 0 and 255";
            case L6810BAD_SEGMENTS:
                return "%L6810-E-BAD_SEGMENTS, Number of segments must be 1 .. 1024";
            case L6810BAD_TRIG_DELAY:
                return "%L6810-E-BAD_TRIG_DELAY, Trigger delay must be between -8 and 247 in units of 8ths of segment size";
            case J_dacOUTRNG:
                return "%J_DAC-E-OUTRNG, Joerger DAC Channels out of range.  Bad chans code !XW";
            case IncaaBAD_ACTIVE_CHANS:
                return "%INCAA-E-BAD_ACTIVE_CHANS, bad active channels selection";
            case IncaaBAD_MASTER:
                return "%INCAA-E-BAD_MASTER, bad master selection, must be 0 or 1";
            case IncaaBAD_EXT_1MHZ:
                return "%INCAA-E-BAD_EXT_1MHZ, bad ext 1mhz selection, must be 0 or 1";
            case IncaaBAD_PTSC:
                return "%INCAA-E-BAD_PTSC, bad PTSC setting";
            case L8212BAD_HEADER:
                return "%L8212-E-BAD_HEADER, Invalid header jumper information (e.g. 49414944432)";
            case L8212BAD_MEMORIES:
                return "%L8212-E-BAD_MEMORIES, Invalid number of memories, must be 1 .. 16";
            case L8212BAD_NOC:
                return "%L8212-E-BAD_NOC, Invalid number of active channels";
            case L8212BAD_OFFSET:
                return "%L8212-E-BAD_OFFSET, Invalid offset must be one of (0, -2048, -4096)";
            case L8212BAD_PTS:
                return "%L8212-E-BAD_PTS, Invalid pts code, must be 0 .. 7";
            case L8212FREQ_TO_HIGH:
                return "%L8212-E-FREQ_TO_HIGH, Frequency to high for selected number of channels";
            case L8212INVALID_NOC:
                return "%L8212-E-INVALID_NOC, Invalid number of active channels";
            case MpbBADTIME:
                return "%MPB-E-BADTIME, Could not read time";
            case MpbBADFREQ:
                return "%MPB-E-BADFREQ, Could not read frequency";
            case L8828BAD_OFFSET:
                return "%L8828-E-BAD_OFFSET, Offset for L8828 must be between 0 and 255";
            case L8828BAD_PRETRIG:
                return "%L8828-E-BAD_PRETRIG, Pre trigger samples for L8828 must be betwwen 0 and 7 eighths";
            case L8828BAD_ACTIVEMEM:
                return "%L8828-E-BAD_ACTIVEMEM, ACTIVEMEM must be beteen 16K and 2M";
            case L8828BAD_CLOCK:
                return "%L8828-E-BAD_CLOCK, Invalid clock frequency specified.";
            case L8818BAD_OFFSET:
                return "%L8818-E-BAD_OFFSET, Offset for L8828 must be between 0 and 255";
            case L8818BAD_PRETRIG:
                return "%L8818-E-BAD_PRETRIG, Pre trigger samples for L8828 must be betwwen 0 and 7 eighths";
            case L8818BAD_ACTIVEMEM:
                return "%L8818-E-BAD_ACTIVEMEM, ACTIVEMEM must be beteen 16K and 2M";
            case L8818BAD_CLOCK:
                return "%L8818-E-BAD_CLOCK, Invalid clock frequency specified.";
            case J_tr612BAD_ACTMEM:
                return "%J_TR612-E-BAD_ACTMEM, ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all";
            case J_tr612BAD_PRETRIG:
                return "%J_TR612-E-BAD_PRETRIG, PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples";
            case J_tr612BAD_MODE:
                return "%J_TR612-E-BAD_MODE, MODE value out of range, must be 0 (for normal) or 1 (for burst mode)";
            case J_tr612BAD_FREQUENCY:
                return "%J_TR612-E-BAD_FREQUENCY, FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external";
            case L8206NODATA:
                return "%L8206-F-NODATA, no data has been written to memory";
            case H912BAD_CLOCK:
                return "%H912-E-BAD_CLOCK, Bad value specified in INT_CLOCK node, use Setup device to correct";
            case H912BAD_BLOCKS:
                return "%H912-E-BAD_BLOCKS, Bad value specified in BLOCKS node, use Setup device to correct";
            case H912BAD_PTS:
                return "%H912-E-BAD_PTS, Bad value specfiied in PTS node, must be an integer value between 1 and 131071";
            case H908BAD_CLOCK:
                return "%H908-E-BAD_CLOCK, Bad value specified in INT_CLOCK node, use Setup device to correct";
            case H908BAD_ACTIVE_CHANS:
                return "%H908-E-BAD_ACTIVE_CHANS, Bad value specified in ACTIVE_CHANS node, use Setup device to correct";
            case H908BAD_PTS:
                return "%H908-E-BAD_PTS, Bad value specfiied in PTS node, must be an integer value between 1 and 131071";
            case Dsp2904CHANNEL_READ_ERROR:
                return "%DSP2904-E-CHANNEL_READ_ERROR, Error reading channel";
            case PyUNHANDLED_EXCEPTION:
                return "%PY-E-UNHANDLED_EXCEPTION, Python device raised an exception, see log files for more details";
            case Dt196bNO_SAMPLES:
                return "%DT196B-E-NO_SAMPLES, Module did not acquire any samples";
            case DevCANNOT_LOAD_SETTINGS:
                return "%DEV-E-CANNOT_LOAD_SETTINGS, Error loading settings from XML";
            case DevCANNOT_GET_BOARD_STATE:
                return "%DEV-E-CANNOT_GET_BOARD_STATE, Cannot retrieve state of daq board";
            case DevACQCMD_FAILED:
                return "%DEV-E-ACQCMD_FAILED, Error executing acqcmd on daq board";
            case DevACQ2SH_FAILED:
                return "%DEV-E-ACQ2SH_FAILED, Error executing acq2sh command on daq board";
            case DevBAD_PARAMETER:
                return "%DEV-E-BAD_PARAMETER, Invalid parameter specified for device";
            case DevCOMM_ERROR:
                return "%DEV-E-COMM_ERROR, Error communicating with device";
            case DevCAMERA_NOT_FOUND:
                return "%DEV-E-CAMERA_NOT_FOUND, Could not find specified camera on the network";
            case DevNOT_A_PYDEVICE:
                return "%DEV-E-NOT_A_PYDEVICE, Device is not a python device.";
            case TreeALREADY_OFF:
                return "%TREE-I-ALREADY_OFF, Node is already OFF";
            case TreeALREADY_ON:
                return "%TREE-I-ALREADY_ON, Node is already ON";
            case TreeALREADY_OPEN:
                return "%TREE-I-ALREADY_OPEN, Tree is already OPEN";
            case TreeALREADY_THERE:
                return "%TREE-W-ALREADY_THERE, Node is already in the tree";
            case TreeBADRECORD:
                return "%TREE-E-BADRECORD, Data corrupted: cannot read record";
            case TreeBOTH_OFF:
                return "%TREE-W-BOTH_OFF, Both this node and its parent are off";
            case TreeBUFFEROVF:
                return "%TREE-E-BUFFEROVF, Output buffer overflow";
            case TreeCONGLOMFULL:
                return "%TREE-E-CONGLOMFULL, Current conglomerate is full";
            case TreeCONGLOM_NOT_FULL:
                return "%TREE-E-CONGLOM_NOT_FULL, Current conglomerate is not yet full";
            case TreeCONTINUING:
                return "%TREE-I-CONTINUING, Operation continuing: note following error";
            case TreeDUPTAG:
                return "%TREE-E-DUPTAG, Tag name already in use";
            case TreeEDITTING:
                return "%TREE-E-EDITTING, Tree file open for edit: operation not permitted";
            case TreeILLEGAL_ITEM:
                return "%TREE-E-ILLEGAL_ITEM, Invalid item code or part number specified";
            case TreeILLPAGCNT:
                return "%TREE-E-ILLPAGCNT, Illegal page count, error mapping tree file";
            case TreeINVDFFCLASS:
                return "%TREE-E-INVDFFCLASS, Invalid data fmt: only CLASS_S can have data in NCI";
            case TreeINVDTPUSG:
                return "%TREE-E-INVDTPUSG, Attempt to store datatype which conflicts with the designated usage of this node";
            case TreeINVPATH:
                return "%TREE-E-INVPATH, Invalid tree pathname specified";
            case TreeINVRECTYP:
                return "%TREE-E-INVRECTYP, Record type invalid for requested operation";
            case TreeINVTREE:
                return "%TREE-E-INVTREE, Invalid tree identification structure";
            case TreeMAXOPENEDIT:
                return "%TREE-E-MAXOPENEDIT, Too many files open for edit";
            case TreeNEW:
                return "%TREE-I-NEW, New tree created";
            case TreeNMN:
                return "%TREE-W-NMN, No More Nodes";
            case TreeNMT:
                return "%TREE-W-NMT, No More Tags";
            case TreeNNF:
                return "%TREE-W-NNF, Node Not Found";
            case TreeNODATA:
                return "%TREE-E-NODATA, No data available for this node";
            case TreeNODNAMLEN:
                return "%TREE-E-NODNAMLEN, Node name too long (12 chars max)";
            case TreeNOEDIT:
                return "%TREE-E-NOEDIT, Tree file is not open for edit";
            case TreeNOLOG:
                return "%TREE-E-NOLOG, Experiment pathname (xxx_path) not defined";
            case TreeNOMETHOD:
                return "%TREE-W-NOMETHOD, Method not available for this object";
            case TreeNOOVERWRITE:
                return "%TREE-E-NOOVERWRITE, Write-once node: overwrite not permitted";
            case TreeNORMAL:
                return "%TREE-S-NORMAL, Normal successful completion";
            case TreeNOTALLSUBS:
                return "%TREE-I-NOTALLSUBS, Main tree opened but not all subtrees found/or connected";
            case TreeNOTCHILDLESS:
                return "%TREE-E-NOTCHILDLESS, Node must be childless to become subtree reference";
            case TreeNOT_IN_LIST:
                return "%TREE-E-NOT_IN_LIST, Tree being opened was not in the list";
            case TreeNOTMEMBERLESS:
                return "%TREE-E-NOTMEMBERLESS, Subtree reference can not have members";
            case TreeNOTOPEN:
                return "%TREE-E-NOTOPEN, No tree file currently open";
            case TreeNOTSON:
                return "%TREE-E-NOTSON, Subtree reference cannot be a member";
            case TreeNOT_CONGLOM:
                return "%TREE-E-NOT_CONGLOM, Head node of conglomerate does not contain a DTYPE_CONGLOM record";
            case TreeNOT_OPEN:
                return "%TREE-W-NOT_OPEN, Tree not currently open";
            case TreeNOWRITEMODEL:
                return "%TREE-E-NOWRITEMODEL, Data for this node can not be written into the MODEL file";
            case TreeNOWRITESHOT:
                return "%TREE-E-NOWRITESHOT, Data for this node can not be written into the SHOT file";
            case TreeNO_CONTEXT:
                return "%TREE-I-NO_CONTEXT, There is no active search to end";
            case TreeOFF:
                return "%TREE-W-OFF, Node is OFF";
            case TreeON:
                return "%TREE-I-ON, Node is ON";
            case TreeOPEN:
                return "%TREE-I-OPEN, Tree is OPEN (no edit)";
            case TreeOPEN_EDIT:
                return "%TREE-I-OPEN_EDIT, Tree is OPEN for edit";
            case TreePARENT_OFF:
                return "%TREE-W-PARENT_OFF, Parent of this node is OFF";
            case TreeREADERR:
                return "%TREE-E-READERR, Error reading record for node";
            case TreeREADONLY:
                return "%TREE-E-READONLY, Tree was opened with readonly access";
            case TreeRESOLVED:
                return "%TREE-S-RESOLVED, Indirect reference successfully resolved";
            case TreeSUCCESS:
                return "%TREE-S-SUCCESS, Operation successful";
            case TreeTAGNAMLEN:
                return "%TREE-E-TAGNAMLEN, Tagname too long (max 24 chars)";
            case TreeTNF:
                return "%TREE-W-TNF, Tag Not Found";
            case TreeTREENF:
                return "%TREE-W-TREENF, Tree Not Found";
            case TreeUNRESOLVED:
                return "%TREE-E-UNRESOLVED, Not an indirect node reference: No action taken";
            case TreeUNSPRTCLASS:
                return "%TREE-E-UNSPRTCLASS, Unsupported descriptor class";
            case TreeUNSUPARRDTYPE:
                return "%TREE-E-UNSUPARRDTYPE, Complex data types not supported as members of arrays";
            case TreeWRITEFIRST:
                return "%TREE-E-WRITEFIRST, Tree has been modified:  write or quit first";
            case TreeFAILURE:
                return "%TREE-E-FAILURE, Operation NOT successful";
            case TreeLOCK_FAILURE:
                return "%TREE-E-LOCK_FAILURE, Error locking file, perhaps NFSLOCKING not enabled on this system";
            case TreeFILE_NOT_FOUND:
                return "%TREE-E-FILE_NOT_FOUND, File or Directory Not Found";
            case TreeCANCEL:
                return "%TREE-W-CANCEL, User canceled operation";
            case TreeNOSEGMENTS:
                return "%TREE-E-NOSEGMENTS, No segments exist in this node";
            case TreeINVDTYPE:
                return "%TREE-E-INVDTYPE, Invalid datatype for data segment";
            case TreeINVSHAPE:
                return "%TREE-E-INVSHAPE, Invalid shape for this data segment";
            case TreeINVSHOT:
                return "%TREE-E-INVSHOT, Invalid shot number - must be -1 (model), 0 (current), or Positive";
            case TreeINVTAG:
                return "%TREE-E-INVTAG, Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores";
            case TreeNOPATH:
                return "%TREE-E-NOPATH, No 'treename'_path environment variable defined. Cannot locate tree files.";
            case TreeTREEFILEREADERR:
                return "%TREE-E-TREEFILEREADERR, Error reading in tree file contents.";
            case TreeMEMERR:
                return "%TREE-E-MEMERR, Memory allocation error.";
            case TreeNOCURRENT:
                return "%TREE-E-NOCURRENT, No current shot number set for this tree.";
            case TreeFOPENW:
                return "%TREE-E-FOPENW, Error opening file for read-write.";
            case TreeFOPENR:
                return "%TREE-E-FOPENR, Error opening file read-only.";
            case TreeFCREATE:
                return "%TREE-E-FCREATE, Error creating new file.";
            case TreeCONNECTFAIL:
                return "%TREE-E-CONNECTFAIL, Error connecting to remote server.";
            case TreeNCIWRITE:
                return "%TREE-E-NCIWRITE, Error writing node characterisitics to file.";
            case TreeDELFAIL:
                return "%TREE-E-DELFAIL, Error deleting file.";
            case TreeRENFAIL:
                return "%TREE-E-RENFAIL, Error renaming file.";
            case TreeEMPTY:
                return "%TREE-W-EMPTY, Empty string provided.";
            case TreePARSEERR:
                return "%TREE-E-PARSEERR, Invalid node search string.";
            case TreeNCIREAD:
                return "%TREE-E-NCIREAD, Error reading node characteristics from file.";
            case TreeNOVERSION:
                return "%TREE-E-NOVERSION, No version available.";
            case TreeDFREAD:
                return "%TREE-E-DFREAD, Error reading from datafile.";
            case TreeCLOSEERR:
                return "%TREE-E-CLOSEERR, Error closing temporary tree file.";
            case TreeMOVEERROR:
                return "%TREE-E-MOVEERROR, Error replacing original treefile with new one.";
            case TreeOPENEDITERR:
                return "%TREE-E-OPENEDITERR, Error reopening new treefile for write access.";
            case TreeREADONLY_TREE:
                return "%TREE-E-READONLY_TREE, Tree is marked as readonly. No write operations permitted.";
            case TreeWRITETREEERR:
                return "%TREE-E-WRITETREEERR, Error writing .tree file";
            case TreeNOWILD:
                return "%TREE-E-NOWILD, No wildcard characters permitted in node specifier";
            case LibINSVIRMEM:
                return "%LIB-F-INSVIRMEM, Insufficient virtual memory";
            case LibINVARG:
                return "%LIB-F-INVARG, Invalid argument";
            case LibINVSTRDES:
                return "%LIB-F-INVSTRDES, Invalid string descriptor";
            case LibKEYNOTFOU:
                return "%LIB-F-KEYNOTFOU, Key not found";
            case LibNOTFOU:
                return "%LIB-F-NOTFOU, Entity not found";
            case LibQUEWASEMP:
                return "%LIB-F-QUEWASEMP, Queue was empty";
            case LibSTRTRU:
                return "%LIB-S-STRTRU, String truncated";
            case StrMATCH:
                return "%STR-S-MATCH, Strings match";
            case StrNOMATCH:
                return "%STR-W-NOMATCH, Strings do not match";
            case StrNOELEM:
                return "%STR-W-NOELEM, Not enough delimited characters";
            case StrINVDELIM:
                return "%STR-W-INVDELIM, Not enough delimited characters";
            case StrSTRTOOLON:
                return "%STR-F-STRTOOLON, String too long";
            case MDSplusWARNING:
                return "%MDSPLUS-W-WARNING, Warning";
            case MDSplusSUCCESS:
                return "%MDSPLUS-S-SUCCESS, Success";
            case MDSplusERROR:
                return "%MDSPLUS-E-ERROR, Error";
            case MDSplusFATAL:
                return "%MDSPLUS-F-FATAL, Fatal";
            case SsSUCCESS:
                return "%SS-S-SUCCESS, Success";
            case SsINTOVF:
                return "%SS-F-INTOVF, Integer overflow";
            case SsINTERNAL:
                return "%SS-?-INTERNAL, This status is meant for internal use only, you should never have seen this message.";
            case TdiBREAK:
                return "%TDI-W-BREAK, BREAK was not in DO FOR SWITCH or WHILE";
            case TdiCASE:
                return "%TDI-W-CASE, CASE was not in SWITCH statement";
            case TdiCONTINUE:
                return "%TDI-W-CONTINUE, CONTINUE was not in DO FOR or WHILE";
            case TdiEXTRANEOUS:
                return "%TDI-W-EXTRANEOUS, Some characters were unused, bad number maybe";
            case TdiRETURN:
                return "%TDI-W-RETURN, Extraneous RETURN statement, not from a FUN";
            case TdiABORT:
                return "%TDI-E-ABORT, Program requested abort";
            case TdiBAD_INDEX:
                return "%TDI-E-BAD_INDEX, Index or subscript is too small or too big";
            case TdiBOMB:
                return "%TDI-E-BOMB, Bad punctuation, could not compile the text";
            case TdiEXTRA_ARG:
                return "%TDI-E-EXTRA_ARG, Too many arguments for function, watch commas";
            case TdiGOTO:
                return "%TDI-E-GOTO, GOTO target label not found";
            case TdiINVCLADSC:
                return "%TDI-E-INVCLADSC, Storage class not valid, must be scalar or array";
            case TdiINVCLADTY:
                return "%TDI-E-INVCLADTY, Invalid mixture of storage class and data type";
            case TdiINVDTYDSC:
                return "%TDI-E-INVDTYDSC, Storage data type is not valid";
            case TdiINV_OPC:
                return "%TDI-E-INV_OPC, Invalid operator code in a function";
            case TdiINV_SIZE:
                return "%TDI-E-INV_SIZE, Number of elements does not match declaration";
            case TdiMISMATCH:
                return "%TDI-E-MISMATCH, Shape of arguments does not match";
            case TdiMISS_ARG:
                return "%TDI-E-MISS_ARG, Missing argument is required for function";
            case TdiNDIM_OVER:
                return "%TDI-E-NDIM_OVER, Number of dimensions is over the allowed 8";
            case TdiNO_CMPLX:
                return "%TDI-E-NO_CMPLX, There are no complex forms of this function";
            case TdiNO_OPC:
                return "%TDI-E-NO_OPC, No support for this function, today";
            case TdiNO_OUTPTR:
                return "%TDI-E-NO_OUTPTR, An output pointer is required";
            case TdiNO_SELF_PTR:
                return "%TDI-E-NO_SELF_PTR, No $VALUE is defined for signal or validation";
            case TdiNOT_NUMBER:
                return "%TDI-E-NOT_NUMBER, Value is not a scalar number and must be";
            case TdiNULL_PTR:
                return "%TDI-E-NULL_PTR, Null pointer where value needed";
            case TdiRECURSIVE:
                return "%TDI-E-RECURSIVE, Overly recursive function, calls itself maybe";
            case TdiSIG_DIM:
                return "%TDI-E-SIG_DIM, Signal dimension does not match data shape";
            case TdiSYNTAX:
                return "%TDI-E-SYNTAX, Bad punctuation or misspelled word or number";
            case TdiTOO_BIG:
                return "%TDI-E-TOO_BIG, Conversion of number lost significant digits";
            case TdiUNBALANCE:
                return "%TDI-E-UNBALANCE, Unbalanced () [] {} ''  or /**/";
            case TdiUNKNOWN_VAR:
                return "%TDI-E-UNKNOWN_VAR, Unknown/undefined variable name";
            case TdiSTRTOOLON:
                return "%TDI-F-STRTOOLON, string is too long (greater than 65535)";
            case TdiTIMEOUT:
                return "%TDI-F-TIMEOUT, task did not complete in alotted time";
            case ApdAPD_APPEND:
                return "%APD-E-APD_APPEND, First argument must be APD or *";
            case ApdDICT_KEYVALPAIR:
                return "%APD-E-DICT_KEYVALPAIR, A Dictionary requires an even number of elements";
            case ApdDICT_KEYCLS:
                return "%APD-E-DICT_KEYCLS, Keys must be scalar, i.e. CLASS_S";
            case MdsdclSUCCESS:
                return "%MDSDCL-S-SUCCESS, Normal successful completion";
            case MdsdclEXIT:
                return "%MDSDCL-S-EXIT, Normal exit";
            case MdsdclERROR:
                return "%MDSDCL-W-ERROR, Unsuccessful execution of command";
            case MdsdclNORMAL:
                return "%MDSDCL-S-NORMAL, Normal successful completion";
            case MdsdclPRESENT:
                return "%MDSDCL-S-PRESENT, Entity is present";
            case MdsdclIVVERB:
                return "%MDSDCL-E-IVVERB, No such command";
            case MdsdclABSENT:
                return "%MDSDCL-W-ABSENT, Entity is absent";
            case MdsdclNEGATED:
                return "%MDSDCL-W-NEGATED, Entity is present but negated";
            case MdsdclNOTNEGATABLE:
                return "%MDSDCL-E-NOTNEGATABLE, Entity cannot be negated";
            case MdsdclIVQUAL:
                return "%MDSDCL-E-IVQUAL, Invalid qualifier";
            case MdsdclPROMPT_MORE:
                return "%MDSDCL-E-PROMPT_MORE, More input required for command";
            case MdsdclTOO_MANY_PRMS:
                return "%MDSDCL-E-TOO_MANY_PRMS, Too many parameters specified";
            case MdsdclTOO_MANY_VALS:
                return "%MDSDCL-E-TOO_MANY_VALS, Too many values";
            case MdsdclMISSING_VALUE:
                return "%MDSDCL-E-MISSING_VALUE, Qualifier value needed";
            case ServerNOT_DISPATCHED:
                return "%SERVER-W-NOT_DISPATCHED, action not dispatched, depended on failed action";
            case ServerINVALID_DEPENDENCY:
                return "%SERVER-E-INVALID_DEPENDENCY, action dependency cannot be evaluated";
            case ServerCANT_HAPPEN:
                return "%SERVER-E-CANT_HAPPEN, action contains circular dependency or depends on action which was not dispatched";
            case ServerINVSHOT:
                return "%SERVER-E-INVSHOT, invalid shot number, cannot dispatch actions in model";
            case ServerABORT:
                return "%SERVER-E-ABORT, Server action was aborted";
            case ServerPATH_DOWN:
                return "%SERVER-E-PATH_DOWN, Path to server lost";
            case ServerSOCKET_ADDR_ERROR:
                return "%SERVER-E-SOCKET_ADDR_ERROR, Cannot obtain ip address socket is bound to.";
            case ServerINVALID_ACTION_OPERATION:
                return "%SERVER-E-INVALID_ACTION_OPERATION, None";
            case CamDONE_Q:
                return "%CAM-S-DONE_Q, I/O completed with X=1, Q=1";
            case CamDONE_NOQ:
                return "%CAM-S-DONE_NOQ, I/O completed with X=1, Q=0";
            case CamDONE_NOX:
                return "%CAM-W-DONE_NOX, I/O completed with X=0 - probable failure";
            case CamSERTRAERR:
                return "%CAM-E-SERTRAERR, serial transmission error on highway";
            case CamSCCFAIL:
                return "%CAM-E-SCCFAIL, serial crate controller failure";
            case CamOFFLINE:
                return "%CAM-E-OFFLINE, crate is offline";
            case TclNORMAL:
                return "%TCL-S-NORMAL, Normal successful completion";
            case TclFAILED_ESSENTIAL:
                return "%TCL-W-FAILED_ESSENTIAL, Essential action failed";
            case TclNO_DISPATCH_TABLE:
                return "%TCL-W-NO_DISPATCH_TABLE, No dispatch table found. Forgot to do DISPATCH/BUILD?";
            default:
                return "%MDSPLUS-?-UNKNOWN, Unknown exception " + status;
        }
    }

    public final static void handleStatus(final int status) throws MdsException {
        final String msg = MdsException.getMdsMessage(status);
        final boolean success = (status & 1) == 1;
        if(!success){
            final MdsException exc = new MdsException(msg, status);
            MdsException.stderr(null, exc);
            throw exc;
        }
        MdsException.stdout(msg);
    }

    private static final String parseMessage(final String message) {
        final String[] parts = message.split(":", 2);
        return parts[parts.length - 1];
    }

    public static final void setStatusLabel(final JLabel status) {
        MdsException.statusLabel = status;
    }

    public static void stderr(final String line, final Exception exc) {
        if(MdsException.statusLabel != null) MdsException.statusLabel.setForeground(Color.RED);
        if(line == null){
            if(exc == null){
                if(MdsException.statusLabel != null) MdsException.statusLabel.setText("");
                return;
            }
            String msg = exc.getMessage();
            if(msg == null) msg = exc.toString();
            if(MdsException.statusLabel != null) MdsException.statusLabel.setText(msg);
            System.err.println(msg);
        }else if(exc == null){
            if(MdsException.statusLabel != null) MdsException.statusLabel.setText(String.format("E:%s", line));
            System.err.println(String.format("%s", line));
        }else{
            String msg = exc.getMessage();
            if(msg == null) msg = exc.toString();
            if(MdsException.statusLabel != null) MdsException.statusLabel.setText(String.format("E:%s (%s)", line, msg));
            System.err.println(String.format("%s\n%s", line, msg));
        }
    }

    public static void stdout(final String line) {
        if(MdsException.statusLabel == null) return;
        MdsException.statusLabel.setForeground(Color.BLACK);
        MdsException.statusLabel.setText(line);
    }
    private final int status;

    public MdsException(final int status){
        this(MdsException.getMdsMessage(status), status);
    }

    public MdsException(final String message){
        super(MdsException.parseMessage(message));
        int new_status = 0;
        try{
            final String[] parts = message.split(":", 2);
            if(parts.length > 1) new_status = Integer.parseInt(parts[0]);
        }catch(final Exception exc){/**/}
        this.status = new_status;
    }

    public MdsException(final String header, final Exception e){
        super(String.format("%s: %s", header, e.getMessage()));
        this.status = 0;
    }

    public MdsException(final String message, final int status){
        super(message);
        this.status = status;
    }

    public final int getStatus() {
        return this.status;
    }
}
