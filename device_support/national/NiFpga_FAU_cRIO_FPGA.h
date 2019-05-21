/*
 * Generated with the FPGA Interface C API Generator 14.0.0
 * for NI-RIO 14.0.0 or later.
 */

#ifndef __NiFpga_FAU_cRIO_FPGA_h__
#define __NiFpga_FAU_cRIO_FPGA_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1400
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_FAU_cRIO_FPGA_Bitfile;
 */
#define NiFpga_FAU_cRIO_FPGA_Bitfile "/opt/mdsplus/device_support/national/NiFpga_FAU_cRIO_FPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_FAU_cRIO_FPGA_Signature = "B2C2E3371BED24945ECB61145287BFF5";

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_IndicatorBool_FifoOverflowed = 0x815E,
   NiFpga_FAU_cRIO_FPGA_IndicatorBool_Mod1DIO4 = 0x8136,
   NiFpga_FAU_cRIO_FPGA_IndicatorBool_Mod1DIO5 = 0x813A,
   NiFpga_FAU_cRIO_FPGA_IndicatorBool_Mod1DIO6 = 0x813E,
   NiFpga_FAU_cRIO_FPGA_IndicatorBool_Mod1DIO7 = 0x8142,
   NiFpga_FAU_cRIO_FPGA_IndicatorBool_ready = 0x812A,
} NiFpga_FAU_cRIO_FPGA_IndicatorBool;

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_IndicatorU16_AcqState = 0x8162,
   NiFpga_FAU_cRIO_FPGA_IndicatorU16_acq_ctd = 0x811E,
   NiFpga_FAU_cRIO_FPGA_IndicatorU16_test_ctd = 0x8122,
   NiFpga_FAU_cRIO_FPGA_IndicatorU16_tick_ctd = 0x8126,
} NiFpga_FAU_cRIO_FPGA_IndicatorU16;

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_IndicatorU64_tick = 0x8158,
} NiFpga_FAU_cRIO_FPGA_IndicatorU64;

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_ControlBool_AcqEna = 0x8166,
   NiFpga_FAU_cRIO_FPGA_ControlBool_Mod5DIO4 = 0x8146,
   NiFpga_FAU_cRIO_FPGA_ControlBool_Mod5DIO5 = 0x814A,
   NiFpga_FAU_cRIO_FPGA_ControlBool_Mod5DIO6 = 0x814E,
   NiFpga_FAU_cRIO_FPGA_ControlBool_Mod5DIO7 = 0x8152,
   NiFpga_FAU_cRIO_FPGA_ControlBool_goToIdle = 0x810E,
   NiFpga_FAU_cRIO_FPGA_ControlBool_stop2 = 0x8156,
} NiFpga_FAU_cRIO_FPGA_ControlBool;

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_ControlU16_CountFreq = 0x812E,
   NiFpga_FAU_cRIO_FPGA_ControlU16_CountMode = 0x8132,
   NiFpga_FAU_cRIO_FPGA_ControlU16_PTEmask = 0x8116,
} NiFpga_FAU_cRIO_FPGA_ControlU16;

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_ControlU32_ContAcqPreiod = 0x8118,
   NiFpga_FAU_cRIO_FPGA_ControlU32_PTEcount = 0x8110,
} NiFpga_FAU_cRIO_FPGA_ControlU32;

typedef enum
{
   NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFOR = 1,
   NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64_AcquisitionFIFORT = 0,
} NiFpga_FAU_cRIO_FPGA_TargetToHostFifoU64;

#endif
