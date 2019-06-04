#define HAS_FIFO_INTERRUPT

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include <linux/io.h>
#include <linux/ptrace.h>

#include <asm/uaccess.h>  // put_user
#include <asm/pgtable.h>

#include <linux/fs.h>

#include <linux/platform_device.h>

#include <linux/interrupt.h>
#include <linux/poll.h>

#include "rpadc_fifo_auto.h"

#include <linux/dmaengine.h>         // dma api
#include <linux/dma/xilinx_dma.h>   // axi dma driver


#include <asm/io.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/delay.h>

#define SUCCESS 0
#define FIFO_LEN 16384

#define DMA_STREAMING_SAMPLES 1024
//static struct platform_device *s_pdev = 0;
// static int s_device_open = 0;
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int device_mmap(struct file *filp, struct vm_area_struct *vma);
static loff_t memory_lseek(struct file *file, loff_t offset, int orig);
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static unsigned int device_poll(struct file *file, struct poll_table_struct *p);

static int deviceAllocated = 0;

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .mmap = device_mmap,
    .llseek = memory_lseek,
    .unlocked_ioctl = device_ioctl,
    .poll = device_poll,
};

#define BUFSIZE 65536

struct rpadc_fifo_auto_dev {
    struct platform_device *pdev;
    struct cdev cdev;
    int busy;
    int irq;
    
    	void * iomap_command_register;
	void * iomap_decimator_register;
	void * iomap_mode_register;
	void * iomap_packetizer;
	void * iomap_post_register;
	void * iomap_pre_register;
	void * iomap_data_fifo;
	void * iomap1_data_fifo;
	void * iomap_time_fifo;
	void * iomap1_time_fifo;

    
    struct semaphore sem;     /* mutual exclusion semaphore     */
    spinlock_t spinLock;     /* spinlock     */
    u32 *fifoBuffer;
    u32 bufSize;
    u32 rIdx, wIdx, bufCount;
    wait_queue_head_t readq;  /* read queue */
    int dma_started;
    int dma_cyclic;
    int started;
#ifdef HAS_DMA
    struct dma_chan *dma_chan;
    dma_addr_t dma_handle;
    dma_addr_t rx_dma_handle;
    char *dma_buffer;
    u32 dma_buf_size;
    dma_cookie_t dma_cookie;
    struct completion dma_cmp;
#endif
#ifdef HAS_FIFO_INTERRUPT
    int fifoHalfInterrupt;
    int fifoOverflow;
#endif

};

////////////////////////////////////////////////////////////////////////////////
//  FIFO IO  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void writeFifo(void *addr, enum AxiStreamFifo_Register op, u32 data ) {
    *(u32 *)(addr+op) = data;
}

u32 readFifo(void *addr, enum AxiStreamFifo_Register op ) {
    return *(u32 *)(addr+op);
}

static void clearFifo(void *dev, void *dev1)
{
   u32 occ, i;
   writeFifo(dev,ISR,0xFFFFFFFF);
   writeFifo(dev,RDFR,0xa5);
   writeFifo(dev,IER,0x04000000);
//   occ = readFifo(dev,RDFO); 
//   for(i = 0; i < occ; i++)
//       readFifo(dev,RDFD4);
}


#ifdef HAS_DMA
static dma_cookie_t dma_prep_buffer(struct rpadc_fifo_auto_dev *dev, struct dma_chan *chan, dma_addr_t buf, size_t len, 
					enum dma_transfer_direction dir, struct completion *cmp);
static void dma_start_transfer(struct dma_chan *chan, struct completion *cmp, dma_cookie_t cookie, int wait);
#endif //HAS_DMA

int writeBuf(struct rpadc_fifo_auto_dev *dev, u32 sample)
{
    spin_lock_irq(&dev->spinLock);
    if(dev->bufCount >= dev->bufSize)
    {
        printk(KERN_DEBUG "ADC FIFO BUFFER OVERFLOW!\n");
	spin_unlock_irq(&dev->spinLock);
	return -1;
    }
    else
    {
        dev->fifoBuffer[dev->wIdx] = sample;
        dev->wIdx = (dev->wIdx + 1) % dev->bufSize;
        dev->bufCount++;
    }
    spin_unlock_irq(&dev->spinLock);
    return 0;
} 

int writeBufSet(struct rpadc_fifo_auto_dev *dev, u32 *buf, int nSamples)
{
    int i;
    spin_lock_irq(&dev->spinLock);
    for(i = 0; i < nSamples; i++)
    {
      if(dev->bufCount >= dev->bufSize)
	{
	    printk(KERN_DEBUG "ADC FIFO BUFFER OVERFLOW  %d    %d!\n", dev->bufCount, dev->bufSize);
	    spin_unlock_irq(&dev->spinLock);
	    return -1;
	}
	else
	{
	    dev->fifoBuffer[dev->wIdx] = buf[i];
	    dev->wIdx = (dev->wIdx + 1) % dev->bufSize;
	    dev->bufCount++;
	}
    }
    spin_unlock_irq(&dev->spinLock);
    return 0;
} 

u32 readBuf(struct rpadc_fifo_auto_dev *dev)
{
    u32 data;
    spin_lock_irq(&dev->spinLock);
    if(dev->bufCount <= 0)
    {
        printk(KERN_DEBUG "ADC FIFO BUFFER UNDERFLOW!\n");  //Should never happen
        data = 0;
    }
    else
    {
        data = dev->fifoBuffer[dev->rIdx];
        dev->rIdx = (dev->rIdx+1) % dev->bufSize;
        dev->bufCount--;
    }
    spin_unlock_irq(&dev->spinLock);
    return data;
}




////////////////////////////////////////////////////////////////////////////////
//  DMA Management  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef HAS_DMA
static void dma_buffer2fifo_buffer(struct rpadc_fifo_auto_dev *dev)
{
    int i;
    int dmaSamples = dev->dma_buf_size/sizeof(u32);
    writeBufSet(dev, dev->dma_buffer, dmaSamples);
    wake_up(&dev->readq);
}

/* Handle a callback called when the DMA transfer is complete to another
 * thread of control
 */
static int dma_sync_callback(struct rpadc_fifo_auto_dev *dev)
{
    int status, i;
    dma_cookie_t lastCookie;
//    printk("DMA SYNC CALLBACK\n");
    status = dma_async_is_tx_complete(dev->dma_chan,  dev->dma_cookie, &lastCookie, NULL); 
    if (status != DMA_COMPLETE) {
			printk(KERN_ERR "DMA returned completion callback status of: %s\n",
			       status == DMA_ERROR ? "error" : "in progress"); }
    if(lastCookie != dev->dma_cookie)
    {
	printk("DMA NOT TERMINATED FOR THIS COOKIE %d  %d\n", lastCookie, dev->dma_cookie);
	dmaengine_terminate_all(dev->dma_chan);
//	return 0;
    }
//    else
    {
#ifdef DMA_SOURCE    
         dma_buffer2fifo_buffer(dev);
#endif
    }
    //Start a new DMA round if device still armed
//    if(!dev->dma_started)
    if(!dev->dma_cyclic)
        return 0;
    for(i = 0; i < dev->dma_buf_size/sizeof(u32); i++)
      ((u32 *)dev->dma_buffer)[i] = 0xffffffff;
 
    dev->dma_cookie = dma_prep_buffer(dev, dev->dma_chan, dev->dma_handle, dev->dma_buf_size, DMA_DEV_TO_MEM,  &dev->dma_cmp);
    if (dma_submit_error(dev->dma_cookie)) {
	printk(KERN_ERR "dma_prep_buffer error\n");
	return -EIO;
    }
//    printk(KERN_INFO "Starting NEW DMA transfers\n");
    dma_start_transfer(dev->dma_chan, &dev->dma_cmp, dev->dma_cookie, 0);
    return 0;
}

/* Prepare a DMA buffer to be used in a DMA transaction, submit it to the DMA engine 
 * to queued and return a cookie that can be used to track that status of the 
 * transaction
 */
static dma_cookie_t dma_prep_buffer(struct rpadc_fifo_auto_dev *dev, struct dma_chan *chan, dma_addr_t buf, size_t len, 
					enum dma_transfer_direction dir, struct completion *cmp) 
{
	enum dma_ctrl_flags flags = /*DMA_CTRL_ACK | */ DMA_PREP_INTERRUPT;
	struct dma_async_tx_descriptor *chan_desc;
	dma_cookie_t cookie;

	chan_desc = dmaengine_prep_slave_single(chan, buf, len, dir, flags);
	if (!chan_desc) {
		printk(KERN_ERR "dmaengine_prep_slave_single error\n");
		cookie = -EBUSY;
	} else {
		chan_desc->callback = dma_sync_callback;
		chan_desc->callback_param = dev;
//		printk("SUBMIT DMA \n");
		cookie = dmaengine_submit(chan_desc);	
//		printk("SUBMIT DMA  cookie: %x\n", cookie);
	}
	return cookie;
}

/* Start a DMA transfer that was previously submitted to the DMA engine and then
 * wait for it complete, timeout or have an error
 */
static void dma_start_transfer(struct dma_chan *chan, struct completion *cmp, 
					dma_cookie_t cookie, int wait)
{
	unsigned long timeout = msecs_to_jiffies(10000);
	enum dma_status status;

	init_completion(cmp);
	dma_async_issue_pending(chan);
    
	if (wait) 
	{
		timeout = wait_for_completion_timeout(cmp, timeout);
		status = dma_async_is_tx_complete(chan, cookie, NULL, NULL);
		if (timeout == 0)  
		{
		    printk(KERN_ERR "DMA timed out\n");
		} 
		else if (status != DMA_COMPLETE) 
		{
		    printk(KERN_ERR "DMA returned completion callback status of: %s\n",
		    status == DMA_ERROR ? "error" : "in progress");
		}
	}
}
#endif  //HAS_DMA

#ifdef HAS_FIFO_INTERRUPT
////////////////////////////////////////////////////////////////////////////////
//  Interrupt Management for FIFO //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void Write(void *addr, enum AxiStreamFifo_Register op, u32 data ) {
    *(u32 *)(addr+op) = data;
}

static u32 Read(void *addr, enum AxiStreamFifo_Register op ) {
    return *(u32 *)(addr+op);
}

// interrupt handler //
irqreturn_t IRQ_cb(int irq, void *dev_id, struct pt_regs *regs) {
    struct rpadc_fifo_auto_dev *dev =  dev_id;
    //struct rpadc_fifo_dev *rpadc = dev_id;
    static int isFirst = 1;
    //Check whether he cause of interrupt has been reveive overrun
    Write(dev->iomap_data_fifo,ISR,0xFFFFFFFF);
    Write(dev->iomap_data_fifo,IER,0x00000000);

    void* fifo  = dev->iomap_data_fifo;
    void* fifo1 = dev->iomap1_data_fifo;

    static u32 prev1, prev2;

    u32 occ = Read(fifo,RDFO);
    {
        isFirst = 0;
    }

//printk("--INTERRUPT: Available samples: %d\n", occ);    
    
    
    if(occ >= FIFO_LEN)
    {
        dev->fifoOverflow = 1;
        wake_up(&dev->readq);
        //When oveflow is detected, disable interrupts
    }

    int i;
    for(i = 0; i < occ; i++)
    {
        u32 currSample = Read(fifo1,RDFD4);
        if(writeBuf(dev, currSample) < 0) //In case of ADC FIFO overflow, abort data readout from FPGA
	{
	  Write(dev->iomap_data_fifo,IER,0x00000000);
	  return IRQ_HANDLED;
	}
     }
    wake_up(&dev->readq);

    if(dev->fifoHalfInterrupt)
        Write(dev->iomap_data_fifo,IER,0x00100000);
    else
        Write(dev->iomap_data_fifo,IER,0x04000000);

//printk("--INTERRUPT: Done\n");    
    return IRQ_HANDLED;
}


#endif //HAS_FIFO_INTERRUPT




// OPEN //
static int device_open(struct inode *inode, struct file *file)
{    
    if(!file->private_data) {
        u32 off;

        struct rpadc_fifo_auto_dev *privateInfo = container_of(inode->i_cdev, struct rpadc_fifo_auto_dev, cdev);

        printk(KERN_DEBUG "OPEN: privateInfo = %0x \n",privateInfo);
        //struct resource *r_mem =  platform_get_resource(s_pdev, IORESOURCE_MEM, 0);
        file->private_data = privateInfo;

        privateInfo->busy = 0;
        privateInfo->wIdx = 0;
        privateInfo->rIdx = 0;
        privateInfo->bufCount = 0;
	if(privateInfo->bufSize > 0)
	     kfree(privateInfo->fifoBuffer);
	privateInfo->bufSize = BUFSIZE;
	privateInfo->fifoBuffer = (u32 *)kmalloc(privateInfo->bufSize * sizeof(u32), GFP_KERNEL);
	privateInfo->dma_started = 0;
	privateInfo->dma_cyclic = 0;
#ifdef HAS_FIFO_INTERRUPT
	privateInfo->fifoOverflow = 0;
	privateInfo->fifoHalfInterrupt = 0;
#endif
    }
    struct rpadc_fifo_auto_dev *privateInfo = (struct rpadc_fifo_auto_dev *)file->private_data;
    if(!privateInfo) return -EFAULT;
    else if (privateInfo->busy) return -EBUSY;
    else privateInfo->busy++;
    privateInfo->started = 0;
    return capable(CAP_SYS_RAWIO) ? 0 : -EPERM;
}

// CLOSE //
static int device_release(struct inode *inode, struct file *file)
{
    struct rpadc_fifo_auto_dev *dev = file->private_data;
    if(!dev) return -EFAULT;
     if(--dev->busy == 0)
    {
        printk(KERN_DEBUG "CLOSE\n");
        wake_up(&dev->readq);
    }
    return 0;
}




static ssize_t device_read(struct file *filp, char *buffer, size_t length,
                           loff_t *offset)
{    
    u32 i = 0;
    struct rpadc_fifo_auto_dev *dev = (struct rpadc_fifo_auto_dev *)filp->private_data;
    u32 *b32 = (u32*)buffer;
#ifdef HAS_FIFO_INTERRUPT
    if(dev->fifoOverflow)
        return -1;
#endif
    while(dev->bufCount == 0)
    {
        if(filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
#ifdef HAS_DMA
        if(wait_event_interruptible(dev->readq, (dev->bufCount > 0 || dev->dma_started == 0)))
	  return -ERESTARTSYS;
	if(!dev->dma_started)
	    return 0;
#else
        if(wait_event_interruptible(dev->readq, (dev->bufCount > 0 || dev->started == 0)))
	    return -ERESTARTSYS;
	if(!dev->started)
	    return 0;
#endif

    }

    u32 occ = dev->bufCount;
    for(i=0; i < min(length/sizeof(u32), occ); ++i) {
        u32 curr = readBuf(dev);
	put_user(curr, b32++);
    }
    return i*sizeof(u32);
}


// WRITE //
static ssize_t device_write(struct file *filp, const char *buff, size_t len,
                            loff_t *off)
{
    printk ("<1>Sorry, this operation isn't supported yet.\n");
    return -EINVAL;
}




// MMAP //
static int device_mmap(struct file *filp, struct vm_area_struct *vma)
{
    printk ("<1>Sorry, this operation isn't supported.\n");
    return -EINVAL;
}

// LSEEK //
static loff_t memory_lseek(struct file *file, loff_t offset, int orig)
{
     printk ("<1>Sorry, this operation isn't supported.\n");
    return -EINVAL;
}

// IOCTL //
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{    
    int i;
    int status = 0;
    struct rpadc_fifo_auto_dev *dev = file->private_data;

    switch (cmd) {
#ifdef HAS_DMA
    case RPADC_FIFO_AUTO_ARM_DMA:
      {
	u32 newDmaBufSize;
	if(!arg)
	{
	  if(dev->dma_buf_size > 0)
	    newDmaBufSize = dev->dma_buf_size;
	  else
	    newDmaBufSize = DMA_STREAMING_SAMPLES * sizeof(u32);
	}
	else
	{  
	    copy_from_user (&newDmaBufSize, (void __user *)arg, sizeof(u32));
	}
 	if(dev->dma_buf_size > 0 && dev->dma_buf_size  != newDmaBufSize)
	{
	   dma_free_coherent(dev->dma_chan->device->dev,dev->dma_buf_size,dev->dma_buffer,dev->dma_handle);
	}
	if(newDmaBufSize != dev->dma_buf_size)
	{
	    dev->dma_buf_size = newDmaBufSize;
	    dev->dma_buffer = dma_alloc_coherent(dev->dma_chan->device->dev,dev->dma_buf_size,&dev->dma_handle,GFP_KERNEL);
	}
	for(i = 0; i < 300; i++)  //Just test
	  ((u32 *)dev->dma_buffer)[i] = 0xFFFFFFFF;
	
//	printk("DMA BUFFER ADDRESS: %x\n", dev->dma_buffer);
	dev->rx_dma_handle = dma_map_single(dev->dma_chan->device->dev, dev->dma_buffer, dev->dma_buf_size, DMA_FROM_DEVICE);	
	return 0;
      }
      case RPADC_FIFO_AUTO_START_DMA:
      {
//Start DMA if DMA buffer previusly allocated
	int cyclic;
	copy_from_user (&cyclic, (void __user *)arg, sizeof(u32));
	if(dev->dma_buf_size > 0)
	{
	    dev->dma_cookie = dma_prep_buffer(dev, dev->dma_chan, dev->dma_handle, dev->dma_buf_size, DMA_DEV_TO_MEM,  &dev->dma_cmp);
	    if (dma_submit_error(dev->dma_cookie) ) {
		printk(KERN_ERR "dma_prep_buffer error\n");
		return -EIO;
	    }
	  //  printk(KERN_INFO "Starting DMA transfers. DMA Buf size = %d\n", dev->dma_buf_size);
            dma_start_transfer(dev->dma_chan, &dev->dma_cmp, dev->dma_cookie, 0);
	    dev->dma_started = 1;
	    dev->dma_cyclic = cyclic;
	}
	dev->wIdx = 0;
        dev->rIdx = 0;
        dev->bufCount = 0;
	return 0;
      }
      case RPADC_FIFO_AUTO_STOP_DMA:
      {
//	  dmaengine_terminate_all(dev->dma_chan);
	  dev->dma_started = 0;
	  dev->dma_cyclic = 0;
	  wake_up(&dev->readq);
	  return 0;
      }
      case RPADC_FIFO_AUTO_IS_DMA_RUNNING:
      {
	  copy_to_user ((void __user *)arg, &dev->dma_started, sizeof(u32));	
	  return 0;
      }
      case RPADC_FIFO_AUTO_GET_DMA_BUFLEN:
      {
	  copy_to_user ((void __user *)arg, &dev->dma_buf_size, sizeof(u32));	
	  return 0;
      }
      case RPADC_FIFO_AUTO_GET_DMA_DATA:
      {
	  copy_to_user ((void __user *)arg, dev->dma_buffer, dev->dma_buf_size);	
	  return 0;
      }

#endif //HAS_DMA      
      
      case RPADC_FIFO_AUTO_SET_DRIVER_BUFLEN:
      {
	  if(dev->dma_started)
	      return 0;
	  dev->wIdx = 0;
	  dev->rIdx = 0;
          dev->bufCount = 0;
	  if(dev->bufSize > 0)
	      kfree(dev->fifoBuffer);
	  copy_from_user (&dev->bufSize, (void __user *)arg, sizeof(u32));
	  dev->fifoBuffer = (u32 *)kmalloc(dev->bufSize * sizeof(u32), GFP_KERNEL);
	  return 0;
      }
      case RPADC_FIFO_AUTO_GET_DRIVER_BUFLEN:
      {
	  copy_to_user ((void __user *)arg, &dev->bufSize, sizeof(u32));	
	  return 0;
      }
      
    	case RPADC_FIFO_AUTO_GET_COMMAND_REGISTER:
	{
		copy_to_user ((void __user *)arg, dev->iomap_command_register, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_SET_COMMAND_REGISTER:
	{
		copy_from_user (dev->iomap_command_register, (void __user *)arg, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_DECIMATOR_REGISTER:
	{
		copy_to_user ((void __user *)arg, dev->iomap_decimator_register, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_SET_DECIMATOR_REGISTER:
	{
		copy_from_user (dev->iomap_decimator_register, (void __user *)arg, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_MODE_REGISTER:
	{
		copy_to_user ((void __user *)arg, dev->iomap_mode_register, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_SET_MODE_REGISTER:
	{
		copy_from_user (dev->iomap_mode_register, (void __user *)arg, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_PACKETIZER:
	{
		copy_to_user ((void __user *)arg, dev->iomap_packetizer, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_SET_PACKETIZER:
	{
		copy_from_user (dev->iomap_packetizer, (void __user *)arg, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_POST_REGISTER:
	{
		copy_to_user ((void __user *)arg, dev->iomap_post_register, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_SET_POST_REGISTER:
	{
		copy_from_user (dev->iomap_post_register, (void __user *)arg, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_PRE_REGISTER:
	{
		copy_to_user ((void __user *)arg, dev->iomap_pre_register, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_SET_PRE_REGISTER:
	{
		copy_from_user (dev->iomap_pre_register, (void __user *)arg, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_DATA_FIFO_LEN:
	{
		u32 val = readFifo(dev->iomap_data_fifo,RDFO);
		copy_to_user ((void __user *)arg, &val, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_DATA_FIFO_VAL:
	{
		u32 val = readFifo(dev->iomap1_data_fifo,RDFD4);
		copy_to_user ((void __user *)arg, &val, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_CLEAR_DATA_FIFO:
	{
		clearFifo(dev->iomap_data_fifo,dev->iomap1_data_fifo);
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_TIME_FIFO_LEN:
	{
		u32 val = readFifo(dev->iomap_time_fifo,RDFO);
		copy_to_user ((void __user *)arg, &val, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_TIME_FIFO_VAL:
	{
		u32 val = readFifo(dev->iomap1_time_fifo,RDFD4);
		copy_to_user ((void __user *)arg, &val, sizeof(u32));
		return 0;
	}
	case RPADC_FIFO_AUTO_CLEAR_TIME_FIFO:
	{
		clearFifo(dev->iomap_time_fifo,dev->iomap1_time_fifo);
		return 0;
	}
	case RPADC_FIFO_AUTO_GET_REGISTERS:
	{
		struct rpadc_fifo_auto_registers currConf;
		memset(&currConf, 0, sizeof(currConf));
		currConf.command_register = *((u32 *)dev->iomap_command_register);
		currConf.decimator_register = *((u32 *)dev->iomap_decimator_register);
		currConf.mode_register = *((u32 *)dev->iomap_mode_register);
		currConf.packetizer = *((u32 *)dev->iomap_packetizer);
		currConf.post_register = *((u32 *)dev->iomap_post_register);
		currConf.pre_register = *((u32 *)dev->iomap_pre_register);
		copy_to_user ((void __user *)arg, &currConf, sizeof(currConf));
	}
	case RPADC_FIFO_AUTO_SET_REGISTERS:
	{
		struct rpadc_fifo_auto_registers currConf;
		copy_from_user (&currConf, (void __user *)arg, sizeof(currConf));
		if(currConf.command_register_enable)
			*((u32 *)dev->iomap_command_register) = currConf.command_register;
		if(currConf.decimator_register_enable)
			*((u32 *)dev->iomap_decimator_register) = currConf.decimator_register;
		if(currConf.mode_register_enable)
			*((u32 *)dev->iomap_mode_register) = currConf.mode_register;
		if(currConf.packetizer_enable)
			*((u32 *)dev->iomap_packetizer) = currConf.packetizer;
		if(currConf.post_register_enable)
			*((u32 *)dev->iomap_post_register) = currConf.post_register;
		if(currConf.pre_register_enable)
			*((u32 *)dev->iomap_pre_register) = currConf.pre_register;
	}

 
 #ifdef HAS_FIFO_INTERRUPT
 
     case RPADC_FIFO_AUTO_FIFO_INT_HALF_SIZE:
        dev->fifoHalfInterrupt = 1;
     //   Write(dev->iomap_data_fifo,IER,0x00100000);
        return 0;
    case RPADC_FIFO_AUTO_FIFO_INT_FIRST_SAMPLE:
        dev->fifoHalfInterrupt = 0;
      //  Write(dev->iomap_data_fifo,IER,0x04000000);
        return 0;
    case RPADC_FIFO_AUTO_FIFO_FLUSH:
	  IRQ_cb(0, dev, 0);
      //  Write(dev->iomap_data_fifo,IER,0x04000000);
    case RPADC_FIFO_AUTO_START_READ:
	  dev->started = 1;
        return 0;
    case RPADC_FIFO_AUTO_STOP_READ:
	  dev->started = 0;
	  IRQ_cb(0, dev, 0);
        return 0;

 
 #endif
 
    default:
        return -EAGAIN;
        break;
    }
    return status;
}


static unsigned int device_poll(struct file *file, struct poll_table_struct *p) 
{
    unsigned int mask=0;
    struct rpadc_fifo_auto_dev *dev =  (struct rpadc_fifo_auto_dev *)file->private_data;

    down(&dev->sem);
    poll_wait(file,&dev->readq,p);
    if(dev->bufCount > 0)
        mask |= POLLIN | POLLRDNORM;
    up(&dev->sem);
    return mask;
}




////////////////////////////////////////////////////////////////////////////////
//  PROBE  MANAGEMENT///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int id_major;
static struct class *rpadc_fifo_auto_class;
static struct rpadc_fifo_auto_dev staticPrivateInfo;

#ifdef HAS_FIFO_INTERRUPT
static void setIrq(struct platform_device *pdev)
{
     staticPrivateInfo.irq = platform_get_irq(pdev,0);
     printk(KERN_DEBUG "IRQ: %x\n",staticPrivateInfo.irq);
     int res = request_irq(staticPrivateInfo.irq, IRQ_cb, IRQF_TRIGGER_RISING ,"rpadc_fifo_auto",&staticPrivateInfo);
     if(res) 
	printk(KERN_INFO "rpadc_fifo_auto: can't get IRQ %d assigned\n",staticPrivateInfo.irq);
     else 
	printk(KERN_INFO "rpadc_fifo_auto: got IRQ %d assigned\n",staticPrivateInfo.irq);
}
#endif






static int rpadc_fifo_auto_probe(struct platform_device *pdev)
{
    int i;
    u32 off;
    static int memIdx;
    struct resource *r_mem;
    struct device *dev = &pdev->dev;

    //s_pdev = pdev;
    printk("rpadc_fifo_auto_probe  %s\n", pdev->name);

    // CHAR DEV //
    if(!deviceAllocated)
    {
        deviceAllocated = 1;
	memIdx = 0;
        printk("registering char dev %s ...\n",pdev->name);
        printk("PLATFORM DEVICE PROBE...%x\n", &staticPrivateInfo);

        int err, devno;
        dev_t newDev;
        err = alloc_chrdev_region(&newDev, 0, 1, DEVICE_NAME);
        id_major = MAJOR(newDev);
        printk("MAJOR ID...%d\n", id_major);
        if(err < 0)
        {
            printk ("alloc_chrdev_region failed\n");
            return err;
        }
        cdev_init(&staticPrivateInfo.cdev, &fops);
        staticPrivateInfo.cdev.owner = THIS_MODULE;
        staticPrivateInfo.cdev.ops = &fops;
        devno = MKDEV(id_major, 0); //Minor Id is 0
        err = cdev_add(&staticPrivateInfo.cdev, devno, 1);
        if(err < 0)
        {
            printk ("cdev_add failed\n");
            return err;
        }
        staticPrivateInfo.pdev = pdev;

        printk(KERN_NOTICE "mknod /dev/%s c %d 0\n", DEVICE_NAME, id_major);

        rpadc_fifo_auto_class = class_create(THIS_MODULE, DEVICE_NAME);
        if (IS_ERR(rpadc_fifo_auto_class))
            return PTR_ERR(rpadc_fifo_auto_class);

        device_create(rpadc_fifo_auto_class, NULL, MKDEV(id_major, 0),
                  NULL, DEVICE_NAME);


      // Initialize semaphores and queues
      sema_init(&staticPrivateInfo.sem, 1);
      spin_lock_init(&staticPrivateInfo.spinLock);
      init_waitqueue_head(&staticPrivateInfo.readq);
      staticPrivateInfo.bufCount = 0;
      staticPrivateInfo.rIdx = 0;
      staticPrivateInfo.wIdx = 0;
      staticPrivateInfo.bufSize = 0;
#ifdef HAS_DMA      
      staticPrivateInfo.dma_buf_size = 0;
      staticPrivateInfo.dma_chan = NULL;
      //Declare DMA Channel
      staticPrivateInfo.dma_chan = dma_request_slave_channel(&pdev->dev, "dma0");
      if (IS_ERR(staticPrivateInfo.dma_chan)) {
	  pr_err("xilinx_dmatest: No Tx channel\n");
	  dma_release_channel(staticPrivateInfo.dma_chan);
	  return -EFAULT;
      }
#endif //HAS_DMA
      	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	off = r_mem->start & ~PAGE_MASK;
	staticPrivateInfo.iomap_command_register = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff);


    }
    else //Further calls for memory resources
    {
//      printk("SUCCESSIVA CHIAMATA A rfx_rpadc_fifo_auto_probe: %s, memIdx: %d\n", pdev->name, memIdx);	
//      r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
//      off = r_mem->start & ~PAGE_MASK;
       switch(memIdx) {
	 
	 	case 0:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_data_fifo = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff);
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap1_data_fifo = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff);
		setIrq(pdev);
	break;
	case 1:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_decimator_register = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff); break;
	case 2:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_mode_register = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff); break;
	case 3:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_packetizer = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff); break;
	case 4:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_post_register = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff); break;
	case 5:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_pre_register = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff); break;
	case 6:
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap_time_fifo = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff);
		r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		off = r_mem->start & ~PAGE_MASK;
		staticPrivateInfo.iomap1_time_fifo = devm_ioremap(&pdev->dev,r_mem->start+off,0xffff);
	break;


	 default: printk("ERROR: Unexpected rpadc_fifo_auto_probe call\n");
      }
      memIdx++;
      printk(KERN_DEBUG"mem start: %x\n",r_mem->start);
      printk(KERN_DEBUG"mem end: %x\n",r_mem->end);
      printk(KERN_DEBUG"mem offset: %x\n",r_mem->start & ~PAGE_MASK);
    }
    return 0;
}

static int rpadc_fifo_auto_remove(struct platform_device *pdev)
{
    printk("PLATFORM DEVICE REMOVE...\n");
    if(rpadc_fifo_auto_class) {
        device_destroy(rpadc_fifo_auto_class,MKDEV(id_major, 0));
        class_destroy(rpadc_fifo_auto_class);
    }
#ifdef HAS_DMA    
    printk("PLATFORM DEVICE REMOVE dma_release_channel...%x  \n",staticPrivateInfo.dma_chan);
    if(staticPrivateInfo.dma_chan)
        dma_release_channel(staticPrivateInfo.dma_chan);
#endif //HAS_DMA
    cdev_del(&staticPrivateInfo.cdev);
    return 0;
}

static const struct of_device_id rpadc_fifo_auto_of_ids[] = {
{ .compatible = "xlnx,axi-dma-test-1.00.a",},
{ .compatible = "xlnx,axi-cfg-register-1.0",},
{ .compatible = "xlnx,axi-fifo-mm-s-4.1",},
{}
};

static struct platform_driver rpadc_fifo_auto_driver = {
   .driver = {
        .name  = MODULE_NAME,
        .owner = THIS_MODULE,
        .of_match_table = rpadc_fifo_auto_of_ids,
     },
    .probe = rpadc_fifo_auto_probe,
    .remove = rpadc_fifo_auto_remove,
};

static int __init rpadc_fifo_auto_init(void)
{
    printk(KERN_INFO "inizializing AXI module ...\n");
    deviceAllocated = 0;
    return platform_driver_register(&rpadc_fifo_auto_driver);
}

static void __exit rpadc_fifo_auto_exit(void)
{
    printk(KERN_INFO "exiting AXI module ...\n");
    platform_driver_unregister(&rpadc_fifo_auto_driver);
}

module_init(rpadc_fifo_auto_init);
module_exit(rpadc_fifo_auto_exit);
MODULE_LICENSE("GPL");
