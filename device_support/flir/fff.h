/***********************************************************************
 *
 * Description of file:
 *   Definition of FFF fileformat
 *
 ***********************************************************************/
#ifndef FFF_H
#define FFF_H

/*=====================================================================*/

#define FLIR_FILE_FORMAT 0x46464600 /* FFF\0 */
#define FILE_FORMAT_VERSION 100     /* 1.00  */
#define FFF_EXT              \
  "FFF" /* Default file name \
           extention */

// Bit mask for trig types
#define FPGA_TRIG_TYPE_MARK 0x01
#define FPGA_TRIG_TYPE_MARK_START 0x02
#define FPGA_TRIG_TYPE_MARK_STOP 0x04
#define FPGA_TRIG_TYPE_MARK_ENABLE 0x08  // Enable image flow
#define FPGA_TRIG_TYPE_MARK_DISABLE 0x10 // Disable image flow

#define FPGA_TRIG_STATE_DEASSERTED 0
#define FPGA_TRIG_STATE_ASSERTED 1

typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned int ULONG;

struct _fpgaheader_t
{
  unsigned char major;
  unsigned char minor;
  unsigned char dp1_trig_type;   // Digital port 1, trig type
  unsigned char dp2_trig_type;   // Digital port 2, trig type
  unsigned char dp1_frame_ts;    // Digital port 1, frame time stamp
  unsigned char dp1_line_ts_MSB; // Digital port 1, line time stamp (MSB)
  unsigned char dp1_line_ts_LSB; // Digital port 1, line time stamp (LSB)
  unsigned char dp2_frame_ts;    // Digital port 2, frame time stamp
  unsigned char dp2_line_ts_MSB; // Digital port 2, line time stamp (MSB)
  unsigned char dp2_line_ts_LSB; // Digital port 2, line time stamp (LSB)

  unsigned char dp1_trig_state; // Digital port 1, trig state
  unsigned char dp2_trig_state; // Digital port 2, trig state

  unsigned char frame_cnt_MSB;
  unsigned char frame_cnt_LSB;

  unsigned char reserved[18];
};

typedef struct _fpgaheader_t FPGA_HEADER;

// main types

typedef enum
{
  /* General tags */
  FFF_TAGID_FREE = 0, /* Marks unused tag descriptor */

  FFF_TAGID_Pixels = 1,
  FFF_TAGID_GainMap = 2,
  FFF_TAGID_OffsMap = 3,
  FFF_TAGID_DeadMap = 4,
  FFF_TAGID_GainDeadMap = 5,
  FFF_TAGID_CoarseMap = 6,
  FFF_TAGID_ImageMap = 7,

  FFF_TAGID_SubFlirFileHead = 0x1e,
  FFF_general_high = 0x1f, /* Reserve space for other general
                              tags */

  /* FLIR TAGs */
  FFF_TAGID_BasicData = 0x20,
  FFF_TAGID_Measure,
  FFF_TAGID_ColorPal,
  FFF_TAGID_TextComment,
  FFF_TAGID_VoiceComment,

  FFF_TAGID_matrix_high = 0x3f, /* reserve space for other system
                                   image blocks */

  /* FLIR Boston reserved TAG number series */
  FFF_TAGID_Boston_reserved = 0x40,
  FFF_TAGID_Boston_reserved_high = 0x5f,

  FFF_highnum = 0x100 /* Guarantee 2 unsigned chars enum */
} TAG_MAIN_T;

/* Sub Tags for FFF_TAGID_Pixels */

enum
{
  FFF_Pixels_BE = 1, /* Big endian pixel data block */
  FFF_Pixels_LE = 2, /* Little endian pixel data block */
  FFF_Pixels_PNG = 3 /* PNG compressed pixel data block (BE or LE) */
};
/* When appropriate, add subID enums for other TAG_MAIN_T too */

/* When subtype isn't used, fill subtype with FFF_SubID_default */
enum
{
  FFF_SubID_default = 1
};

#pragma pack(push, 1)

typedef struct tagFLIRFILEHEAD
{
  char szFormatID[4];           /* Fileformat ID 'FFF\0'  4   4       */
  char szOrigin[16];            /* File origin           16  20       */
  unsigned int dwVersion;       /* File format version    4  24       */
  unsigned int dwIndexOff;      /* Pointer to indexes     4  28       */
  unsigned int dwNumUsedIndex;  /* Number of indexes      4  32       */
  unsigned int dwNextID;        /* Next free index ID     4  36       */
  unsigned short usSwapPattern; /* Swap pattern (0=MSBF)  2  38       */
  unsigned short usSpare[7];    /* Spare                 14  52       */
  unsigned int reserved[2];     /* reserved               8  60       */
  unsigned int dwChecksum;      /* Head & index checksum  4  64 unsigned chars */
} FLIRFILEHEAD;

typedef struct _geometric_info_t
{
  /** Size of one pixel in unsigned chars.
      Normal size is 2 unsigned chars (16 bit pixels)
      or 3 (for colorized YCbCr pixels) */
  unsigned short pixelSize;

  unsigned short imageWidth;  //!< Image width in pixels
  unsigned short imageHeight; //!< Image height in pixels

  /** @name Upper left coordinates
      X and Y coordinates for upper left corner
      relative original in case this image is a
      cutout, normally 0 */
  /*@{*/
  unsigned short upperLeftX;
  unsigned short upperLeftY;
  /*@}*/

  /** @name Valid pixels
      The following four number identifies the
      valid pixels area within the image.
      Sometimes the first row and column only
      contains calibration pixels that should not
      be considered as real pixels */
  /*@{*/
  unsigned short firstValidX;  //!< Normally 0
  unsigned short lastValidX;   //!< Normally imageWidth - 1
  unsigned short firstValidY;  //!< Normally 0
  unsigned short lastValidY;   //!< Normally imageHeight - 1
                               /*@}*/
  unsigned short detectorDeep; //!< Number of bits from detector A/D

  /** Type of detector to be able to differ
      between technologies if necessary.
      Defined in AppCore/core_imgflow/imgflow_state.hpp */
  unsigned short detectorID;
  /**  Type of upsampling from Detector to IR pixels.
       Defined in AppCore/core_imgflow/imgflow_state.hpp */
  unsigned short upSampling;
  unsigned short frameCtr;      //!< Image number from frame buffer
  unsigned short minMeasRadius; //!< See AppCore/core_imgflow/imgflow_state.hpp
                                //!< for reference
  unsigned char
      stripeFields;         //!< Number of striped fields this image consists of
  unsigned char reserved;   //!< For future use - should be set to 0
  unsigned short reserved1; //!< For future use - should be set to 0
} GEOMETRIC_INFO_T;         //!< sizeof struct == 32 unsigned chars

struct IMAGEINFO_T
{
  unsigned int imageTime;       //!< Time in seconds since 1970-01-01 00:00 (UTC)
  unsigned long imageMilliTime; //!< Milliseconds since last second

  short timeZoneBias;          //!< Time zone bias in minutes
                               //!    UTC = local time + bias
  short swReserved1;           //!< filler == 0
  long focusPos;               //!< Focus position as counter value
  float fTSTemp[7];            //!< Temp sensor values converted to Kelvin
  float fTSTempExt[4];         //!< Lens temp sensors et.c. Converted to Kelvin
  unsigned short trigInfoType; //!< 0 = No info, 1 = THV 900 type info
  unsigned short trigHit;      //!< hit count - microsecs from trig
                               //!    reference
  unsigned short trigFlags;    //!< trig flags, type dependent
  unsigned short reserved1;
  unsigned int trigCount; //!< trig counter
  short manipulType;      //!< defines how to interpret manipFactors
  short manipFactors[5];  //!< Used average factors
  /** Detecor settings - camera type dependent */
  int detectorPars[20]; //!< Currently used detector parameters like
                        //!    used bias, offsets. Usage is camera
                        //!    dependent
  int reserved[5];      //!< For future use

}; //!< sizeof struct == 184 unsigned chars

/**
   Distribution information
*/
struct DISTR_DATA_T
{
  /** Framegrab independent distribution data */
  char imgName[16]; /* (4 longs) */

  unsigned short distrLive;     //!< TRUE (1) when image distribution is
                                //!	'LIVE'. FALSE (0) otherwise
  unsigned short distrRecalled; //!< TRUE (1) when image distribution is
                                //!       recalled. FALSE (0) otherwise.
                                //!< TRUE also implies that distrLive ==
                                //!       FALSE
  int curGlobalOffset;
  float curGlobalGain; //!< globalOffset/Gain to generate LUT from
                       //!  updated continously when live only
#define regulationOn 1
  unsigned short regulMethodMask; //!< Method used for o/g calculation
  unsigned short visualImage;     //!< TRUE (1) for TV (visual)
                                  //!  FALSE (0) for IR image
  float focusDistance;            //!< focusDistance in meters.
                                  //!    0 means not defined.
                                  //!    NOT calculated by image source

  unsigned short StripeHeight; //!< 0 = not striped
  unsigned short StripeStart;  //!< Striping start line if striped
  unsigned short imageFreq;    //!< Image frequency, defines the nominal
                               //!    image frequency in Hz
  unsigned short typePixStreamCtrlData;
  //!< 0 = no such data,
  //!    other types TBD
  unsigned short PixStreamDataLine;
  //!< At which line to find
  //!    PixStreamCtrlData if any

#define IMGSMSK_NUC 0x20 //!< Bit set means that NUC is in progress

  short errStatus; //!< bit field, mask definitions above

  unsigned short imageMilliFreq; //!< Image frequency, milliHz part of imageFreq

  short reserved; //!< For future use
  int reserved2[3];
}; //!< sizeof struct == 64 unsigned chars

typedef struct _pres_par_t
{
  signed int level; /* Level as normalized pixel value (apix), Level is defined
                       as middle of span (in pixel units) */
  signed int span;  /* Span as normalized pixel value (apix) */
  unsigned char reserved[40];
} PRES_PAR_T; /* sizeof struct == 48 unsigned chars */

/**
 *  FFF tag index
 */
typedef struct tagFLIRFILEINDEX
{
  unsigned short wMainType; /**< Main type of index     2   2
                         Should be one of TAG_MAIN_T        */
  unsigned short wSubType;  /**< Sub type of index      2   4       */
  unsigned int dwVersion;   /**< Version for data       4   8       */
  unsigned int dwIndexID;   /**< Index ID               4  12       */
  unsigned int dwDataPtr;   /**< Pointer to data        4  16       */
  unsigned int dwDataSize;  /**< Size of data           4  20       */
  unsigned int dwParent;    /**< Parentnr               4  24
                          may be set to 0 when not used      */
  unsigned int dwObjectNr;  /**< This object nr         4  28
                          may be set to 0 when not used      */
  unsigned int dwChecksum;  /**< Data checksum          4  32 unsigned chars
 
    dwChecksum is a checksum of the tag data block. It may be
    set to 0 when not used. The algorithm is CRC32 with the
    Ethernet polynom. */
} FLIRFILEINDEX;

struct _bidata_t
{
  GEOMETRIC_INFO_T GeometricInfo; // 32 unsigned chars
  unsigned char ObjectParameters[48];
  unsigned char CalibParameters[132];
  unsigned char CalibInfo[564];
  unsigned char AdjustParameters[48];
  PRES_PAR_T PresentParameters;
  unsigned char DisplayParameters[28];
  IMAGEINFO_T ImageInfo;
  DISTR_DATA_T DistributionData;
};

typedef struct _bidata_t BI_DATA_T;

#pragma pack(pop)

/*---------------------------------------------------------------------*/
/* #endif for fff.h include                                            */
/*---------------------------------------------------------------------*/
#endif
