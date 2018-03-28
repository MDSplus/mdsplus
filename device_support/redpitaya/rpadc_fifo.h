#ifndef RPADC_FIFO_0_H
#define RPADC_FIFO_0_H


#include <linux/types.h>
#include <asm/ioctl.h>



#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME "rpadc_fifo"  /* Dev name as it appears in /proc/devices */
#define MODULE_NAME "rpadc_fifo"

#define RFX_RPADC_IOCTL_BASE	'W'
#define RFX_RPADC_RESOFFSET 			_IO(RFX_RPADC_IOCTL_BASE, 0)
#define RFX_RPADC_RESET     			_IO(RFX_RPADC_IOCTL_BASE, 1)
#define RFX_RPADC_CLEAR     			_IO(RFX_RPADC_IOCTL_BASE, 2)
#define RFX_RPADC_GETSR     			_IO(RFX_RPADC_IOCTL_BASE, 3)
#define RFX_RPADC_OVERFLOW  			_IO(RFX_RPADC_IOCTL_BASE, 4)
#define RFX_RPADC_FIFO_INT_HALF_SIZE  		_IO(RFX_RPADC_IOCTL_BASE, 5)
#define RFX_RPADC_FIFO_INT_FIRST_SAMPLE  	_IO(RFX_RPADC_IOCTL_BASE, 6)
#define RFX_RPADC_GET_CONFIG  			_IO(RFX_RPADC_IOCTL_BASE, 7)
#define RFX_RPADC_SET_CONFIG  			_IO(RFX_RPADC_IOCTL_BASE, 8)
#define RFX_RPADC_ARM  				_IO(RFX_RPADC_IOCTL_BASE, 9)
#define RFX_RPADC_TRIGGER  			_IO(RFX_RPADC_IOCTL_BASE, 10)
#define RFX_RPADC_STOP  			_IO(RFX_RPADC_IOCTL_BASE, 11)
#define RFX_RPADC_LAST_TIME  			_IO(RFX_RPADC_IOCTL_BASE, 12)
#define RFX_RPADC_SET_BUFSIZE  			_IO(RFX_RPADC_IOCTL_BASE, 13)
#define RFX_RPADC_GET_BUFSIZE  			_IO(RFX_RPADC_IOCTL_BASE, 14)
    
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


#pragma pack(1)
struct rfx_rpadc_fifo {
    int isr;   ///< interrupt status register
    int esr;   ///< interrupt status register
    int pad[0x40];
};

struct rfx_rpadc {
    struct rfx_rpadc *next,*prev;
    const char *name;
    struct rfx_rpadc_fifo fifo;
    int    fd;
    int prets;
    int posts;
};




enum rpadc_mode {
  STREAMING,
  EVENT_STREAMING
};


struct rpadc_configuration
{
    enum rpadc_mode mode;   		//STREAMING or EVENT_STREAMING
    unsigned short trig_threshold;		//Signal level for trigger
    char trig_above_threshold;  //If true, trigger when above threshold, below threshold otherwise
    char trig_from_chana;	//If true, the trigger is derived from channel A
    unsigned char trig_samples;          //Number of samples above/below threshol for validating trigger
    unsigned short pre_samples;		//Number of pre-trigger samples
    unsigned short post_samples;         //Number of post-trigger samples
    unsigned int decimation;		//Decimation factor (base frequency: 125MHz)
};





#ifndef __KERNEL__
// api functions here //

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>


/****** OLD STUFF
struct rfx_rpadc_fifo *rpadc_get_device(const char *dev_file ) {
    static struct rfx_rpadc_fifo *dev = NULL;
    int fd;
    if(!dev) {
        if(dev_file) fd = open(dev_file, O_RDWR | O_SYNC);
        else fd = open("/dev/"DEVICE_NAME, O_RDWR | O_SYNC);
        if(fd < 0) {
            printf(" ERROR: failed to open device file\n");
            return NULL;
        }
        dev = mmap(NULL, sizeof(struct rfx_rpadc_fifo), PROT_READ | PROT_WRITE, MAP_SHARED,fd,0);
    }

    if(!dev) {
        printf(" ERROR: failed to mmap device memory\n");
        return NULL;
    }
    return dev;
}

int rpadc_release_device() {
    struct rfx_rpadc_fifo *dev = rpadc_get_device(0);
    int status;
    if(dev) {
        status = munmap(dev, sizeof(struct rfx_rpadc_fifo));
        if(status == 0) dev = NULL;
    }
    return status;
}
*////////////////////////////////////

#endif // __KERNEL__




#ifdef __cplusplus
}
#endif

#endif // RPADC_FIFO_0_H
