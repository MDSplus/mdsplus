/*****************************************************************************
 *
 * File: acq32ioctl.h - Application interface to acq32 Linux driver
 *
 * $RCSfile$
 *
 * Copyright (C) 1999 D-TACQ Solutions Ltd
 * not to be used without owner's permission
 *
 * Description:
 *
 * $Id$
 * $Log$
 * Revision 1.12  2001/03/30 20:17:01  pgm
 * UDMA first cut working
 *
 * Revision 1.11  2001/03/25 19:19:04  pgm
 * first cut i2o command processing
 *
 * Revision 1.10  2000/10/28 21:19:15  pgm
 * getSetNumSamples
 *
 * Revision 1.9  2000/10/08 19:57:01  pgm
 * TriggeredContinuous etc
 *
 * Revision 1.8  2000/10/01 21:53:51  pgm
 * bigdump, sping, debug etc
 *
 * Revision 1.7  2000/09/26 19:46:52  pgm
 * GATED_CONTINUOUS pre/post, lseek, spin
 *
 * Revision 1.6  2000/07/01 07:25:39  pgm
 * SOFT_CONTINUOUS
 *
 * Revision 1.5  2000/06/11 19:29:29  pgm
 * getChannelMask
 *
 * Revision 1.4  2000/06/04 18:41:00  pgm
 * Signal Conditioning protocol implemented
 *
 * Revision 1.3  2000/05/20 08:27:03  pgm
 * ICD REV6 function codes
 *
 * Revision 1.2  1999/11/20 21:20:30  pgm
 * 991120 time for bed
 *
 * Revision 1.1.1.1  1999/10/25 13:14:08  pgm
 * linux include
 *
 * Revision 1.7  1999/10/20 19:57:16  pgm
 * mixed in ebsa driver funcs + new rom offset hakker
 *
 * Revision 1.6  1999/10/14 22:51:50  pgm
 * simul engine runs - SHIP IT QUICKcvs status | grep Status
 *
 * Revision 1.5  1999/10/14 21:10:28  pgm
 * master command handling works
 *
 * Revision 1.4  1999/10/12 21:24:20  pgm
 * master command handling top down connection OK
 *
 * Revision 1.3  1999/10/12 10:23:47  pgm
 * basic master command/readback struct WORKS
 *
 * Revision 1.2  1999/10/02 21:27:14  pgm
 * first pass compile OK. /proc/acq32 stuffed
 *
 * Revision 1.1  1999/10/02 19:33:46  pgm
 * lets go
 *
 *
\*****************************************************************************/

#ifndef _ACQ32IOCTL_H_
#define _ACQ32IOCTL_H_

#include <sys/ioctl.h>

/*
 * whence arg for seek
 */

#define SEEK_TRIG (SEEK_END + 1)

/*
 * Ioctl definitions
 */

/* Use 'e' as magic number */
#define ACQ32_IOC_MAGIC 'a'

#define ACQ32_IOCRESET _IO(ACQ32_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */

#define ACQ32_IOCGMEM _IOR(ACQ32_IOC_MAGIC, 1, sizeof(void *))
#define ACQ32_IOSLIMIT _IO(ACQ32_IOC_MAGIC, 2) // set readout limit

// arg for LIMIT is limit count of samples per read

#define ACQ32_IOSFORMAT _IO(ACQ32_IOC_MAGIC, 3) // set output format

#define ACQ32_IOARMSPIN _IO(ACQ32_IOC_MAGIC, 4)  // make ARM spin
#define ACQ32_IOARMRESET _IO(ACQ32_IOC_MAGIC, 5) // reset ARM

#define ACQ32_IOSDEBUG _IOW(ACQ32_IOC_MAGIC, 6, 4) // set debug level
// arg for format is

enum ChannelMode {
  CM_BINARY,
  CM_HEX,
  CM_DEC,
  CM_VOLTS,

  CM_EXCLUSIVE_OPTS = 0xf,

  CM_LINENUMS = 0x40,
  CM_DOS_LINES = 0x80
};

/*
 * access the mailboxes 0 <= ix <= 3
 */

#define ACQ32_IO_MBX_NR 10

#define ACQ32_IOSMBX(ix) _IO(ACQ32_IOC_MAGIC, ACQ32_IO_MBX_NR + (ix))

#define ACQ32_IOGMBX(ix)                                                       \
  _IOR(ACQ32_IOC_MAGIC, ACQ32_IO_MBX_NR + (ix), sizeof(unsigned *))

#define ACQ32_IOSROM_WORD_A01 _IO(ACQ32_IOC_MAGIC, 14)
#define ACQ32_IOCHARDRESET _IO(ACQ32_IOC_MAGIC, 15) /* debugging tool */
#define ACQ32_IOSENDI2O _IO(ACQ32_IOC_MAGIC, 16)
#define ACQ32_IOREAD_LOCALBUF _IO(ACQ32_IOC_MAGIC, 17)
#define ACQ32_IOC_MAXNR 18

/*
 * ioctls are less important that command strings:
 */

/*
 * channel commands - apply to row dev (non-volatile)
 *                    or channel dev (volatile)
 */

#define CC_FORMAT "format"
#define CC_FORMAT_BIN "bin"
#define CC_FORMAT_HEX "hex"
#define CC_FORMAT_DEC "dec"
#define CC_FORMAT_VOLTS "volts"
#define CC_FORMAT_LINENUMS "lines"
#define CC_FORMAT_DOS "dos"

#define CC_LIMIT "limit" // limit n - limit output to n samples

#define CC_SEEK "seek" // seek {whence} n
#define CC_SEEK_BEGIN "start"
#define CC_SEEK_END "end"
#define CC_SEEK_CURRENT "current"
#define CC_SEEK_TRIG "trig"

#define CC_STREAM "stream" // stream [stride [, mean]]

/*
 * master commands - apply to master dev only
 */

#define MC_GET_NUMCHANNELS "getNumChannels"
#define MC_GET_NUMSAMPLES "getNumSamples"
#define MC_GET_SETSAMPLES "getSetNumSamples"
#define MC_GET_STATE "getState"

#define MC_SET_CHANNEL_MASK "setChannelMask" // <mask> {0|1}*N
#define MC_GET_CHANNEL_MASK "getChannelMask"

#define MC_GET_MEMORY_DEPTH "getMemoryDepth" // <channel>
#define MC_SET_MODE "setMode"                // <mode> <samples> [RUN_BLOCK]
#define MC_GET_MODE "getMode"                // <mode> <samples>
#define MC_SET_ARM "setArm"
#define MC_SET_ABORT "setAbort"

#define MC_SET_ROUTE "setRoute"             // raw, row, channel
#define MC_SET_DISTRIBUTOR "setDistributor" // 0..n

#define MC_MODE_GATED_TRANSIENT "GATED_TRANSIENT"
#define MC_MODE_GATED_CONTINUOUS "GATED_CONTINUOUS"
#define MC_MODE_SOFT_TRANSIENT "SOFT_TRANSIENT"
#define MC_MODE_SOFT_CONTINUOUS "SOFT_CONTINUOUS"

#define MC_SET_MODE_GC "setModeGatedContinuous"      // <pre> <post>
#define MC_SET_MODE_TRC "setModeTriggeredContinuous" // <pre> <post>

#define MC_STATE_STOP "ST_STOP"
#define MC_STATE_ARM "ST_ARM"
#define MC_STATE_RUN "ST_RUN"
#define MC_STATE_TRIGGER "ST_TRIGGER"

#define MC_MODE_RUN_BLOCK "RUN_BLOCK"

#define MC_SET_ARMED "setArmed"
#define MC_SET_ABORT "setAbort"

#define MC_GET_NUMSAMPLES "getNumSamples"

#define MC_SET_INTERNAL_CLOCK "setInternalClock"
#define MC_GET_INTERNAL_CLOCK "getInternalClock"
#define MC_SET_SAMPLE_TAGGING "setSampleTagging"

#define MC_BIGDUMP "bigdump"
#define MC_GET_FWREV "getFwrev"

/*
 * signal conditioning commands/queries
 * valid on SC device
 */

#define SC_SET_DIO "setDIO"         // setDIO <dio-mask>
#define SC_GET_DIO "getDIO"         // getDIO
#define SC_SET_CHANNEL "setChannel" // setChannel <ch>,<g1>,<g2>,<v_ex>
#define SC_SET_STREAMING "stream"   // stream <stride> <mean>
#define DIO_MASK_INPUT '-'
#define DIO_MASK_OUTPUT1 '1'
#define DIO_MASK_OUTPUT0 '0'
#define DIO_MASK_INPUT0 'L'
#define DIO_MASK_INPUT1 'H'

/*
 * response prefixes
 */
#define ACQ32_ACK "ACQ32:"
#define ACQ32_ACK_FAIL "ERROR"
#define SIGCOND_ACK "SIGCOND:"

/*
 * acq32 specific error codes
 */

enum { EACQ32_NO_INCOMING_I2O = 3200, EACQ32_NO_MAPPING };

/*
 * ACQ32_IOREAD_LOCALBUF
 */

struct READ_LOCALBUF_DESCR {
  short *buffer; // MUST be buffer returned from mmap()
  int nsamples;
  int istart;
  int istride;
};

#endif //    _ACQ32IOCTL_H_
