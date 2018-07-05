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
    public static final int ApdAPD_APPEND                     = 0xfdd000a;
    public static final int ApdDICT_KEYVALPAIR                = 0xfdd0012;
    public static final int ApdDICT_KEYCLS                    = 0xfdd001a;
    public static final int DevBAD_ENDIDX                     = 662470666;
    public static final int DevBAD_FILTER                     = 662470674;
    public static final int DevBAD_FREQ                       = 662470682;
    public static final int DevBAD_GAIN                       = 662470690;
    public static final int DevBAD_HEADER                     = 662470698;
    public static final int DevBAD_HEADER_IDX                 = 662470706;
    public static final int DevBAD_MEMORIES                   = 662470714;
    public static final int DevBAD_MODE                       = 662470722;
    public static final int DevBAD_NAME                       = 662470730;
    public static final int DevBAD_OFFSET                     = 662470738;
    public static final int DevBAD_STARTIDX                   = 662470746;
    public static final int DevNOT_TRIGGERED                  = 662470754;
    public static final int DevFREQ_TO_HIGH                   = 662470762;
    public static final int DevINVALID_NOC                    = 662470770;
    public static final int DevRANGE_MISMATCH                 = 662470778;
    public static final int DevCAMACERR                       = 662470786;
    public static final int DevBAD_VERBS                      = 662470794;
    public static final int DevBAD_COMMANDS                   = 662470802;
    public static final int DevCAM_ADNR                       = 662470810;
    public static final int DevCAM_ERR                        = 662470818;
    public static final int DevCAM_LOSYNC                     = 662470826;
    public static final int DevCAM_LPE                        = 662470834;
    public static final int DevCAM_TMO                        = 662470842;
    public static final int DevCAM_TPE                        = 662470850;
    public static final int DevCAM_STE                        = 662470858;
    public static final int DevCAM_DERR                       = 662470866;
    public static final int DevCAM_SQ                         = 662470874;
    public static final int DevCAM_NOSQ                       = 662470882;
    public static final int DevCAM_SX                         = 662470890;
    public static final int DevCAM_NOSX                       = 662470898;
    public static final int DevINV_SETUP                      = 662470906;
    public static final int DevPYDEVICE_NOT_FOUND             = 662470914;
    public static final int DevPY_INTERFACE_LIBRARY_NOT_FOUND = 662470922;
    public static final int DevIO_STUCK                       = 662470930;
    public static final int DevUNKOWN_STATE                   = 662470938;
    public static final int DevWRONG_TREE                     = 662470946;
    public static final int DevWRONG_PATH                     = 662470954;
    public static final int DevWRONG_SHOT                     = 662470962;
    public static final int DevOFFLINE                        = 662470970;
    public static final int DevTRIGGERED_NOT_STORED           = 662470978;
    public static final int DevNO_NAME_SPECIFIED              = 662470986;
    public static final int DevBAD_ACTIVE_CHAN                = 662470994;
    public static final int DevBAD_TRIG_SRC                   = 662471002;
    public static final int DevBAD_CLOCK_SRC                  = 662471010;
    public static final int DevBAD_PRE_TRIG                   = 662471018;
    public static final int DevBAD_POST_TRIG                  = 662471026;
    public static final int DevBAD_CLOCK_FREQ                 = 662471034;
    public static final int DevTRIGGER_FAILED                 = 662471042;
    public static final int DevERROR_READING_CHANNEL          = 662471050;
    public static final int DevERROR_DOING_INIT               = 662471058;
    public static final int ReticonNORMAL                     = 662471065;
    public static final int ReticonBAD_FRAMES                 = 662471074;
    public static final int ReticonBAD_FRAME_SELECT           = 662471082;
    public static final int ReticonBAD_NUM_STATES             = 662471090;
    public static final int ReticonBAD_PERIOD                 = 662471098;
    public static final int ReticonBAD_PIXEL_SELECT           = 662471106;
    public static final int ReticonDATA_CORRUPTED             = 662471114;
    public static final int ReticonTOO_MANY_FRAMES            = 662471122;
    public static final int J221NORMAL                        = 662471465;
    public static final int J221INVALID_DATA                  = 662471476;
    public static final int J221NO_DATA                       = 662471482;
    public static final int TimingINVCLKFRQ                   = 662471866;
    public static final int TimingINVDELDUR                   = 662471874;
    public static final int TimingINVOUTCTR                   = 662471882;
    public static final int TimingINVPSEUDODEV                = 662471890;
    public static final int TimingINVTRGMOD                   = 662471898;
    public static final int TimingNOPSEUDODEV                 = 662471907;
    public static final int TimingTOO_MANY_EVENTS             = 662471914;
    public static final int B2408NORMAL                       = 662472265;
    public static final int B2408OVERFLOW                     = 662472275;
    public static final int B2408TRIG_LIM                     = 662472284;
    public static final int FeraNORMAL                        = 662472665;
    public static final int FeraDIGNOTSTRARRAY                = 662472674;
    public static final int FeraNODIG                         = 662472682;
    public static final int FeraMEMNOTSTRARRAY                = 662472690;
    public static final int FeraNOMEM                         = 662472698;
    public static final int FeraPHASE_LOST                    = 662472706;
    public static final int FeraCONFUSED                      = 662472716;
    public static final int FeraOVER_RUN                      = 662472724;
    public static final int FeraOVERFLOW                      = 662472731;
    public static final int Hm650NORMAL                       = 662473065;
    public static final int Hm650DLYCHNG                      = 662473076;
    public static final int Hv4032NORMAL                      = 662473465;
    public static final int Hv4032WRONG_POD_TYPE              = 662473474;
    public static final int Hv1440NORMAL                      = 662473865;
    public static final int Hv1440WRONG_POD_TYPE              = 662473874;
    public static final int Hv1440BAD_FRAME                   = 662473882;
    public static final int Hv1440BAD_RANGE                   = 662473890;
    public static final int Hv1440OUTRNG                      = 662473898;
    public static final int Hv1440STUCK                       = 662473906;
    public static final int JoergerBAD_PRE_TRIGGER            = 662474266;
    public static final int JoergerBAD_ACT_MEMORY             = 662474274;
    public static final int JoergerBAD_GAIN                   = 662474282;
    public static final int U_of_mBAD_WAVE_LENGTH             = 662474666;
    public static final int U_of_mBAD_SLIT_WIDTH              = 662474674;
    public static final int U_of_mBAD_NUM_SPECTRA             = 662474682;
    public static final int U_of_mBAD_GRATING                 = 662474690;
    public static final int U_of_mBAD_EXPOSURE                = 662474698;
    public static final int U_of_mBAD_FILTER                  = 662474706;
    public static final int U_of_mGO_FILE_ERROR               = 662474714;
    public static final int U_of_mDATA_FILE_ERROR             = 662474722;
    public static final int IdlNORMAL                         = 662475065;
    public static final int IdlERROR                          = 662475074;
    public static final int B5910aBAD_CHAN                    = 662475466;
    public static final int B5910aBAD_CLOCK                   = 662475474;
    public static final int B5910aBAD_ITERATIONS              = 662475482;
    public static final int B5910aBAD_NOC                     = 662475490;
    public static final int B5910aBAD_SAMPS                   = 662475498;
    public static final int J412NOT_SORTED                    = 662475866;
    public static final int J412NO_DATA                       = 662475874;
    public static final int J412BADCYCLES                     = 662475882;
    public static final int Tr16NORMAL                        = 662476265;
    public static final int Tr16BAD_MEMSIZE                   = 662476274;
    public static final int Tr16BAD_ACTIVEMEM                 = 662476282;
    public static final int Tr16BAD_ACTIVECHAN                = 662476290;
    public static final int Tr16BAD_PTS                       = 662476298;
    public static final int Tr16BAD_FREQUENCY                 = 662476306;
    public static final int Tr16BAD_MASTER                    = 662476314;
    public static final int Tr16BAD_GAIN                      = 662476322;
    public static final int A14NORMAL                         = 662476665;
    public static final int A14BAD_CLK_DIVIDE                 = 662476674;
    public static final int A14BAD_MODE                       = 662476682;
    public static final int A14BAD_CLK_POLARITY               = 662476690;
    public static final int A14BAD_STR_POLARITY               = 662476698;
    public static final int A14BAD_STP_POLARITY               = 662476706;
    public static final int A14BAD_GATED                      = 662476714;
    public static final int L6810NORMAL                       = 662477065;
    public static final int L6810BAD_ACTIVECHAN               = 662477074;
    public static final int L6810BAD_ACTIVEMEM                = 662477082;
    public static final int L6810BAD_FREQUENCY                = 662477090;
    public static final int L6810BAD_FULL_SCALE               = 662477098;
    public static final int L6810BAD_MEMORIES                 = 662477106;
    public static final int L6810BAD_COUPLING                 = 662477114;
    public static final int L6810BAD_OFFSET                   = 662477122;
    public static final int L6810BAD_SEGMENTS                 = 662477130;
    public static final int L6810BAD_TRIG_DELAY               = 662477138;
    public static final int J_dacOUTRNG                       = 662477466;
    public static final int IncaaBAD_ACTIVE_CHANS             = 662477866;
    public static final int IncaaBAD_MASTER                   = 662477874;
    public static final int IncaaBAD_EXT_1MHZ                 = 662477882;
    public static final int IncaaBAD_PTSC                     = 662477890;
    public static final int L8212BAD_HEADER                   = 662478266;
    public static final int L8212BAD_MEMORIES                 = 662478274;
    public static final int L8212BAD_NOC                      = 662478282;
    public static final int L8212BAD_OFFSET                   = 662478290;
    public static final int L8212BAD_PTS                      = 662478298;
    public static final int L8212FREQ_TO_HIGH                 = 662478306;
    public static final int L8212INVALID_NOC                  = 662478314;
    public static final int MpbBADTIME                        = 662478666;
    public static final int MpbBADFREQ                        = 662478674;
    public static final int L8828BAD_OFFSET                   = 662479066;
    public static final int L8828BAD_PRETRIG                  = 662479074;
    public static final int L8828BAD_ACTIVEMEM                = 662479082;
    public static final int L8828BAD_CLOCK                    = 662479090;
    public static final int L8818BAD_OFFSET                   = 662479466;
    public static final int L8818BAD_PRETRIG                  = 662479474;
    public static final int L8818BAD_ACTIVEMEM                = 662479482;
    public static final int L8818BAD_CLOCK                    = 662479490;
    public static final int J_tr612BAD_ACTMEM                 = 662479546;
    public static final int J_tr612BAD_PRETRIG                = 662479554;
    public static final int J_tr612BAD_MODE                   = 662479562;
    public static final int J_tr612BAD_FREQUENCY              = 662479570;
    public static final int L8206NODATA                       = 662479868;
    public static final int H912BAD_CLOCK                     = 662479946;
    public static final int H912BAD_BLOCKS                    = 662479954;
    public static final int H912BAD_PTS                       = 662479962;
    public static final int H908BAD_CLOCK                     = 662480026;
    public static final int H908BAD_ACTIVE_CHANS              = 662480034;
    public static final int H908BAD_PTS                       = 662480042;
    public static final int Dsp2904CHANNEL_READ_ERROR         = 662480106;
    public static final int PyUNHANDLED_EXCEPTION             = 662480186;
    public static final int Dt196bNO_SAMPLES                  = 662480266;
    public static final int DevCANNOT_LOAD_SETTINGS           = 662480290;
    public static final int DevCANNOT_GET_BOARD_STATE         = 662480298;
    public static final int DevACQCMD_FAILED                  = 662480306;
    public static final int DevACQ2SH_FAILED                  = 662480314;
    public static final int DevBAD_PARAMETER                  = 662480322;
    public static final int DevCOMM_ERROR                     = 662480330;
    public static final int DevCAMERA_NOT_FOUND               = 662480338;
    public static final int DevNOT_A_PYDEVICE                 = 662480346;
    public static final int TreeALREADY_OFF                   = 265388075;
    public static final int TreeALREADY_ON                    = 265388083;
    public static final int TreeALREADY_OPEN                  = 265388091;
    public static final int TreeALREADY_THERE                 = 265388168;
    public static final int TreeBADRECORD                     = 265388218;
    public static final int TreeBOTH_OFF                      = 265388184;
    public static final int TreeBUFFEROVF                     = 265388306;
    public static final int TreeCONGLOMFULL                   = 265388322;
    public static final int TreeCONGLOM_NOT_FULL              = 265388330;
    public static final int TreeCONTINUING                    = 265390435;
    public static final int TreeDUPTAG                        = 265388234;
    public static final int TreeEDITTING                      = 265388434;
    public static final int TreeILLEGAL_ITEM                  = 265388298;
    public static final int TreeILLPAGCNT                     = 265388242;
    public static final int TreeINVDFFCLASS                   = 265388346;
    public static final int TreeINVDTPUSG                     = 265388426;
    public static final int TreeINVPATH                       = 265388290;
    public static final int TreeINVRECTYP                     = 265388354;
    public static final int TreeINVTREE                       = 265388226;
    public static final int TreeMAXOPENEDIT                   = 265388250;
    public static final int TreeNEW                           = 265388059;
    public static final int TreeNMN                           = 265388128;
    public static final int TreeNMT                           = 265388136;
    public static final int TreeNNF                           = 265388144;
    public static final int TreeNODATA                        = 265388258;
    public static final int TreeNODNAMLEN                     = 265388362;
    public static final int TreeNOEDIT                        = 265388274;
    public static final int TreeNOLOG                         = 265388458;
    public static final int TreeNOMETHOD                      = 265388208;
    public static final int TreeNOOVERWRITE                   = 265388418;
    public static final int TreeNORMAL                        = 265388041;
    public static final int TreeNOTALLSUBS                    = 265388067;
    public static final int TreeNOTCHILDLESS                  = 265388282;
    public static final int TreeNOT_IN_LIST                   = 265388482;
    public static final int TreeNOTMEMBERLESS                 = 265388402;
    public static final int TreeNOTOPEN                       = 265388266;
    public static final int TreeNOTSON                        = 265388410;
    public static final int TreeNOT_CONGLOM                   = 265388386;
    public static final int TreeNOT_OPEN                      = 265388200;
    public static final int TreeNOWRITEMODEL                  = 265388442;
    public static final int TreeNOWRITESHOT                   = 265388450;
    public static final int TreeNO_CONTEXT                    = 265388099;
    public static final int TreeOFF                           = 265388192;
    public static final int TreeON                            = 265388107;
    public static final int TreeOPEN                          = 265388115;
    public static final int TreeOPEN_EDIT                     = 265388123;
    public static final int TreePARENT_OFF                    = 265388176;
    public static final int TreeREADERR                       = 265388474;
    public static final int TreeREADONLY                      = 265388466;
    public static final int TreeRESOLVED                      = 265388049;
    public static final int TreeSUCCESS                       = 265389633;
    public static final int TreeTAGNAMLEN                     = 265388370;
    public static final int TreeTNF                           = 265388152;
    public static final int TreeTREENF                        = 265388160;
    public static final int TreeUNRESOLVED                    = 265388338;
    public static final int TreeUNSPRTCLASS                   = 265388314;
    public static final int TreeUNSUPARRDTYPE                 = 265388394;
    public static final int TreeWRITEFIRST                    = 265388378;
    public static final int TreeFAILURE                       = 265392034;
    public static final int TreeLOCK_FAILURE                  = 265392050;
    public static final int TreeFILE_NOT_FOUND                = 265392042;
    public static final int TreeCANCEL                        = 265391232;
    public static final int TreeNOSEGMENTS                    = 265392058;
    public static final int TreeINVDTYPE                      = 265392066;
    public static final int TreeINVSHAPE                      = 265392074;
    public static final int TreeINVSHOT                       = 265392090;
    public static final int TreeINVTAG                        = 265392106;
    public static final int TreeNOPATH                        = 265392114;
    public static final int TreeTREEFILEREADERR               = 265392122;
    public static final int TreeMEMERR                        = 265392130;
    public static final int TreeNOCURRENT                     = 265392138;
    public static final int TreeFOPENW                        = 265392146;
    public static final int TreeFOPENR                        = 265392154;
    public static final int TreeFCREATE                       = 265392162;
    public static final int TreeCONNECTFAIL                   = 265392170;
    public static final int TreeNCIWRITE                      = 265392178;
    public static final int TreeDELFAIL                       = 265392186;
    public static final int TreeRENFAIL                       = 265392194;
    public static final int TreeEMPTY                         = 265392200;
    public static final int TreePARSEERR                      = 265392210;
    public static final int TreeNCIREAD                       = 265392218;
    public static final int TreeNOVERSION                     = 265392226;
    public static final int TreeDFREAD                        = 265392234;
    public static final int TreeCLOSEERR                      = 265392242;
    public static final int TreeMOVEERROR                     = 265392250;
    public static final int TreeOPENEDITERR                   = 265392258;
    public static final int TreeREADONLY_TREE                 = 265392266;
    public static final int LibINSVIRMEM                      = 1409556;
    public static final int LibINVARG                         = 1409588;
    public static final int LibINVSTRDES                      = 1409572;
    public static final int LibKEYNOTFOU                      = 1409788;
    public static final int LibNOTFOU                         = 1409652;
    public static final int LibQUEWASEMP                      = 1409772;
    public static final int LibSTRTRU                         = 1409041;
    public static final int StrMATCH                          = 2393113;
    public static final int StrNOMATCH                        = 2392584;
    public static final int StrNOELEM                         = 2392600;
    public static final int StrINVDELIM                       = 2392592;
    public static final int StrSTRTOOLON                      = 2392180;
    public static final int MDSplusWARNING                    = 65536;
    public static final int MDSplusSUCCESS                    = 65545;
    public static final int MDSplusERROR                      = 65554;
    public static final int MDSplusFATAL                      = 65572;
    public static final int SsSUCCESS                         = 1;
    public static final int SsINTOVF                          = 1148;
    public static final int SsINTERNAL                        = -1;
    public static final int TdiBREAK                          = 265519112;
    public static final int TdiCASE                           = 265519120;
    public static final int TdiCONTINUE                       = 265519128;
    public static final int TdiEXTRANEOUS                     = 265519136;
    public static final int TdiRETURN                         = 265519144;
    public static final int TdiABORT                          = 265519154;
    public static final int TdiBAD_INDEX                      = 265519162;
    public static final int TdiBOMB                           = 265519170;
    public static final int TdiEXTRA_ARG                      = 265519178;
    public static final int TdiGOTO                           = 265519186;
    public static final int TdiINVCLADSC                      = 265519194;
    public static final int TdiINVCLADTY                      = 265519202;
    public static final int TdiINVDTYDSC                      = 265519210;
    public static final int TdiINV_OPC                        = 265519218;
    public static final int TdiINV_SIZE                       = 265519226;
    public static final int TdiMISMATCH                       = 265519234;
    public static final int TdiMISS_ARG                       = 265519242;
    public static final int TdiNDIM_OVER                      = 265519250;
    public static final int TdiNO_CMPLX                       = 265519258;
    public static final int TdiNO_OPC                         = 265519266;
    public static final int TdiNO_OUTPTR                      = 265519274;
    public static final int TdiNO_SELF_PTR                    = 265519282;
    public static final int TdiNOT_NUMBER                     = 265519290;
    public static final int TdiNULL_PTR                       = 265519298;
    public static final int TdiRECURSIVE                      = 265519306;
    public static final int TdiSIG_DIM                        = 265519314;
    public static final int TdiSYNTAX                         = 265519322;
    public static final int TdiTOO_BIG                        = 265519330;
    public static final int TdiUNBALANCE                      = 265519338;
    public static final int TdiUNKNOWN_VAR                    = 265519346;
    public static final int TdiSTRTOOLON                      = 265519356;
    public static final int TdiTIMEOUT                        = 265519364;
    public static final int MdsdclSUCCESS                     = 134348809;
    public static final int MdsdclEXIT                        = 134348817;
    public static final int MdsdclERROR                       = 134348824;
    public static final int MdsdclNORMAL                      = 134349609;
    public static final int MdsdclPRESENT                     = 134349617;
    public static final int MdsdclIVVERB                      = 134349626;
    public static final int MdsdclABSENT                      = 134349632;
    public static final int MdsdclNEGATED                     = 134349640;
    public static final int MdsdclNOTNEGATABLE                = 134349650;
    public static final int MdsdclIVQUAL                      = 134349658;
    public static final int MdsdclPROMPT_MORE                 = 134349666;
    public static final int MdsdclTOO_MANY_PRMS               = 134349674;
    public static final int MdsdclTOO_MANY_VALS               = 134349682;
    public static final int MdsdclMISSING_VALUE               = 134349690;
    public static final int ServerNOT_DISPATCHED              = 266436616;
    public static final int ServerINVALID_DEPENDENCY          = 266436626;
    public static final int ServerCANT_HAPPEN                 = 266436634;
    public static final int ServerINVSHOT                     = 266436642;
    public static final int ServerABORT                       = 266436658;
    public static final int ServerPATH_DOWN                   = 266436674;
    public static final int ServerSOCKET_ADDR_ERROR           = 266436682;
    public static final int ServerINVALID_ACTION_OPERATION    = 266436690;
    public static final int CamDONE_Q                         = 134316041;
    public static final int CamDONE_NOQ                       = 134316049;
    public static final int CamDONE_NOX                       = 134320128;
    public static final int CamSERTRAERR                      = 134322178;
    public static final int CamSCCFAIL                        = 134322242;
    public static final int CamOFFLINE                        = 134322282;
    public static final int TclNORMAL                         = 2752521;
    public static final int TclFAILED_ESSENTIAL               = 2752528;

    public static final String getMdsMessage(final int status) {
        switch(status){
            default:
                return "%MDSPLUS-?-UNKNOWN, Unknown exception " + status;
            case ApdAPD_APPEND:
                return "First argument must be APD or *";
            case ApdDICT_KEYVALPAIR:
                return "A Dictionary requires an even number of elements";
            case ApdDICT_KEYCLS:
                return "Keys must be scalar, i.e. CLASS_S";
            case DevBAD_ENDIDX:
                return "unable to read end index for channel";
            case DevBAD_FILTER:
                return "illegal filter selected";
            case DevBAD_FREQ:
                return "illegal digitization frequency selected";
            case DevBAD_GAIN:
                return "illegal gain selected";
            case DevBAD_HEADER:
                return "unable to read header selection";
            case DevBAD_HEADER_IDX:
                return "unknown header configuration index";
            case DevBAD_MEMORIES:
                return "unable to read number of memory modules";
            case DevBAD_MODE:
                return "illegal mode selected";
            case DevBAD_NAME:
                return "unable to read module name";
            case DevBAD_OFFSET:
                return "illegal offset selected";
            case DevBAD_STARTIDX:
                return "unable to read start index for channel";
            case DevNOT_TRIGGERED:
                return "device was not triggered,  check wires and triggering device";
            case DevFREQ_TO_HIGH:
                return "the frequency is set to high for the requested number of channels";
            case DevINVALID_NOC:
                return "the NOC (number of channels) requested is greater than the physical number of channels";
            case DevRANGE_MISMATCH:
                return "the range specified on the menu doesn't match the range setting on the device";
            case DevCAMACERR:
                return "Error doing CAMAC IO";
            case DevBAD_VERBS:
                return "Error reading interpreter list (:VERBS)";
            case DevBAD_COMMANDS:
                return "Error reading command list";
            case DevCAM_ADNR:
                return "CAMAC: Address not recognized (2160)";
            case DevCAM_ERR:
                return "CAMAC: Error reported by crate controler";
            case DevCAM_LOSYNC:
                return "CAMAC: Lost Syncronization error";
            case DevCAM_LPE:
                return "CAMAC: Longitudinal Parity error";
            case DevCAM_TMO:
                return "CAMAC: Highway time out error";
            case DevCAM_TPE:
                return "CAMAC: Transverse Parity error";
            case DevCAM_STE:
                return "CAMAC: Serial Transmission error";
            case DevCAM_DERR:
                return "CAMAC: Delayed error from SCC";
            case DevCAM_SQ:
                return "CAMAC: I/O completion with Q = 1";
            case DevCAM_NOSQ:
                return "CAMAC: I/O completion with Q = 0";
            case DevCAM_SX:
                return "CAMAC: I/O completion with X = 1";
            case DevCAM_NOSX:
                return "CAMAC: I/O completion with X = 0";
            case DevINV_SETUP:
                return "device was not properly set up";
            case DevPYDEVICE_NOT_FOUND:
                return "Python device class not found.";
            case DevPY_INTERFACE_LIBRARY_NOT_FOUND:
                return "The needed device hardware interface library could not be loaded.";
            case DevIO_STUCK:
                return "I/O to Device is stuck. Check network connection and board status.";
            case DevUNKOWN_STATE:
                return "Device returned unrecognized state string";
            case DevWRONG_TREE:
                return "Attempt to digitizerinto different tree than it was armed with";
            case DevWRONG_PATH:
                return "Attempt to store digitizer into different path than it was armed with";
            case DevWRONG_SHOT:
                return "Attempt to store digitizer into different shot than it was armed with";
            case DevOFFLINE:
                return "Device is not on line.  Check network connection";
            case DevTRIGGERED_NOT_STORED:
                return "Device was triggered but not stored.";
            case DevNO_NAME_SPECIFIED:
                return "Device name must be specifed - pleas fill it in.";
            case DevBAD_ACTIVE_CHAN:
                return "Active channels either not available or invalid";
            case DevBAD_TRIG_SRC:
                return "Trigger source either not available or invalid";
            case DevBAD_CLOCK_SRC:
                return "Clock source either not available or invalid";
            case DevBAD_PRE_TRIG:
                return "Pre trigger samples either not available or invalid";
            case DevBAD_POST_TRIG:
                return "Clock source either not available or invalid";
            case DevBAD_CLOCK_FREQ:
                return "Clock frequency either not available or invalid";
            case DevTRIGGER_FAILED:
                return "Device trigger method failed";
            case DevERROR_READING_CHANNEL:
                return "Error reading data for channel from device";
            case DevERROR_DOING_INIT:
                return "Error sending ARM command to device";
            case ReticonNORMAL:
                return "successful completion";
            case ReticonBAD_FRAMES:
                return "frame count must be less than or equal to 2048";
            case ReticonBAD_FRAME_SELECT:
                return "frame interval must be 1,2,4,8,16,32 or 64";
            case ReticonBAD_NUM_STATES:
                return "number of states must be between 1 and 4";
            case ReticonBAD_PERIOD:
                return "period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec";
            case ReticonBAD_PIXEL_SELECT:
                return "pixel selection must be an array of 256 boolean values";
            case ReticonDATA_CORRUPTED:
                return "data in memory is corrupted or framing error detected, no data stored";
            case ReticonTOO_MANY_FRAMES:
                return "over 8192 frame start indicators in data read from memory";
            case J221NORMAL:
                return "successful completion";
            case J221INVALID_DATA:
                return "ignoring invalid data in channel !SL";
            case J221NO_DATA:
                return "no valid data was found for any channel";
            case TimingINVCLKFRQ:
                return "Invalid clock frequency";
            case TimingINVDELDUR:
                return "Invalid pulse delay or duration, must be less than 655 seconds";
            case TimingINVOUTCTR:
                return "Invalid output mode selected";
            case TimingINVPSEUDODEV:
                return "Invalid pseudo device attached to this decoder channel";
            case TimingINVTRGMOD:
                return "Invalid trigger mode selected";
            case TimingNOPSEUDODEV:
                return "No Pseudo device attached to this channel ... disabling";
            case TimingTOO_MANY_EVENTS:
                return "More than 16 events used by this decoder";
            case B2408NORMAL:
                return "successful completion";
            case B2408OVERFLOW:
                return "Triggers received after overflow";
            case B2408TRIG_LIM:
                return "Trigger limit possibly exceeded";
            case FeraNORMAL:
                return "successful completion";
            case FeraDIGNOTSTRARRAY:
                return "The digitizer names must be an array of strings";
            case FeraNODIG:
                return "The digitizer names must be specified";
            case FeraMEMNOTSTRARRAY:
                return "The memory names must be an array of strings";
            case FeraNOMEM:
                return "The memory names must be specified";
            case FeraPHASE_LOST:
                return "Data phase lost No FERA data stored";
            case FeraCONFUSED:
                return "Fera Data inconsitant.  Data for this point zered.";
            case FeraOVER_RUN:
                return "Possible FERA memory overrun, too many triggers.";
            case FeraOVERFLOW:
                return "Possible FERA data saturated.  Data point zeroed";
            case Hm650NORMAL:
                return "successful completion";
            case Hm650DLYCHNG:
                return "HM650 requested delay can not be processed by hardware.";
            case Hv4032NORMAL:
                return "successful completion";
            case Hv4032WRONG_POD_TYPE:
                return "HV40321A n and p can only be used with the HV4032 device";
            case Hv1440NORMAL:
                return "successful completion";
            case Hv1440WRONG_POD_TYPE:
                return "HV1443 can only be used with the HV1440 device";
            case Hv1440BAD_FRAME:
                return "HV1440 could not read the frame";
            case Hv1440BAD_RANGE:
                return "HV1440 could not read the range";
            case Hv1440OUTRNG:
                return "HV1440 out of range";
            case Hv1440STUCK:
                return "HV1440 not responding with Q";
            case JoergerBAD_PRE_TRIGGER:
                return "bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7";
            case JoergerBAD_ACT_MEMORY:
                return "bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8";
            case JoergerBAD_GAIN:
                return "bad gain specified, specify a value of 1,2,4 or 8";
            case U_of_mBAD_WAVE_LENGTH:
                return "bad wave length specified, specify value between 0 and 13000";
            case U_of_mBAD_SLIT_WIDTH:
                return "bad slit width specified, specify value between 0 and 500";
            case U_of_mBAD_NUM_SPECTRA:
                return "bad number of spectra specified, specify value between 1 and 100";
            case U_of_mBAD_GRATING:
                return "bad grating type specified, specify value between 1 and 5";
            case U_of_mBAD_EXPOSURE:
                return "bad exposure time specified, specify value between 30 and 3000";
            case U_of_mBAD_FILTER:
                return "bad neutral density filter specified, specify value between 0 and 5";
            case U_of_mGO_FILE_ERROR:
                return "error creating new go file";
            case U_of_mDATA_FILE_ERROR:
                return "error opening datafile";
            case IdlNORMAL:
                return "successful completion";
            case IdlERROR:
                return "IDL returned a non zero error code";
            case B5910aBAD_CHAN:
                return "error evaluating data for channel !SL";
            case B5910aBAD_CLOCK:
                return "invalid internal clock range specified";
            case B5910aBAD_ITERATIONS:
                return "invalid number of iterations specified";
            case B5910aBAD_NOC:
                return "invalid number of active channels specified";
            case B5910aBAD_SAMPS:
                return "number of samples specificed invalid";
            case J412NOT_SORTED:
                return "times specified for J412 module were not sorted";
            case J412NO_DATA:
                return "there were no times specifed for J412 module";
            case J412BADCYCLES:
                return "The number of cycles must be 1 .. 255";
            case Tr16NORMAL:
                return "successful completion";
            case Tr16BAD_MEMSIZE:
                return "Memory size must be in 128K, 256K, 512k, 1024K";
            case Tr16BAD_ACTIVEMEM:
                return "Active Mem must be power of 2 8K to 1024K";
            case Tr16BAD_ACTIVECHAN:
                return "Active channels must be in 1,2,4,8,16";
            case Tr16BAD_PTS:
                return "PTS must be power of 2 32 to 1024K";
            case Tr16BAD_FREQUENCY:
                return "Invalid clock frequency";
            case Tr16BAD_MASTER:
                return "Master must be 0 or 1";
            case Tr16BAD_GAIN:
                return "Gain must be 1, 2, 4, or 8";
            case A14NORMAL:
                return "successful completion";
            case A14BAD_CLK_DIVIDE:
                return "Clock divide must be one of 1,2,4,10,20,40, or 100";
            case A14BAD_MODE:
                return "Mode must be in the range of 0 to 4";
            case A14BAD_CLK_POLARITY:
                return "Clock polarity must be either 0 (rising) or 1 (falling)";
            case A14BAD_STR_POLARITY:
                return "Start polarity must be either 0 (rising) or 1 (falling)";
            case A14BAD_STP_POLARITY:
                return "Stop polarity must be either 0 (rising) or 1 (falling)";
            case A14BAD_GATED:
                return "Gated clock must be either 0 (not gated) or 1 (gated)";
            case L6810NORMAL:
                return "successful completion";
            case L6810BAD_ACTIVECHAN:
                return "Active chans must be 1, 2, or 4";
            case L6810BAD_ACTIVEMEM:
                return "Active memory must be power of 2 LE 8192";
            case L6810BAD_FREQUENCY:
                return "Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]";
            case L6810BAD_FULL_SCALE:
                return "Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]";
            case L6810BAD_MEMORIES:
                return "Memories must 1 .. 16";
            case L6810BAD_COUPLING:
                return "Channel source / coupling must be one of 0 .. 7";
            case L6810BAD_OFFSET:
                return "Offset must be between 0 and 255";
            case L6810BAD_SEGMENTS:
                return "Number of segments must be 1 .. 1024";
            case L6810BAD_TRIG_DELAY:
                return "Trigger delay must be between -8 and 247 in units of 8ths of segment size";
            case J_dacOUTRNG:
                return "Joerger DAC Channels out of range.  Bad chans code !XW";
            case IncaaBAD_ACTIVE_CHANS:
                return "bad active channels selection";
            case IncaaBAD_MASTER:
                return "bad master selection, must be 0 or 1";
            case IncaaBAD_EXT_1MHZ:
                return "bad ext 1mhz selection, must be 0 or 1";
            case IncaaBAD_PTSC:
                return "bad PTSC setting";
            case L8212BAD_HEADER:
                return "Invalid header jumper information (e.g. 49414944432)";
            case L8212BAD_MEMORIES:
                return "Invalid number of memories, must be 1 .. 16";
            case L8212BAD_NOC:
                return "Invalid number of active channels";
            case L8212BAD_OFFSET:
                return "Invalid offset must be one of (0, -2048, -4096)";
            case L8212BAD_PTS:
                return "Invalid pts code, must be 0 .. 7";
            case L8212FREQ_TO_HIGH:
                return "Frequency to high for selected number of channels";
            case L8212INVALID_NOC:
                return "Invalid number of active channels";
            case MpbBADTIME:
                return "Could not read time";
            case MpbBADFREQ:
                return "Could not read frequency";
            case L8828BAD_OFFSET:
                return "Offset for L8828 must be between 0 and 255";
            case L8828BAD_PRETRIG:
                return "Pre trigger samples for L8828 must be betwwen 0 and 7 eighths";
            case L8828BAD_ACTIVEMEM:
                return "ACTIVEMEM must be beteen 16K and 2M";
            case L8828BAD_CLOCK:
                return "Invalid clock frequency specified.";
            case L8818BAD_OFFSET:
                return "Offset for L8828 must be between 0 and 255";
            case L8818BAD_PRETRIG:
                return "Pre trigger samples for L8828 must be betwwen 0 and 7 eighths";
            case L8818BAD_ACTIVEMEM:
                return "ACTIVEMEM must be beteen 16K and 2M";
            case L8818BAD_CLOCK:
                return "Invalid clock frequency specified.";
            case J_tr612BAD_ACTMEM:
                return "ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all";
            case J_tr612BAD_PRETRIG:
                return "PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples";
            case J_tr612BAD_MODE:
                return "MODE value out of range, must be 0 (for normal) or 1 (for burst mode)";
            case J_tr612BAD_FREQUENCY:
                return "FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external";
            case L8206NODATA:
                return "no data has been written to memory";
            case H912BAD_CLOCK:
                return "Bad value specified in INT_CLOCK node, use Setup device to correct";
            case H912BAD_BLOCKS:
                return "Bad value specified in BLOCKS node, use Setup device to correct";
            case H912BAD_PTS:
                return "Bad value specfiied in PTS node, must be an integer value between 1 and 131071";
            case H908BAD_CLOCK:
                return "Bad value specified in INT_CLOCK node, use Setup device to correct";
            case H908BAD_ACTIVE_CHANS:
                return "Bad value specified in ACTIVE_CHANS node, use Setup device to correct";
            case H908BAD_PTS:
                return "Bad value specfiied in PTS node, must be an integer value between 1 and 131071";
            case Dsp2904CHANNEL_READ_ERROR:
                return "Error reading channel";
            case PyUNHANDLED_EXCEPTION:
                return "Python device raised and exception, see log files for more details";
            case Dt196bNO_SAMPLES:
                return "Module did not acquire any samples";
            case DevCANNOT_LOAD_SETTINGS:
                return "Error loading settings from XML";
            case DevCANNOT_GET_BOARD_STATE:
                return "Cannot retrieve state of daq board";
            case DevACQCMD_FAILED:
                return "Error executing acqcmd on daq board";
            case DevACQ2SH_FAILED:
                return "Error executing acq2sh command on daq board";
            case DevBAD_PARAMETER:
                return "Invalid parameter specified for device";
            case DevCOMM_ERROR:
                return "Error communicating with device";
            case DevCAMERA_NOT_FOUND:
                return "Could not find specified camera on the network";
            case DevNOT_A_PYDEVICE:
                return "Device is not a python device.";
            case TreeALREADY_OFF:
                return "Node is already OFF";
            case TreeALREADY_ON:
                return "Node is already ON";
            case TreeALREADY_OPEN:
                return "Tree is already OPEN";
            case TreeALREADY_THERE:
                return "Node is already in the tree";
            case TreeBADRECORD:
                return "Data corrupted: cannot read record";
            case TreeBOTH_OFF:
                return "Both this node and its parent are off";
            case TreeBUFFEROVF:
                return "Output buffer overflow";
            case TreeCONGLOMFULL:
                return "Current conglomerate is full";
            case TreeCONGLOM_NOT_FULL:
                return "Current conglomerate is not yet full";
            case TreeCONTINUING:
                return "Operation continuing: note following error";
            case TreeDUPTAG:
                return "Tag name already in use";
            case TreeEDITTING:
                return "Tree file open for edit: operation not permitted";
            case TreeILLEGAL_ITEM:
                return "Invalid item code or part number specified";
            case TreeILLPAGCNT:
                return "Illegal page count, error mapping tree file";
            case TreeINVDFFCLASS:
                return "Invalid data fmt: only CLASS_S can have data in NCI";
            case TreeINVDTPUSG:
                return "Attempt to store datatype which conflicts with the designated usage of this node";
            case TreeINVPATH:
                return "Invalid tree pathname specified";
            case TreeINVRECTYP:
                return "Record type invalid for requested operation";
            case TreeINVTREE:
                return "Invalid tree identification structure";
            case TreeMAXOPENEDIT:
                return "Too many files open for edit";
            case TreeNEW:
                return "New tree created";
            case TreeNMN:
                return "No More Nodes";
            case TreeNMT:
                return "No More Tags";
            case TreeNNF:
                return "Node Not Found";
            case TreeNODATA:
                return "No data available for this node";
            case TreeNODNAMLEN:
                return "Node name too long (12 chars max)";
            case TreeNOEDIT:
                return "Tree file is not open for edit";
            case TreeNOLOG:
                return "Experiment pathname (xxx_path) not defined";
            case TreeNOMETHOD:
                return "Method not available for this object";
            case TreeNOOVERWRITE:
                return "Write-once node: overwrite not permitted";
            case TreeNORMAL:
                return "Normal successful completion";
            case TreeNOTALLSUBS:
                return "Main tree opened but not all subtrees found/or connected";
            case TreeNOTCHILDLESS:
                return "Node must be childless to become subtree reference";
            case TreeNOT_IN_LIST:
                return "Tree being opened was not in the list";
            case TreeNOTMEMBERLESS:
                return "Subtree reference can not have members";
            case TreeNOTOPEN:
                return "No tree file currently open";
            case TreeNOTSON:
                return "Subtree reference cannot be a member";
            case TreeNOT_CONGLOM:
                return "Head node of conglomerate does not contain a DTYPE_CONGLOM record";
            case TreeNOT_OPEN:
                return "Tree not currently open";
            case TreeNOWRITEMODEL:
                return "Data for this node can not be written into the MODEL file";
            case TreeNOWRITESHOT:
                return "Data for this node can not be written into the SHOT file";
            case TreeNO_CONTEXT:
                return "There is no active search to end";
            case TreeOFF:
                return "Node is OFF";
            case TreeON:
                return "Node is ON";
            case TreeOPEN:
                return "Tree is OPEN (no edit)";
            case TreeOPEN_EDIT:
                return "Tree is OPEN for edit";
            case TreePARENT_OFF:
                return "Parent of this node is OFF";
            case TreeREADERR:
                return "Error reading record for node";
            case TreeREADONLY:
                return "Tree was opened with readonly access";
            case TreeRESOLVED:
                return "Indirect reference successfully resolved";
            case TreeSUCCESS:
                return "Operation successful";
            case TreeTAGNAMLEN:
                return "Tagname too long (max 24 chars)";
            case TreeTNF:
                return "Tag Not Found";
            case TreeTREENF:
                return "Tree Not Found";
            case TreeUNRESOLVED:
                return "Not an indirect node reference: No action taken";
            case TreeUNSPRTCLASS:
                return "Unsupported descriptor class";
            case TreeUNSUPARRDTYPE:
                return "Complex data types not supported as members of arrays";
            case TreeWRITEFIRST:
                return "Tree has been modified:  write or quit first";
            case TreeFAILURE:
                return "Operation NOT successful";
            case TreeLOCK_FAILURE:
                return "Error locking file, perhaps NFSLOCKING not enabled on this system";
            case TreeFILE_NOT_FOUND:
                return "File or Directory Not Found";
            case TreeCANCEL:
                return "User canceled operation";
            case TreeNOSEGMENTS:
                return "No segments exist in this node";
            case TreeINVDTYPE:
                return "Invalid datatype for data segment";
            case TreeINVSHAPE:
                return "Invalid shape for this data segment";
            case TreeINVSHOT:
                return "Invalid shot number - must be -1 (model), 0 (current), or Positive";
            case TreeINVTAG:
                return "Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores";
            case TreeNOPATH:
                return "No 'treename'_path environment variable defined. Cannot locate tree files.";
            case TreeTREEFILEREADERR:
                return "Error reading in tree file contents.";
            case TreeMEMERR:
                return "Memory allocation error.";
            case TreeNOCURRENT:
                return "No current shot number set for this tree.";
            case TreeFOPENW:
                return "Error opening file for read-write.";
            case TreeFOPENR:
                return "Error opening file read-only.";
            case TreeFCREATE:
                return "Error creating new file.";
            case TreeCONNECTFAIL:
                return "Error connecting to remote server.";
            case TreeNCIWRITE:
                return "Error writing node characterisitics to file.";
            case TreeDELFAIL:
                return "Error deleting file.";
            case TreeRENFAIL:
                return "Error renaming file.";
            case TreeEMPTY:
                return "Empty string provided.";
            case TreePARSEERR:
                return "Invalid node search string.";
            case TreeNCIREAD:
                return "Error reading node characteristics from file.";
            case TreeNOVERSION:
                return "No version available.";
            case TreeDFREAD:
                return "Error reading from datafile.";
            case TreeCLOSEERR:
                return "Error closing temporary tree file.";
            case TreeMOVEERROR:
                return "Error replacing original treefile with new one.";
            case TreeOPENEDITERR:
                return "Error reopening new treefile for write access.";
            case TreeREADONLY_TREE:
                return "Tree is marked as readonly. No write operations permitted.";
            case LibINSVIRMEM:
                return "Insufficient virtual memory";
            case LibINVARG:
                return "Invalid argument";
            case LibINVSTRDES:
                return "Invalid string descriptor";
            case LibKEYNOTFOU:
                return "Key not found";
            case LibNOTFOU:
                return "Entity not found";
            case LibQUEWASEMP:
                return "Queue was empty";
            case LibSTRTRU:
                return "String truncated";
            case StrMATCH:
                return "Strings match";
            case StrNOMATCH:
                return "Strings do not match";
            case StrNOELEM:
                return "Not enough delimited characters";
            case StrINVDELIM:
                return "Not enough delimited characters";
            case StrSTRTOOLON:
                return "String too long";
            case MDSplusWARNING:
                return "Warning";
            case MDSplusSUCCESS:
                return "Success";
            case MDSplusERROR:
                return "Error";
            case MDSplusFATAL:
                return "Fatal";
            case SsSUCCESS:
                return "Success";
            case SsINTOVF:
                return "Integer overflow";
            case SsINTERNAL:
                return "This status is meant for internal use only, you should never have seen this message.";
            case TdiBREAK:
                return "BREAK was not in DO FOR SWITCH or WHILE";
            case TdiCASE:
                return "CASE was not in SWITCH statement";
            case TdiCONTINUE:
                return "CONTINUE was not in DO FOR or WHILE";
            case TdiEXTRANEOUS:
                return "Some characters were unused, bad number maybe";
            case TdiRETURN:
                return "Extraneous RETURN statement, not from a FUN";
            case TdiABORT:
                return "Program requested abort";
            case TdiBAD_INDEX:
                return "Index or subscript is too small or too big";
            case TdiBOMB:
                return "Bad punctuation, could not compile the text";
            case TdiEXTRA_ARG:
                return "Too many arguments for function, watch commas";
            case TdiGOTO:
                return "GOTO target label not found";
            case TdiINVCLADSC:
                return "Storage class not valid, must be scalar or array";
            case TdiINVCLADTY:
                return "Invalid mixture of storage class and data type";
            case TdiINVDTYDSC:
                return "Storage data type is not valid";
            case TdiINV_OPC:
                return "Invalid operator code in a function";
            case TdiINV_SIZE:
                return "Number of elements does not match declaration";
            case TdiMISMATCH:
                return "Shape of arguments does not match";
            case TdiMISS_ARG:
                return "Missing argument is required for function";
            case TdiNDIM_OVER:
                return "Number of dimensions is over the allowed 8";
            case TdiNO_CMPLX:
                return "There are no complex forms of this function";
            case TdiNO_OPC:
                return "No support for this function, today";
            case TdiNO_OUTPTR:
                return "An output pointer is required";
            case TdiNO_SELF_PTR:
                return "No $VALUE is defined for signal or validation";
            case TdiNOT_NUMBER:
                return "Value is not a scalar number and must be";
            case TdiNULL_PTR:
                return "Null pointer where value needed";
            case TdiRECURSIVE:
                return "Overly recursive function, calls itself maybe";
            case TdiSIG_DIM:
                return "Signal dimension does not match data shape";
            case TdiSYNTAX:
                return "Bad punctuation or misspelled word or number";
            case TdiTOO_BIG:
                return "Conversion of number lost significant digits";
            case TdiUNBALANCE:
                return "Unbalanced () [] {} '' \" \" or /**/";
            case TdiUNKNOWN_VAR:
                return "Unknown/undefined variable name";
            case TdiSTRTOOLON:
                return "string is too long (greater than 65535)";
            case TdiTIMEOUT:
                return "task did not complete in alotted time";
            case MdsdclSUCCESS:
                return "Normal successful completion";
            case MdsdclEXIT:
                return "Normal exit";
            case MdsdclERROR:
                return "Unsuccessful execution of command";
            case MdsdclNORMAL:
                return "Normal successful completion";
            case MdsdclPRESENT:
                return "Entity is present";
            case MdsdclIVVERB:
                return "No such command";
            case MdsdclABSENT:
                return "Entity is absent";
            case MdsdclNEGATED:
                return "Entity is present but negated";
            case MdsdclNOTNEGATABLE:
                return "Entity cannot be negated";
            case MdsdclIVQUAL:
                return "Invalid qualifier";
            case MdsdclPROMPT_MORE:
                return "More input required for command";
            case MdsdclTOO_MANY_PRMS:
                return "Too many parameters specified";
            case MdsdclTOO_MANY_VALS:
                return "Too many values";
            case MdsdclMISSING_VALUE:
                return "Qualifier value needed";
            case ServerNOT_DISPATCHED:
                return "action not dispatched, depended on failed action";
            case ServerINVALID_DEPENDENCY:
                return "action dependency cannot be evaluated";
            case ServerCANT_HAPPEN:
                return "action contains circular dependency or depends on action which was not dispatched";
            case ServerINVSHOT:
                return "invalid shot number, cannot dispatch actions in model";
            case ServerABORT:
                return "Server action was aborted";
            case ServerPATH_DOWN:
                return "Path to server lost";
            case ServerSOCKET_ADDR_ERROR:
                return "Cannot obtain ip address socket is bound to.";
            case ServerINVALID_ACTION_OPERATION:
                return "None";
            case CamDONE_Q:
                return "I/O completed with X=1, Q=1";
            case CamDONE_NOQ:
                return "I/O completed with X=1, Q=0";
            case CamDONE_NOX:
                return "I/O completed with X=0 - probable failure";
            case CamSERTRAERR:
                return "serial transmission error on highway";
            case CamSCCFAIL:
                return "serial crate controller failure";
            case CamOFFLINE:
                return "crate is offline";
            case TclNORMAL:
                return "Normal successful completion";
            case TclFAILED_ESSENTIAL:
                return "Essential action failed";
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
