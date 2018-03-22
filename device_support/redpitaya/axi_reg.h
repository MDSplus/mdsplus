#ifndef AXI_REG_H
#define AXI_REG_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


#define RP_OK 0;
#define RP_EMMD 1;
#define RP_EUMD 1;
#define RP_ECMD 1;
#define RP_EOMD 1;


static int dev_mem_fd = 0;
int axi_reg_Init()
{
    if (!dev_mem_fd) {
        if((dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) { return RP_EOMD; }
    }
    return RP_OK;
}

int axi_reg_Release()
{
    if (dev_mem_fd) {
        if(close(dev_mem_fd) < 0) {
            return RP_ECMD;
        }
    }
    return RP_OK;
}

void * axi_reg_Map(size_t size, size_t offset)
{
    if(dev_mem_fd == -1) { return NULL; }
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem_fd, offset);
}


#endif // AXI_REG_H
