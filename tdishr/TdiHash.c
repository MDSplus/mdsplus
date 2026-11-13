/* C code produced by gperf version 3.0.4 */
/* Command-line: gperf tdishr/TdiHash.c.in  */
/* Computed positions: -k'1-2,4-5,7,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


#include "tdirefcat.h"
#include "tdireffunction.h"
#ifdef _WIN32
// Windows uses long to cast position and cause a compiler warning
#define long size_t
#endif
struct fun { int name; int idx; };
#include <string.h>

#define TOTAL_KEYWORDS 461
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 21
#define MIN_HASH_VALUE 12
#define MAX_HASH_VALUE 1934
/* maximum key range = 1923, duplicates = 0 */

#ifndef GPERF_DOWNCASE
#define GPERF_DOWNCASE 1
static unsigned char gperf_downcase[256] =
  {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
     45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255
  };
#endif

#ifndef GPERF_CASE_STRNCMP
#define GPERF_CASE_STRNCMP 1
static int
gperf_case_strncmp (s1, s2, n)
     register const char *s1;
     register const char *s2;
     register unsigned int n;
{
  for (; n > 0;)
    {
      unsigned char c1 = gperf_downcase[(unsigned char)*s1++];
      unsigned char c2 = gperf_downcase[(unsigned char)*s2++];
      if (c1 != 0 && c1 == c2)
        {
          n--;
          continue;
        }
      return (int)c1 - (int)c2;
    }
  return 0;
}
#endif

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned short asso_values[] =
    {
      1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935,
      1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935,
      1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935,
      1935, 1935, 1935, 1935, 1935, 1935,    0, 1935,  483, 1935,
      1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935,   60,    0,
        55, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935, 1935,
      1935, 1935, 1935, 1935, 1935,   90,   75,  100,    5,   15,
       160,  390,  490,   80,   75,  200,   30,  194,    0,   35,
       471,  627,   70,  225,    5,   20,  722,  522,  501,  511,
        15, 1935,   30, 1935, 1935,  150, 1935,   90,   75,  100,
         5,   15,  160,  390,  490,   80,   75,  200,   30,  194,
         0,   35,  471,  627,   70,  225,    5,   20,  722,  522,
       501,  511,   15, 1935,   30, 1935, 1935, 1935, 1935, 1935
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]+2];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

struct stringpool_t
  {
    char stringpool_str12[sizeof("LT")];
    char stringpool_str18[sizeof("LEN")];
    char stringpool_str27[sizeof("XD")];
    char stringpool_str32[sizeof("LE")];
    char stringpool_str34[sizeof("READ")];
    char stringpool_str38[sizeof("LLT")];
    char stringpool_str48[sizeof("LLE")];
    char stringpool_str54[sizeof("MEAN")];
    char stringpool_str64[sizeof("COSD")];
    char stringpool_str65[sizeof("COUNT")];
    char stringpool_str69[sizeof("SORT")];
    char stringpool_str74[sizeof("STATEMENT")];
    char stringpool_str78[sizeof("MOD")];
    char stringpool_str84[sizeof("REAL")];
    char stringpool_str89[sizeof("BUILTIN_OPCODE")];
    char stringpool_str91[sizeof("BUILD_EVENT")];
    char stringpool_str92[sizeof("GT")];
    char stringpool_str94[sizeof("LIST")];
    char stringpool_str99[sizeof("CULL")];
    char stringpool_str103[sizeof("SIN")];
    char stringpool_str104[sizeof("SIGN")];
    char stringpool_str105[sizeof("REBIN")];
    char stringpool_str106[sizeof("LBOUND")];
    char stringpool_str108[sizeof("AND")];
    char stringpool_str109[sizeof("ATAN")];
    char stringpool_str111[sizeof("REWIND")];
    char stringpool_str112[sizeof("GE")];
    char stringpool_str113[sizeof("ADD")];
    char stringpool_str114[sizeof("SIND")];
    char stringpool_str115[sizeof("ANINT")];
    char stringpool_str116[sizeof("RETURN")];
    char stringpool_str118[sizeof("MIN")];
    char stringpool_str119[sizeof("ZERO")];
    char stringpool_str120[sizeof("ATAND")];
    char stringpool_str122[sizeof("BUILD_OPAQUE")];
    char stringpool_str124[sizeof("SCAN")];
    char stringpool_str126[sizeof("SIGNED")];
    char stringpool_str129[sizeof("HUGE")];
    char stringpool_str134[sizeof("SIZE")];
    char stringpool_str136[sizeof("REPEAT")];
    char stringpool_str139[sizeof("CASE")];
    char stringpool_str149[sizeof("LOG2")];
    char stringpool_str156[sizeof("BUILD_RANGE")];
    char stringpool_str158[sizeof("BUILD_ROUTINE")];
    char stringpool_str160[sizeof("LOG10")];
    char stringpool_str161[sizeof("MODULO")];
    char stringpool_str162[sizeof("BUILD_ACTION")];
    char stringpool_str163[sizeof("ALL")];
    char stringpool_str170[sizeof("LABEL")];
    char stringpool_str171[sizeof("ATAN2D")];
    char stringpool_str175[sizeof("BUILD_CONDITION")];
    char stringpool_str178[sizeof("CONTINUE")];
    char stringpool_str179[sizeof("ABS1")];
    char stringpool_str182[sizeof("PRODUCT")];
    char stringpool_str183[sizeof("REF")];
    char stringpool_str184[sizeof("ARGD")];
    char stringpool_str185[sizeof("SCALE")];
    char stringpool_str187[sizeof("SELECTED_INT_KIND")];
    char stringpool_str188[sizeof("SELECTED_REAL_KIND")];
    char stringpool_str189[sizeof("GOTO")];
    char stringpool_str194[sizeof("AINT")];
    char stringpool_str198[sizeof("LAMINATE")];
    char stringpool_str200[sizeof("BUILD_CALL")];
    char stringpool_str202[sizeof("IN")];
    char stringpool_str208[sizeof("INT")];
    char stringpool_str214[sizeof("INOT")];
    char stringpool_str215[sizeof("INAND")];
    char stringpool_str217[sizeof("REM")];
    char stringpool_str219[sizeof("INAND_NOT")];
    char stringpool_str220[sizeof("ATAN2")];
    char stringpool_str226[sizeof("MEDIAN")];
    char stringpool_str228[sizeof("SUBTRACT")];
    char stringpool_str232[sizeof("DO")];
    char stringpool_str234[sizeof("BUILD_FUNCTION")];
    char stringpool_str235[sizeof("INOUT")];
    char stringpool_str236[sizeof("DECODE")];
    char stringpool_str237[sizeof("SUM")];
    char stringpool_str244[sizeof("REPLICATE")];
    char stringpool_str247[sizeof("PROJECT")];
    char stringpool_str250[sizeof("BTEST")];
    char stringpool_str251[sizeof("CONCAT")];
    char stringpool_str254[sizeof("DICT")];
    char stringpool_str256[sizeof("CONDITIONAL")];
    char stringpool_str260[sizeof("ABORT")];
    char stringpool_str261[sizeof("GETDBI")];
    char stringpool_str265[sizeof("POWER")];
    char stringpool_str267[sizeof("AND_NOT")];
    char stringpool_str268[sizeof("BUILD_DIM")];
    char stringpool_str269[sizeof("DBLE")];
    char stringpool_str271[sizeof("BUILD_METHOD")];
    char stringpool_str278[sizeof("COS")];
    char stringpool_str279[sizeof("SET_RANGE")];
    char stringpool_str281[sizeof("GETNCI")];
    char stringpool_str282[sizeof("LOGICAL")];
    char stringpool_str283[sizeof("STRING_OPCODE")];
    char stringpool_str284[sizeof("ALLOCATED")];
    char stringpool_str286[sizeof("MINLOC")];
    char stringpool_str288[sizeof("KIND")];
    char stringpool_str296[sizeof("MAXLOC")];
    char stringpool_str301[sizeof("RANDOM_SEED")];
    char stringpool_str302[sizeof("DEFAULT")];
    char stringpool_str303[sizeof("BEGIN_OF")];
    char stringpool_str304[sizeof("IAND")];
    char stringpool_str305[sizeof("IBSET")];
    char stringpool_str306[sizeof("PUBLIC")];
    char stringpool_str307[sizeof("H_FLOAT")];
    char stringpool_str308[sizeof("IOR")];
    char stringpool_str310[sizeof("ROUTINE_OF")];
    char stringpool_str311[sizeof("BUILD_SLOPE")];
    char stringpool_str312[sizeof("G_FLOAT")];
    char stringpool_str314[sizeof("INTERSECT")];
    char stringpool_str316[sizeof("SIZEOF")];
    char stringpool_str318[sizeof("MODEL_OF")];
    char stringpool_str321[sizeof("MAKE_METHOD")];
    char stringpool_str322[sizeof("INVERSE")];
    char stringpool_str323[sizeof("MAKE_FUNCTION")];
    char stringpool_str327[sizeof("BUILD_SIGNAL")];
    char stringpool_str329[sizeof("ASIN")];
    char stringpool_str332[sizeof("LASTLOC")];
    char stringpool_str335[sizeof("RANDOM")];
    char stringpool_str337[sizeof("INQUIRE")];
    char stringpool_str339[sizeof("MAKE_CONDITION")];
    char stringpool_str340[sizeof("ASIND")];
    char stringpool_str344[sizeof("INOR")];
    char stringpool_str345[sizeof("MAKE_SLOPE")];
    char stringpool_str346[sizeof("DIVIDE")];
    char stringpool_str352[sizeof("MAKE_ROUTINE")];
    char stringpool_str358[sizeof("INTERPOL")];
    char stringpool_str359[sizeof("IEOR")];
    char stringpool_str360[sizeof("DEALLOCATE")];
    char stringpool_str362[sizeof("MAT_ROT")];
    char stringpool_str366[sizeof("MAT_ROT_INT")];
    char stringpool_str367[sizeof("BUILD_CONGLOM")];
    char stringpool_str368[sizeof("POST_INC")];
    char stringpool_str375[sizeof("MATMUL")];
    char stringpool_str383[sizeof("POST_DEC")];
    char stringpool_str387[sizeof("CONDITION_OF")];
    char stringpool_str389[sizeof("MAKE_PROCEDURE")];
    char stringpool_str392[sizeof("D_FLOAT")];
    char stringpool_str397[sizeof("PRESENT")];
    char stringpool_str398[sizeof("LGT")];
    char stringpool_str401[sizeof("MAKE_ACTION")];
    char stringpool_str402[sizeof("IOR_NOT")];
    char stringpool_str403[sizeof("ABS")];
    char stringpool_str405[sizeof("MAKE_RANGE")];
    char stringpool_str406[sizeof("PROMOTE")];
    char stringpool_str408[sizeof("LGE")];
    char stringpool_str409[sizeof("PRECISION")];
    char stringpool_str411[sizeof("MAKE_SIGNAL")];
    char stringpool_str413[sizeof("IDENT_OF")];
    char stringpool_str417[sizeof("ADJUSTL")];
    char stringpool_str419[sizeof("MAKE_CALL")];
    char stringpool_str420[sizeof("DESCR")];
    char stringpool_str427[sizeof("RMS")];
    char stringpool_str428[sizeof("LOG")];
    char stringpool_str429[sizeof("COMMA")];
    char stringpool_str434[sizeof("DATA")];
    char stringpool_str435[sizeof("ACHAR")];
    char stringpool_str436[sizeof("X_TO_I")];
    char stringpool_str437[sizeof("DIM")];
    char stringpool_str440[sizeof("ACOSD")];
    char stringpool_str443[sizeof("DATE_AND_TIME")];
    char stringpool_str446[sizeof("RAW_OF")];
    char stringpool_str447[sizeof("PROCEDURE_OF")];
    char stringpool_str449[sizeof("DSQL")];
    char stringpool_str450[sizeof("IBCLR")];
    char stringpool_str451[sizeof("ENCODE")];
    char stringpool_str452[sizeof("LEN_TRIM")];
    char stringpool_str454[sizeof("ELSE")];
    char stringpool_str455[sizeof("MAKE_WITH_ERROR")];
    char stringpool_str459[sizeof("SUBSCRIPT")];
    char stringpool_str468[sizeof("INOR_NOT")];
    char stringpool_str469[sizeof("ACCUMULATE")];
    char stringpool_str475[sizeof("MERGE")];
    char stringpool_str477[sizeof("INTERRUPT_OF")];
    char stringpool_str482[sizeof("EUBOUND")];
    char stringpool_str483[sizeof("IEOR_NOT")];
    char stringpool_str484[sizeof("$")];
    char stringpool_str489[sizeof("ISQL")];
    char stringpool_str491[sizeof("CSHIFT")];
    char stringpool_str492[sizeof("ELBOUND")];
    char stringpool_str493[sizeof("IAND_NOT")];
    char stringpool_str497[sizeof("ADJUSTR")];
    char stringpool_str498[sizeof("BIT_SIZE")];
    char stringpool_str499[sizeof("RANK")];
    char stringpool_str501[sizeof("STRING")];
    char stringpool_str502[sizeof("PRE_DEC")];
    char stringpool_str503[sizeof("NE")];
    char stringpool_str508[sizeof("DECOMPILE")];
    char stringpool_str510[sizeof("IS_IN")];
    char stringpool_str513[sizeof("FUN")];
    char stringpool_str514[sizeof("NOT")];
    char stringpool_str519[sizeof("DATE_TIME")];
    char stringpool_str520[sizeof("RANGE")];
    char stringpool_str521[sizeof("ARG_OF")];
    char stringpool_str522[sizeof("IF")];
    char stringpool_str531[sizeof("MAKE_CONGLOM")];
    char stringpool_str535[sizeof("ICHAR")];
    char stringpool_str537[sizeof("FLATTEN")];
    char stringpool_str539[sizeof("RESET_PRIVATE")];
    char stringpool_str543[sizeof("$TRUE")];
    char stringpool_str545[sizeof("FOPEN")];
    char stringpool_str546[sizeof("$N0")];
    char stringpool_str550[sizeof("BYTE")];
    char stringpool_str551[sizeof("$T0")];
    char stringpool_str556[sizeof("DIGITS")];
    char stringpool_str558[sizeof("IF_ERROR")];
    char stringpool_str560[sizeof("WORD")];
    char stringpool_str561[sizeof("BUILD_PROCEDURE")];
    char stringpool_str562[sizeof("UNION")];
    char stringpool_str563[sizeof("ARG")];
    char stringpool_str564[sizeof("PACK")];
    char stringpool_str565[sizeof("NINT")];
    char stringpool_str566[sizeof("MAKE_PROGRAM")];
    char stringpool_str569[sizeof("BUILD_DISPATCH")];
    char stringpool_str570[sizeof("BREAK")];
    char stringpool_str571[sizeof("$RE")];
    char stringpool_str572[sizeof("MAKE_DIM")];
    char stringpool_str575[sizeof("NAND")];
    char stringpool_str576[sizeof("$NA")];
    char stringpool_str577[sizeof("PRE_INC")];
    char stringpool_str578[sizeof("FIT")];
    char stringpool_str579[sizeof("NOR")];
    char stringpool_str584[sizeof("MAKE_PARAM")];
    char stringpool_str588[sizeof("NEAREST")];
    char stringpool_str590[sizeof("FTELL")];
    char stringpool_str591[sizeof("DIM_OF")];
    char stringpool_str592[sizeof("BUILD_DEPENDENCY")];
    char stringpool_str593[sizeof("LONG_UNSIGNED")];
    char stringpool_str598[sizeof("FOR")];
    char stringpool_str599[sizeof("MAP")];
    char stringpool_str600[sizeof("ISHFT")];
    char stringpool_str607[sizeof("SPREAD")];
    char stringpool_str608[sizeof("QUALIFIERS_OF")];
    char stringpool_str610[sizeof("MAKE_WITH_UNITS")];
    char stringpool_str612[sizeof("INT_UNSIGNED")];
    char stringpool_str614[sizeof("ANY")];
    char stringpool_str615[sizeof("WAIT")];
    char stringpool_str617[sizeof("$AMU")];
    char stringpool_str621[sizeof("WRITE")];
    char stringpool_str623[sizeof("RESET_PUBLIC")];
    char stringpool_str624[sizeof("VAL")];
    char stringpool_str625[sizeof("FLOAT")];
    char stringpool_str628[sizeof("UBOUND")];
    char stringpool_str629[sizeof("MAX")];
    char stringpool_str632[sizeof("VECTOR")];
    char stringpool_str633[sizeof("ISQL_SET")];
    char stringpool_str634[sizeof("BACKSPACE")];
    char stringpool_str636[sizeof("$A0")];
    char stringpool_str637[sizeof("RESHAPE")];
    char stringpool_str638[sizeof("COMPILE")];
    char stringpool_str645[sizeof("$I")];
    char stringpool_str646[sizeof("ELEMENT")];
    char stringpool_str647[sizeof("$CAL")];
    char stringpool_str648[sizeof("PERFORMANCE_OF")];
    char stringpool_str649[sizeof("CHAR")];
    char stringpool_str654[sizeof("ACOS")];
    char stringpool_str655[sizeof("OUT")];
    char stringpool_str658[sizeof("FFT")];
    char stringpool_str661[sizeof("SQRT")];
    char stringpool_str664[sizeof("VAR")];
    char stringpool_str665[sizeof("ESIZE")];
    char stringpool_str667[sizeof("ENDFILE")];
    char stringpool_str668[sizeof("BUILD_WITH_ERROR")];
    char stringpool_str673[sizeof("NOR_NOT")];
    char stringpool_str676[sizeof("FINITE")];
    char stringpool_str681[sizeof("DPROD")];
    char stringpool_str682[sizeof("MINEXPONENT")];
    char stringpool_str685[sizeof("$C")];
    char stringpool_str688[sizeof("SET_EXPONENT")];
    char stringpool_str689[sizeof("MULTIPLY")];
    char stringpool_str691[sizeof("VALIDATION")];
    char stringpool_str692[sizeof("MAXEXPONENT")];
    char stringpool_str694[sizeof("RC_DROOP")];
    char stringpool_str695[sizeof("$ME")];
    char stringpool_str696[sizeof("ISHFTC")];
    char stringpool_str697[sizeof("DO_TASK")];
    char stringpool_str698[sizeof("CLASS_OF")];
    char stringpool_str699[sizeof("BYTE_UNSIGNED")];
    char stringpool_str700[sizeof("FLOOR")];
    char stringpool_str702[sizeof("$2PI")];
    char stringpool_str703[sizeof("$TORR")];
    char stringpool_str708[sizeof("FRACTION")];
    char stringpool_str709[sizeof("DECOMPRESS")];
    char stringpool_str714[sizeof("SQUEEZE")];
    char stringpool_str719[sizeof("WORD_UNSIGNED")];
    char stringpool_str720[sizeof("SHIFT_LEFT")];
    char stringpool_str722[sizeof("F_FLOAT")];
    char stringpool_str725[sizeof("CLASS")];
    char stringpool_str729[sizeof("COMPLETION_OF")];
    char stringpool_str733[sizeof("INTEGRAL")];
    char stringpool_str735[sizeof("IBITS")];
    char stringpool_str736[sizeof("BUILD_PARAM")];
    char stringpool_str737[sizeof("COMPLETION_MESSAGE_OF")];
    char stringpool_str738[sizeof("BUILD_PROGRAM")];
    char stringpool_str739[sizeof("SLOPE_OF")];
    char stringpool_str741[sizeof("END_OF")];
    char stringpool_str743[sizeof("OR_NOT")];
    char stringpool_str745[sizeof("CVT")];
    char stringpool_str747[sizeof("DTYPE_RANGE")];
    char stringpool_str750[sizeof("SMOOTH")];
    char stringpool_str751[sizeof("TEXT")];
    char stringpool_str756[sizeof("KIND_OF")];
    char stringpool_str758[sizeof("$SHOT")];
    char stringpool_str761[sizeof("SHIFT_RIGHT")];
    char stringpool_str764[sizeof("NAND_NOT")];
    char stringpool_str768[sizeof("ERROR_OF")];
    char stringpool_str769[sizeof("OR")];
    char stringpool_str772[sizeof("ERRORLOGS_OF")];
    char stringpool_str781[sizeof("$DEFAULT")];
    char stringpool_str787[sizeof("MAKE_OPAQUE")];
    char stringpool_str788[sizeof("FT_FLOAT")];
    char stringpool_str793[sizeof("DIAGONAL")];
    char stringpool_str801[sizeof("$MU0")];
    char stringpool_str807[sizeof("TUPLE")];
    char stringpool_str810[sizeof("DATA_WITH_UNITS")];
    char stringpool_str812[sizeof("SOLVE")];
    char stringpool_str815[sizeof("TAN")];
    char stringpool_str819[sizeof("LONG")];
    char stringpool_str823[sizeof("BUILD_WITH_UNITS")];
    char stringpool_str825[sizeof("ON_ERROR")];
    char stringpool_str826[sizeof("TAND")];
    char stringpool_str828[sizeof("SQUARE")];
    char stringpool_str829[sizeof("VALUE_OF")];
    char stringpool_str831[sizeof("MAKE_DEPENDENCY")];
    char stringpool_str849[sizeof("SORTVAL")];
    char stringpool_str854[sizeof("VALIDATION_OF")];
    char stringpool_str855[sizeof("QUADWORD")];
    char stringpool_str860[sizeof("AS_IS")];
    char stringpool_str862[sizeof("$SHOTNAME")];
    char stringpool_str864[sizeof("QUADWORD_UNSIGNED")];
    char stringpool_str867[sizeof("DOT_PRODUCT")];
    char stringpool_str871[sizeof("FCLOSE")];
    char stringpool_str873[sizeof("MAKE_DISPATCH")];
    char stringpool_str876[sizeof("$GN")];
    char stringpool_str885[sizeof("$K")];
    char stringpool_str894[sizeof("METHOD_OF")];
    char stringpool_str903[sizeof("MAKE_WINDOW")];
    char stringpool_str906[sizeof("NDESC")];
    char stringpool_str907[sizeof("$EPSILON0")];
    char stringpool_str910[sizeof("CONJG")];
    char stringpool_str913[sizeof("SYSTEM_CLOCK")];
    char stringpool_str917[sizeof("EXTEND")];
    char stringpool_str918[sizeof("UNPACK")];
    char stringpool_str919[sizeof("$FALSE")];
    char stringpool_str927[sizeof("CEILING")];
    char stringpool_str937[sizeof("DSCPTR")];
    char stringpool_str939[sizeof("EXPONENT")];
    char stringpool_str943[sizeof("FIRSTLOC")];
    char stringpool_str946[sizeof("IACHAR")];
    char stringpool_str950[sizeof("WINDOW_OF")];
    char stringpool_str955[sizeof("UNITS_OF")];
    char stringpool_str963[sizeof("MINVAL")];
    char stringpool_str964[sizeof("PROGRAM_OF")];
    char stringpool_str965[sizeof("$ATM")];
    char stringpool_str973[sizeof("MAXVAL")];
    char stringpool_str978[sizeof("EPSILON")];
    char stringpool_str980[sizeof("DEBUG")];
    char stringpool_str982[sizeof("UNITS")];
    char stringpool_str984[sizeof("$EXPT")];
    char stringpool_str985[sizeof("$DEGREE")];
    char stringpool_str989[sizeof("PRIVATE")];
    char stringpool_str994[sizeof("H_COMPLEX")];
    char stringpool_str996[sizeof("ELSEWHERE")];
    char stringpool_str999[sizeof("G_COMPLEX")];
    char stringpool_str1002[sizeof("IMAGE_OF")];
    char stringpool_str1003[sizeof("PHASE_OF")];
    char stringpool_str1004[sizeof("NDESC_OF")];
    char stringpool_str1007[sizeof("DERIVATIVE")];
    char stringpool_str1008[sizeof("FS_FLOAT")];
    char stringpool_str1012[sizeof("EOSHIFT")];
    char stringpool_str1015[sizeof("DECOMPILE_DEPENDENCY")];
    char stringpool_str1016[sizeof("SHAPE")];
    char stringpool_str1017[sizeof("$P0")];
    char stringpool_str1018[sizeof("SPAWN")];
    char stringpool_str1020[sizeof("UNSIGNED")];
    char stringpool_str1023[sizeof("$THIS")];
    char stringpool_str1031[sizeof("BUILD_PATH")];
    char stringpool_str1034[sizeof("COSH")];
    char stringpool_str1037[sizeof("$PI")];
    char stringpool_str1038[sizeof("HELP_OF")];
    char stringpool_str1041[sizeof("RAMP")];
    char stringpool_str1042[sizeof("I_TO_X")];
    char stringpool_str1048[sizeof("EXECUTE")];
    char stringpool_str1053[sizeof("NAME_OF")];
    char stringpool_str1055[sizeof("AIMAG")];
    char stringpool_str1061[sizeof("LANGUAGE_OF")];
    char stringpool_str1066[sizeof("WHILE")];
    char stringpool_str1068[sizeof("EXTRACT")];
    char stringpool_str1072[sizeof("DISPATCH_OF")];
    char stringpool_str1079[sizeof("D_COMPLEX")];
    char stringpool_str1084[sizeof("SINH")];
    char stringpool_str1090[sizeof("ATANH")];
    char stringpool_str1095[sizeof("TRANSPOSE_MUL")];
    char stringpool_str1101[sizeof("$RYDBERG")];
    char stringpool_str1106[sizeof("WHERE")];
    char stringpool_str1107[sizeof("$ROPRAND")];
    char stringpool_str1110[sizeof("TRANSFER")];
    char stringpool_str1115[sizeof("RRSPACING")];
    char stringpool_str1116[sizeof("BUILD_WINDOW")];
    char stringpool_str1128[sizeof("$QE")];
    char stringpool_str1131[sizeof("TRANSLATE")];
    char stringpool_str1135[sizeof("FSEEK")];
    char stringpool_str1136[sizeof("OBJECT_OF")];
    char stringpool_str1143[sizeof("SWITCH")];
    char stringpool_str1145[sizeof("COMPILE_DEPENDENCY")];
    char stringpool_str1151[sizeof("$MP")];
    char stringpool_str1158[sizeof("TIME_OUT_OF")];
    char stringpool_str1180[sizeof("DSCPTR_OF")];
    char stringpool_str1182[sizeof("RADIX")];
    char stringpool_str1184[sizeof("TRIM")];
    char stringpool_str1190[sizeof("EVALUATE")];
    char stringpool_str1197[sizeof("ESHAPE")];
    char stringpool_str1208[sizeof("$HBAR")];
    char stringpool_str1212[sizeof("TRANSPOSE_")];
    char stringpool_str1213[sizeof("EXT_FUNCTION")];
    char stringpool_str1222[sizeof("INDEX")];
    char stringpool_str1223[sizeof("$EV")];
    char stringpool_str1246[sizeof("CMPLX")];
    char stringpool_str1265[sizeof("$G")];
    char stringpool_str1273[sizeof("VERIFY")];
    char stringpool_str1276[sizeof("$VALUE")];
    char stringpool_str1277[sizeof("FIX_ROPRAND")];
    char stringpool_str1279[sizeof("SHOW_PRIVATE")];
    char stringpool_str1287[sizeof("ARRAY")];
    char stringpool_str1302[sizeof("OPCODE_BUILTIN")];
    char stringpool_str1303[sizeof("AXIS_OF")];
    char stringpool_str1304[sizeof("EQUALS_FIRST")];
    char stringpool_str1313[sizeof("SHOW_PUBLIC")];
    char stringpool_str1327[sizeof("$GAS")];
    char stringpool_str1329[sizeof("UPCASE")];
    char stringpool_str1338[sizeof("$NARG")];
    char stringpool_str1341[sizeof("OPTIONAL")];
    char stringpool_str1365[sizeof("EXP")];
    char stringpool_str1368[sizeof("EQUALS")];
    char stringpool_str1377[sizeof("BSEARCH")];
    char stringpool_str1409[sizeof("F_COMPLEX")];
    char stringpool_str1412[sizeof("$FARADAY")];
    char stringpool_str1422[sizeof("OCTAWORD")];
    char stringpool_str1431[sizeof("OCTAWORD_UNSIGNED")];
    char stringpool_str1458[sizeof("SPACING")];
    char stringpool_str1465[sizeof("$H")];
    char stringpool_str1478[sizeof("WHEN_OF")];
    char stringpool_str1489[sizeof("TASK_OF")];
    char stringpool_str1525[sizeof("$MISSING")];
    char stringpool_str1532[sizeof("USING")];
    char stringpool_str1533[sizeof("UNARY_MINUS")];
    char stringpool_str1552[sizeof("CONVOLVE")];
    char stringpool_str1577[sizeof("SHOW_VM")];
    char stringpool_str1612[sizeof("FT_COMPLEX")];
    char stringpool_str1630[sizeof("$ALPHA")];
    char stringpool_str1631[sizeof("STD_DEV")];
    char stringpool_str1646[sizeof("EQ")];
    char stringpool_str1659[sizeof("ABSSQ")];
    char stringpool_str1691[sizeof("OPCODE_STRING")];
    char stringpool_str1742[sizeof("EQV")];
    char stringpool_str1796[sizeof("TANH")];
    char stringpool_str1809[sizeof("UNARY_PLUS")];
    char stringpool_str1828[sizeof("TINY")];
    char stringpool_str1832[sizeof("FS_COMPLEX")];
    char stringpool_str1934[sizeof("NEQV")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "LT",
    "LEN",
    "XD",
    "LE",
    "READ",
    "LLT",
    "LLE",
    "MEAN",
    "COSD",
    "COUNT",
    "SORT",
    "STATEMENT",
    "MOD",
    "REAL",
    "BUILTIN_OPCODE",
    "BUILD_EVENT",
    "GT",
    "LIST",
    "CULL",
    "SIN",
    "SIGN",
    "REBIN",
    "LBOUND",
    "AND",
    "ATAN",
    "REWIND",
    "GE",
    "ADD",
    "SIND",
    "ANINT",
    "RETURN",
    "MIN",
    "ZERO",
    "ATAND",
    "BUILD_OPAQUE",
    "SCAN",
    "SIGNED",
    "HUGE",
    "SIZE",
    "REPEAT",
    "CASE",
    "LOG2",
    "BUILD_RANGE",
    "BUILD_ROUTINE",
    "LOG10",
    "MODULO",
    "BUILD_ACTION",
    "ALL",
    "LABEL",
    "ATAN2D",
    "BUILD_CONDITION",
    "CONTINUE",
    "ABS1",
    "PRODUCT",
    "REF",
    "ARGD",
    "SCALE",
    "SELECTED_INT_KIND",
    "SELECTED_REAL_KIND",
    "GOTO",
    "AINT",
    "LAMINATE",
    "BUILD_CALL",
    "IN",
    "INT",
    "INOT",
    "INAND",
    "REM",
    "INAND_NOT",
    "ATAN2",
    "MEDIAN",
    "SUBTRACT",
    "DO",
    "BUILD_FUNCTION",
    "INOUT",
    "DECODE",
    "SUM",
    "REPLICATE",
    "PROJECT",
    "BTEST",
    "CONCAT",
    "DICT",
    "CONDITIONAL",
    "ABORT",
    "GETDBI",
    "POWER",
    "AND_NOT",
    "BUILD_DIM",
    "DBLE",
    "BUILD_METHOD",
    "COS",
    "SET_RANGE",
    "GETNCI",
    "LOGICAL",
    "STRING_OPCODE",
    "ALLOCATED",
    "MINLOC",
    "KIND",
    "MAXLOC",
    "RANDOM_SEED",
    "DEFAULT",
    "BEGIN_OF",
    "IAND",
    "IBSET",
    "PUBLIC",
    "H_FLOAT",
    "IOR",
    "ROUTINE_OF",
    "BUILD_SLOPE",
    "G_FLOAT",
    "INTERSECT",
    "SIZEOF",
    "MODEL_OF",
    "MAKE_METHOD",
    "INVERSE",
    "MAKE_FUNCTION",
    "BUILD_SIGNAL",
    "ASIN",
    "LASTLOC",
    "RANDOM",
    "INQUIRE",
    "MAKE_CONDITION",
    "ASIND",
    "INOR",
    "MAKE_SLOPE",
    "DIVIDE",
    "MAKE_ROUTINE",
    "INTERPOL",
    "IEOR",
    "DEALLOCATE",
    "MAT_ROT",
    "MAT_ROT_INT",
    "BUILD_CONGLOM",
    "POST_INC",
    "MATMUL",
    "POST_DEC",
    "CONDITION_OF",
    "MAKE_PROCEDURE",
    "D_FLOAT",
    "PRESENT",
    "LGT",
    "MAKE_ACTION",
    "IOR_NOT",
    "ABS",
    "MAKE_RANGE",
    "PROMOTE",
    "LGE",
    "PRECISION",
    "MAKE_SIGNAL",
    "IDENT_OF",
    "ADJUSTL",
    "MAKE_CALL",
    "DESCR",
    "RMS",
    "LOG",
    "COMMA",
    "DATA",
    "ACHAR",
    "X_TO_I",
    "DIM",
    "ACOSD",
    "DATE_AND_TIME",
    "RAW_OF",
    "PROCEDURE_OF",
    "DSQL",
    "IBCLR",
    "ENCODE",
    "LEN_TRIM",
    "ELSE",
    "MAKE_WITH_ERROR",
    "SUBSCRIPT",
    "INOR_NOT",
    "ACCUMULATE",
    "MERGE",
    "INTERRUPT_OF",
    "EUBOUND",
    "IEOR_NOT",
    "$",
    "ISQL",
    "CSHIFT",
    "ELBOUND",
    "IAND_NOT",
    "ADJUSTR",
    "BIT_SIZE",
    "RANK",
    "STRING",
    "PRE_DEC",
    "NE",
    "DECOMPILE",
    "IS_IN",
    "FUN",
    "NOT",
    "DATE_TIME",
    "RANGE",
    "ARG_OF",
    "IF",
    "MAKE_CONGLOM",
    "ICHAR",
    "FLATTEN",
    "RESET_PRIVATE",
    "$TRUE",
    "FOPEN",
    "$N0",
    "BYTE",
    "$T0",
    "DIGITS",
    "IF_ERROR",
    "WORD",
    "BUILD_PROCEDURE",
    "UNION",
    "ARG",
    "PACK",
    "NINT",
    "MAKE_PROGRAM",
    "BUILD_DISPATCH",
    "BREAK",
    "$RE",
    "MAKE_DIM",
    "NAND",
    "$NA",
    "PRE_INC",
    "FIT",
    "NOR",
    "MAKE_PARAM",
    "NEAREST",
    "FTELL",
    "DIM_OF",
    "BUILD_DEPENDENCY",
    "LONG_UNSIGNED",
    "FOR",
    "MAP",
    "ISHFT",
    "SPREAD",
    "QUALIFIERS_OF",
    "MAKE_WITH_UNITS",
    "INT_UNSIGNED",
    "ANY",
    "WAIT",
    "$AMU",
    "WRITE",
    "RESET_PUBLIC",
    "VAL",
    "FLOAT",
    "UBOUND",
    "MAX",
    "VECTOR",
    "ISQL_SET",
    "BACKSPACE",
    "$A0",
    "RESHAPE",
    "COMPILE",
    "$I",
    "ELEMENT",
    "$CAL",
    "PERFORMANCE_OF",
    "CHAR",
    "ACOS",
    "OUT",
    "FFT",
    "SQRT",
    "VAR",
    "ESIZE",
    "ENDFILE",
    "BUILD_WITH_ERROR",
    "NOR_NOT",
    "FINITE",
    "DPROD",
    "MINEXPONENT",
    "$C",
    "SET_EXPONENT",
    "MULTIPLY",
    "VALIDATION",
    "MAXEXPONENT",
    "RC_DROOP",
    "$ME",
    "ISHFTC",
    "DO_TASK",
    "CLASS_OF",
    "BYTE_UNSIGNED",
    "FLOOR",
    "$2PI",
    "$TORR",
    "FRACTION",
    "DECOMPRESS",
    "SQUEEZE",
    "WORD_UNSIGNED",
    "SHIFT_LEFT",
    "F_FLOAT",
    "CLASS",
    "COMPLETION_OF",
    "INTEGRAL",
    "IBITS",
    "BUILD_PARAM",
    "COMPLETION_MESSAGE_OF",
    "BUILD_PROGRAM",
    "SLOPE_OF",
    "END_OF",
    "OR_NOT",
    "CVT",
    "DTYPE_RANGE",
    "SMOOTH",
    "TEXT",
    "KIND_OF",
    "$SHOT",
    "SHIFT_RIGHT",
    "NAND_NOT",
    "ERROR_OF",
    "OR",
    "ERRORLOGS_OF",
    "$DEFAULT",
    "MAKE_OPAQUE",
    "FT_FLOAT",
    "DIAGONAL",
    "$MU0",
    "TUPLE",
    "DATA_WITH_UNITS",
    "SOLVE",
    "TAN",
    "LONG",
    "BUILD_WITH_UNITS",
    "ON_ERROR",
    "TAND",
    "SQUARE",
    "VALUE_OF",
    "MAKE_DEPENDENCY",
    "SORTVAL",
    "VALIDATION_OF",
    "QUADWORD",
    "AS_IS",
    "$SHOTNAME",
    "QUADWORD_UNSIGNED",
    "DOT_PRODUCT",
    "FCLOSE",
    "MAKE_DISPATCH",
    "$GN",
    "$K",
    "METHOD_OF",
    "MAKE_WINDOW",
    "NDESC",
    "$EPSILON0",
    "CONJG",
    "SYSTEM_CLOCK",
    "EXTEND",
    "UNPACK",
    "$FALSE",
    "CEILING",
    "DSCPTR",
    "EXPONENT",
    "FIRSTLOC",
    "IACHAR",
    "WINDOW_OF",
    "UNITS_OF",
    "MINVAL",
    "PROGRAM_OF",
    "$ATM",
    "MAXVAL",
    "EPSILON",
    "DEBUG",
    "UNITS",
    "$EXPT",
    "$DEGREE",
    "PRIVATE",
    "H_COMPLEX",
    "ELSEWHERE",
    "G_COMPLEX",
    "IMAGE_OF",
    "PHASE_OF",
    "NDESC_OF",
    "DERIVATIVE",
    "FS_FLOAT",
    "EOSHIFT",
    "DECOMPILE_DEPENDENCY",
    "SHAPE",
    "$P0",
    "SPAWN",
    "UNSIGNED",
    "$THIS",
    "BUILD_PATH",
    "COSH",
    "$PI",
    "HELP_OF",
    "RAMP",
    "I_TO_X",
    "EXECUTE",
    "NAME_OF",
    "AIMAG",
    "LANGUAGE_OF",
    "WHILE",
    "EXTRACT",
    "DISPATCH_OF",
    "D_COMPLEX",
    "SINH",
    "ATANH",
    "TRANSPOSE_MUL",
    "$RYDBERG",
    "WHERE",
    "$ROPRAND",
    "TRANSFER",
    "RRSPACING",
    "BUILD_WINDOW",
    "$QE",
    "TRANSLATE",
    "FSEEK",
    "OBJECT_OF",
    "SWITCH",
    "COMPILE_DEPENDENCY",
    "$MP",
    "TIME_OUT_OF",
    "DSCPTR_OF",
    "RADIX",
    "TRIM",
    "EVALUATE",
    "ESHAPE",
    "$HBAR",
    "TRANSPOSE_",
    "EXT_FUNCTION",
    "INDEX",
    "$EV",
    "CMPLX",
    "$G",
    "VERIFY",
    "$VALUE",
    "FIX_ROPRAND",
    "SHOW_PRIVATE",
    "ARRAY",
    "OPCODE_BUILTIN",
    "AXIS_OF",
    "EQUALS_FIRST",
    "SHOW_PUBLIC",
    "$GAS",
    "UPCASE",
    "$NARG",
    "OPTIONAL",
    "EXP",
    "EQUALS",
    "BSEARCH",
    "F_COMPLEX",
    "$FARADAY",
    "OCTAWORD",
    "OCTAWORD_UNSIGNED",
    "SPACING",
    "$H",
    "WHEN_OF",
    "TASK_OF",
    "$MISSING",
    "USING",
    "UNARY_MINUS",
    "CONVOLVE",
    "SHOW_VM",
    "FT_COMPLEX",
    "$ALPHA",
    "STD_DEV",
    "EQ",
    "ABSSQ",
    "OPCODE_STRING",
    "EQV",
    "TANH",
    "UNARY_PLUS",
    "TINY",
    "FS_COMPLEX",
    "NEQV"
  };
#define stringpool ((const char *) &stringpool_contents)
#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct fun *
in_word_set (str, len)
     register const char *str;
     register unsigned int len;
{
  static const struct fun wordlist[] =
    {
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str12,229},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str18,217},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str27,400},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str32,216},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str34,295},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str38,222},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str48,221},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str54,237},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str64,107},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str65,109},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str69,402},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str74,331},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str78,245},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str84,296},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str89,89},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str91,76},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str92,177},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str94,458},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str99,390},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str103,318},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str104,316},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str105,297},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str106,130},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str108,45},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str109,56},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str111,303},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str112,174},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str113,38},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str114,319},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str115,47},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str116,302},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str118,241},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str119,371},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str120,59},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str122,454},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str124,308},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str126,317},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str129,181},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str134,136},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str136,299},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str139,92},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str149,225},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str156,83},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str158,84},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str160,224},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str161,412},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str162,70},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str163,43},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str170,212},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str171,58},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str175,71},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str178,104},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str179,33},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str182,280},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str183,298},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str184,50},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str185,307},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str187,413},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str188,414},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str189,176},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str194,42},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str198,213},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str200,397},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str202,192},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str208,201},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str214,198},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str215,193},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str217,441},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str219,194},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str220,57},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str226,238},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str228,336},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str232,131},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str234,77},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str235,199},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str236,118},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str237,337},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str244,300},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str247,282},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str250,69},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str251,101},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str254,456},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str256,102},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str260,31},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str261,389},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str265,274},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str267,46},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str268,74},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str269,115},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str271,78},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str278,106},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str279,311},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str281,175},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str282,226},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str283,334},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str284,44},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str286,243},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str288,137},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str296,235},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str301,291},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str302,121},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str303,64},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str304,185},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str305,68},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str306,284},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str307,183},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str308,207},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str310,305},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str311,86},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str312,179},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str314,204},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str316,321},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str318,246},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str321,425},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str322,206},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str323,424},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str327,85},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str329,53},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str332,215},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str335,290},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str337,200},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str339,419},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str340,54},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str344,196},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str345,440},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str346,129},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str352,430},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str358,203},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str359,210},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str360,116},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str362,231},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str366,232},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str367,72},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str368,273},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str375,230},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str383,272},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str387,401},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str389,427},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str392,140},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str397,275},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str398,220},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str401,418},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str402,208},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str403,32},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str405,429},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str406,283},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str408,219},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str409,142},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str411,431},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str413,188},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str417,39},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str419,434},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str420,123},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str427,304},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str428,223},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str429,98},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str434,112},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str435,35},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str436,393},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str437,126},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str440,37},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str443,113},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str446,294},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str447,279},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str449,415},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str450,63},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str451,146},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str452,218},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str454,144},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str455,447},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str459,335},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str468,197},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str469,439},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str475,239},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str477,443},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str482,157},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str483,211},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str484,0},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str489,416},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str491,110},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str492,143},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str493,186},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str497,40},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str498,411},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str499,293},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str501,333},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str502,276},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str503,252},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str508,119},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str510,209},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str513,171},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str514,258},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str519,114},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str520,141},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str521,51},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str522,189},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str531,420},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str535,187},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str537,460},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str539,376},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str543,29},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str545,265},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str546,19},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str550,90},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str551,27},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str556,125},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str558,190},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str560,368},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str561,81},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str562,352},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str563,49},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str564,270},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str565,255},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str566,428},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str569,75},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str570,66},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str571,24},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str572,422},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str575,249},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str576,20},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str577,277},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str578,165},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str579,256},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str584,426},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str588,253},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str590,417},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str591,127},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str592,73},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str593,228},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str598,169},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str599,394},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str600,312},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str607,328},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str608,287},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str610,433},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str612,205},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str614,48},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str615,363},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str617,3},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str621,370},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str623,377},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str624,357},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str625,167},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str628,138},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str629,233},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str632,361},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str633,449},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str634,62},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str636,1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str637,301},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str638,99},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str645,14},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str646,374},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str647,5},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str648,399},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str649,94},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str654,36},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str655,269},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str658,163},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str661,329},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str664,360},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str665,156},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str667,147},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str668,445},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str673,257},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str676,410},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str681,133},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str682,242},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str685,4},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str688,310},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str689,247},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str691,385},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str692,234},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str694,375},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str695,16},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str696,313},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str697,448},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str698,435},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str699,91},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str700,168},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str702,372},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str703,28},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str708,170},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str709,120},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str714,459},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str719,369},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str720,314},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str722,173},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str725,95},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str729,100},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str733,202},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str735,65},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str736,79},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str737,442},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str738,82},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str739,322},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str741,148},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str743,268},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str745,111},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str747,292},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str750,323},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str751,344},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str756,437},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str758,388},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str761,315},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str764,250},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str768,446},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str769,267},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str772,398},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str781,386},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str787,455},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str788,452},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str793,124},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str801,408},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str807,457},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str810,404},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str812,324},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str815,340},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str819,227},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str823,88},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str825,262},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str826,341},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str828,330},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str829,359},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str831,421},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str849,325},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str854,358},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str855,285},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str860,55},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str862,444},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str864,286},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str867,132},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str871,96},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str873,423},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str876,407},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str885,15},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str894,240},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str903,432},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str906,251},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str907,406},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str910,103},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str913,339},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str917,391},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str918,355},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str919,8},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str927,93},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str937,134},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str939,161},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str943,164},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str946,184},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str950,367},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str955,354},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str963,244},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str964,281},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str965,405},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str973,236},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str978,150},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str980,117},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str982,353},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str984,387},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str985,6},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str989,278},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str994,182},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str996,145},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str999,178},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1002,191},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1003,271},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1004,438},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1007,122},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1008,450},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1012,149},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1015,396},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1016,135},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1017,21},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1018,327},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1020,356},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1023,403},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1031,80},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1034,108},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1037,22},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1038,180},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1041,289},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1042,392},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1048,159},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1053,248},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1055,41},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1061,214},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1066,366},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1068,409},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1072,128},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1079,139},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1084,320},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1090,60},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1095,382},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1101,26},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1106,365},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1107,25},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1110,347},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1115,306},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1116,87},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1128,23},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1131,381},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1135,309},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1136,259},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1143,338},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1145,395},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1151,18},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1158,345},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1180,436},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1182,288},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1184,349},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1190,158},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1197,155},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1208,13},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1212,348},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1213,162},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1222,195},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1223,7},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1246,97},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1265,10},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1273,362},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1276,30},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1277,166},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1279,378},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1287,52},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1302,263},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1303,61},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1304,153},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1313,379},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1327,11},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1329,383},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1338,373},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1341,266},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1365,160},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1368,152},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1377,67},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1409,172},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1412,9},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1422,260},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1431,261},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1458,326},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1465,12},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1478,364},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1489,343},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1525,17},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1532,384},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1533,350},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1552,105},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1577,380},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1612,453},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1630,2},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1631,332},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1646,151},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1659,34},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1691,264},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1742,154},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1796,342},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1809,351},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1828,346},
      {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1832,451},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str1934,254}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + stringpool;

              if ((((unsigned char)*str ^ (unsigned char)*s) & ~32) == 0 && !gperf_case_strncmp (str, s, len) && s[len] == '\0')
                return &wordlist[key];
            }
        }
    }
  return 0;
}

int tdi_hash(const int len, const char *const pstring)
{
    const struct fun *fun = in_word_set(pstring, len);
    return fun ? fun->idx : -1;
}
