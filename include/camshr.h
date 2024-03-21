#ifndef _CAMSHR_H
#define _CAMSHR_H

extern int CamBytcnt(unsigned short *iosb);
extern int CamError(int *xexp, int *qexp, unsigned short *iosb_in);
extern int CamX(unsigned short *iosb_in);
extern int CamQ(unsigned short *iosb_in);
extern int CamGetStat(unsigned short *iosb_in);
extern int CamXandQ(unsigned short *iosb_in);
extern int CamFQrepw(char *name, int a, int f, int count, void *data, int mem,
                     unsigned short *iosb);
extern int CamFQstopw(char *name, int a, int f, int count, void *data, int mem,
                      unsigned short *iosb);
extern int CamFStopw(char *name, int a, int f, int count, void *data, int mem,
                     unsigned short *iosb);
extern int CamQrepw(char *name, int a, int f, int count, void *data, int mem,
                    unsigned short *iosb);
extern int CamQscanw(char *name, int a, int f, int count, void *data, int mem,
                     unsigned short *iosb);
extern int CamQstopw(char *name, int a, int f, int count, void *data, int mem,
                     unsigned short *iosb);
extern int CamStopw(char *name, int a, int f, int count, void *data, int mem,
                    unsigned short *iosb);
extern int CamPiow(char *name, int a, int f, void *data, int mem,
                   unsigned short *iosb);
extern int CamPioQrepw(char *name, int a, int f, void *data, int mem,
                       unsigned short *iosb);
extern int CamVerbose(int mode);

typedef struct
{
  char scsi_port;
  char scsi_address;
  char slot;
  char crate;
} CamKey;

extern int CamXlateLogicalname(char *Name, CamKey *key);
#endif
