/*
 * VME PIO driver
 */
#include <io/dec/vme/vbareg.h>
/* vmp_data structure definition to pass into ioctl for function
 * VMP_MAP_PIO_ADDR and VMP_GET_PIO_INFO.
 */

struct pio_info
{
  unsigned int pio_addr;   /* VME address to map outbound to */
  unsigned int pio_size;   /* Size to map outbound to */
  unsigned int pio_am;     /* Address modifier for pio_addr */
  unsigned int pio_access; /* Access type */
  unsigned int intcount;
};

struct vmpioctl
{
  unsigned int vmp_addr;   /* VMEbus address  */
  unsigned int vmp_size;   /* Size VMEbus Window */
  unsigned int vmp_am;     /* Address modifier */
  char *vmp_buf;           /* Start of mem.  buffer to be mapped to VME =
                            */
  unsigned int vmp_return; /* Return value */
};

struct vmpintr_info
{
  int priority; /* Interrupt priority */
  int vector;   /* Interrupt vector */
};

/* ioctl defines
 * VMP_MAP_PIO_ADDR maps outbound to a VME address.
 * VMP_UNMAP_PIO_ADDR unmaps the outbound mapping.
 * VMP_GET_PIO_INFO returns information about the outbound mapping.
 */
#define DMA 2
#define NON_DMA 1
#define VMP_SET_HIGHSPEED_DMA _IO(5, 2)
#define VMP_UNSET_HIGHSPEED_DMA _IO(5, 3)
#define VMP_MAP_PIO_ADDR _IOW(5, 4, struct pio_info)
#define VMP_UNMAP_PIO_ADDR _IO(5, 5)
#define VMP_GET_PIO_INFO _IOR(5, 6, struct pio_info)
#define VMP_INTR_TEST _IO(5, 7)
#define VMP_DMA_IN _IOWR(5, 8, struct vmpioctl)
#define VMP_DMA_OUT _IOWR(5, 9, struct vmpioctl)
#define VMP_SET_PROC_INFO _IOW(5, 10, int)
#define VMP_UNSET_PROC_INFO _IO(5, 11)
#define VMP_GET_SIGNAL _IO(5, 12)
#define VMP_ADD_INTR _IOW(5, 13, struct vmpintr_info)
#define VMP_DEL_INTR _IO(5, 14)
#define VMP_CLR_INTCNT _IO(5, 15)
