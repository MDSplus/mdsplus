/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1998    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/*
 * @(#)$RCSfile$ $Revision$ (DEC) $Date$
 */

#include <sys/ioctl.h>

/***************************************************
 * dmaexreg.h   Device Register Header File for    *
 * dmaex.c 09-July-1996                            *
 *                                                 *
 * Device register offsets for DMAEX device        *
 ***************************************************/

#define DMAEX_CSR_OFF 0   /* One byte control/status register */
#define DMAEX_COUNT_OFF 1 /* Short byte count */
#define DMAEX_ADDR_OFF 4  /* 32-bit VMEbus transfer address */

/****************************************************
 * Convenience defines for calls to sizeof operator *
 ****************************************************/

#define DMAEX_CSR_SIZE sizeof(char)
#define DMAEX_COUNT_SIZE sizeof(short)
#define DMAEX_ADDR_SIZE sizeof(vme_addr_t)

/***************************************************
 * Bits for csr device offset register             *
 ***************************************************/

#define IE 0001     /* Interrupt Enable */
#define DMA_GO 0002 /* Start DMA */
#define RESET 0010  /* Ready for data transfer */
#define ERROR 0020  /* Indicate error */
#define READ 0040   /* Indicate data transfer is read */

/***************************************************
 *         Define the ioctl macros                 *
 *                                                 *
 * This instructs the kernel on how much data will *
 * be moved between the device driver and the user *
 * program as a result of a call to the ioctl      *
 * interface.  Note they are all read/write in     *
 * this case, meaning that the dmaex_ioctl         *
 * structure parameter can be written into by the  *
 * user code and by the driver itself.             *
 ***************************************************/

struct dmaex_ioctl_data
{
  unsigned long data[6];
};

/*
 * PHYS_CONTIG_BUF_SIZE defines the number of bytes
 * of physically contiguous memory that will be allocated
 * by the the driver. This number MUST match the value
 * specified in the CMA_Option of the sysconfigtab file
 * fragment.
 */
#define PHYS_CONTIG_BUF_SIZE (128 * 1024)
#define CONTIG_RD_WRT_BUF_SIZE (PHYS_CONTIG_BUF_SIZE / 2)

/*
 * The following definitions define the data transfer
 * mode to be used by dmaex_read and dmaex_write. These
 * interfaces calls the physio interface which then invokes
 * dmaex_minphys and dmaex_strategy interfaces. The
 * dmaex_strategy interface then performs the data transfer
 * using the specified transfer mode.
 */
#define PIO_XFER_MODE 0
#define DEVICE_DMA_MODE 1
#define BLOCK_DMA_MODE 2

/*
 * The following definitions define how dmaex_mmap will
 * interpert the requested mmap call.
 *   MMAP_VME_TO_U_MEM   - This mode causes the dmaex_mmap interface to
 *                         return a kernel page frame number corresponding
 *                         to an outbound mapped VMEbus address.
 *   MMAP_K_TO_U_MEM_WRT - This mode causes the dmaex_mmap interface to
 *                         return a kernel page frame number corresponding
 *                         to the physically contiguous write buffer
 *                         allocated by contig_malloc and the cma_dd interface.
 *   MMAP_K_TO_U_MEM_RD  - This mode causes the dmaex_mmap interface to
 *                         return a kernel page frame number corresponding
 *                         to the physically contiguous read buffer
 *                         allocated by contig_malloc and the cma_dd interface.
 */
#define MMAP_VME_TO_U_MEM 0
#define MMAP_K_TO_U_MEM_WRT 1
#define MMAP_K_TO_U_MEM_RD 2

enum dmaex_commands
{
  C1,
  C2,
  C3,
  C4,
  C5,
  C6,
  C7,
  C8,
  C9,
  C10,
  C11,
  C12,
  C13,
  C14,
  C15,
  C16,
  C17,
  C18,
  C19,
  C20,
  C21,
  C22,
  C23,
  C24,
  C25,
  C26,
  C27,
  C28
};

#define SET_MMAP_MODE _IOWR('t', C1, struct dmaex_ioctl_data)
#define GET_MMAP_MODE _IOWR('t', C2, struct dmaex_ioctl_data)
#define SET_STRATEGY_XFER_MODE _IOWR('t', C3, struct dmaex_ioctl_data)
#define GET_STRATEGY_XFER_MODE _IOWR('t', C4, struct dmaex_ioctl_data)
#define SETUP_VME_FOR_MMAP_PIO _IOWR('t', C5, struct dmaex_ioctl_data)
#define GET_VME_INFO_FOR_MMAP_PIO _IOWR('t', C6, struct dmaex_ioctl_data)
#define UNMAP_VME_FOR_MMAP_PIO _IOWR('t', C7, struct dmaex_ioctl_data)
#define SET_STRATEGY_INFO_BLK_DMA _IOWR('t', C8, struct dmaex_ioctl_data)
#define GET_STRATEGY_INFO_BLK_DMA _IOWR('t', C9, struct dmaex_ioctl_data)
#define CLR_STRATEGY_INFO_BLK_DMA _IOWR('t', C10, struct dmaex_ioctl_data)
#define SETUP_VME_FOR_STRATEGY_PIO _IOWR('t', C11, struct dmaex_ioctl_data)
#define GET_VME_INFO_FOR_STRATEGY_PIO _IOWR('t', C12, struct dmaex_ioctl_data)
#define UNMAP_VME_FOR_STRATEGY_PIO _IOWR('t', C13, struct dmaex_ioctl_data)
#define MAP_SYS_MEM_TO_VME _IOWR('t', C14, struct dmaex_ioctl_data)
#define GET_SYS_MEM_INFO _IOWR('t', C15, struct dmaex_ioctl_data)
#define UNMAP_SYS_MEM_TO_VME _IOWR('t', C16, struct dmaex_ioctl_data)
#define SETUP_DMA_BLK_WRT _IOWR('t', C17, struct dmaex_ioctl_data)
#define GET_DMA_BLK_WRT _IOWR('t', C18, struct dmaex_ioctl_data)
#define DO_DMA_BLK_WRT _IOWR('t', C19, struct dmaex_ioctl_data)
#define CLR_DMA_BLK_WRT _IOWR('t', C20, struct dmaex_ioctl_data)
#define SETUP_DMA_BLK_RD _IOWR('t', C21, struct dmaex_ioctl_data)
#define GET_DMA_BLK_RD _IOWR('t', C22, struct dmaex_ioctl_data)
#define DO_DMA_BLK_RD _IOWR('t', C23, struct dmaex_ioctl_data)
#define CLR_DMA_BLK_RD _IOWR('t', C24, struct dmaex_ioctl_data)
#define SET_INT_HANDLER _IOWR('t', C25, struct dmaex_ioctl_data)
#define CLR_INT_HANDLER _IOWR('t', C26, struct dmaex_ioctl_data)
#define VME_POST_IRQ _IOWR('t', C27, struct dmaex_ioctl_data)
#define VME_CLR_IRQ _IOWR('t', C28, struct dmaex_ioctl_data)
