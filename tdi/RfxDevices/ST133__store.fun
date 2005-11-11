public fun ST133__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES =     30;
    private _N_HEAD =		    0;
    private _N_COMMENT =        1;
    private _N_IP_ADDRESS =     2;
    private _N_CLOCK_MODE =     3;
    private _N_CLOCK_SOURCE =   4;
    private _N_SPEC_GROOVES =   5;
    private _N_SPEC_WAV_MID =   6;
    private _N_SPEC_MIR_POS =   7;
    private _N_GLUE_FLAG    =   8;
    private _N_X_DIM_DET =      9;
    private _N_Y_DIM_DET =     10;
    private _N_X_DIM =         11;
    private _N_Y_DIM =         12;
    private _N_DET_TEMP =      13;
    private _N_EXP_TIME =      14;
    private _N_READ_OUT =      15;
    private _N_CLEANS =        16;
    private _N_N_SKIP_CLEAN =  17;
    private _N_N_FRAMES =      18;
    private _N_NROI =          19;
    private _N_ROI =           20;
    private _N_NUM_ACC =       21;
    private _N_XCAL_VALID =    22;
    private _N_XCAL_STING =    23;
    private _N_XCAL_POL_ORD =  24;
    private _N_XCAL_COEFF =    25;
    private _N_HEADER =        26;
    private _N_DATA   =        27;


    private _N_HEADER_SIZE   =        4100;



/*
// Definitions of array sizes:
//  ---------------------------
*/

   private _HDRNAMEMAX   =120;     /*Max char str length for file name*/
   private _USERINFOMAX  =1000;    /*User information space*/
   private _COMMENTMAX   =80;      /*User comment string max length (5 comments)*/
   private _LABELMAX     =16;      /*Label string max length*/
   private _FILEVERMAX   =16;      /*File version string max length*/
   private _DATEMAX      =10;      /*String length of file creation date string as ddmmmyyyy\0*/
   private _ROIMAX       =10;      /*Max size of roi array of structures*/
   private _TIMEMAX      =7 ;      /*Max time store as hhmmss\0*/



   private _WINX_ControllerVersion   =           0;  /*Hardware Version*/
   private _WINX_LogicOutput         =           2;  /*Definition of Output BNC*/
   private _WINX_AmpHiCapLowNoise    =           4;  /*Amp Switching Mode*/
   private _WINX_xDimDet             =           6;  /*Detector x dimension of chip.*/
   private _WINX_mode                =           8;  /*timing mode*/
   private _WINX_exp_sec             =          10;  /*alternitive exposure, in sec.*/
   private _WINX_VChipXdim           =          14;  /*Virtual Chip X dim*/
   private _WINX_VChipYdim           =          16;  /*Virtual Chip Y dim*/
   private _WINX_yDimDet             =          18; /*y dimension of CCD or detector.*/
   private _WINX_date			=			   20;  /*date [DATEMAX] */
   private _WINX_VirtualChipFlag =              30;  /*On/Off*/
   private _WINX_Spare_1          =             32;  /*[2] */
   private _WINX_noscan           =             34;  /*Old number of scans - should always be -1*/
   private _WINX_DetTemperature   =             36;  /*Detector Temperature Set*/
   private _WINX_DetType           =            40;  /*CCD/DiodeArray type*/
   private _WINX_xdim              =            42;  /*actual # of pixels on x axis*/
   private _WINX_stdiode           =            44;  /*trigger diode*/
   private _WINX_DelayTime         =            46;  /*Used with Async Mode*/
   private _WINX_ShutterControl    =            50;  /*Normal, Disabled Open, Disabled Closed*/
   private _WINX_AbsorbLive        =            52;  /*On/Off*/
   private _WINX_AbsorbMode        =            54;  /*Reference Strip or File*/
   private _WINX_CanDoVirtualChipFlag  =        56;  /*T/F Cont/Chip able to do Virtual Chip*/
   private _WINX_ThresholdMinLive      =        58;  /*On/Off*/
   private _WINX_ThresholdMinVal       =        60;  /*Threshold Minimum Value*/
   private _WINX_ThresholdMaxLive      =        64;  /*On/Off*/
   private _WINX_ThresholdMaxVal       =        66;  /*Threshold Maximum Value*/
   private _WINX_SpecAutoSpectroMode   =        70;  /*T/F Spectrograph Used*/
   private _WINX_SpecCenterWlNm        =        72;  /*Center Wavelength in Nm*/
   private _WINX_SpecGlueFlag          =        76;  /*T/F File is Glued*/
   private _WINX_SpecGlueStartWlNm     =        78;  /*Starting Wavelength in Nm*/
   private _WINX_SpecGlueEndWlNm       =        82;  /*Starting Wavelength in Nm*/
   private _WINX_SpecGlueMinOvrlpNm    =        86;  /*Minimum Overlap in Nm*/
   private _WINX_SpecGlueFinalResNm    =        90;  /*Final Resolution in Nm*/
   private _WINX_PulserType            =        94;  /*0=None, PG200=1, PTG=2, DG535=3*/
   private _WINX_CustomChipFlag        =        96;  /*T/F Custom Chip Used*/
   private _WINX_XPrePixels            =        98;  /*Pre Pixels in X direction*/
   private _WINX_XPostPixels           =       100;  /*Post Pixels in X direction*/
   private _WINX_YPrePixels            =       102;  /*Pre Pixels in Y direction */
   private _WINX_YPostPixels           =       104;  /*Post Pixels in Y direction*/
   private _WINX_asynen                =       106;  /*asynchron enable flag  0 = off*/
   private _WINX_datatype              =       108;  /*experiment datatype*/
													/* 0 =   float (4 bytes)*/
													/* 1 =   long (4 bytes)*/
													/* 2 =   short (2 bytes)*/
													/* 3 =   unsigned short (2 bytes)*/
   private _WINX_PulserMode           =        110;  /*Repetitive/Sequential*/
   private _WINX_PulserOnChipAccums   =        112;  /*Num PTG On-Chip Accums*/
   private _WINX_PulserRepeatExp      =        114;  /*Num Exp Repeats (Pulser SW Accum)*/
   private _WINX_PulseRepWidth        =        118;  /*Width Value for Repetitive pulse (usec)*/
   private _WINX_PulseRepDelay        =        122;  /*Width Value for Repetitive pulse (usec)*/
   private _WINX_PulseSeqStartWidth   =        126;  /*Start Width for Sequential pulse (usec)*/
   private _WINX_PulseSeqEndWidth     =        130;  /*End Width for Sequential pulse (usec)*/
   private _WINX_PulseSeqStartDelay   =        134;  /*Start Delay for Sequential pulse (usec)*/
   private _WINX_PulseSeqEndDelay     =        138;  /*End Delay for Sequential pulse (usec)*/
   private _WINX_PulseSeqIncMode      =        142;  /*Increments: 1=Fixed, 2=Exponential*/
   private _WINX_PImaxUsed            =        144;  /*PI-Max type controller flag*/
   private _WINX_PImaxMode            =        146;  /*PI-Max mode*/
   private _WINX_PImaxGain            =        148;  /*PI-Max Gain*/
   private _WINX_BackGrndApplied      =        150;  /*1 if background subtraction done*/
   private _WINX_PImax2nsBrdUsed      =        152;  /*T/F PI-Max 2ns Board Used*/
   private _WINX_minblk               =        154;  /*min. # of strips per skips*/
   private _WINX_numminblk            =        156;  /*# of min-blocks before geo skps*/
   private _WINX_SpecMirrorLocation=		      158;  /*Spectro Mirror Location, 0=Not Present [2]*/
   private _WINX_SpecSlitLocation=			  162;  /*Spectro Slit Location, 0=Not Present [4]*/
   private _WINX_CustomTimingFlag=             170;  /*T/F Custom Timing Used*/
   private _WINX_ExperimentTimeLocal   =       172;  /*Experiment Local Time as hhmmss\0 [TIMEMAX]*/
   private _WINX_ExperimentTimeUTC	=		  179;  /*Experiment UTC Time as hhmmss\0 [TIMEMAX]*/
   private _WINX_ExposUnits         =          186;  /*User Units for Exposure*/
   private _WINX_ADCoffset          =          188;  /*ADC offset*/
   private _WINX_ADCrate            =          190;  /*ADC rate*/
   private _WINX_ADCtype            =          192;  /*ADC type*/
   private _WINX_ADCresolution      =          194;  /*ADC resolution*/
   private _WINX_ADCbitAdjust       =          196;  /*ADC bit adjust*/
   private _WINX_gain               =          198;  /*gain*/
   private _WINX_Comments		=			  200;  /*File Comments [5][COMMENTMAX]*/
   private _WINX_geometric      =              600;  /*geometric ops: rotate 0x01,*/
												   /* reverse 0x02, flip 0x04*/
   private _WINX_xlabel     =                  602;  /*intensity display string [LABELMAX]*/
   private _WINX_cleans     =                  618;  /*cleans*/
   private _WINX_NumSkpPerCln          =       620;  /*number of skips per clean.*/
   private _WINX_SpecMirrorPos         =       622;  /*Spectrograph Mirror Positions [2]*/
   private _WINX_SpecSlitPos           =       626;  /*Spectrograph Slit Positions [4]*/
   private _WINX_AutoCleansActive      =       642;  /*T/F*/
   private _WINX_UseContCleansInst     =       644;  /*T/F*/
   private _WINX_AbsorbStripNum        =       646;  /*Absorbance Strip Number*/
   private _WINX_SpecSlitPosUnits      =       648;  /*Spectrograph Slit Position Units*/
   private _WINX_SpecGrooves           =       650;  /*Spectrograph Grating Grooves*/
   private _WINX_srccmp                =       654;  /*number of source comp. diodes*/
   private _WINX_ydim                  =       656;  /*y dimension of raw data.*/
   private _WINX_scramble              =       658;  /*0=scrambled,1=unscrambled*/
   private _WINX_ContinuousCleansFlag  =       660;  /*T/F Continuous Cleans Timing Option*/
   private _WINX_ExternalTriggerFlag   =       662;  /*T/F External Trigger Timing Option*/
   private _WINX_lnoscan               =       664;  /*Number of scans (Early WinX)*/
   private _WINX_lavgexp               =       668;  /*Number of Accumulations*/
   private _WINX_ReadoutTime           =       672;  /*Experiment readout time*/
   private _WINX_TriggeredModeFlag     =       676;  /*T/F Triggered Timing Option*/
   private _WINX_Spare_2               =       678;  /*[10] */
   private _WINX_sw_version            =       688;  /*Version of SW creating this file [FILEVERMAX] */
   private _WINX_type                  =       704;  /* 1 = new120 (Type II)    */         
                                            /* 2 = old120 (Type I ) */          
                                            /* 3 = ST130         */             
                                            /* 4 = ST121          */            
                                            /* 5 = ST138              */        
                                            /* 6 = DC131 (PentaMax)       */    
                                            /* 7 = ST133 (MicroMax/SpectroMax)*/
                                            /* 8 = ST135 (GPIB)  */             
                                            /* 9 = VICCD        */              
                                            /*10 = ST116 (GPIB)  */             
                                            /*11 = OMA3 (GPIB)    */            
                                            /*12 = OMA4           */            
   private _WINX_flatFieldApplied      =       706;  /*1 if flat field was applied.*/
   private _WINX_Spare_3               =       708;  /*[16] */
   private _WINX_kin_trig_mode         =       724;  /*Kinetics Trigger Mode*/
   private _WINX_dlabel                =       726;  /*Data label. [LABELMAX] */
   private _WINX_Spare_4               =       742;  /*[436] */
   private _WINX_PulseFileName		=		  1178;  /*Name of Pulser File with [HDRNAMEMAX]*/
                                                    /*Pulse Widths/Delays (for Z-Slice)*/
   private _WINX_AbsorbFileName     =         1298;  /*Name of Absorbance File (if File Mode) [HDRNAMEMAX]*/
   private _WINX_NumExpRepeats      =         1418;  /*Number of Times experiment repeated*/
   private _WINX_NumExpAccums       =         1422;  /*Number of Time experiment accumulated*/
   private _WINX_YT_Flag            =         1426;  /*Set to 1 if this file contains YT data*/
   private _WINX_clkspd_us          =         1428;  /*Vert Clock Speed in micro-sec*/
   private _WINX_HWaccumFlag        =         1432;  /*set to 1 if accum done by Hardware.*/
   private _WINX_StoreSync          =         1434;  /*set to 1 if store sync used*/
   private _WINX_BlemishApplied     =         1436;  /*set to 1 if blemish removal applied*/
   private _WINX_CosmicApplied      =         1438;  /*set to 1 if cosmic ray removal applied*/
   private _WINX_CosmicType         =         1440;  /*if cosmic ray applied, this is type*/
   private _WINX_CosmicThreshold    =         1442;  /*Threshold of cosmic ray removal.  */
   private _WINX_NumFrames          =         1446;  /*number of frames in file.     */    
   private _WINX_MaxIntensity       =         1450;  /*max intensity of data (future)    */
   private _WINX_MinIntensity       =         1454;  /*min intensity of data (future)    */
   private _WINX_ylabel             =         1458;  /*y axis label. [LABELMAX]             */        
   private _WINX_ShutterType        =         1474;  /*shutter type.                     */
   private _WINX_shutterComp        =         1476;  /*shutter compensation time.        */
   private _WINX_readoutMode        =         1480;  /*readout mode, full,kinetics, etc  */
   private _WINX_WindowSize         =         1482;  /*window size for kinetics only.    */
   private _WINX_clkspd             =         1484;  /*clock speed for kinetics & frame transfer*/
   private _WINX_interface_type     =         1486;  /*computer interface                */
                                                   /* (isa-taxi, pci, eisa, etc.)        */     
   private _WINX_NumROIsInExperiment    =     1488;  /*May be more than the 10 allowed in*/
												   /* this header (if 0, assume 1)        */    
   private _WINX_Spare_5             =        1490;  /*[16]                                */   
   private _WINX_controllerNum       =        1506;  /*if multiple controller system will*/
                                                   /*have controller number data came from.  */
                                                   /*this is a future item.  */                
   private _WINX_SWmade             =         1508;  /*Which software package created this file */
   private _WINX_NumROI             =         1510;  /*number of ROIs used. if 0 assume 1.  */    


/*                                      
//-------------------------------------------------------------------------------

//                        ROI entries   (1512 - 1631)

  struct ROIinfo                           
  {                                                   
    WORD  startx                            //left x start value.               
    WORD  endx                              //right x value.                    
    WORD  groupx                            //amount x is binned/grouped in hw. 
    WORD  starty                            //top y start value.                
    WORD  endy                              //bottom y value.                   
    WORD  groupy                            //amount y is binned/grouped in hw. 
  } ROIinfoblk[ROIMAX]                   
                                            //ROI Starting Offsets:     
                                                 
                                            //  ROI  1  =  1512     
                                            //  ROI  2  =  1524     
                                            //  ROI  3  =  1536     
                                            //  ROI  4  =  1548     
                                            //  ROI  5  =  1560     
                                            //  ROI  6  =  1572     
                                            //  ROI  7  =  1584     
                                            //  ROI  8  =  1596     
                                            //  ROI  9  =  1608     
                                            //  ROI 10  =  1620

//-------------------------------------------------------------------------------
*/
   private _WINX_FlatField           =        1632;  /*Flat field file name.  [HDRNAMEMAX]    */  
   private _WINX_background          =        1752;  /*background sub. file name. [HDRNAMEMAX]   */
   private _WINX_blemish             =        1872;  /*blemish file name.        [HDRNAMEMAX]  */
   private _WINX_file_header_ver     =        1992;  /*version of this file header */
   private _WINX_YT_Info             =        1996;  /*Reserved for YT information [1000] */
   private _WINX_WinView_id          =        2996;  /*== 0x01234567L if file created by WinX*/
/*
//-------------------------------------------------------------------------------

//                        START OF X CALIBRATION STRUCTURE (3000 - 3488)
*/
   private _WINX_X_offset             =         3000;  /*offset for absolute data scaling*/
   private _WINX_X_factor             =         3008;  /*factor for absolute data scaling*/
   private _WINX_X_current_unit       =         3016;  /*selected scaling unit           */
   private _WINX_X_reserved1          =         3017;  /*reserved                        */
   private _WINX_X_string             =         3018;  /*special string for scaling [40] */     
   private _WINX_X_reserved2          =         3058;  /*reserved [40]                    */   
   private _WINX_X_calib_valid        =         3098;  /*flag if calibration is valid    */
   private _WINX_X_input_unit         =         3099;  /*current input units for         */
                                                     /*"calib_value"                  */
   private _WINX_X_polynom_unit       =         3100;  /*linear UNIT and used           */ 
                                                     /*in the "polynom_coeff"         */
   private _WINX_X_polynom_order       =        3101;  /*ORDER of calibration POLYNOM    */
   private _WINX_X_calib_count          =       3102;  /*valid calibration data pairs    */
   private _WINX_X_pixel_position       =       3103;  /*pixel pos. of calibration data [10] */
   private _WINX_X_calib_value          =       3183;  /*calibration VALUE at above pos  [10] */
   private _WINX_X_polynom_coeff        =       3263;  /*polynom COEFFICIENTS  [6]           */
   private _WINX_X_laser_position       =       3311;  /*laser wavenumber for relativ WN */
   private _WINX_X_reserved3            =       3319;  /*reserved                        */
   private _WINX_X_new_calib_flag       =       3320;  /*If set to 200, valid label below*/
   private _WINX_X_calib_label          =       3321;  /*Calibration label (NULL term'd)  [81]*/
   private _WINX_X_expansion            =       3402;  /*Calibration Expansion area  [87]    */

/*
//-------------------------------------------------------------------------------

//                        START OF Y CALIBRATION STRUCTURE (3489 - 3977)
*/

   private _WINX_Y_offset              =        3489;  /*offset for absolute data scaling*/
   private _WINX_Y_factor              =        3497;  /*factor for absolute data scaling*/
   private _WINX_Y_current_unit        =        3505;  /*selected scaling unit           */
   private _WINX_Y_reserved1           =        3506;  /*reserved                        */
   private _WINX_Y_string              =        3507;  /*special string for scaling [40]   */   
   private _WINX_Y_reserved2           =        3547;  /*reserved   [40]                   */   
   private _WINX_Y_calib_valid         =        3587;  /*flag if calibration is valid    */
   private _WINX_Y_input_unit          =        3588;  /*current input units for         */
                                                     /*"calib_value"                   */
   private _WINX_Y_polynom_unit        =        3589;  /*linear UNIT and used            */
                                                     /*in the "polynom_coeff"          */
   private _WINX_Y_polynom_order       =        3590;  /*ORDER of calibration POLYNOM    */
   private _WINX_Y_calib_count         =        3591;  /*valid calibration data pairs    */
   private _WINX_Y_pixel_position      =        3592;  /*pixel pos. of calibration data [10]  */
   private _WINX_Y_calib_value         =        3672;  /*calibration VALUE at above pos [10] */
   private _WINX_Y_polynom_coeff       =        3752;  /*polynom COEFFICIENTS   [6]         */
   private _WINX_Y_laser_position      =        3800;  /*laser wavenumber for relativ WN */
   private _WINX_Y_reserved3           =        3808;  /*reserved                        */
   private _WINX_Y_new_calib_flag      =        3809;  /*If set to 200, valid label below*/
   private _WINX_Y_calib_label         =        3810;  /*Calibration label (NULL term'd) [81] */
   private _WINX_Y_expansion           =        3891;  /*Calibration Expansion area [87]     */

/*

                         END OF CALIBRATION STRUCTURES

-------------------------------------------------------------------------------
*/
   private _WINX_Istring                 =    3978;  /*special intensity scaling string [40]*/
   private _WINX_Spare_6                 =    4018;  /* [25] */
   private _WINX_SpecType                =    4043;  /*spectrometer type (acton, spex, etc.)*/
   private _WINX_SpecModel               =    4044;  /*spectrometer model (type dependent)*/
   private _WINX_PulseBurstUsed          =    4045;  /*pulser burst mode on/off*/
   private _WINX_PulseBurstCount         =    4046;  /*pulser triggers per burst*/
   private _WINX_ulseBurstPeriod         =    4050;  /*pulser burst period (in usec)*/
   private _WINX_PulseBracketUsed        =    4058;  /*pulser bracket pulsing on/off*/
   private _WINX_PulseBracketType        =    4059;  /*pulser bracket pulsing type*/
   private _WINX_PulseTimeConstFast      =    4060;  /*pulser slow exponential time constant (in usec)*/
   private _WINX_PulseAmplitudeFast      =    4068;  /*pulser fast exponential amplitude constant*/
   private _WINX_PulseTimeConstSlow      =    4076;  /*pulser slow exponential time constant (in usec)*/
   private _WINX_PulseAmplitudeSlow      =    4084;  /*pulser slow exponential amplitude constant*/
   private _WINX_AnalogGain              =    4092;  /*analog gain*/
   private _WINX_AvGainUsed              =    4094;  /*avalanche gain was used*/
   private _WINX_AvGain                  =    4096;  /*avalanche gain value*/
   private _WINX_lastvalue               =    4098;  /*Always the LAST value in the header*/

/******************************************************************************************************
                         END OF HEADER

-------------------------------------------------------------------------------


                                      4100  Start of Data



        ***************************** E.O.F.*****************************


  Custom Data Types used in the structure:
  ----------------------------------------

    BYTE = unsigned char
    WORD = unsigned short
    DWORD = unsigned long




  READING DATA:
  -------------

    The data follows the header beginning at offset 4100.

    Data is stored as sequential points.
    
    The X, Y and Frame dimensions are determined by the header.

      The X dimension of the stored data is in "xdim" ( Offset 42  ).
      The Y dimension of the stored data is in "ydim" ( Offset 656 ).
      The number of frames of data stored is in "NumFrames" ( Offset 1446 ).

    The size of a frame (in bytes) is:

      One frame size = xdim x ydim x (datatype Offset 108)

**********************************************************************************************/

_debug = 1;

	_ip_address = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), "");
	if(_ip_address == "")
	{
    	DevLogErr(_nid, "Invalid Crate IP specification");
 		abort();
	}

    _shot = $SHOT;

    write(*, "ST133__store");

	MdsConnect(_ip_address);

	_header = MdsValue('ST133ReadHeader($)', _shot);


	if( size(_header) == _N_HEADER_SIZE)
	{

	    _grooves = ST133HeaderParam(_header, _WINX_SpecGrooves, float(0));	
		if( _debug ) write(*, "_grooves ", _grooves);
		DevPutValue(DevHead(_nid) + _N_SPEC_GROOVES, _grooves);

	    _wave_mid = ST133HeaderParam( _header, _WINX_SpecCenterWlNm, float(0));
		if( _debug ) write(*, "_wave_mid ", _wave_mid);
		DevPutValue(DevHead(_nid) + _N_SPEC_WAV_MID, _wave_mid);

		_mir_pos_type = zero(2, 0W);
		_mirrow_pos = ST133HeaderParam( _header, _WINX_SpecMirrorPos, _mir_pos_type);
		if( _debug ) write(*, "_mirrow_pos ", _mirrow_pos);
		DevPutValue(DevHead(_nid) + _N_SPEC_MIR_POS, _mirrow_pos);

		_glue_flag = ST133HeaderParam( _header, _WINX_SpecGlueFlag, 0B);
		if( _debug ) write(*, "_glue_flag ", _glue_flag);
		DevPutValue(DevHead(_nid) + _N_GLUE_FLAG, _glue_flag);

	    _x_dim_det = ST133HeaderParam( _header, _WINX_xDimDet, 0UW);
		if( _debug ) write(*, "_x_dim_det ", _x_dim_det);
		DevPutValue(DevHead(_nid) + _N_X_DIM_DET, _x_dim_det);

	    _y_dim_det = ST133HeaderParam( _header, _WINX_yDimDet, 0UW);	
		if( _debug ) write(*, "_y_dim_det ", _y_dim_det);
		DevPutValue(DevHead(_nid) + _N_Y_DIM_DET, _y_dim_det);

	    _x_dim = ST133HeaderParam( _header, _WINX_xdim, 0UW);
		if( _debug ) write(*, "_x_dim ", _x_dim);
		DevPutValue(DevHead(_nid) + _N_X_DIM, _x_dim);

	    _y_dim = ST133HeaderParam( _header, _WINX_ydim, 0UW);
		if( _debug ) write(*, "_y_dim ", _y_dim);
		DevPutValue(DevHead(_nid) + _N_Y_DIM, _y_dim);

	    _det_temp = ST133HeaderParam( _header, _WINX_DetTemperature, float(0));
		if( _debug ) write(*, "_det_temp ", _det_temp);
		DevPutValue(DevHead(_nid) + _N_DET_TEMP, _det_temp);

	    _exp_sec = ST133HeaderParam( _header, _WINX_exp_sec, float(0));
		if( _debug ) write(*, "_exp_sec ", _exp_sec);
		DevPutValue(DevHead(_nid) + _N_EXP_TIME, _exp_sec);

		_read_out = ST133HeaderParam( _header, _WINX_ReadoutTime, float(0));
		if( _debug ) write(*, "_read_out ", _read_out);
		DevPutValue(DevHead(_nid) + _N_READ_OUT, _read_out);

		_cleans = ST133HeaderParam( _header, _WINX_cleans, 0UW);
		if( _debug ) write(*, "_cleans ", _cleans);
		DevPutValue(DevHead(_nid) + _N_CLEANS, _cleans);

		_n_skip_cleans = ST133HeaderParam( _header, _WINX_NumSkpPerCln, 0UW);
		if( _debug ) write(*, "_n_skip_cleans ", _n_skip_cleans);
		DevPutValue(DevHead(_nid) + _N_N_SKIP_CLEAN, _n_skip_cleans);

/*                      
		_n_scans = ST133HeaderParam( _header, _WINX_lnoscan, 0UW);
		if( _debug ) write(*, "_n_scans ", _n_scans);
*/
		_n_frames = ST133HeaderParam( _header, _WINX_NumFrames, 0UW);
		if( _debug ) write(*, "_n_frames ", _n_frames);
		DevPutValue(DevHead(_nid) + _N_N_FRAMES, _n_frames);

/*
		_n_exp_roi = ST133HeaderParam( _header, _WINX_NumROIsInExperiment, 0UW);
		if(_n_exp_roi == 0) _n_exp_roi = 1;
		if( _debug ) write(*, "_n_exp_roi ", _n_exp_roi);
*/

		_num_roi = ST133HeaderParam( _header, _WINX_NumROI, 0UW);
		if(_num_roi == 0) _num_roi = 1;
		if( _debug ) write(*, "_num_roi ", _num_roi);
		DevPutValue(DevHead(_nid) + _N_NROI, _num_roi);

		_roi_type = zero(6 * _ROIMAX, 0UW);
		_roi = ST133HeaderParam( _header, _WINX_NumROI+2, _roi_type);
		if( _debug ) write(*, "_roi ",  _roi );
		DevPutValue(DevHead(_nid) + _N_ROI, _roi );

		_num_acc = ST133HeaderParam( _header, _WINX_lavgexp, 0L);
		if( _debug ) write(*, "_num_acc ", _num_acc);
		DevPutValue(DevHead(_nid) + _N_NUM_ACC, _num_acc);
		
		_calib_x_valid = ST133HeaderParam( _header, _WINX_X_calib_valid, 0B);
		if( _debug ) write(*, "_calib_x_valid ", _calib_x_valid);
		DevPutValue(DevHead(_nid) + _N_XCAL_VALID, _calib_x_valid);

		if(_calib_x_valid)
		{
			_cal_x_type = repeat(" ",40);
			_cal_x_string = ST133HeaderParam( _header, _WINX_X_string, _cal_x_type);

			_cal_x_pol_ord = ST133HeaderParam( _header, _WINX_X_polynom_order, 0B);

			_poly_type = zero(6, ft_float(0));
			_cal_x_coeff = ST133HeaderParam( _header, _WINX_X_polynom_coeff, _poly_type);
		}
		else
		{ 
			_cal_x_string = "";
			_cal_x_pol_ord = 0;
			_cal_x_count = 0;
			_cal_x_coeff = zero(6, float(0));
		}

		if( _debug ) write(*, "_cal_x_string ", _cal_x_string);
		DevPutValue(DevHead(_nid) + _N_XCAL_STING, _cal_x_string);

		if( _debug ) write(*, "_cal_x_pol_ord ", _cal_x_pol_ord);
		DevPutValue(DevHead(_nid) + _N_XCAL_POL_ORD, _cal_x_pol_ord);

		if( _debug ) write(*, "_cal_x_coeff ", _cal_x_coeff);
		DevPutValue(DevHead(_nid) + _N_XCAL_COEFF, _cal_x_coeff);

		DevPutValue(DevHead(_nid) + _N_HEADER, _header);

		_data = MdsValue('ST133ReadData($)', _shot);
			
		 DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_clock = 0);

		 if(_ext_clock == 1)
	         {
			_clock = evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE));
			_clock = execute('evaluate(`_clock)');
			if( _debug ) write(*,_clock);
			_trig_time = dscptr(_clock, 0);
			_delta = dscptr(_clock, 2);
		 }
		 else
		 {
			_trig_time = 0.0;
			_delta = _exp_sec;
		 }
		
		 _end_time = _trig_time + (_n_frames - 1) * _delta;

		_dim =  " "//_trig_time//" : "//_end_time//" : "//_delta;


		_sig_img = compile('build_signal(($VALUE), set_range(`_x_dim, `_y_dim, `_n_frames, data(`_data)), '//_dim//'  )');
		
			
		_sig_nid =  DevHead(_nid) + _N_DATA;
		_status = TreeShr->TreePutRecord(val(_sig_nid),xd(_sig_img),val(0));
		if(! (_status & 1))
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}


	} else {
		MdsDisconnect();
		DevLogErr(_nid, 'File not found for shot: '//_shot);
		abort();
	}


	MdsDisconnect();

    return(1);
}
