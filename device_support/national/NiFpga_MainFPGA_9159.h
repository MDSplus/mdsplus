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
#define NiFpga_MainFPGA_9159_Bitfile "/opt/mdsplus/device_support/national/NiFpga_MainFPGA_9159.lvbitx"


/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_MainFPGA_9159_Signature = "EAB8E96CABC7B0809FBBCB198A3D7DF1";

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorBool_DeepSW0 = 0x811E,
   NiFpga_MainFPGA_9159_IndicatorBool_DeepSW1 = 0x811A,
   NiFpga_MainFPGA_9159_IndicatorBool_DeepSW2 = 0x8116,
   NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI0 = 0x8132,
   NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI1 = 0x812E,
   NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI2 = 0x812A,
   NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI3 = 0x8126,
   NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI4 = 0x8122,
} NiFpga_MainFPGA_9159_IndicatorBool;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorU8_SelectSet = 0x8112,
   NiFpga_MainFPGA_9159_IndicatorU8_SelectedSetMonitorAO = 0x813E,
} NiFpga_MainFPGA_9159_IndicatorU8;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_A = 0x81C2,
   NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_B = 0x81B2,
   NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_C = 0x818A,
} NiFpga_MainFPGA_9159_IndicatorI16;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorU16_PeriodToAOus = 0x8142,
   NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_A = 0x81C6,
   NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_B = 0x81B6,
   NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_C = 0x818E,
   NiFpga_MainFPGA_9159_IndicatorU16_Ver = 0x8146,
} NiFpga_MainFPGA_9159_IndicatorU16;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo1 = 0x81E8,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo2_A = 0x81CC,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo2_B = 0x8194,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo2_C = 0x816C,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo3_A = 0x81BC,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo3_B = 0x81AC,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo3_C = 0x8184,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo4 = 0x8164,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_AO = 0x8148,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_Diag = 0x8134,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_To_AO_A = 0x815C,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_To_AO_B = 0x8158,
   NiFpga_MainFPGA_9159_IndicatorI32_Ciclo_To_AO_C = 0x8150,
   NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_A = 0x81E0,
   NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_B = 0x81A8,
   NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_C = 0x8180,
} NiFpga_MainFPGA_9159_IndicatorI32;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorU32_Period2t_A = 0x81D0,
   NiFpga_MainFPGA_9159_IndicatorU32_Period2t_B = 0x8198,
   NiFpga_MainFPGA_9159_IndicatorU32_Period2t_C = 0x8170,
   NiFpga_MainFPGA_9159_IndicatorU32_Period3t_A = 0x81D8,
   NiFpga_MainFPGA_9159_IndicatorU32_Period3t_B = 0x81A0,
   NiFpga_MainFPGA_9159_IndicatorU32_Period3t_C = 0x8178,
   NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_A = 0x81D4,
   NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_B = 0x819C,
   NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_C = 0x8174,
   NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_A = 0x81C8,
   NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_B = 0x81B8,
   NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_C = 0x8190,
} NiFpga_MainFPGA_9159_IndicatorU32;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorU64_ChannelLiving = 0x8138,
} NiFpga_MainFPGA_9159_IndicatorU64;

typedef enum
{
   NiFpga_MainFPGA_9159_ControlBool_ForceLocal = 0x816A,
   NiFpga_MainFPGA_9159_ControlBool_ResetReader_A = 0x81DE,
   NiFpga_MainFPGA_9159_ControlBool_ResetReader_B = 0x81A6,
   NiFpga_MainFPGA_9159_ControlBool_ResetReader_C = 0x817E,
   NiFpga_MainFPGA_9159_ControlBool_Start = 0x810E,
   NiFpga_MainFPGA_9159_ControlBool_Stop = 0x81E6,
} NiFpga_MainFPGA_9159_ControlBool;

typedef enum
{
   NiFpga_MainFPGA_9159_ControlU16_FrameStrobePerioduSec = 0x81EE,
   NiFpga_MainFPGA_9159_ControlU16_HighPhaseFrameStrobeDurationTicks = 0x81F2,
} NiFpga_MainFPGA_9159_ControlU16;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorArrayI16_AI_A = 0x8160,
   NiFpga_MainFPGA_9159_IndicatorArrayI16_AI_B = 0x8154,
   NiFpga_MainFPGA_9159_IndicatorArrayI16_AI_C = 0x814C,
} NiFpga_MainFPGA_9159_IndicatorArrayI16;

typedef enum
{
   NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI_A = 18,
   NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI_B = 18,
   NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI_C = 18,
} NiFpga_MainFPGA_9159_IndicatorArrayI16Size;

typedef enum
{
   NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_A = 2,
   NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_B = 1,
   NiFpga_MainFPGA_9159_TargetToHostFifoI16_FIFOTOPC_C = 0,
} NiFpga_MainFPGA_9159_TargetToHostFifoI16;

#endif
