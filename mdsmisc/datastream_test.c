#include <mdsdescrip.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <time.h>


extern int registerListener(char *experiment, char *tree, int shot);
extern void unregisterListener(int listenerId);
extern struct descriptor_xd *getNewSamplesSerialized(int id);
extern int TdiDecompile();

static void printDecompiled(struct descriptor *inD)
{
  int status;
  EMPTYXD(out_xd);
  char *buf;

  status = TdiDecompile(inD, &out_xd MDS_END_ARG);
  if (!(status & 1)) {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  buf = malloc(out_xd.pointer->length + 1);
  memcpy(buf, out_xd.pointer->pointer, out_xd.pointer->length);
  buf[out_xd.pointer->length] = 0;
  out_xd.pointer->pointer[out_xd.pointer->length - 1] = 0;
  printf("%s\n", buf);
  free(buf);
  MdsFree1Dx(&out_xd, 0);
}

int main(int argc, char *argv[])
{
	int id;
	int status;
	struct timespec waitTime;
	struct descriptor_xd *retXd;
	EMPTYXD(xd);

	id = registerListener("2 * test_0", "test", 1);
	while(1)
	{
		retXd = getNewSamplesSerialized(id);
		if(!retXd->pointer)
		{
			printf("Error: Null serialized  returned\n");
			exit(0);
		}
		if(retXd->pointer->class != CLASS_A)
		{
			printf("Error: Unexpected serilized class\n");
			exit(0);
		}
		status = MdsSerializeDscIn(retXd->pointer->pointer, &xd);
		if(!(status & 1))
		{
			printf("Error: decompressing result\n");
			exit(0);
		}
		printDecompiled(xd.pointer);
		MdsFree1Dx(&xd, 0);
		MdsFree1Dx(retXd, 0);
		free((char *)retXd);
	}
}

