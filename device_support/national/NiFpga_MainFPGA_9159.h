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
#define NiFpga_MainFPGA_9159_Bitfile "/opt/mdsplus/device_support/national/NiFpga_MainFPGA_9159.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char *const NiFpga_MainFPGA_9159_Signature = "F0FD95F8497540327573383BFD4B28CA";

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorBool_DipSW0 = 0x8136,
  NiFpga_MainFPGA_9159_IndicatorBool_DipSW1 = 0x8132,
  NiFpga_MainFPGA_9159_IndicatorBool_DipSW2 = 0x812E,
  NiFpga_MainFPGA_9159_IndicatorBool_ENABLEDMA = 0x8112,
  NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI0 = 0x814A,
  NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI1 = 0x8146,
  NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI2 = 0x8142,
  NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI3 = 0x813E,
  NiFpga_MainFPGA_9159_IndicatorBool_Mod14DI4 = 0x813A,
} NiFpga_MainFPGA_9159_IndicatorBool;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorU8_SelectSet = 0x812A,
  NiFpga_MainFPGA_9159_IndicatorU8_SelectedSetMonitorAO = 0x8152,
} NiFpga_MainFPGA_9159_IndicatorU8;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_A = 0x81AE,
  NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_B = 0x819E,
  NiFpga_MainFPGA_9159_IndicatorI16_TransmittedNum_C = 0x817E,
} NiFpga_MainFPGA_9159_IndicatorI16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorU16_AOPeriodus = 0x8156,
  NiFpga_MainFPGA_9159_IndicatorU16_Master_STATUS = 0x811A,
  NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_A = 0x81B2,
  NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_B = 0x81A2,
  NiFpga_MainFPGA_9159_IndicatorU16_ToRT_Periodus_C = 0x8182,
  NiFpga_MainFPGA_9159_IndicatorU16_Ver = 0x815A,
} NiFpga_MainFPGA_9159_IndicatorU16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorI32_ACQ_TriggerSignalCycle = 0x81CC,
  NiFpga_MainFPGA_9159_IndicatorI32_AI_Cycle = 0x8160,
  NiFpga_MainFPGA_9159_IndicatorI32_AO_Cycle = 0x815C,
  NiFpga_MainFPGA_9159_IndicatorI32_FIFO_Write_A = 0x81A8,
  NiFpga_MainFPGA_9159_IndicatorI32_FIFO_Write_B = 0x8198,
  NiFpga_MainFPGA_9159_IndicatorI32_FIFO_Write_C = 0x8178,
  NiFpga_MainFPGA_9159_IndicatorI32_READERSLAVE_A = 0x81B8,
  NiFpga_MainFPGA_9159_IndicatorI32_READERSLAVE_B = 0x8188,
  NiFpga_MainFPGA_9159_IndicatorI32_READERSLAVE_C = 0x8168,
  NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_A = 0x81C4,
  NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_B = 0x8194,
  NiFpga_MainFPGA_9159_IndicatorI32_RoundsToGetTrigger_C = 0x8174,
} NiFpga_MainFPGA_9159_IndicatorI32;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorU32_Period2t_A = 0x81BC,
  NiFpga_MainFPGA_9159_IndicatorU32_Period2t_B = 0x818C,
  NiFpga_MainFPGA_9159_IndicatorU32_Period2t_C = 0x816C,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved1_C = 0x8170,
  NiFpga_MainFPGA_9159_IndicatorU32_Receaved2_C = 0x8184,
  NiFpga_MainFPGA_9159_IndicatorU32_Receied2_A = 0x81B4,
  NiFpga_MainFPGA_9159_IndicatorU32_Received1_A = 0x81C0,
  NiFpga_MainFPGA_9159_IndicatorU32_Received1_B = 0x8190,
  NiFpga_MainFPGA_9159_IndicatorU32_Received2_B = 0x81A4,
} NiFpga_MainFPGA_9159_IndicatorU32;

typedef enum
{
  NiFpga_MainFPGA_9159_ControlBool_DEBUG_PLC_START = 0x8116,
  NiFpga_MainFPGA_9159_ControlBool_DEBUG_PLC_STOP = 0x810E,
  NiFpga_MainFPGA_9159_ControlBool_Internal_ACQ_Trigger = 0x8166,
  NiFpga_MainFPGA_9159_ControlBool_START_DMA = 0x8122,
  NiFpga_MainFPGA_9159_ControlBool_STOP_DMA = 0x811E,
  NiFpga_MainFPGA_9159_ControlBool_Start = 0x814E,
  NiFpga_MainFPGA_9159_ControlBool_Stop = 0x81CA,
} NiFpga_MainFPGA_9159_ControlBool;

typedef enum
{
  NiFpga_MainFPGA_9159_ControlU16_ACQ_Trigger_HighDurationTicks = 0x81D6,
  NiFpga_MainFPGA_9159_ControlU16_ACQ_Trigger_PerioduSec = 0x81D2,
} NiFpga_MainFPGA_9159_ControlU16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorArrayI16_AI = 0x8124,
} NiFpga_MainFPGA_9159_IndicatorArrayI16;

typedef enum
{
  NiFpga_MainFPGA_9159_IndicatorArrayI16Size_AI = 32,
} NiFpga_MainFPGA_9159_IndicatorArrayI16Size;

typedef enum
{
  NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_A = 2,
  NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_B = 1,
  NiFpga_MainFPGA_9159_TargetToHostFifoU64_FIFOTOPC_C = 0,
} NiFpga_MainFPGA_9159_TargetToHostFifoU64;

#endif
