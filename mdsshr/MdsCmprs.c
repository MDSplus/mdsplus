/*	MdsCmprs.C
	An algorithm to compress data with small differences.
	Only data less than 32 bits can be packed.
	The run length may be varied up to the maximum by repeated calls.

	F77: MdsCmprs(nitems, %descr(items), %descr(pack), bit)
	F77: MdsXpand(nitems, %descr(pack), %descr(items), bit)

	nitems	is number of items to use.
	items	is a descriptor of uncompressed data.
	pack	is a descriptor of compressed array of sufficient size to hold the result.
	bit	is a modified longword bit offset from the beginning of pack.
	Error if pack cannot hold all the compressed data.

	Each block of information has a header and two data blocks.
	Each block is independent, it has its own starting value.
	The header has the number of bits (y) and the count (x) of each data block.
	Think of the data histogrammed by its significance (y) vs. index (x).
	Header block: yn & xn-1 & ye-1 & xe
	Normal block: xn fields of yn each.
	Exception block: xe fields of ye each.
	The exception block is absent if there are no exceptions.
	The normal block is absent if all items are zero.

	The input is differenced and these numbers are checked
	for the number of bits in their representation.
	The data is optimally divided into normal points and exceptions.
	The normal points have the differences except those points that
	are exceptions are marked with the most negative twos complement
	number for the size of the normal point.
	Thus non-exceptional points are in a symmetric range about 0.
	The exception list can approach 31/32 as long as the difference list.

	Numerical example: items = NINT(256.*SIN(LOG(1..10)))
	items	= [0,164,228,252,256,250,238,224,207,190]
	diff	= [0,164, 64, 24,  4, -6,-12,-14,-17,-17]
	i	= [0,  9,  8,  6,  4,  4,  5,  5,  6,  6]
	y	= [1,2,3,4,5,6,7,8,9]
	tally	= [1,0,0,2,2,3,0,1,1]
	xsum	= [1,1,1,3,5,8,8,9,10]
	ye = 9; yn = 6; xe = 2; xn = 10
	for MAXX 1024:
	pack	= [0x00880246, 0x84620800, 0x4befcb4e, 0x080a] 110 bits
	.0...8...0...a...4...b...e...f...c...b...4...e...8...4...6...2...0...8...0...0...0...0...8...8...0...2...4...6
	00100000001010010010111110111111001011010011101000010001100010000010000000000000000000100010000000001001000110
	.......64......164...-17...-17...-14...-12....-6.....4....24....ex....ex.....0......xe=2.....8....xn-1=9..yn=6

	On VAXstation 3100, time for 1000 items in microseconds/item (1000 trials):
	dtype:		B	W	L	F	ANINT(F)
	pk:		na	na	3.2 to 3.5
	unpk:		na	na	3.0 >=10000 bits required
	cmprs:		10.8	9.8	8.3	14.6	15.4
	xpand:		6.0	6.0	5.7	6.7	8.2
	bits:		2689	2671	2671	22408	17000
	compress:	11.9	10.8	9.1	16.2	16.8
	decompress:	6.1	6.1	5.7	6.8	8.3
	data bytes:	337	334	334	2801	2125
	data bits:	2696	2672	2672	22408	17000

	Ken Klare, LANL P-4	(c)1990
	Based on ideas of Thomas W. Fredian, MIT (c)1985, the DELTA method.
	Assumes: 8-bit bytes, 32-bit long, 2's complement
	MdsPk/MdsUnpk do right-to-left packing/unpacking, not assumed here.
*/

#include <mdsdescrip.h>
#include <mdsshr.h>
#include <string.h>
#include <librtl_messages.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAXX 1024		/*length of longest run allowed*/
#define MAXY 32			/*maximum bits that we can pack*/
#define BITSX 10		/*number of bits for run length*/
#define BITSY 6			/*number of bits in y 0-32*/
typedef union
{
  int       l;
  struct
  {
    short     s0,
                s1;
  }         s;
}        *PF;

union Y_X
{
  int       dummy;
  struct
  {
    unsigned  y:BITSY;
    unsigned  x:BITSX;
  }         fields;
};
struct HEADER
{
  union Y_X n;
  union Y_X e;
};

static int FIELDSY = BITSY + BITSX;
static int FIELDSX = 2;

int       MdsCmprs(
		              int *nitems_ptr,
		              struct descriptor_a *items_dsc_ptr,
		              struct descriptor_a *pack_dsc_ptr,
		              int *bit_ptr)
{
  int       nitems = *nitems_ptr;
  int       step = items_dsc_ptr->length;
  int       dtype = items_dsc_ptr->dtype;
  register PF px = (PF) items_dsc_ptr->pointer;
  unsigned char *pxuc = (unsigned char *)px;
  char      *pxc = (char *)px;
  unsigned short *pxus = (unsigned short *)px;
  short     *pxs = (short *)px;
  char     *ppack = pack_dsc_ptr->pointer;
  int       limit = pack_dsc_ptr->arsize * 8 - 2 * (BITSY + BITSX);
  register int j,
              yy;
  register int i,
             *p32,
             *pn,
             *pe;
  struct HEADER header;
  int       maxim,
              mark,
              old,
              best,
              test;
  int       ye,
              xe,
              yn,
              xn,
              xsum,
             *ptally;
  int       tally[MAXY + 1];
  int       diff[MAXX],
              exce[MAXX];
  static int signif[65] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7};
/***************************
Text is by the character.
Otherwise try int or short.
ASSUMES power-of-two size.
***************************/
  if (dtype == DTYPE_T)
    step = sizeof(unsigned char);
  else if ((step & (sizeof(int) - 1)) == 0)
    step = sizeof(int);
  else if ((step & (sizeof(short) - 1)) == 0)
    step = sizeof(short);
  else
    step = sizeof(char);
  nitems *= (int) items_dsc_ptr->length / step;

/***************
Do this in runs.
***************/
  while (nitems > 0)
  {
    memset((void *)tally,0,sizeof(tally));
    xn = j = MIN(nitems, MAXX);
    nitems -= j;
    p32 = pe = exce;

  /*********************
  Sign and field extend.
  Temporary is exce.
  Repoint for ints.
  *********************/
    switch (dtype)
    {
     default:
      if (step == sizeof(int))
	goto case_LU;
      if (step == sizeof(short))
	goto case_WU;
      goto case_BU;
     case DTYPE_T:
     case DTYPE_BU:
  case_BU:for (; --j >= 0;)
	*pe++ = *pxuc++;
      break;
     case DTYPE_B:
      for (; --j >= 0;)
	*pe++ = *pxc++;
      break;
     case DTYPE_WU:
  case_WU:for (; --j >= 0;)
	*pe++ = *pxus++;
      break;
     case DTYPE_W:
      for (; --j >= 0;)
	*pe++ = *pxs++;
      break;
     case DTYPE_L:
     case DTYPE_LU:
  case_LU:p32 = (int *) px;
      px = (PF) ((int *) px + j);
      break;
    /********************************************
    * Swapping words on Vax makes floats ordered.
    * NEED to consider sign swap to make ordered.
    * ASSUMES Vax-format reals. Little-endian.
    ********************************************/
     case DTYPE_F:
     case DTYPE_FC:
     case DTYPE_D:
     case DTYPE_DC:
     case DTYPE_G:
     case DTYPE_GC:
     case DTYPE_H:
     case DTYPE_HC:
      for (; --j >= 0;)
	((PF) pe)->s.s0 = ((PF) px)->s.s1, ((PF) pe)->s.s1 = ((PF) px)->s.s0, ++pe, ++px;
      break;
    }

  /******************************
  Pack input must be longword.
  With old=0 here, get new start.
  Number of bits of significance.
  For example, 5 requires 4 bits
  for the range -7 to +7.
  ******************************/
    old = 0;
    for (pn = diff, j = xn; --j >= 0; old = *p32++)
    {
      if ((int) (*pn++ = i = *p32 - old) < 0)
	i = -i;
      if ((unsigned int) i <= 64)
	yy = signif[i];
      else
      {
	yy = 0;
	if ((unsigned int) i > 0x1000000)
	{
	  i = (unsigned int) i >> 24;
	  yy += 24;
	}
	if ((unsigned int) i > 0x1000)
	{
	  i = (int) i >> 12;
	  yy += 12;
	}
	if ((unsigned int) i > 0x40)
	{
	  i = (int) i >> 6;
	  yy += 6;
	}
	yy += signif[i];
      }
      ++tally[yy];
    }
  /***************************
  Special case for all zeroes.
  ***************************/
    if (tally[0] == xn)
    {
      yn = 0;
      xe = 0;
      ye = 0;
    }
    else
    {
    /**************************************************
    Determine exception width and do xn-sum(1:y).
    tally[0] is y=1 count, i.e., the number of zeros.
    Generally this is faster than working from maximum.
    The 33-bit bin is folded into the 32-bit bin
    because numbers wrap at that point. 32-bit assumed.
    **************************************************/
      tally[MAXY - 1] += tally[MAXY];
      for (ptally = tally, xsum = xn; (*ptally = xsum -= *ptally) > 0; ++ptally);
      yn = ye = yy = (ptally - tally) + 1;
    /*******************************************
    We have ptally pointed at last nonempty bin.
    Determine optimal breakpoint for exceptions.
    Cost of level y is (xn-sum(1:y))*ye + xn*y
    For y=ye there are no exceptions.
    *******************************************/
      best = xn * yy;
      while (--yy > 0)
      {
	if ((test = xn * yy + *--ptally * ye) < best)
	{
	  best = test;
	  yn = yy;
	}
      }
      xe = tally[yn - 1];
    }
  /*********************
  Build exception table.
  Assume 2's complement.
  *********************/
    mark = -1 << yn - 1;
    maxim = ~mark;
    for (pn = diff, pe = exce, j = xe; --j >= 0;)
    {
      while (*pn <= maxim && *pn > mark)
	++pn;
      *pe++ = *pn;
      *pn++ = mark;
    }

  /******************************
  Must have enough room to store.
  ******************************/
    if (*bit_ptr + xe * ye + xn * yn > limit)
      return LibSTRTRU;
    header.n.fields.x = xn - 1;
    header.n.fields.y = yn;
    header.e.fields.x = xe;
    header.e.fields.y = ye - 1;
    MdsPk((char *) &FIELDSY, (int *) &FIELDSX, (int *) ppack, (int *) &header, (int *) bit_ptr);
    MdsPk((char *) &yn, (int *) &xn, (int *) ppack, (int *) diff, (int *) bit_ptr);
    MdsPk((char *) &ye, (int *) &xe, (int *) ppack, (int *) exce, (int *) bit_ptr);
  }
  return 1;
}
/*----------------------------------------------------------
	MdsXpand.C
	Expand compressed data.
*/
int       MdsXpand(
		              int *nitems_ptr,
		              struct descriptor_a *pack_dsc_ptr,
		              struct descriptor_a *items_dsc_ptr,
		              int *bit_ptr)
{
  int       nitems = *nitems_ptr;
  char     *ppack = pack_dsc_ptr->pointer;
  int       limit = pack_dsc_ptr->arsize * 8;
  int       step = items_dsc_ptr->length;
  int       dtype = items_dsc_ptr->dtype;
  register PF px = (PF) items_dsc_ptr->pointer;
  struct HEADER header;
  int       f;
  register PF pf;
  register int j;
  register int *pn,
             *pe,
              mark,
              old;
  int       ye,
              xe,
              yn,
              xn,
              xhead;
  int       diff[MAXX],
              exce[MAXX];
  if (dtype == DTYPE_T)
    step = sizeof(unsigned char);
  else if ((step & (sizeof(int) - 1)) == 0)
    step = sizeof(int);
  else if ((step & (sizeof(short) - 1)) == 0)
    step = sizeof(short);
  else
    step = sizeof(char);
  nitems *= (int) items_dsc_ptr->length / step;
/********************************
Note the sign-extended unpacking.
********************************/
  while (nitems > 0)
  {
    if (*bit_ptr + 2 * (BITSY + BITSX) > limit)
      break;
    MdsUnpk((char *) &FIELDSY, (int *) &FIELDSX, (int *) ppack, (int *) &header, (int *) bit_ptr);
    xhead = j = header.n.fields.x + 1;
    if (j > nitems)
      j = nitems;
    xn = j;
    yn = -(int)header.n.fields.y;
    xe = header.e.fields.x;
    ye = -(int)(header.e.fields.y + 1);
    if (*bit_ptr - ye * xe - yn * j > limit)
      break;
    nitems -= j;
    MdsUnpk((char *) &yn, (int *) &xn, (int *) ppack, (int *) diff, (int *) bit_ptr);
    if (xe)
    {
      *bit_ptr -= yn * (xhead - j);
      pe = exce;
      MdsUnpk((char *) &ye, (int *) &xe, (int *) ppack, (int *) pe, (int *) bit_ptr);
      mark = -1 << -yn - 1;
    }

  /***********************************
  Summation. Old=0 here, is new start.
  Sign and field extend.
  Note, signed and unsigned are same.
  ***********************************/
    pn = diff;
    old = 0;
    switch (dtype)
    {
     default:
      if (step == sizeof(int))
	goto case_LU;
      if (step == sizeof(short))
	goto case_WU;
      goto case_BU;
     case DTYPE_T:
     case DTYPE_B:
     case DTYPE_BU:
#define load(type) { type *newone;\
                     for (newone = (type *)px; --j >= 0;pn++,newone++)\
                     {\
                        old += (xe && (*pn == mark)) ? *pe++ : *pn ;\
                        *newone = (type)old;\
                     }\
                     px = (PF)newone;\
                   }
  case_BU:load(char);
      break;
     case DTYPE_W:
     case DTYPE_WU:
  case_WU:load(short);
      break;
     case DTYPE_L:
     case DTYPE_LU:
  case_LU:load(int);
      break;
    /********************************************
    * Swapping words on Vax makes floats ordered.
    * NEED to consider sign swap to make ordered.
    * ASSUMES Vax-format reals. Little-endian.
    ********************************************/
     case DTYPE_F:
     case DTYPE_FC:
     case DTYPE_D:
     case DTYPE_DC:
     case DTYPE_G:
     case DTYPE_GC:
     case DTYPE_H:
     case DTYPE_HC:
      pf = (PF) & f;
      pf->l = 0;
      if (xe)
	for (; --j >= 0;)
	  if (*pn != mark)
	    pf->l += *pn++, ((PF) px)->s.s0 = pf->s.s1, ((PF) px)->s.s1 = pf->s.s0, ++px;
	  else
	    ++pn, pf->l += *pe++, ((PF) px)->s.s0 = pf->s.s1, ((PF) px)->s.s1 = pf->s.s0, ++px;
      else
	for (; --j >= 0;)
	  pf->l += *pn++, ((PF) px)->s.s0 = pf->s.s1, ((PF) px)->s.s1 = pf->s.s0, ++px;
      break;
    }
  }
  if (nitems > 0)
    return LibINVSTRDES;
  return 1;
}
