#ifndef MDSMSG_CONSTANTS

#define MDSMSG_CONSTANTS
#define MSG_FATAL 0
#define MSG_SUCCESS 1
#define MSG_ERROR 2
#define MSG_INFO 3
#define MSG_WARNING 4
#define MSG_MASK 0x8008000
#define MSG_FACNUM_M 0x40000
#define MSG_MSGNUM_M 8

#endif

#define DEV$_BAD_ENDIDX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_FILTER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 2 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_FREQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 3 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_GAIN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 4 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_HEADER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 5 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_HEADER_IDX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 6 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_MEMORIES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 7 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_MODE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 8 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_NAME \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 9 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_OFFSET \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 10 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_STARTIDX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 11 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_NOT_TRIGGERED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 12 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_FREQ_TOO_HIGH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 13 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_INVALID_NOC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 14 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_RANGE_MISMATCH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 15 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAMACERR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 16 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_VERBS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 17 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_COMMANDS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 18 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_ADNR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 19 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_ERR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 20 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_LOSYNC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 21 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_LPE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 22 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_TMO \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 23 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_TPE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 24 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_STE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 25 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_DERR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 26 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_SQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 27 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_NOSQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 28 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_SX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 29 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_NOSX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 30 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_INV_SETUP \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 31 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 51 * MSG_MSGNUM_M + MSG_SUCCESS)
#define RETICON$_BAD_FRAMES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 52 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_BAD_FRAME_SELECT \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 53 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_BAD_NUM_STATES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 54 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_BAD_PERIOD \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 55 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_BAD_PIXEL_SELECT \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 56 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_DATA_CORRUPTED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 57 * MSG_MSGNUM_M + MSG_ERROR)
#define RETICON$_TOO_MANY_FRAMES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 58 * MSG_MSGNUM_M + MSG_ERROR)
#define J221$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 101 * MSG_MSGNUM_M + MSG_SUCCESS)
#define J221$_INVALID_DATA \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 102 * MSG_MSGNUM_M + MSG_WARNING)
#define J221$_NO_DATA \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 103 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_INVCLKFRQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 151 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_INVDELDUR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 152 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_INVOUTCTR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 153 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_INVPSEUDODEV \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 154 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_INVTRGMOD \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 155 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_NOPSEUDODEV \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 156 * MSG_MSGNUM_M + MSG_INFO)
#define TIMING$_TOO_MANY_EVENTS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 157 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_INVDUTY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 158 * MSG_MSGNUM_M + MSG_ERROR)
#define TIMING$_ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 159 * MSG_MSGNUM_M + MSG_ERROR)
#define B2408$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 201 * MSG_MSGNUM_M + MSG_SUCCESS)
#define B2408$_OVERFLOW \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 202 * MSG_MSGNUM_M + MSG_INFO)
#define B2408$_TRIG_LIM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 203 * MSG_MSGNUM_M + MSG_WARNING)
#define FERA$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 251 * MSG_MSGNUM_M + MSG_SUCCESS)
#define FERA$_DIGNOTSTRARRAY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 252 * MSG_MSGNUM_M + MSG_ERROR)
#define FERA$_NODIG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 253 * MSG_MSGNUM_M + MSG_ERROR)
#define FERA$_MEMNOTSTRARRAY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 254 * MSG_MSGNUM_M + MSG_ERROR)
#define FERA$_NOMEM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 255 * MSG_MSGNUM_M + MSG_ERROR)
#define FERA$_PHASE_LOST \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 256 * MSG_MSGNUM_M + MSG_ERROR)
#define FERA$_CONFUSED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 257 * MSG_MSGNUM_M + MSG_WARNING)
#define FERA$_OVER_RUN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 258 * MSG_MSGNUM_M + MSG_WARNING)
#define FERA$_OVERFLOW \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 259 * MSG_MSGNUM_M + MSG_INFO)
#define HM650$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 301 * MSG_MSGNUM_M + MSG_SUCCESS)
#define HM650$_DLYCHNG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 302 * MSG_MSGNUM_M + MSG_WARNING)
#define HV4032$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 351 * MSG_MSGNUM_M + MSG_SUCCESS)
#define HV4032$_WRONG_POD_TYPE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 352 * MSG_MSGNUM_M + MSG_ERROR)
#define HV1440$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 401 * MSG_MSGNUM_M + MSG_SUCCESS)
#define HV1440$_WRONG_POD_TYPE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 402 * MSG_MSGNUM_M + MSG_ERROR)
#define HV1440$_BAD_FRAME \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 403 * MSG_MSGNUM_M + MSG_ERROR)
#define HV1440$_BAD_RANGE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 404 * MSG_MSGNUM_M + MSG_ERROR)
#define HV1440$_OUTRNG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 405 * MSG_MSGNUM_M + MSG_ERROR)
#define HV1440$_STUCK \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 406 * MSG_MSGNUM_M + MSG_ERROR)
#define JOERGER$_BAD_PRE_TRIGGER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 451 * MSG_MSGNUM_M + MSG_ERROR)
#define JOERGER$_BAD_ACT_MEMORY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 452 * MSG_MSGNUM_M + MSG_ERROR)
#define JOERGER$_BAD_GAIN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 453 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_BAD_WAVE_LENGTH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 501 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_BAD_SLIT_WIDTH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 502 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_BAD_NUM_SPECTRA \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 503 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_BAD_GRATING \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 504 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_BAD_EXPOSURE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 505 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_BAD_FILTER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 506 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_GO_FILE_ERROR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 507 * MSG_MSGNUM_M + MSG_ERROR)
#define U_OF_M$_DATA_FILE_ERROR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 508 * MSG_MSGNUM_M + MSG_ERROR)
#define IDL$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 551 * MSG_MSGNUM_M + MSG_SUCCESS)
#define IDL$_ERROR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 552 * MSG_MSGNUM_M + MSG_ERROR)
#define B5910A$_BAD_CHAN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 601 * MSG_MSGNUM_M + MSG_ERROR)
#define B5910A$_BAD_CLOCK \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 602 * MSG_MSGNUM_M + MSG_ERROR)
#define B5910A$_BAD_ITERATIONS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 603 * MSG_MSGNUM_M + MSG_ERROR)
#define B5910A$_BAD_NOC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 604 * MSG_MSGNUM_M + MSG_ERROR)
#define B5910A$_BAD_SAMPS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 605 * MSG_MSGNUM_M + MSG_ERROR)
#define J412$_NOT_SORTED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 651 * MSG_MSGNUM_M + MSG_ERROR)
#define J412$_NO_DATA \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 652 * MSG_MSGNUM_M + MSG_ERROR)
#define J412$_BADCYCLES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 653 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 701 * MSG_MSGNUM_M + MSG_SUCCESS)
#define TR16$_BAD_MEMSIZE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 702 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_BAD_ACTIVEMEM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 703 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_BAD_ACTIVECHAN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 704 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_BAD_PTS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 705 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_BAD_FREQUENCY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 706 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_BAD_MASTER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 707 * MSG_MSGNUM_M + MSG_ERROR)
#define TR16$_BAD_GAIN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 708 * MSG_MSGNUM_M + MSG_ERROR)
#define A14$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 751 * MSG_MSGNUM_M + MSG_SUCCESS)
#define A14$_BAD_CLK_DIVIDE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 752 * MSG_MSGNUM_M + MSG_ERROR)
#define A14$_BAD_MODE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 753 * MSG_MSGNUM_M + MSG_ERROR)
#define A14$_BAD_CLK_POLARITY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 754 * MSG_MSGNUM_M + MSG_ERROR)
#define A14$_BAD_STR_POLARITY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 755 * MSG_MSGNUM_M + MSG_ERROR)
#define A14$_BAD_STP_POLARITY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 756 * MSG_MSGNUM_M + MSG_ERROR)
#define A14$_BAD_GATED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 757 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_NORMAL \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 801 * MSG_MSGNUM_M + MSG_SUCCESS)
#define L6810$_BAD_ACTIVECHAN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 802 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_ACTIVEMEM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 803 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_FREQUENCY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 804 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_FULL_SCALE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 805 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_MEMORIES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 806 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_COUPLING \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 807 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_OFFSET \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 808 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_SEGMENTS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 809 * MSG_MSGNUM_M + MSG_ERROR)
#define L6810$_BAD_TRIG_DELAY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 810 * MSG_MSGNUM_M + MSG_ERROR)
#define J_DAC$_OUTRNG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 851 * MSG_MSGNUM_M + MSG_ERROR)
#define INCAA$_BAD_ACTIVE_CHANS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 901 * MSG_MSGNUM_M + MSG_ERROR)
#define INCAA$_BAD_MASTER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 902 * MSG_MSGNUM_M + MSG_ERROR)
#define INCAA$_BAD_EXT_1MHZ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 903 * MSG_MSGNUM_M + MSG_ERROR)
#define INCAA$_BAD_PTSC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 904 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_BAD_HEADER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 951 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_BAD_MEMORIES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 952 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_BAD_NOC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 953 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_BAD_OFFSET \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 954 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_BAD_PTS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 955 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_FREQ_TOO_HIGH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 956 * MSG_MSGNUM_M + MSG_ERROR)
#define L8212$_INVALID_NOC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 957 * MSG_MSGNUM_M + MSG_ERROR)
#define MPB$_BADTIME \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1001 * MSG_MSGNUM_M + MSG_ERROR)
#define MPB$_BADFREQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1002 * MSG_MSGNUM_M + MSG_ERROR)
#define L8828$_BAD_OFFSET \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1051 * MSG_MSGNUM_M + MSG_ERROR)
#define L8828$_BAD_PRETRIG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1052 * MSG_MSGNUM_M + MSG_ERROR)
#define L8828$_BAD_ACTIVEMEM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1053 * MSG_MSGNUM_M + MSG_ERROR)
#define L8828$_BAD_CLOCK \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1054 * MSG_MSGNUM_M + MSG_ERROR)
#define L8818$_BAD_OFFSET \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1101 * MSG_MSGNUM_M + MSG_ERROR)
#define L8818$_BAD_PRETRIG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1102 * MSG_MSGNUM_M + MSG_ERROR)
#define L8818$_BAD_ACTIVEMEM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1103 * MSG_MSGNUM_M + MSG_ERROR)
#define L8818$_BAD_CLOCK \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1104 * MSG_MSGNUM_M + MSG_ERROR)
#define J_TR612$_BAD_ACTMEM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1111 * MSG_MSGNUM_M + MSG_ERROR)
#define J_TR612$_BAD_PRETRIG \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1112 * MSG_MSGNUM_M + MSG_ERROR)
#define J_TR612$_BAD_MODE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1113 * MSG_MSGNUM_M + MSG_ERROR)
#define J_TR612$_BAD_FREQUENCY \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1114 * MSG_MSGNUM_M + MSG_ERROR)
#define L8206$_NODATA \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1151 * MSG_MSGNUM_M + MSG_WARNING)
#define H912$_BAD_CLOCK \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1161 * MSG_MSGNUM_M + MSG_ERROR)
#define H912$_BAD_BLOCKS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1162 * MSG_MSGNUM_M + MSG_ERROR)
#define H912$_BAD_PTS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1163 * MSG_MSGNUM_M + MSG_ERROR)
#define H908$_BAD_CLOCK \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1171 * MSG_MSGNUM_M + MSG_ERROR)
#define H908$_BAD_ACTIVE_CHANS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1172 * MSG_MSGNUM_M + MSG_ERROR)
#define H908$_BAD_PTS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1173 * MSG_MSGNUM_M + MSG_ERROR)
#define DSP2904$_CHANNEL_READ_ERROR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1181 * MSG_MSGNUM_M + MSG_ERROR)
#define PY$_UNHANDLED_EXCEPTION \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1191 * MSG_MSGNUM_M + MSG_ERROR)
#define DT196B$_NO_SAMPLES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1201 * MSG_MSGNUM_M + MSG_ERROR)
#define ACQ$_INITIALIZATION_ERROR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1202 * MSG_MSGNUM_M + MSG_ERROR)
#define ACQ$_SETTINGS_NOT_LOADED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1203 * MSG_MSGNUM_M + MSG_ERROR)
#define ACQ$_WRONG_TREE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1204 * MSG_MSGNUM_M + MSG_ERROR)
#define ACQ$_WRONG_PATH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1205 * MSG_MSGNUM_M + MSG_ERROR)
#define ACQ$_WRONG_SHOT \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1206 * MSG_MSGNUM_M + MSG_ERROR)

#ifdef MSG_LIBRARY
#include <stdlib.h>
EXPORT int getmsg(int sts, char **facnam, char **msgnam, char **msgtext)
{
  static struct msg
  {
    int sts;
    char *facnam;
    char *msgnam;
    char *msgtext;
  } msgs[] = {
      {DEV$_BAD_ENDIDX, "DEV", "BAD_ENDIDX",
       "unable to read end index for channel"},
      {DEV$_BAD_FILTER, "DEV", "BAD_FILTER", "illegal filter selected"},
      {DEV$_BAD_FREQ, "DEV", "BAD_FREQ",
       "illegal digitization frequency selected"},
      {DEV$_BAD_GAIN, "DEV", "BAD_GAIN", "illegal gain selected"},
      {DEV$_BAD_HEADER, "DEV", "BAD_HEADER", "unable to read header selection"},
      {DEV$_BAD_HEADER_IDX, "DEV", "BAD_HEADER_IDX",
       "unknown header configuration index"},
      {DEV$_BAD_MEMORIES, "DEV", "BAD_MEMORIES",
       "unable to read number of memory modules"},
      {DEV$_BAD_MODE, "DEV", "BAD_MODE", "illegal mode selected"},
      {DEV$_BAD_NAME, "DEV", "BAD_NAME", "unable to read module name"},
      {DEV$_BAD_OFFSET, "DEV", "BAD_OFFSET", "illegal offset selected"},
      {DEV$_BAD_STARTIDX, "DEV", "BAD_STARTIDX",
       "unable to read start index for channel"},
      {DEV$_NOT_TRIGGERED, "DEV", "NOT_TRIGGERED",
       "device was not triggered,  check wires and triggering device"},
      {DEV$_FREQ_TOO_HIGH, "DEV", "FREQ_TOO_HIGH",
       "the frequency is set to high for the requested number of channels"},
      {DEV$_INVALID_NOC, "DEV", "INVALID_NOC",
       "the NOC (number of channels) requested is greater than the physical "
       "number of channels"},
      {DEV$_RANGE_MISMATCH, "DEV", "RANGE_MISMATCH",
       "the range specified on the menu doesn't match the range setting on the "
       "device"},
      {DEV$_CAMACERR, "DEV", "CAMACERR", "Error doing CAMAC IO"},
      {DEV$_BAD_VERBS, "DEV", "BAD_VERBS",
       "Error reading interpreter list (:VERBS)"},
      {DEV$_BAD_COMMANDS, "DEV", "BAD_COMMANDS", "Error reading command list"},
      {DEV$_CAM_ADNR, "DEV", "CAM_ADNR",
       "CAMAC: Address not recognized (2160)"},
      {DEV$_CAM_ERR, "DEV", "CAM_ERR",
       "CAMAC: Error reported by crate controler"},
      {DEV$_CAM_LOSYNC, "DEV", "CAM_LOSYNC",
       "CAMAC: Lost Syncronization error"},
      {DEV$_CAM_LPE, "DEV", "CAM_LPE", "CAMAC: Longitudinal Parity error"},
      {DEV$_CAM_TMO, "DEV", "CAM_TMO", "CAMAC: Highway time out error"},
      {DEV$_CAM_TPE, "DEV", "CAM_TPE", "CAMAC: Transverse Parity error"},
      {DEV$_CAM_STE, "DEV", "CAM_STE", "CAMAC: Serial Transmission error"},
      {DEV$_CAM_DERR, "DEV", "CAM_DERR", "CAMAC: Delayed error from SCC"},
      {DEV$_CAM_SQ, "DEV", "CAM_SQ", "CAMAC: I/O completion with Q = 1"},
      {DEV$_CAM_NOSQ, "DEV", "CAM_NOSQ", "CAMAC: I/O completion with Q = 0"},
      {DEV$_CAM_SX, "DEV", "CAM_SX", "CAMAC: I/O completion with X = 1"},
      {DEV$_CAM_NOSX, "DEV", "CAM_NOSX", "CAMAC: I/O completion with X = 0"},
      {DEV$_INV_SETUP, "DEV", "INV_SETUP", "device was not properly set up"},
      {RETICON$_NORMAL, "RETICON", "NORMAL", "successful completion"},
      {RETICON$_BAD_FRAMES, "RETICON", "BAD_FRAMES",
       "frame count must be less than or equal to 2048"},
      {RETICON$_BAD_FRAME_SELECT, "RETICON", "BAD_FRAME_SELECT",
       "frame interval must be 1,2,4,8,16,32 or 64"},
      {RETICON$_BAD_NUM_STATES, "RETICON", "BAD_NUM_STATES",
       "number of states must be between 1 and 4"},
      {RETICON$_BAD_PERIOD, "RETICON", "BAD_PERIOD",
       "period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 "
       "msec"},
      {RETICON$_BAD_PIXEL_SELECT, "RETICON", "BAD_PIXEL_SELECT",
       "pixel selection must be an array of 256 boolean values"},
      {RETICON$_DATA_CORRUPTED, "RETICON", "DATA_CORRUPTED",
       "data in memory is corrupted or framing error detected, no data stored"},
      {RETICON$_TOO_MANY_FRAMES, "RETICON", "TOO_MANY_FRAMES",
       "over 8192 frame start indicators in data read from memory"},
      {J221$_NORMAL, "J221", "NORMAL", "successful completion"},
      {J221$_INVALID_DATA, "J221", "INVALID_DATA",
       "ignoring invalid data in channel !SL"},
      {J221$_NO_DATA, "J221", "NO_DATA",
       "no valid data was found for any channel"},
      {TIMING$_INVCLKFRQ, "TIMING", "INVCLKFRQ", "Invalid clock frequency"},
      {TIMING$_INVDELDUR, "TIMING", "INVDELDUR",
       "Invalid pulse delay or duration, must be less than 655 seconds"},
      {TIMING$_INVOUTCTR, "TIMING", "INVOUTCTR",
       "Invalid output mode selected"},
      {TIMING$_INVPSEUDODEV, "TIMING", "INVPSEUDODEV",
       "Invalid pseudo device attached to this decoder channel"},
      {TIMING$_INVTRGMOD, "TIMING", "INVTRGMOD",
       "Invalid trigger mode selected"},
      {TIMING$_NOPSEUDODEV, "TIMING", "NOPSEUDODEV",
       "No Pseudo device attached to this channel ... disabling"},
      {TIMING$_TOO_MANY_EVENTS, "TIMING", "TOO_MANY_EVENTS",
       "More than 16 events used by this decoder"},
      {TIMING$_INVDUTY, "TIMING", "INVDUTY",
       "Invalid duty cycle specified for clock"},
      {TIMING$_, "TIMING", "", "<"},
      {B2408$_NORMAL, "B2408", "NORMAL", "successful completion"},
      {B2408$_OVERFLOW, "B2408", "OVERFLOW",
       "Triggers received after overflow"},
      {B2408$_TRIG_LIM, "B2408", "TRIG_LIM", "Trigger limit possibly exceeded"},
      {FERA$_NORMAL, "FERA", "NORMAL", "successful completion"},
      {FERA$_DIGNOTSTRARRAY, "FERA", "DIGNOTSTRARRAY",
       "The digitizer names must be an array of strings"},
      {FERA$_NODIG, "FERA", "NODIG", "The digitizer names must be specified"},
      {FERA$_MEMNOTSTRARRAY, "FERA", "MEMNOTSTRARRAY",
       "The memory names must be an array of strings"},
      {FERA$_NOMEM, "FERA", "NOMEM", "The memory names must be specified"},
      {FERA$_PHASE_LOST, "FERA", "PHASE_LOST",
       "Data phase lost No FERA data stored"},
      {FERA$_CONFUSED, "FERA", "CONFUSED",
       "Fera Data inconsitant.  Data for this point zered."},
      {FERA$_OVER_RUN, "FERA", "OVER_RUN",
       "Possible FERA memory overrun, too many triggers."},
      {FERA$_OVERFLOW, "FERA", "OVERFLOW",
       "Possible FERA data saturated.  Data point zeroed"},
      {HM650$_NORMAL, "HM650", "NORMAL", "successful completion"},
      {HM650$_DLYCHNG, "HM650", "DLYCHNG",
       "HM650 requested delay can not be processed by hardware."},
      {HV4032$_NORMAL, "HV4032", "NORMAL", "successful completion"},
      {HV4032$_WRONG_POD_TYPE, "HV4032", "WRONG_POD_TYPE",
       "HV40321A n and p can only be used with the HV4032 device"},
      {HV1440$_NORMAL, "HV1440", "NORMAL", "successful completion"},
      {HV1440$_WRONG_POD_TYPE, "HV1440", "WRONG_POD_TYPE",
       "HV1443 can only be used with the HV1440 device"},
      {HV1440$_BAD_FRAME, "HV1440", "BAD_FRAME",
       "HV1440 could not read the frame"},
      {HV1440$_BAD_RANGE, "HV1440", "BAD_RANGE",
       "HV1440 could not read the range"},
      {HV1440$_OUTRNG, "HV1440", "OUTRNG", "HV1440 out of range"},
      {HV1440$_STUCK, "HV1440", "STUCK", "HV1440 not responding with Q"},
      {JOERGER$_BAD_PRE_TRIGGER, "JOERGER", "BAD_PRE_TRIGGER",
       "bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7"},
      {JOERGER$_BAD_ACT_MEMORY, "JOERGER", "BAD_ACT_MEMORY",
       "bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8"},
      {JOERGER$_BAD_GAIN, "JOERGER", "BAD_GAIN",
       "bad gain specified, specify a value of 1,2,4 or 8"},
      {U_OF_M$_BAD_WAVE_LENGTH, "U_OF_M", "BAD_WAVE_LENGTH",
       "bad wave length specified, specify value between 0 and 13000"},
      {U_OF_M$_BAD_SLIT_WIDTH, "U_OF_M", "BAD_SLIT_WIDTH",
       "bad slit width specified, specify value between 0 and 500"},
      {U_OF_M$_BAD_NUM_SPECTRA, "U_OF_M", "BAD_NUM_SPECTRA",
       "bad number of spectra specified, specify value between 1 and 100"},
      {U_OF_M$_BAD_GRATING, "U_OF_M", "BAD_GRATING",
       "bad grating type specified, specify value between 1 and 5"},
      {U_OF_M$_BAD_EXPOSURE, "U_OF_M", "BAD_EXPOSURE",
       "bad exposure time specified, specify value between 30 and 3000"},
      {U_OF_M$_BAD_FILTER, "U_OF_M", "BAD_FILTER",
       "bad neutral density filter specified, specify value between 0 and 5"},
      {U_OF_M$_GO_FILE_ERROR, "U_OF_M", "GO_FILE_ERROR",
       "error creating new go file"},
      {U_OF_M$_DATA_FILE_ERROR, "U_OF_M", "DATA_FILE_ERROR",
       "error opening datafile"},
      {IDL$_NORMAL, "IDL", "NORMAL", "successful completion"},
      {IDL$_ERROR, "IDL", "ERROR", "IDL returned a non zero error code"},
      {B5910A$_BAD_CHAN, "B5910A", "BAD_CHAN",
       "error evaluating data for channel !SL"},
      {B5910A$_BAD_CLOCK, "B5910A", "BAD_CLOCK",
       "invalid internal clock range specified"},
      {B5910A$_BAD_ITERATIONS, "B5910A", "BAD_ITERATIONS",
       "invalid number of iterations specified"},
      {B5910A$_BAD_NOC, "B5910A", "BAD_NOC",
       "invalid number of active channels specified"},
      {B5910A$_BAD_SAMPS, "B5910A", "BAD_SAMPS",
       "number of samples specificed invalid"},
      {J412$_NOT_SORTED, "J412", "NOT_SORTED",
       "times specified for J412 module were not sorted"},
      {J412$_NO_DATA, "J412", "NO_DATA",
       "there were no times specifed for J412 module"},
      {J412$_BADCYCLES, "J412", "BADCYCLES",
       "The number of cycles must be 1 .. 255"},
      {TR16$_NORMAL, "TR16", "NORMAL", "successful completion"},
      {TR16$_BAD_MEMSIZE, "TR16", "BAD_MEMSIZE",
       "Memory size must be in 128K, 256K, 512k, 1024K"},
      {TR16$_BAD_ACTIVEMEM, "TR16", "BAD_ACTIVEMEM",
       "Active Mem must be power of 2 8K to 1024K"},
      {TR16$_BAD_ACTIVECHAN, "TR16", "BAD_ACTIVECHAN",
       "Active channels must be in 1,2,4,8,16"},
      {TR16$_BAD_PTS, "TR16", "BAD_PTS", "PTS must be power of 2 32 to 1024K"},
      {TR16$_BAD_FREQUENCY, "TR16", "BAD_FREQUENCY", "Invalid clock frequency"},
      {TR16$_BAD_MASTER, "TR16", "BAD_MASTER", "Master must be 0 or 1"},
      {TR16$_BAD_GAIN, "TR16", "BAD_GAIN", "Gain must be 1, 2, 4, or 8"},
      {A14$_NORMAL, "A14", "NORMAL", "successful completion"},
      {A14$_BAD_CLK_DIVIDE, "A14", "BAD_CLK_DIVIDE",
       "Clock divide must be one of 1,2,4,10,20,40, or 100"},
      {A14$_BAD_MODE, "A14", "BAD_MODE", "Mode must be in the range of 0 to 4"},
      {A14$_BAD_CLK_POLARITY, "A14", "BAD_CLK_POLARITY",
       "Clock polarity must be either 0 (rising) or 1 (falling)"},
      {A14$_BAD_STR_POLARITY, "A14", "BAD_STR_POLARITY",
       "Start polarity must be either 0 (rising) or 1 (falling)"},
      {A14$_BAD_STP_POLARITY, "A14", "BAD_STP_POLARITY",
       "Stop polarity must be either 0 (rising) or 1 (falling)"},
      {A14$_BAD_GATED, "A14", "BAD_GATED",
       "Gated clock must be either 0 (not gated) or 1 (gated)"},
      {L6810$_NORMAL, "L6810", "NORMAL", "successful completion"},
      {L6810$_BAD_ACTIVECHAN, "L6810", "BAD_ACTIVECHAN",
       "Active chans must be 1, 2, or 4"},
      {L6810$_BAD_ACTIVEMEM, "L6810", "BAD_ACTIVEMEM",
       "Active memory must be power of 2 LE 8192"},
      {L6810$_BAD_FREQUENCY, "L6810", "BAD_FREQUENCY",
       "Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, "
       "100,  200, 500, 1000, 2000, 5000]"},
      {L6810$_BAD_FULL_SCALE, "L6810", "BAD_FULL_SCALE",
       "Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, "
       "102.4]"},
      {L6810$_BAD_MEMORIES, "L6810", "BAD_MEMORIES", "Memories must 1 .. 16"},
      {L6810$_BAD_COUPLING, "L6810", "BAD_COUPLING",
       "Channel source / coupling must be one of 0 .. 7"},
      {L6810$_BAD_OFFSET, "L6810", "BAD_OFFSET",
       "Offset must be between 0 and 255"},
      {L6810$_BAD_SEGMENTS, "L6810", "BAD_SEGMENTS",
       "Number of segments must be 1 .. 1024"},
      {L6810$_BAD_TRIG_DELAY, "L6810", "BAD_TRIG_DELAY",
       "Trigger delay must be between -8 and 247 in units of 8ths of segment "
       "size"},
      {J_DAC$_OUTRNG, "J_DAC", "OUTRNG",
       "Joerger DAC Channels out of range.  Bad chans code !XW"},
      {INCAA$_BAD_ACTIVE_CHANS, "INCAA", "BAD_ACTIVE_CHANS",
       "bad active channels selection"},
      {INCAA$_BAD_MASTER, "INCAA", "BAD_MASTER",
       "bad master selection, must be 0 or 1"},
      {INCAA$_BAD_EXT_1MHZ, "INCAA", "BAD_EXT_1MHZ",
       "bad ext 1mhz selection, must be 0 or 1"},
      {INCAA$_BAD_PTSC, "INCAA", "BAD_PTSC", "bad PTSC setting"},
      {L8212$_BAD_HEADER, "L8212", "BAD_HEADER",
       "Invalid header jumper information (e.g. 49414944432)"},
      {L8212$_BAD_MEMORIES, "L8212", "BAD_MEMORIES",
       "Invalid number of memories, must be 1 .. 16"},
      {L8212$_BAD_NOC, "L8212", "BAD_NOC", "Invalid number of active channels"},
      {L8212$_BAD_OFFSET, "L8212", "BAD_OFFSET",
       "Invalid offset must be one of (0, -2048, -4096)"},
      {L8212$_BAD_PTS, "L8212", "BAD_PTS", "Invalid pts code, must be 0 .. 7"},
      {L8212$_FREQ_TOO_HIGH, "L8212", "FREQ_TOO_HIGH",
       "Frequency to high for selected number of channels"},
      {L8212$_INVALID_NOC, "L8212", "INVALID_NOC",
       "Invalid number of active channels"},
      {MPB$_BADTIME, "MPB", "BADTIME", "Could not read time"},
      {MPB$_BADFREQ, "MPB", "BADFREQ", "Could not read frequency"},
      {L8828$_BAD_OFFSET, "L8828", "BAD_OFFSET",
       "Offset for L8828 must be between 0 and 255"},
      {L8828$_BAD_PRETRIG, "L8828", "BAD_PRETRIG",
       "Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"},
      {L8828$_BAD_ACTIVEMEM, "L8828", "BAD_ACTIVEMEM",
       "ACTIVEMEM must be beteen 16K and 2M"},
      {L8828$_BAD_CLOCK, "L8828", "BAD_CLOCK",
       "Invalid clock frequency specified."},
      {L8818$_BAD_OFFSET, "L8818", "BAD_OFFSET",
       "Offset for L8828 must be between 0 and 255"},
      {L8818$_BAD_PRETRIG, "L8818", "BAD_PRETRIG",
       "Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"},
      {L8818$_BAD_ACTIVEMEM, "L8818", "BAD_ACTIVEMEM",
       "ACTIVEMEM must be beteen 16K and 2M"},
      {L8818$_BAD_CLOCK, "L8818", "BAD_CLOCK",
       "Invalid clock frequency specified."},
      {J_TR612$_BAD_ACTMEM, "J_TR612", "BAD_ACTMEM",
       "ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all"},
      {J_TR612$_BAD_PRETRIG, "J_TR612", "BAD_PRETRIG",
       "PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 "
       "pretrigger samples"},
      {J_TR612$_BAD_MODE, "J_TR612", "BAD_MODE",
       "MODE value out of range, must be 0 (for normal) or 1 (for burst mode)"},
      {J_TR612$_BAD_FREQUENCY, "J_TR612", "BAD_FREQUENCY",
       "FREQUENCY value out of range, must be 0-4 where "
       "0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external"},
      {L8206$_NODATA, "L8206", "NODATA", "no data has been written to memory"},
      {H912$_BAD_CLOCK, "H912", "BAD_CLOCK",
       "Bad value specified in INT_CLOCK node, use Setup device to correct"},
      {H912$_BAD_BLOCKS, "H912", "BAD_BLOCKS",
       "Bad value specified in BLOCKS node, use Setup device to correct"},
      {H912$_BAD_PTS, "H912", "BAD_PTS",
       "Bad value specfiied in PTS node, must be an integer value between 1 "
       "and 131071"},
      {H908$_BAD_CLOCK, "H908", "BAD_CLOCK",
       "Bad value specified in INT_CLOCK node, use Setup device to correct"},
      {H908$_BAD_ACTIVE_CHANS, "H908", "BAD_ACTIVE_CHANS",
       "Bad value specified in ACTIVE_CHANS node, use Setup device to correct"},
      {H908$_BAD_PTS, "H908", "BAD_PTS",
       "Bad value specfiied in PTS node, must be an integer value between 1 "
       "and 131071"},
      {DSP2904$_CHANNEL_READ_ERROR, "DSP2904", "CHANNEL_READ_ERROR",
       "Error reading channel"},
      {PY$_UNHANDLED_EXCEPTION, "PY", "UNHANDLED_EXCEPTION",
       "Python device raised and exception, see log files for more details"},
      {DT196B$_NO_SAMPLES, "DT196B", "NO_SAMPLES",
       "Module did not acquire any samples"},
      {ACQ$_INITIALIZATION_ERROR, "ACQ", "INITIALIZATION_ERROR",
       "Error during module initialization"},
      {ACQ$_WRONG_TREE, "ACQ", "WRONG_TREE",
       "Attempt to store ACQ module into different tree than it was armed "
       "with"},
      {ACQ$_WRONG_PATH, "ACQ", "WRONG_PATH",
       "Attempt to store ACQ module into different path than it was armed "
       "with"},
      {ACQ$_WRONG_SHOT, "ACQ", "WRONG_SHOT",
       "Attempt to store ACQ module into different shot than it was armed "
       "with"}};
  size_t i;
  int status = 0;
  for (i = 0; i < sizeof(msgs) / sizeof(struct msg); i++)
  {
    if (msgs[i].sts == sts)
    {
      *facnam = msgs[i].facnam;
      *msgnam = msgs[i].msgnam;
      *msgtext = msgs[i].msgtext;
      status = 1;
      break;
    }
  }
  return status;
}

#endif
