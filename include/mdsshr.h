#pragma once

#define MdsCOMPRESSIBLE 3
#include <config.h>
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
///
///     USED INTERNALLY BY MDSplus kernel libraries.
///     Compress data by passing the data to the compression routine specified or the default
///     routine, MdsCmprs.
///
///	An algorithm to compress data with small differences.
///	Only data less than 32 bits can be packed.
///	The run length may be varied up to the maximum by repeated calls.
///
///	Each block of information has a header and two data blocks.
///	Each block is independent, it has its own starting value.
///	The header has the number of bits (y) and the count (x) of each data block.
///	Think of the data histogrammed by its significance (y) vs. index (x).
///	Header block: yn & xn-1 & ye-1 & xe
///	Normal block: xn fields of yn each.
///	Exception block: xe fields of ye each.
///	The exception block is absent if there are no exceptions.
///	The normal block is absent if all items are zero.
///
///	The input is differenced and these numbers are checked
///	for the number of bits in their representation.
///	The data is optimally divided into normal points and exceptions.
///	The normal points have the differences except those points that
///	are exceptions are marked with the most negative twos complement
///	number for the size of the normal point.
///	Thus non-exceptional points are in a symmetric range about 0.
///	The exception list can approach 31/32 as long as the difference list.
///
///	Numerical example: items = NINT(256.*SIN(LOG(1..10)))
///	items	= [0,164,228,252,256,250,238,224,207,190]
///	diff	= [0,164, 64, 24,  4, -6,-12,-14,-17,-17]
///	i	= [0,  9,  8,  6,  4,  4,  5,  5,  6,  6]
///	y	= [1,2,3,4,5,6,7,8,9]
///	tally	= [1,0,0,2,2,3,0,1,1]
///	xsum	= [1,1,1,3,5,8,8,9,10]
///	ye = 9; yn = 6; xe = 2; xn = 10
///	for MAXX 1024:
///	pack	= [0x00880246, 0x84620800, 0x4befcb4e, 0x080a] 110 bits
///	.0...8...0...a...4...b...e...f...c...b...4...e...8...4...6...2...0...8...0...0...0...0...8...8...0...2...4...6
///	00100000001010010010111110111111001011010011101000010001100010000010000000000000000000100010000000001001000110
///	.......64......164...-17...-17...-14...-12....-6.....4....24....ex....ex.....0......xe=2.....8....xn-1=9..yn=6
///
///  Ken Klare, LANL P-4	(c)1990
///	Based on ideas of Thomas W. Fredian, MIT (c)1985, the DELTA method.
///	Assumes: 8-bit bytes, 32-bit long, 2's complement
///	MdsPk/MdsUnpk do right-to-left packing/unpacking, not assumed here.
///
/// \param cimage the name of the shared library where the compression routine is to be found.
///        If Null then the default compression will be used.
/// \param centry the name of the compression image to use for compressing the data.
///        If Null then the default compression will be used.
/// \param in the data to be compressed.
/// \param out the compressed data.
/// \return status value. Either 1 for success or LibSTRTRU status code if data is not compressible
///
  EXPORT extern int MdsCompress(struct descriptor const *cimage, struct descriptor const *centry,
				struct descriptor const *in, struct descriptor_xd *out);
  EXPORT extern int MdsDecompress(struct descriptor_r *rec_ptr, struct descriptor_xd *out_ptr);
  EXPORT extern int MdsCopyDxXd(struct descriptor const *in, struct descriptor_xd *out);
  EXPORT extern int MdsCopyDxXdZ(struct descriptor const *in, struct descriptor_xd *out, void **zone,
				 int (*fixup_nid) (), void *fixup_nid_arg, int (*fixup_path) (),
				 void *fixup_path_arg);
  EXPORT extern char *MdsDescrToCstring(struct descriptor const *string_dsc);
  EXPORT extern int MDSfprintf(FILE * fp, char const *fmt, ...);
  EXPORT extern void MdsFree(void *);
  EXPORT extern int MdsGet1Dx(unsigned int const *len, unsigned char const *dtype, struct descriptor_xd *dsc,
			      void **zone);
  EXPORT extern int MdsGet1DxA(struct descriptor_a const *in, unsigned short const *len, unsigned char const *dtype,
			       struct descriptor_xd *out);
  EXPORT extern int MdsGet1DxS(unsigned short const *len, unsigned char const *dtype,
			       struct descriptor_xd *out);
  EXPORT extern int MdsGetCurrentShotId(char const *experiment);
  EXPORT extern char *MdsGetMsg(int sts);
  EXPORT extern int MdsFree1Dx(struct descriptor_xd *dsc, void **zone);
  EXPORT extern int MdsMsg(int sts, char const *fmt, ...);
  EXPORT extern void MdsPk(char *nbits, int *nitems, int pack[], int items[], int *bit);
  EXPORT extern int MDSQueueEvent(char const *event, int *eventid);
  EXPORT extern int MDSGetEventQueue(int eventid, int timeout, int *data_len, char **data);
  EXPORT extern int MdsSetCurrentShotId(char const *experiment, int shot);
  EXPORT extern int MdsSerializeDscIn(char const *in, struct descriptor_xd *out);
  EXPORT extern int MdsSerializeDscOutZ(struct descriptor const *in, struct descriptor_xd *out,
					int (*fixupNid) (), void *fixupNidArg, int (*fixupPath) (),
					void *fixupPathArg, int compress, int *compressible,
					unsigned int *length, unsigned int *reclen,
					unsigned char *dtype, unsigned char *classType,
					int altbuflen, void *altbuf, int *data_in_altbuf);
  EXPORT extern int MdsSerializeDscOut(struct descriptor const *in, struct descriptor_xd *out);
  EXPORT extern void MdsUnpk(char *nbits, int *nitems, int pack[], int items[], int *bit);
  EXPORT extern int MdsXpand(int *nitems, struct descriptor_a *pack, struct descriptor_a *items,
			     int *bit);
  EXPORT extern int MDSEvent(char const *eventName, int num_bytes, char *data);
  EXPORT extern int MDSEventAst(char const *eventName, void (*astadr) (void *, int, char *),
				void *astprm, int *eventid);
  EXPORT extern int MDSEventCan(int eventid);
  EXPORT extern int MDSUdpEvent(char const *eventName, int bufLen, char const *buf);
  EXPORT extern int MDSUdpEventAst(char const *eventName, void (*astadr) (void *, int, char *),
				   void *astprm, int *eventid);
  EXPORT extern int MDSUdpEventCan(int id);
  EXPORT extern int MDSWfevent(char const *evname, int buflen, char *data, int *datlen);
  EXPORT extern int MDSWfeventTimed(char const *evname, int buflen, char *data, int *datlen, int timeout);
  EXPORT extern char *MdsDtypeString(int id);
  EXPORT extern char *MdsClassString(int id);
  EXPORT extern int MDSprintf(char const *fmt, ...);
  EXPORT extern char *MdsUsageString(int id);
  EXPORT extern char *TranslateLogical(char const *name);
  EXPORT extern void TranslateLogicalFree(char *value);
  EXPORT extern int TranslateLogicalXd(struct descriptor const *in, struct descriptor_xd *out);
  EXPORT extern const char *MdsRelease();
  EXPORT extern void MdsFloatToTime(double floatTime, uint64_t * outTime);
  EXPORT extern void MdsFloatToDelta(double floatTime, uint64_t * outTime);
  EXPORT extern void MdsTimeToFloat(uint64_t inTime, float *outFloat);
  EXPORT extern void MdsTimeToDouble(uint64_t inTime, double *outFloat);
  EXPORT extern int MdsPutEnv(char const *cmd);
  EXPORT extern void MdsGlobalLock();
  EXPORT extern void MdsGlobalUnlock();
  EXPORT extern int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out);


#ifdef __cplusplus
}
#endif
