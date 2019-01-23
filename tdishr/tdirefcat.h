/*	tdirefcat.h
	Data type information table reference.

	Ken Klare, LANL CTR-7	(c)1989,1990
*/
#include <mdsdescrip.h>
#define TdiCAT_COMPLEX	0x1000
#define TdiCAT_WIDE_EXP	0x0800
#define TdiCAT_FLOAT		0x0700
#define TdiCAT_B		0x0300
#define TdiCAT_BU		0x0100
#define TdiCAT_LENGTH	0x00ff
#define TdiCAT_SIGNED	(0x8000 | TdiCAT_B | TdiCAT_LENGTH)
#define TdiCAT_UNSIGNED	(0x8000 | TdiCAT_BU | TdiCAT_LENGTH)
#define TdiCAT_F		(0x8000 | TdiCAT_FLOAT | 3)
#define TdiCAT_D		(0x8000 | TdiCAT_FLOAT | 7)
#define TdiCAT_FC		(TdiCAT_COMPLEX | TdiCAT_F)

struct TdiCatStruct {
  unsigned short in_cat;
  dtype_t in_dtype;
  unsigned short out_cat;
  dtype_t out_dtype;
  unsigned short digits;
};
struct TdiCatStruct_table {
  char *name;			/*text for decompile */
  unsigned short cat;		/*category code */
  unsigned char length;		/*size in bytes */
  unsigned char digits;		/*size of text conversion */
  char *fname;			/*exponent name for floating decompile */
};
#include <inttypes.h>
extern const unsigned char TdiCAT_MAX;
extern const struct TdiCatStruct_table TdiREF_CAT[];
