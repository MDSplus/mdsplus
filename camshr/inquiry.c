#include <stdio.h>

int main(int argc, char **argv)
{
  char inqdat[56];
  unsigned char inqcmd[]={0x12,0,0,0,sizeof(inqdat)-1,0};
  int scsi_stat;
  int numgot;
  if (argc < 2)
  {
    printf("usage: inquiry scsi-device-number (i.e. 0,1,2,3)\n");
    return 0;
  }
  scsi_stat = scsi_io(atoi(argv[1]), 1, inqcmd, 6, &inqdat, sizeof(inqdat)-1, 
		      0, 0,0, &numgot);
  if (scsi_stat == 0 && (numgot > 8))
  {
    inqdat[numgot]=0;
    printf("Got: %s\n",&inqdat[8]);
  }
  return 0;
}

