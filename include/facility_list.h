/**********************************************************************
* FACILITY_LIST.H --
*
* List of Facility id numbers, for use in high-order portion of Status.
*
* Typical usage:
*    #define STATUS(F,C)       ((F<<16) + C)
*    #define TCL_STS_SUCCESS    STATUS(TCL_FACILITY,0x0001)
*    #define TCL_STS_ERROR      STATUS(TCL_FACILITY,0x0002)
*
*                In general, odd numbers indicate Success,
*                            even numbers indicate Error
*
***********************************************************************/

#ifndef __FACILITY_LIST_H
#define __FACILITY_LIST_H    1


#define MDSDCL_FACILITY        0x4001
#define CLI_FACILITY           0x4002
#define CCL_FACILITY           0x4003
#define TCL_FACILITY           0x4004

#define STATUS(F,C)  ((F<<16) + C)
#define STS_TEXT(N)   {N,#N}

struct stsText  {
        int   stsL_num;
        char  *stsA_name;
       };

#endif
