#ifndef RFX_DAC_H
#define RFX_DAC_H


#include <linux/types.h>
#include <asm/ioctl.h>



#ifdef __cplusplus
extern "C" {
#endif

//Temporaneo
#define DMA_SOURCE 1
////////////////


#define DEVICE_NAME "rfx_dac"  /* Dev name as it appears in /proc/devices */
#define MODULE_NAME "rfx_dac"

//Generic IOCTL commands  

#define RFX_DAC_IOCTL_BASE	'W'
#define RFX_DAC_ARM_DMA    			_IO(RFX_DAC_IOCTL_BASE, 1)
#define RFX_DAC_START_DMA     		_IO(RFX_DAC_IOCTL_BASE, 2)
#define RFX_DAC_STOP_DMA     			_IO(RFX_DAC_IOCTL_BASE, 3)
#define RFX_DAC_SET_DMA_BUFLEN     		_IO(RFX_DAC_IOCTL_BASE, 4)
#define RFX_DAC_GET_DMA_BUFLEN     		_IO(RFX_DAC_IOCTL_BASE, 5)
#define RFX_DAC_IS_DMA_RUNNING     		_IO(RFX_DAC_IOCTL_BASE, 6)
#define RFX_DAC_GET_DMA_DATA     		_IO(RFX_DAC_IOCTL_BASE, 7)
#define RFX_DAC_SET_DRIVER_BUFLEN     	_IO(RFX_DAC_IOCTL_BASE, 8)
#define RFX_DAC_GET_DRIVER_BUFLEN     	_IO(RFX_DAC_IOCTL_BASE, 9)
#define RFX_DAC_GET_REGISTERS			_IO(RFX_DAC_IOCTL_BASE, 10)
#define RFX_DAC_SET_REGISTERS			_IO(RFX_DAC_IOCTL_BASE, 11)
#define RFX_DAC_FIFO_INT_HALF_SIZE		_IO(RFX_DAC_IOCTL_BASE, 12)
#define RFX_DAC_FIFO_INT_FIRST_SAMPLE		_IO(RFX_DAC_IOCTL_BASE, 13)
#define RFX_DAC_FIFO_FLUSH			_IO(RFX_DAC_IOCTL_BASE, 14)
#define RFX_DAC_START_READ			_IO(RFX_DAC_IOCTL_BASE, 15)
#define RFX_DAC_STOP_READ			_IO(RFX_DAC_IOCTL_BASE, 16)
#define RFX_DAC_GET_AXI_CFG_REGISTER_0		_IO(RFX_DAC_IOCTL_BASE, 20)
#define RFX_DAC_SET_AXI_CFG_REGISTER_0		_IO(RFX_DAC_IOCTL_BASE, 21)
#define RFX_DAC_GET_CLOCK_DIVIDE_REG		_IO(RFX_DAC_IOCTL_BASE, 22)
#define RFX_DAC_SET_CLOCK_DIVIDE_REG		_IO(RFX_DAC_IOCTL_BASE, 23)
#define RFX_DAC_GET_CMD_REG		_IO(RFX_DAC_IOCTL_BASE, 24)
#define RFX_DAC_SET_CMD_REG		_IO(RFX_DAC_IOCTL_BASE, 25)
#define RFX_DAC_GET_MODE_REG		_IO(RFX_DAC_IOCTL_BASE, 26)
#define RFX_DAC_SET_MODE_REG		_IO(RFX_DAC_IOCTL_BASE, 27)
#define RFX_DAC_GET_AXI_FIFO_MM_S_0_LEN		_IO(RFX_DAC_IOCTL_BASE, 28)
#define RFX_DAC_GET_AXI_FIFO_MM_S_0_VAL		_IO(RFX_DAC_IOCTL_BASE, 29)
#define RFX_DAC_SET_AXI_FIFO_MM_S_0_VAL		_IO(RFX_DAC_IOCTL_BASE, 30)
#define RFX_DAC_CLEAR_AXI_FIFO_MM_S_0		_IO(RFX_DAC_IOCTL_BASE, 31)
  

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

struct rfx_dac_registers
{
    	char axi_cfg_register_0_enable;
	unsigned int axi_cfg_register_0;
	char clock_divide_reg_enable;
	unsigned int clock_divide_reg;
	char cmd_reg_enable;
	unsigned int cmd_reg;
	char mode_reg_enable;
	unsigned int mode_reg;

};



#ifdef __cplusplus
}
#endif

#endif // RFX_DAC_H
