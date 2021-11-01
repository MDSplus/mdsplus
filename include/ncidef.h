#ifndef NCIDEF_H
#define NCIDEF_H
/**************************************
  NCIDEF.H - definitions of constants
  used in  item list arguments to the
  routines:

    TreeGetNci
    TreeSetNci
**************************************/
#include <inttypes.h>
#ifdef MDSOBJECTSCPPSHRVS_EXPORTS
// visual studio uses int types for typedef
#define TYPEDEF(bytes) enum
#define ENDDEF(type, name) \
  ;                        \
  typedef type name
#else
#define TYPEDEF(bytes) typedef enum __attribute__((__packed__))
#define ENDDEF(type, name) name
#endif

TYPEDEF(4){
    NciM_STATE = 0x00000001,
    NciM_PARENT_STATE = 0x00000002,
    NciM_ESSENTIAL = 0x00000004,
    NciM_CACHED = 0x00000008,
    NciM_VERSIONS = 0x00000010,
    NciM_SEGMENTED = 0x00000020,
    NciM_SETUP_INFORMATION = 0x00000040,
    NciM_WRITE_ONCE = 0x00000080,
    NciM_COMPRESSIBLE = 0x00000100,
    NciM_DO_NOT_COMPRESS = 0x00000200,
    NciM_COMPRESS_ON_PUT = 0x00000400,
    NciM_NO_WRITE_MODEL = 0x00000800,
    NciM_NO_WRITE_SHOT = 0x00001000,
    NciM_PATH_REFERENCE = 0x00002000,
    NciM_NID_REFERENCE = 0x00004000,
    NciM_INCLUDE_IN_PULSE = 0x00008000,
    NciM_COMPRESS_SEGMENTS = 0x00010000,
 } ENDDEF(uint32_t, ncim_t);
TYPEDEF(4){
    NciK_IS_CHILD = 1,
    NciK_IS_MEMBER = 2,
} ENDDEF(uint32_t, ncik_t);
TYPEDEF(4){
    NciEND_OF_LIST = 0,
    NciSET_FLAGS = 1,
    NciCLEAR_FLAGS = 2,
    NciTIME_INSERTED = 4,
    NciOWNER_ID = 5,
    NciCLASS = 6,
    NciDTYPE = 7,
    NciLENGTH = 8,
    NciSTATUS = 9,
    NciCONGLOMERATE_ELT = 10,
    NciGET_FLAGS = 12,
    NciNODE_NAME = 13,
    NciPATH = 14,
    NciDEPTH = 15,
    NciPARENT = 16,
    NciBROTHER = 17,
    NciMEMBER = 18,
    NciCHILD = 19,
    NciPARENT_RELATIONSHIP = 20,
    NciCONGLOMERATE_NIDS = 21,
    NciORIGINAL_PART_NAME = 22,
    NciNUMBER_OF_MEMBERS = 23,
    NciNUMBER_OF_CHILDREN = 24,
    NciMEMBER_NIDS = 25,
    NciCHILDREN_NIDS = 26,
    NciFULLPATH = 27,
    NciMINPATH = 28,
    NciUSAGE = 29,
    NciPARENT_TREE = 30,
    NciRLENGTH = 31,
    NciNUMBER_OF_ELTS = 32,
    NciDATA_IN_NCI = 33,
    NciERROR_ON_PUT = 34,
    NciRFA = 35,
    NciIO_STATUS = 36,
    NciIO_STV = 37,
    NciDTYPE_STR = 38,
    NciUSAGE_STR = 39,
    NciCLASS_STR = 40,
    NciVERSION = 41,
    NciCOMPRESSION_METHOD = 42,
    NciCOMPRESSION_METHOD_STR = 43,
} ENDDEF(int16_t, nci_t);
#undef TYPEDEF
#undef ENDDEF
typedef struct nci_itm
{
  int16_t buffer_length;
  nci_t code;
  void *pointer;
  int *return_length_address;
} NCI_ITM;
#endif
