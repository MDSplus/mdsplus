#ifndef RFX_STREAM_H
#define RFX_STREAM_H


#include <linux/types.h>
#include <asm/ioctl.h>



#ifdef __cplusplus
extern "C" {
#endif

//Temporaneo
#define DMA_SOURCE 1
////////////////


#define DEVICE_NAME "rfx_stream"  /* Dev name as it appears in /proc/devices */
#define MODULE_NAME "rfx_stream"

//Generic IOCTL commands  

#define RFX_STREAM_IOCTL_BASE	'W'
#define RFX_STREAM_ARM_DMA    			_IO(RFX_STREAM_IOCTL_BASE, 1)
#define RFX_STREAM_START_DMA     		_IO(RFX_STREAM_IOCTL_BASE, 2)
#define RFX_STREAM_STOP_DMA     			_IO(RFX_STREAM_IOCTL_BASE, 3)
#define RFX_STREAM_SET_DMA_BUFLEN     		_IO(RFX_STREAM_IOCTL_BASE, 4)
#define RFX_STREAM_GET_DMA_BUFLEN     		_IO(RFX_STREAM_IOCTL_BASE, 5)
#define RFX_STREAM_IS_DMA_RUNNING     		_IO(RFX_STREAM_IOCTL_BASE, 6)
#define RFX_STREAM_GET_DMA_DATA     		_IO(RFX_STREAM_IOCTL_BASE, 7)
#define RFX_STREAM_SET_DRIVER_BUFLEN     	_IO(RFX_STREAM_IOCTL_BASE, 8)
#define RFX_STREAM_GET_DRIVER_BUFLEN     	_IO(RFX_STREAM_IOCTL_BASE, 9)
#define RFX_STREAM_GET_REGISTERS			_IO(RFX_STREAM_IOCTL_BASE, 10)
#define RFX_STREAM_SET_REGISTERS			_IO(RFX_STREAM_IOCTL_BASE, 11)
#define RFX_STREAM_FIFO_INT_HALF_SIZE		_IO(RFX_STREAM_IOCTL_BASE, 12)
#define RFX_STREAM_FIFO_INT_FIRST_SAMPLE		_IO(RFX_STREAM_IOCTL_BASE, 13)
#define RFX_STREAM_FIFO_FLUSH			_IO(RFX_STREAM_IOCTL_BASE, 14)
#define RFX_STREAM_START_READ			_IO(RFX_STREAM_IOCTL_BASE, 15)
#define RFX_STREAM_STOP_READ			_IO(RFX_STREAM_IOCTL_BASE, 16)
#define RFX_STREAM_GET_CMD_REG		_IO(RFX_STREAM_IOCTL_BASE, 20)
#define RFX_STREAM_SET_CMD_REG		_IO(RFX_STREAM_IOCTL_BASE, 21)
#define RFX_STREAM_GET_COMMAND_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 22)
#define RFX_STREAM_SET_COMMAND_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 23)
#define RFX_STREAM_GET_DEADTIME_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 24)
#define RFX_STREAM_SET_DEADTIME_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 25)
#define RFX_STREAM_GET_DECIMATOR_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 26)
#define RFX_STREAM_SET_DECIMATOR_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 27)
#define RFX_STREAM_GET_K1_REG		_IO(RFX_STREAM_IOCTL_BASE, 28)
#define RFX_STREAM_SET_K1_REG		_IO(RFX_STREAM_IOCTL_BASE, 29)
#define RFX_STREAM_GET_K2_REG		_IO(RFX_STREAM_IOCTL_BASE, 30)
#define RFX_STREAM_SET_K2_REG		_IO(RFX_STREAM_IOCTL_BASE, 31)
#define RFX_STREAM_GET_LEV_TRIG_COUNT		_IO(RFX_STREAM_IOCTL_BASE, 32)
#define RFX_STREAM_SET_LEV_TRIG_COUNT		_IO(RFX_STREAM_IOCTL_BASE, 33)
#define RFX_STREAM_GET_MODE_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 34)
#define RFX_STREAM_SET_MODE_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 35)
#define RFX_STREAM_GET_OFFSET_REG		_IO(RFX_STREAM_IOCTL_BASE, 36)
#define RFX_STREAM_SET_OFFSET_REG		_IO(RFX_STREAM_IOCTL_BASE, 37)
#define RFX_STREAM_GET_PACKETIZER		_IO(RFX_STREAM_IOCTL_BASE, 38)
#define RFX_STREAM_SET_PACKETIZER		_IO(RFX_STREAM_IOCTL_BASE, 39)
#define RFX_STREAM_GET_POST_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 40)
#define RFX_STREAM_SET_POST_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 41)
#define RFX_STREAM_GET_PRE_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 42)
#define RFX_STREAM_SET_PRE_REGISTER		_IO(RFX_STREAM_IOCTL_BASE, 43)
#define RFX_STREAM_GET_STEP_HI_REG		_IO(RFX_STREAM_IOCTL_BASE, 44)
#define RFX_STREAM_SET_STEP_HI_REG		_IO(RFX_STREAM_IOCTL_BASE, 45)
#define RFX_STREAM_GET_STEP_LO_REG		_IO(RFX_STREAM_IOCTL_BASE, 46)
#define RFX_STREAM_SET_STEP_LO_REG		_IO(RFX_STREAM_IOCTL_BASE, 47)
#define RFX_STREAM_GET_TIME_OFS_HI_REG		_IO(RFX_STREAM_IOCTL_BASE, 48)
#define RFX_STREAM_SET_TIME_OFS_HI_REG		_IO(RFX_STREAM_IOCTL_BASE, 49)
#define RFX_STREAM_GET_TIME_OFS_LO_REG		_IO(RFX_STREAM_IOCTL_BASE, 50)
#define RFX_STREAM_SET_TIME_OFS_LO_REG		_IO(RFX_STREAM_IOCTL_BASE, 51)
#define RFX_STREAM_GET_DATA_FIFO_LEN		_IO(RFX_STREAM_IOCTL_BASE, 52)
#define RFX_STREAM_GET_DATA_FIFO_VAL		_IO(RFX_STREAM_IOCTL_BASE, 53)
#define RFX_STREAM_SET_DATA_FIFO_VAL		_IO(RFX_STREAM_IOCTL_BASE, 54)
#define RFX_STREAM_CLEAR_DATA_FIFO		_IO(RFX_STREAM_IOCTL_BASE, 55)
#define RFX_STREAM_GET_TIME_FIFO_LEN		_IO(RFX_STREAM_IOCTL_BASE, 56)
#define RFX_STREAM_GET_TIME_FIFO_VAL		_IO(RFX_STREAM_IOCTL_BASE, 57)
#define RFX_STREAM_SET_TIME_FIFO_VAL		_IO(RFX_STREAM_IOCTL_BASE, 58)
#define RFX_STREAM_CLEAR_TIME_FIFO		_IO(RFX_STREAM_IOCTL_BASE, 59)
#define RFX_STREAM_GET_TIMES_FIFO_LEN		_IO(RFX_STREAM_IOCTL_BASE, 60)
#define RFX_STREAM_GET_TIMES_FIFO_VAL		_IO(RFX_STREAM_IOCTL_BASE, 61)
#define RFX_STREAM_SET_TIMES_FIFO_VAL		_IO(RFX_STREAM_IOCTL_BASE, 62)
#define RFX_STREAM_CLEAR_TIMES_FIFO		_IO(RFX_STREAM_IOCTL_BASE, 63)
  

#ifndef AXI_ENUMS_DEFINED
#define AXI_ENUMS_DEFINED
enum AxiStreamFifo_Register {
    ISR   = 0x00,   ///< Interrupt Status Register (ISR)
    IER   = 0x04,   ///< Interrupt Enable Register (IER)
    TDFR  = 0x08,   ///< Transmit Data FIFO Reset (TDFR)
    TDFV  = 0x0c,   ///< Transmit Data FIFO Vacancy (TDFV)
    TDFD  = 0x10,   ///< Transmit Data FIFO 32-bit Wide Data Write Port
    TDFD4 = 0x1000, ///< Transmit Data FIFO for AXI4 Data Write Port
    TLR   = 0x14,   ///< Transmit Length Register (TLR)
    RDFR  = 0x18,   ///< Receive Data FIFO reset (RDFR)
    RDFO  = 0x1c,   ///< Receive Data FIFO Occupancy (RDFO)
    RDFD  = 0x20,   ///< Receive Data FIFO 32-bit Wide Data Read Port (RDFD)
    RDFD4 = 0x1000, ///< Receive Data FIFO for AXI4 Data Read Port (RDFD)
    RLR   = 0x24,   ///< Receive Length Register (RLR)
    SRR   = 0x28,   ///< AXI4-Stream Reset (SRR)
    TDR   = 0x2c,   ///< Transmit Destination Register (TDR)
    RDR   = 0x30,   ///< Receive Destination Register (RDR)
    /// not supported yet .. ///
    TID   = 0x34,   ///< Transmit ID Register
    TUSER = 0x38,   ///< Transmit USER Register
    RID   = 0x3c,   ///< Receive ID Register
    RUSER = 0x40    ///< Receive USER Register
};

enum AxiStreamFifo_ISREnum {
    ISR_RFPE = 1 << 19,  ///< Receive FIFO Programmable Empty
    ISR_RFPF = 1 << 20,  ///< Receive FIFO Programmable Full
    ISR_TFPE = 1 << 21,  ///< Transmit FIFO Programmable Empty
    ISR_TFPF = 1 << 22,  ///< Transmit FIFO Programmable Full
    ISR_RRC = 1 << 23,   ///< Receive Reset Complete
    ISR_TRC = 1 << 24,   ///< Transmit Reset Complete
    ISR_TSE = 1 << 25,   ///< Transmit Size Error
    ISR_RC = 1 << 26,    ///< Receive Complete
    ISR_TC = 1 << 27,    ///< Transmit Complete
    ISR_TPOE = 1 << 28,  ///< Transmit Packet Overrun Error
    ISR_RPUE = 1 << 29,  ///< Receive Packet Underrun Error
    ISR_RPORE = 1 << 30, ///< Receive Packet Overrun Read Error
    ISR_RPURE = 1 << 31, ///< Receive Packet Underrun Read Error
};

enum RegisterIdx {
    FIFO_00_IDX = 0,
    FIFO_01_IDX = 1,
    FIFO_10_IDX = 2,
    FIFO11_IDX = 3,
    COMMAND_REG_IDX = 4,
    PRE_POST_REG_IDX = 5,
    DEC_REG_IDX = 6,
    MODE_REG_IDX = 8
};
#endif

#pragma pack(1)

struct rfx_stream_registers
{
    	char cmd_reg_enable;
	unsigned int cmd_reg;
	char command_register_enable;
	unsigned int command_register;
	char deadtime_register_enable;
	unsigned int deadtime_register;
	char decimator_register_enable;
	unsigned int decimator_register;
	char k1_reg_enable;
	unsigned int k1_reg;
	char k2_reg_enable;
	unsigned int k2_reg;
	char lev_trig_count_enable;
	unsigned int lev_trig_count;
	char mode_register_enable;
	unsigned int mode_register;
	char offset_reg_enable;
	unsigned int offset_reg;
	char packetizer_enable;
	unsigned int packetizer;
	char post_register_enable;
	unsigned int post_register;
	char pre_register_enable;
	unsigned int pre_register;
	char step_hi_reg_enable;
	unsigned int step_hi_reg;
	char step_lo_reg_enable;
	unsigned int step_lo_reg;
	char time_ofs_hi_reg_enable;
	unsigned int time_ofs_hi_reg;
	char time_ofs_lo_reg_enable;
	unsigned int time_ofs_lo_reg;

};



#ifdef __cplusplus
}
#endif

#endif // RFX_STREAM_H
