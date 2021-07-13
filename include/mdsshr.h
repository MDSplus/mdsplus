#ifndef _MDSSHR_H
#define _MDSSHR_H
#define MdsCOMPRESSIBLE 3
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <stdio.h>
#include <usagedef.h>
#ifdef __cplusplus
extern "C"
{
#endif

  ////////////////////////////////////////////////////////////////////////////////
  ///
  ///     USED INTERNALLY BY MDSplus kernel libraries.
  ///     Compress data by passing the data to the compression routine specified
  ///     or the default routine, MdsCmprs.
  ///
  ///	An algorithm to compress data with small differences.
  ///	Only data less than 32 bits can be packed.
  ///	The run length may be varied up to the maximum by repeated calls.
  ///
  ///	Each block of information has a header and two data blocks.
  ///	Each block is independent, it has its own starting value.
  ///	The header has the number of bits (y) and the count (x) of each data
  /// block. 	Think of the data histogrammed by its significance (y) vs. index
  /// (x). 	Header block: yn & xn-1 & ye-1 & xe 	Normal block: xn fields of yn each.
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
  /// \param cimage the name of the shared library where the compression routine
  /// is to be found.
  ///        If Null then the default compression will be used.
  /// \param centry the name of the compression image to use for compressing the
  /// data.
  ///        If Null then the default compression will be used.
  /// \param in the data to be compressed.
  /// \param out the compressed data.
  /// \return status value. Either 1 for success or LibSTRTRU status code if data
  /// is not compressible
  ///

  extern int MdsCompress(const mdsdsc_t *const cimage,
                         const mdsdsc_t *const centry, const mdsdsc_t *const in,
                         mdsdsc_xd_t *const out);
  extern int MdsCompareXd(const mdsdsc_t *const dsc1_ptr,
                          const mdsdsc_t *const dsc2_ptr);
  extern int MdsDecompress(const mdsdsc_r_t *const rec_ptr,
                           mdsdsc_xd_t *const out_ptr);
  extern int MdsCopyDxXd(const mdsdsc_t *const in, mdsdsc_xd_t *const out);
  extern int MdsCopyDxXdZ(const mdsdsc_t *const in, mdsdsc_xd_t *const out,
                          void **const zone, int (*const fixup_nid)(),
                          void *const fixup_nid_arg, int (*const fixup_path)(),
                          void *const fixup_path_arg);
  extern char *MdsDescrToCstring(const mdsdsc_t *const string_dsc);
  extern void MdsEnableSandbox();
  extern int MDSfprintf(FILE *const fp, const char *const fmt, ...);
  extern void MdsFree(void *const ptr);
  extern void MdsFreeDescriptor(mdsdsc_t *d);
  extern int MdsGet1Dx(const l_length_t *const len, const dtype_t *const dtype,
                       mdsdsc_xd_t *const dsc, void **const zone);
  extern int MdsGet1DxA(const mdsdsc_a_t *const in, const length_t *const len,
                        const dtype_t *const dtype, mdsdsc_xd_t *const out);
  extern int MdsGet1DxS(const length_t *const len, const dtype_t *const dtype,
                        mdsdsc_xd_t *const out);
  extern char *MdsGetMsg(const int status);
  extern void MdsGetMsgDsc(const int status, mdsdsc_t *const out);
  extern int MdsGetStdMsg(const int status, const char **fac_out,
                          const char **msgnam_out, const char **text_out);
  extern int MdsFree1Dx(mdsdsc_xd_t *const dsc, void **const zone);
  extern int MdsMsg(const int status, const char *const fmt, ...);
  extern int MDSQueueEvent(const char *const event, int *const eventid);
  extern int MDSGetEventQueue(const int eventid, const int timeout,
                              int *const data_len, char **const data);
  extern int MdsSandboxEnabled();
  extern int MdsSerializeDscIn(const char *const in, mdsdsc_xd_t *const out);
  extern int MdsSerializeDscOutZ(
      const mdsdsc_t *const in, mdsdsc_xd_t *const out, int (*const fixupNid)(),
      void *const fixupNidArg, int (*const fixupPath)(), void *const fixupPathArg,
      const int compress, int *const compressible, l_length_t *const length,
      l_length_t *const reclen, dtype_t *const dtype, class_t *const classType,
      const int altbuflen, void *const altbuf, int *const data_in_altbuf);
  extern int MdsSerializeDscOut(const mdsdsc_t *const in, mdsdsc_xd_t *const out);
  extern int MDSSetEventTimeout(const int seconds);
  extern int MDSEvent(const char *eventName, int num_bytes, char *data);
  extern int MDSEventAst(const char *eventName,
                         void (*astadr)(void *, int, char *), void *astprm,
                         int *eventid);
  extern int MDSEventAstMask(const char *eventName,
                             void (*astadr)(void *, int, char *), void *astprm,
                             int *eventid, unsigned int cpuMask);
  extern int MDSEventCan(const int eventid);
  extern int MDSWfevent(const char *const evname, const int buflen,
                        char *const data, int *const datlen);
  extern int MDSWfeventTimed(const char *const evname, const int buflen,
                             char *const data, int *const datlen,
                             const int timeout);
  extern char *MdsDtypeString(const dtype_t id);
  extern char *MdsClassString(const class_t id);
  extern char *MdsUsageString(const usage_t id);
  extern int MDSprintf(const char *fmt, ...);
  extern char *TranslateLogical(const char *name);
  extern void TranslateLogicalFree(char *const value);
  extern int TranslateLogicalXd(const mdsdsc_t *const in, mdsdsc_xd_t *const out);
  extern const char *MdsRelease();
  extern const mdsdsc_t *MdsReleaseDsc();
  extern int MdsPutEnv(const char *const cmd);
  extern void MdsGlobalLock();
  extern void MdsGlobalUnlock();
  extern int MdsXpand(int *const nitems_ptr, const mdsdsc_a_t *const pack_dsc_ptr,
                      mdsdsc_a_t *const items_dsc_ptr, int *const bit_ptr);
  extern char *Now32(char *buf);

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
  // FREEXD
  static void __attribute__((unused)) free_xd(void *ptr)
  {
    MdsFree1Dx((mdsdsc_xd_t *)ptr, NULL);
  }
#define FREEXD_ON_EXIT(ptr) pthread_cleanup_push(free_xd, ptr)
#define FREEXD_IF(ptr, c) pthread_cleanup_pop(c)
#define FREEXD_NOW(ptr) FREEXD_IF(ptr, 1)
#define FREEXD_CANCEL(ptr) FREEXD_IF(ptr, 0)
#define INIT_AND_FREEXD_ON_EXIT(xd) \
  EMPTYXD(xd);                      \
  FREEXD_ON_EXIT(&xd)
#endif

#ifdef __cplusplus
}
#endif
#endif
