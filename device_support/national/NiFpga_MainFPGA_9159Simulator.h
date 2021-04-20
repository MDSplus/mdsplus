/*
 * Generated with the FPGA Interface C API Generator 18.0.0
 * for NI-RIO 18.0.0 or later.
 */

#ifndef __NiFpga_MainFPGA_9159_h__
#define __NiFpga_MainFPGA_9159_h__

#ifndef NiFpga_Version
#define NiFpga_Version 1800
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_MainFPGA_9159_Bitfile;
 */
//#define NiFpga_MainFPGA_9159_Bitfile "NiFpga_MainFPGA_9159.lvbitx"
#define NiFpga_MainFPGA_9159_Bitfile \
  "/opt/mdsplus/device_support/national/NiFpga_MainFPGA_9159Simulator.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
// VER 0 static const char* const NiFpga_MainFPGA_9159_Signature =
// "AD2675C50BBA1F0A5F9C162B414ED6B2"; VER 1 static const char* const
// NiFpga_MainFPGA_9159_Signature = "56E5D55C64142B9C4CC6BDA9B04689B8";
static const char *const NiFpga_MainFPGA_9159_Signature =
    "548F078B3618EB26C2F9C8C5D8B7A5CB";

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_A = 0x8196,
  NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_B = 0x8186,
  NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_C = 0x815E,
} NiFpga_MainFPGA_9159_IndicatorI16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorU16_PeriodToAOus = 0x8116,
  NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_A = 0x819A,
  NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_B = 0x818A,
  NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_C = 0x8162,
  NiFpga_MainFPGA_9159_IndicatorU16_Ver = 0x810E,
} NiFpga_MainFPGA_9159_IndicatorU16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo1 = 0x81BC,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo2_A = 0x81A0,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo2_B = 0x8168,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo2_C = 0x8140,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo3_A = 0x8190,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo3_B = 0x8180,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo3_C = 0x8158,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo4 = 0x8138,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_AO = 0x8118,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_To_AO_A = 0x8130,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_To_AO_B = 0x812C,
  NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_To_AO_C = 0x8124,
  NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_A = 0x81B4,
  NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_B = 0x817C,
  NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_C = 0x8154,
} NiFpga_MainFPGA_9159_IndicatorI32;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorU32_Period2t_A = 0x81A4,
  NiFpga_MainFPGA_9159_IndicatorU32_Period2t_B = 0x816C,
  NiFpga_MainFPGA_9159_IndicatorU32_Period2t_C = 0x8144,
  NiFpga_MainFPGA_9159_IndicatorU32_Period3t_A = 0x81AC,
  NiFpga_MainFPGA_9159_IndicatorU32_Period3t_B = 0x8174,
  NiFpga_MainFPGA_9159_IndicatorU32_Period3t_C = 0x814C,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_A = 0x81A8,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_B = 0x8170,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_C = 0x8148,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_A = 0x819C,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_B = 0x818C,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_C = 0x8164,
} NiFpga_MainFPGA_9159_IndicatorU32;

typedef enum
{
  NiFpga_MainFPGA_9159_ControlBool_ForceLocal = 0x813E,
  NiFpga_MainFPGA_9159_ControlBool_ResetReader_A = 0x81B2,
  NiFpga_MainFPGA_9159_ControlBool_ResetReader_B = 0x817A,
  NiFpga_MainFPGA_9159_ControlBool_ResetReader_C = 0x8152,
  NiFpga_MainFPGA_9159_ControlBool_Stop = 0x81BA,
} NiFpga_MainFPGA_9159_ControlBool;

typedef enum
{
  NiFpga_MainFPGA_9159_ControlU16_FrameStrobePerioduSec = 0x81C2,
  NiFpga_MainFPGA_9159_ControlU16_HighPhaseFrameStrobeDurationTicks = 0x81C6,
} NiFpga_MainFPGA_9159_ControlU16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorArrayBool_outputarray = 0x8112,
} NiFpga_MainFPGA_9159_IndicatorArrayBool;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorArrayBoolSize_outputarray = 3,
} NiFpga_MainFPGA_9159_IndicatorArrayBoolSize;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorArrayI16_AI_A = 0x8134,
  NiFpga_MainFPGA_9159_IndicatorArrayI16_AI_B = 0x8128,
  NiFpga_MainFPGA_9159_IndicatorArrayI16_AI_C = 0x8120,
} NiFpga_MainFPGA_9159_IndicatorArrayI16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI_A = 18,
  NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI_B = 18,
  NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI_C = 18,
} NiFpga_MainFPGA_9159_IndicatorArrayI16Size;

typedef enum
{
  NiFpga_MainFPGA_9159_ControlArrayI32_IndexArray = 0x811C,
} NiFpga_MainFPGA_9159_ControlArrayI32;

typedef enum
{
  NiFpga_MainFPGA_9159_ControlArrayI32Size_IndexArray = 16,
} NiFpga_MainFPGA_9159_ControlArrayI32Size;

typedef enum
{
  NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A = 2,
  NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B = 1,
  NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C = 0,
} NiFpga_MainFPGA_9159_TargetToHostFifoI16;

//===================================================================
typedef enum
{
  NiFpga_MainFPGA_9159Simulator_ControlI32_SinSamp = 0x8118,
} NiFpga_MainFPGA_9159Simulator_ControlI32;

typedef enum
{
  NiFpga_MainFPGA_9159Simulator_ControlSgl_frequency = 0x8110,
} NiFpga_MainFPGA_9159Simulator_ControlSgl;

#endif
