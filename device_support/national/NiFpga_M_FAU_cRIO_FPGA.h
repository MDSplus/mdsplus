/*
 * Generated with the FPGA Interface C API Generator 18.0.0
 * for NI-RIO 18.0.0 or later.
 */

#ifndef __NiFpga_M_FAU_cRIO_FPGA_h__
#define __NiFpga_M_FAU_cRIO_FPGA_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1800
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_M_FAU_cRIO_FPGA_Bitfile;
 */
#define NiFpga_M_FAU_cRIO_FPGA_Bitfile "/opt/mdsplus/device_support/national/NiFpga_M_FAU_cRIO_FPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_M_FAU_cRIO_FPGA_Signature = "6EE2C06633F5A38B38D1A18197FD10B6";

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_CONT_ACQ = 0x8176,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_Clock = 0x81EA,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FLS_PE1 = 0x81AA,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FLS_PE2 = 0x81A6,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FLS_PE3 = 0x81A2,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FLS_PE4 = 0x819E,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FLS_PE5 = 0x819A,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FLS_PE6 = 0x8196,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_FifoOverflowed = 0x815A,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_IEE1588PPS = 0x8136,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_IEE1588valid = 0x8156,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_OIU_PE1 = 0x8192,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_OIU_PE2 = 0x818E,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_OIU_PE3 = 0x818A,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_OIU_PE4 = 0x8186,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_OIU_PE5 = 0x8182,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_OIU_PE6 = 0x817E,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_Strobe = 0x81E6,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_StrobeDeleyed = 0x81DE,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorBool_isAcq = 0x813A,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorBool;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorU16_AcqState = 0x813E,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorU16;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorU32_Counterout = 0x81E0,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorU32;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorI64_TicksError = 0x8150,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorI64;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorU64_tick = 0x810C,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorU64_tick_fpga = 0x814C,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorU64;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_ControlBool_AcqEna = 0x8142,
   NiFpga_M_FAU_cRIO_FPGA_ControlBool_ContAcqEna = 0x815E,
   NiFpga_M_FAU_cRIO_FPGA_ControlBool_HwEnaAcq = 0x8132,
   NiFpga_M_FAU_cRIO_FPGA_ControlBool_SyncPtpEna = 0x812E,
   NiFpga_M_FAU_cRIO_FPGA_ControlBool_stopSCTML = 0x817A,
} NiFpga_M_FAU_cRIO_FPGA_ControlBool;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX1_mode = 0x814A,
   NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX1_op = 0x811A,
   NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX2_mode = 0x8126,
   NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX2_op = 0x8116,
   NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX3_mode = 0x811E,
   NiFpga_M_FAU_cRIO_FPGA_ControlU16_TX3_op = 0x8112,
} NiFpga_M_FAU_cRIO_FPGA_ControlU16;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_BitsNum = 0x8164,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_ClockPeriod = 0x81D8,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_ContinuosAcqTickPeriod = 0x8160,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_DelayTick = 0x8170,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_StrobeONTick = 0x8168,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_StrobePeriod = 0x816C,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_TX1_tk_sp = 0x8144,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_TX2_tk_sp = 0x8128,
   NiFpga_M_FAU_cRIO_FPGA_ControlU32_TX3_tk_sp = 0x8120,
} NiFpga_M_FAU_cRIO_FPGA_ControlU32;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_FLSOPB1 = 0x81EE,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_FLSOPB2 = 0x81D6,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_FLSOPB3 = 0x81D2,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_FLSOPB4 = 0x81CE,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_FLSOPB5 = 0x81CA,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_FLSOPB6 = 0x81C6,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_OIUOPB1 = 0x81AE,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_OIUOPB2 = 0x81B2,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_OIUOPB3 = 0x81B6,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_OIUOPB4 = 0x81BA,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_OIUOPB5 = 0x81BE,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool_OIUOPB6 = 0x81C2,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBool;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_FLSOPB1 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_FLSOPB2 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_FLSOPB3 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_FLSOPB4 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_FLSOPB5 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_FLSOPB6 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_OIUOPB1 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_OIUOPB2 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_OIUOPB3 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_OIUOPB4 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_OIUOPB5 = 16,
   NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize_OIUOPB6 = 16,
} NiFpga_M_FAU_cRIO_FPGA_IndicatorArrayBoolSize;

typedef enum
{
   NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOFLS = 1,
   NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOOIU = 0,
   NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT = 2,
} NiFpga_M_FAU_cRIO_FPGA_TargetToHostFifoU64;

#endif
